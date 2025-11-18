#include "stm32f10x.h"
#include "Key.h"

#define KEY_PRESSED				1
#define KEY_UNPRESSED			0

uint8_t Key_Flag[KEY_COUNT];

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetState(void)
{
	return (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) ? KEY_PRESSED : KEY_UNPRESSED;
}

uint8_t Key_Check(uint8_t Flag)
{
	if (Key_Flag[0] & Flag)
	{
		Key_Flag[0] &= ~Flag;
		return 1;
	}
	return 0;
}

void Key_Tick(void)
{
	static uint8_t last_state = KEY_UNPRESSED;
	static uint16_t press_time = 0;
	static uint16_t release_time = 0;
	static uint8_t click_count = 0;
	
	uint8_t current_state = Key_GetState();
	
	if (current_state == KEY_PRESSED && last_state == KEY_UNPRESSED)
	{
		Key_Flag[0] |= KEY_DOWN;
		press_time = 0;
		click_count++;
	}
	
	if (current_state == KEY_UNPRESSED && last_state == KEY_PRESSED)
	{
		Key_Flag[0] |= KEY_UP;
		release_time = 0;
		
		if (press_time < 30) 
		{
			Key_Flag[0] |= KEY_SINGLE;
		}
	}
	
	if (current_state == KEY_PRESSED)
	{
		press_time++;
		if (press_time > 50) 
		{
			Key_Flag[0] |= KEY_LONG;
		}
	}
	
	if (current_state == KEY_UNPRESSED)
	{
		release_time++;
		if (release_time > 20 && click_count == 1) 
		{
			Key_Flag[0] |= KEY_SINGLE;
			click_count = 0;
		}
		else if (release_time > 20) 
		{
			click_count = 0;
		}
	}
	
	if (current_state == KEY_PRESSED && release_time < 20 && release_time > 0 && click_count == 2)
	{
		Key_Flag[0] |= KEY_DOUBLE;
		click_count = 0;
	}
	
	if (current_state == KEY_PRESSED)
	{
		Key_Flag[0] |= KEY_HOLD;
	}
	else
	{
		Key_Flag[0] &= ~KEY_HOLD;
	}
	
	last_state = current_state;
}
