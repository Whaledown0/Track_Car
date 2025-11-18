#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Motor.h"
#include "Timer.h"

extern volatile uint32_t system_tick;

int main(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();
	Timer_Init();
	
	OLED_ShowString(1, 7, "ASC");
	OLED_ShowString(2, 2, "YES");
	OLED_ShowString(2, 1, ">");
	while (1)
	{
			Key_Tick();
			if (Key_Check(KEY_SINGLE))
			{
				OLED_Clear();
				OLED_ShowString(2, 8, "begin");
				//开始运行
			}
			Delay_ms(20);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		system_tick++;  
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
