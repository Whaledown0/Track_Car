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

extern volatile uint32_t system_tick;
void AutoTrack_Run(int16_t base_speed);  

// ========== 创建两个PID控制器实例 ==========
PID_TypeDef PID_Motor1, PID_Motor2;

int main(void)
{
    // ----- 初始化所有外设 -----
    OLED_Init();                  // OLED 显示屏
    Timer_Init();                 // 定时器
    Encoder1_Init();              // 电机1 编码器
    Encoder2_Init();              // 电机2 编码器
    Motor_Init();                 // 电机驱动初始化
    Key_Init();                   // 按键初始化
    IR_Sensor_Init();             // 红外传感器初始化
    
    // ----- 初始化PID控制器 -----
    PID_Init(&PID_Motor1, 3.0f, 0.1f, 0.05f, -1200.0f, 1200.0f);
    PID_Init(&PID_Motor2, 3.0f, 0.1f, 0.05f, -1200.0f, 1200.0f);
    
    // ----- 初始化菜单系统 -----
    Menu_Init();
    
    // 显示欢迎信息
    OLED_Clear();
    OLED_ShowString(1, 1, "Auto Track Car");
    OLED_ShowString(2, 1, "System Ready");
    Delay_ms(1000);
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
        
        // ===== 自动循迹模式处理 =====
        if (Menu_IsAutoRunning()) {
            // 读取红外传感器
            IR_Sensor_Read();
            
            // 获取当前速度档位
            uint8_t current_speed = Menu_GetSpeedLevel();
            
            // 根据速度档位设置基础速度
            int16_t base_speed = 0;
            switch(current_speed) {
                case SPEED_LOW: base_speed = 300; break;
                case SPEED_MEDIUM: base_speed = 500; break;
                case SPEED_HIGH: base_speed = 800; break;
                default: base_speed = 0; break;
            }
            
            // 执行自动循迹算法
            AutoTrack_Run(base_speed);
        }
        
        Delay_ms(20);
    }
}



// ===== TIM2 中断服务函数（PID控制）=====
void TIM2_IRQHandler(void)
{
    static uint16_t Count;
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Count++;
        if (Count >= 10)  // 10ms * 10 = 100ms 控制周期
        {
            Count = 0;

            // 获取两个电机速度
            int16_t pulse1 = Encoder1_Get();
            float speed1 = Encoder1_GetRPM(pulse1);
            
            int16_t pulse2 = Encoder2_Get();
            float speed2 = Encoder2_GetRPM(pulse2);
            PID_Motor2.Speed = speed2;

            // 电机1的目标速度 = 电机2的实际速度（跟随控制）
            float target1 = speed2;
            
            // PID计算电机1输出
            float output1 = PID_Calculate(&PID_Motor1, target1, speed1);
            
            // 特殊逻辑处理
            float deltaOutput = output1 - PID_Motor1.Output;
            if (deltaOutput > 0) {
                output1 += 20;
            } else if (deltaOutput < 0) {
                output1 -= 25;
            }
            
            // 重新限制输出范围
            output1 = Constrain(output1, -300.0f, 300.0f);
            PID_Motor1.Output = output1;
            
            // 只有在自动运行模式下才设置电机速度
            if (Menu_IsAutoRunning()) {
                Motor1_SetSpeed((int)output1);
            }
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

// 自动循迹运行函数
void AutoTrack_Run(int16_t base_speed)
{
    uint8_t position = IR_GetPosition();
    
    // 根据红外传感器位置调整电机速度实现循迹
    switch(position) {
        case 0: // 严重偏右，大幅左转
            Motor_SetSpeed(-base_speed * 30, base_speed * 70);
            break;
        case 1: // 偏右，左转
            Motor_SetSpeed(-base_speed * 20, base_speed * 60);
            break;
        case 2: // 居中，直行
            Motor_SetSpeed(base_speed, base_speed);
            break;
        case 3: // 偏左，右转
            Motor_SetSpeed(base_speed * 60, -base_speed * 20);
            break;
        case 4: // 严重偏左，大幅右转
            Motor_SetSpeed(base_speed * 70, -base_speed * 30);
            break;
        default: // 默认直行
            Motor_SetSpeed(base_speed, base_speed);
            break;
    }
    
    // PID控制（如果需要）
    if (base_speed > 0) {
        PID_Motor2.Target = base_speed;
    }
}
