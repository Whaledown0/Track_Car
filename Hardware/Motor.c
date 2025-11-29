#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
// 全局变量，保存电机使能状态（用于刹车）
// 注意：这个函数需要你先实现好
// void PWM_SetEnable(uint8_t Motor, uint8_t State);
// 或者，如果你的电机驱动板有独立的使能引脚，在这里操作
/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    // 电机1方向控制引脚 (PB12, PB13)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // 电机2方向控制引脚 (PB14, PB15)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // 初始状态为刹车（IN1=IN2=1）
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    
    PWM_Init(); 
}
/**
  * 函    数：电机1设置速度（与PID输出匹配）
  * 参    数：Speed 速度值，范围 -1000 ~ 1000
  *                   < 0: 反转, 速度绝对值越大越快
  *                   = 0: 刹车
  *                   > 0: 正转, 速度值越大越快
  * 返 回 值：无
  */
void Motor1_SetSpeed(int16_t Speed)
{
    if (Speed > 1000) Speed = 1000;
    if (Speed < -1000) Speed = -1000;
	Speed = Speed*0.07;
    if (Speed == 0) {
        // 刹车模式 (IN1=IN2=1)
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
        PWM_SetCompare3(0); // PWM设为0
    }
    else if (Speed > 0) {
        // 正转 (IN1=1, IN2=0)
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
        PWM_SetCompare3(Speed); // PID输出直接作为PWM值
    }
    else { // Speed < 0
        // 反转 (IN1=0, IN2=1)
        GPIO_ResetBits(GPIOB, GPIO_Pin_12);
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
        PWM_SetCompare3(-Speed); // 取绝对值作为PWM值
    }
}
/**
  * 函    数：电机2设置速度（与PID输出匹配，并修正方向）
  * 参    数：Speed 速度值，范围 -1000 ~ 1000
  * 返 回 值：无
  */
void Motor2_SetSpeed(int16_t Speed)
{
    if (Speed > 1000) Speed = 1000;
    if (Speed < -1000) Speed = -1000;
    // 注意：电机2的逻辑和电机1保持一致
    // 这是为了确保左右电机转向同步
    Speed = -Speed*0.07*1.003;
    if (Speed == 0) {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
        PWM_SetCompare4(0);
    }
    else if (Speed > 0) {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
        PWM_SetCompare4(Speed);
    }
    else { // Speed < 0
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
        PWM_SetCompare4(-Speed);
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
    Motor1_SetSpeed(Speed);
}

void Motor2_SetSpeed_Compat(int8_t Speed)
{
    Motor2_SetSpeed(Speed);
}
