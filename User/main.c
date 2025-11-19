#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Motor.h"
#include "Timer.h"
#include "PID.h"
#include "Encoder.h"
#include "Serial.h"

extern volatile uint32_t system_tick;

// ========== 创建两个PID控制器实例 ==========
PID_TypeDef PID_Motor1, PID_Motor2;

// ========== 全局变量 ==========
int8_t renwu = 0; // 默认第一项功能
uint8_t RxData;

int main(void)
{
		// ----- 初始化所有外设 -----
		OLED_Init();          // OLED 显示屏
		Serial_Init();        // 串口
		Timer_Init();         // 定时器（包含 TIM2，每 10ms 中断一次）
		Encoder1_Init();      // 电机1 编码器
		Encoder2_Init();      // 电机2 编码器
		Motor_Init();         // 电机驱动初始化
		Key_Init();
	
		OLED_ShowString(1, 7, "ASC");
		OLED_ShowString(2, 2, "YES");
		OLED_ShowString(2, 1, ">");
	
		// ----- 初始化PID控制器 -----
    // 电机1 PID参数：Kp=3.0, Ki=0.1, Kd=0.05，输出限制：-1200~1200
    PID_Init(&PID_Motor1, 3.0f, 0.1f, 0.05f, -1200.0f, 1200.0f);
    
    // 电机2 PID参数（可根据需要设置不同参数）
    PID_Init(&PID_Motor2, 3.0f, 0.1f, 0.05f, -1200.0f, 1200.0f);
	
		while (1) //还没写菜单
		{
				Key_Tick();
				if (Key_Check(KEY_SINGLE))
				{
					OLED_Clear();
					OLED_ShowString(2, 8, "begin");
					//开始运行
				}
				
				RxData = Serial_GetRxData();
				PID_Motor2.Target = RxData;  // 通过串口设置目标速度（选择给好的那个电机目标速度）

				// --- 显示电机1信息 ---
				OLED_ShowNum(1, 1, (int)PID_Motor1.Target, 3);      // 目标转速
				OLED_ShowNum(2, 1, (int)PID_Motor1.Speed, 4);       // 实际转速
				OLED_ShowNum(3, 1, (int)PID_Motor1.Output, 5);      // 控制输出
				OLED_ShowNum(4, 1, renwu, 1);                       // 当前任务
					
				Serial_Printf("Target:%.1f,Speed:%.1f,Output:%.1f\r\n", 
											 PID_Motor1.Target, PID_Motor1.Speed, PID_Motor1.Output);
				Delay_ms(20);
		}
}

void TIM2_IRQHandler(void)
{
    static uint16_t Count;
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        Count++;
        if (Count >= 10)  // 10ms 控制周期
        {
              Count = 0;

              // 获取两个电机速度
              int16_t pulse1 = Encoder1_Get();
              float speed1 = Encoder1_GetRPM(pulse1);
                
              int16_t pulse2 = Encoder2_Get();
              float speed2 = Encoder2_GetRPM(pulse2);
              PID_Motor2.Speed = speed2;

              // 电机1的目标速度 = 电机2的实际速度
              float target1 = speed2;
                
              // 特殊处理：根据deltaOutput调整
              float output1 = PID_Calculate(&PID_Motor1, target1, speed1);
                
              // 特殊逻辑处理（您原来的代码）
              float deltaOutput = output1 - PID_Motor1.Output;
              if (deltaOutput > 0) {
                  output1 += 20;
              } else if (deltaOutput < 0) {
                  output1 -= 25;
              }
                
              // 重新限制输出范围
              output1 = Constrain(output1, -300.0f, 300.0f);
              PID_Motor1.Output = output1;
                
              Motor1_SetSpeed((int)output1);
          }
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
