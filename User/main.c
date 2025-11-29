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

static int16_t g_speed1 = 0, g_speed2 = 0;
static int16_t g_last_speed1 = 0, g_last_speed2 = 0;

// ========== 创建PID控制器实例 ==========
// 原有的电机控制器（此项目未使用，保留以防万一）
PID_TypeDef PID_Motor1, PID_Motor2; 

// 新增：转向控制器和速度控制器
PID_TypeDef PID_Steering;        // 控制转向，输入为位置误差，输出为速度差
PID_TypeDef PID_Track_Speed;     // 控制循迹时的速度，使速度更平稳
PID_TypeDef PID_Follow;          // 新增：电机跟随控制器（叠加在转向控制上）

// 在main函数外部定义一个转换宏，方便调试时显示RPM值
#define ENCODER_PULSES_PER_REV 2496.0f 
#define PULSE_TO_RPM(pulses) (((float)(pulses) * 3600.0f) / (ENCODER_PULSES_PER_REV))

// 速度平滑滤波相关
#define SMOOTH_FILTER_SIZE 5
static int16_t speed1_buffer[SMOOTH_FILTER_SIZE] = {0};
static int16_t speed2_buffer[SMOOTH_FILTER_SIZE] = {0};
static uint8_t filter_index = 0;

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

    // 新增：初始化转向PID控制器 - 增强转向幅度
    PID_Init(&PID_Steering, 25.0f, 0.3f, 0.02f, -1800.0f, 1800.0f);  // 加大Kp和限幅
    
    // 新增：初始化循迹速度PID控制器
    PID_Init(&PID_Track_Speed, 0.5f, 0.1f, 0.02f, -2000.0f, 2000.0f);

    // 新增：初始化电机跟随PID控制器
    PID_Init(&PID_Follow, 1.2f, 0.08f, 0.15f, -3000.0f, 3000.0f);

    // ----- 初始化菜单系统 -----
    Menu_Init();
    
    Menu_UpdateDisplay();

    while (1)
    {
        // ===== 非阻塞按键扫描 =====
        Key_Scan();
        
        // ===== 获取按键事件 =====
        uint8_t key1_event = Key_GetEvent(0);  // KEY1事件
        uint8_t key2_event = Key_GetEvent(1);  // KEY2事件
        
        // ===== 按键  =====
        if (key1_event != KEY_NO_EVENT) {
            Menu_HandleKeyEvent(0, key1_event);  // 恢复KEY1处理
        }
        if (key2_event != KEY_NO_EVENT) {
            Menu_HandleKeyEvent(1, key2_event);  // 保持KEY2处理
        }
		
//		Menu_UpdateDisplay();
        
        
        
        Delay_ms(50);
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // ========== 改用分别读取，不清零 ==========
        int16_t current_count1 = Encoder1_GetCount();
        int16_t current_count2 = Encoder2_GetCount();
        
        // 计算增量速度（相对于上次读取的变化）
        g_speed1 = current_count1 - g_last_speed1;
        g_speed2 = current_count2 - g_last_speed2;
        
        // 更新上次值
        g_last_speed1 = current_count1;
        g_last_speed2 = current_count2;

        if (Menu_IsAutoRunning()) {
            // ========== 1. 设置基础速度（油门）==========
            int16_t base_speed = 600; 
            
            // ========== 2. 转向控制 - 停止一侧轮胎的原地转向 ==========
            uint8_t ir_position = IR_GetPosition();
            
            // 方案：根据红外位置直接控制电机启停，实现原地转向
            int16_t left_motor_speed = 0;
            int16_t right_motor_speed = 0;
            
            switch(ir_position) {
                case 0: // 严重左偏 - 停止左电机，右电机全速（原地右转）
                    left_motor_speed = 0;      // 停止左电机
                    right_motor_speed = base_speed;  // 右电机全速
                    break;
                    
                case 1: // 轻微左偏 - 左电机低速，右电机高速（小半径右转）
                    left_motor_speed = base_speed * 0.3f;   // 左电机30%速度
                    right_motor_speed = base_speed;         // 右电机全速
                    break;
                    
                case 2: // 正中 - 两轮同速直行
                    left_motor_speed = base_speed;  // 左电机全速
                    right_motor_speed = base_speed; // 右电机全速
                    break;
                    
                case 3: // 轻微右偏 - 左电机高速，右电机低速（小半径左转）
                    left_motor_speed = base_speed;         // 左电机全速
                    right_motor_speed = base_speed * 0.3f;  // 右电机30%速度
                    break;
                    
                case 4: // 严重右偏 - 停止右电机，左电机全速（原地左转）
                    left_motor_speed = base_speed;  // 左电机全速
                    right_motor_speed = 0;      // 停止右电机
                    break;
                    
                default: // 默认直行
                    left_motor_speed = base_speed;
                    right_motor_speed = base_speed;
                    break;
            }
            
            // ========== 3. 极端情况的强化转向（完全停止一侧）==========
            if(ir_position == 0) {
                // 严重左偏：完全停止左电机，右电机甚至可以超速
                left_motor_speed = 0;                    // 完全停止左电机
                right_motor_speed = base_speed * 1.1f;   // 右电机110%速度（如果电机支持）
            } 
            else if(ir_position == 4) {
                // 严重右偏：完全停止右电机，左电机甚至可以超速
                left_motor_speed = base_speed * 1.1f;   // 左电机110%速度
                right_motor_speed = 0;                   // 完全停止右电机
            }
            
            // ========== 4. 限幅保护 ==========
            left_motor_speed = Constrain(left_motor_speed, 0, 1000);
            right_motor_speed = Constrain(right_motor_speed, 0, 1000);
            
            // ========== 5. 设置电机 - 实现停止一侧轮胎的转向 ==========
            Motor1_SetSpeed(left_motor_speed);   // 左电机
            Motor2_SetSpeed(right_motor_speed);  // 右电机
            
        } else {
            Motor1_SetSpeed(0);
            Motor2_SetSpeed(0);
            PID_Reset(&PID_Steering);
            PID_Reset(&PID_Follow);
            
            // 停止时清零速度记录
            g_speed1 = 0;
            g_speed2 = 0;
            g_last_speed1 = 0;
            g_last_speed2 = 0;
        }
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
