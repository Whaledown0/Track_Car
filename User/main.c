#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Motor.h"
#include "Timer.h"
#include "PID.h"
#include "Encoder.h"
#include "ir_sensor.h"
#include "Menu.h"

// 注意：system_tick 通常由 SysTick 中断管理，如果由 TIM2 管理，请确保正确配置
extern volatile uint32_t system_tick; 

// ========== 创建PID控制器实例 ==========
// 原有的电机控制器（此项目未使用，保留以防万一）
PID_TypeDef PID_Motor1, PID_Motor2; 

// 新增：转向控制器和速度控制器
PID_TypeDef PID_Steering;   // 控制转向，输入为位置误差，输出为速度差
PID_TypeDef PID_Track_Speed; // 控制循迹时的速度，使速度更平稳

// 在main函数外部定义一个转换宏，方便调试时显示RPM值
// 你需要替换 ENCODER1_PULSES_REV 和 ENCODER2_PULSES_REV 为你编码器的实际线数
#define ENCODER_PULSES_PER_REV 2496.0f 

#define PULSE_TO_RPM(pulses) (((float)(pulses) * 3600.0f) / (ENCODER_PULSES_PER_REV))


int main(void)
{
    // ----- 初始化所有外设 -----
    OLED_Init();                  // OLED 显示屏
    Timer_Init();                 // 定时器 (TIM2, 10ms中断)
    Encoder1_Init();              // 电机1 编码器
    Encoder2_Init();              // 电机2 编码器
    Motor_Init();                 // 电机驱动初始化
    Key_Init();                   // 按键初始化
    IR_Sensor_Init();             // 红外传感器初始化
    
    // ----- 初始化PID控制器 -----
    // 原有PID参数（本项目未使用）
    PID_Init(&PID_Motor1, 3.0f, 0.1f, 0.05f, -4800.0f, 4800.0f);
    PID_Init(&PID_Motor2, 3.0f, 0.1f, 0.05f, -4800.0f, 4800.0f);

    // 新增：初始化转向PID控制器 (参数需要实际调试)
    // Kp: 比例，影响响应速度和稳定性
    // Ki: 积分，消除稳态误差
    // Kd: 微分，抑制超调和振荡
    // 输出范围: -1000 到 1000, 作为速度差加到左右电机上
    PID_Init(&PID_Steering, 8.0f, 0.0f, 0.1f, -1000.0f, 1000.0f); 
    
    // 新增：初始化循迹速度PID控制器 (参数需要实际调试)
    // 目标是让电机的实际速度稳定在设定的 pulse/10ms 目标值上
    PID_Init(&PID_Track_Speed, 0.5f, 0.1f, 0.02f, -2000.0f, 2000.0f);

    // ----- 初始化菜单系统 -----
    Menu_Init();
    
    // 显示欢迎信息
    OLED_Clear();
    OLED_ShowString(1, 1, "PID Track Car Ver2");
    OLED_ShowString(2, 1, "System Ready");
    Delay_ms(1000);
    OLED_Clear();
    Menu_UpdateDisplay();

    while (1)
    {
        // ===== 非阻塞按键扫描 =====
        Key_Scan();
        
        // ===== 获取按键事件 =====
        uint8_t key1_event = Key_GetEvent(0);  // KEY1事件
        uint8_t key2_event = Key_GetEvent(1);  // KEY2事件
        
        // ===== 处理按键事件 =====
        if (key1_event != KEY_NO_EVENT) {
            Menu_HandleKeyEvent(0, key1_event);
        }
        if (key2_event != KEY_NO_EVENT) {
            Menu_HandleKeyEvent(1, key2_event);
        }
        
        // 在主循环中可以更新一些不紧急的显示信息
        if (Menu_IsAutoRunning()) {
            uint8_t pos = IR_GetPosition();
            OLED_ShowString(1, 1, "Run:    ");
            OLED_ShowNum(1, 5, pos, 1);
        } else {
            OLED_ShowString(1, 1, "Standby");
        }
        
		
		int a=Encoder1_GetSpeed()*10;
		OLED_ShowNum(3,5,a,3);
		
		
        Delay_ms(50); // 主循环可以适当降低频率
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        int16_t speed1_pulse = Encoder1_GetSpeed(); 
        int16_t speed2_pulse = Encoder2_GetSpeed();
        if (Menu_IsAutoRunning()) {
            // ========== 1. 设置基础速度（油门）==========
            // 这是一个固定的速度值，比如600（对应约144 RPM）
            int16_t base_speed = 600; 
            // ========== 2. 转向控制 ==========
            uint8_t ir_position = IR_GetPosition();
            float steering_error = 0.0f;
            switch(ir_position) {
                // 注意：这里的误差定义要确保和PID参数匹配
                // 严重左偏 -> 负误差 -> 需要右转 (左电机快，右电机慢 -> 最终速度差为负)
                case 0: steering_error = -2.0f; break; 
                case 1: steering_error = -1.0f; break; 
                case 2: steering_error = 0.0f; break; 
                case 3: steering_error = 1.0f; break; 
                case 4: steering_error = 2.0f; break; 
                default: steering_error = 0.0f; break; 
            }
            
            // PID计算，输出一个速度差
            // 这个差值将被加到左边电机，减到右边电机
            int16_t steering_delta = (int16_t)PID_Calculate(&PID_Steering, 0.0f, steering_error);
            // ========== 3. 最终合成电机速度 ==========
            int16_t final_speed_left = base_speed + steering_delta;
            int16_t final_speed_right = base_speed - steering_delta;
            
            // 限幅，防止超出电机驱动能力 (-1000 到 1000)
            final_speed_left = Constrain(final_speed_left, -1000, 1000);
            final_speed_right = Constrain(final_speed_right, -1000, 1000);
            // 设置电机
            Motor1_SetSpeed(final_speed_left);
            Motor2_SetSpeed(final_speed_right);
        } else {
            // 停止电机并重置PID
            Motor1_SetSpeed(0);
            Motor2_SetSpeed(0);
            PID_Reset(&PID_Steering);
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
