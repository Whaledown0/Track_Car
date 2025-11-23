#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB12~PB15引脚初始化为推挽输出	
	
	PWM_Init();													//初始化直流电机的底层PWM
}

/**
  * 函    数：电机1设置速度（适配循迹算法）
  * 参    数：Speed 要设置的速度，范围：-1000~1000
  * 返 回 值：无
  * 说    明：为了适配循迹算法，扩大速度范围到±1000
  */
void Motor1_SetSpeed(int16_t Speed)
{
    // 限幅到±1000
    if (Speed > 1000) Speed = 1000;
    if (Speed < -1000) Speed = -1000;
    
    if (Speed >= 0)							//如果设置正转的速度值
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);	//PB12置高电平
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);	//PB13置低电平，设置方向为正转
		PWM_SetCompare3(Speed / 10);		//PWM设置为速度值/10，因为PWM范围是0-100
	}
	else									//否则，即设置反转的速度值
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);	//PB12置低电平
		GPIO_SetBits(GPIOB, GPIO_Pin_13);	//PB13置高电平，设置方向为反转
		PWM_SetCompare3((-Speed) / 10);	    //PWM设置为负的速度值/10
	}
}

/**
  * 函    数：电机2设置速度（适配循迹算法）
  * 参    数：Speed 要设置的速度，范围：-1000~1000
  * 返 回 值：无
  * 说    明：为了适配循迹算法，扩大速度范围到±1000
  */
void Motor2_SetSpeed(int16_t Speed)
{
    // 限幅到±1000
    if (Speed > 1000) Speed = 1000;
    if (Speed < -1000) Speed = -1000;
    
    if (Speed >= 0)							//如果设置正转的速度值
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_14);	//PB14置高电平
		GPIO_ResetBits(GPIOB, GPIO_Pin_15);	//PB15置低电平，设置方向为正转
		PWM_SetCompare4(Speed / 10);		//PWM设置为速度值/10，因为PWM范围是0-100
	}
	else									//否则，即设置反转的速度值
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);	//PB14置低电平
		GPIO_SetBits(GPIOB, GPIO_Pin_15);	//PB15置高电平，设置方向为反转
		PWM_SetCompare4((-Speed) / 10);	    //PWM设置为负的速度值/10
	}
}

/**
  * 函    数：双电机同时设置速度（循迹专用）
  * 参    数：Speed1 电机1速度，-1000~1000
  * 参    数：Speed2 电机2速度，-1000~1000
  * 返 回 值：无
  */
void Motor_SetSpeed(int16_t Speed1, int16_t Speed2)
{
    Motor1_SetSpeed(Speed1);
    Motor2_SetSpeed(Speed2);
}

/**
  * 函    数：停止所有电机
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Stop(void)
{
    Motor1_SetSpeed(0);
    Motor2_SetSpeed(0);
}

/**
  * 函    数：设置电机速度（兼容旧接口，范围-100~100）
  * 参    数：Speed 要设置的速度，范围：-100~100
  * 返 回 值：无
  * 说    明：为了保持兼容性，保留原函数名但内部转换为新范围
  */
void Motor1_SetSpeed_Compat(int8_t Speed)
{
    // 将-100~100映射到-500~500范围
    Motor1_SetSpeed(Speed * 5);
}

void Motor2_SetSpeed_Compat(int8_t Speed)
{
    // 将-100~100映射到-500~500范围
    Motor2_SetSpeed(Speed * 5);
}
