#include "Encoder.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// --------------------------
// 电机1：TIM3 (PA6/PA7) - 最高分辨率配置
// --------------------------
void Encoder1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_ICInitTypeDef TIM_ICInitStruct = {0};

    // 使能时钟
    RCC_APB2PeriphClockCmd(ENCODER1_GPIO_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(ENCODER1_TIM_RCC, ENABLE);

    // 配置GPIO为上拉输入
    GPIO_InitStruct.GPIO_Pin = ENCODER1_CHA_PIN | ENCODER1_CHB_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(ENCODER1_GPIO_PORT, &GPIO_InitStruct);

    // 配置定时器基本参数
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER1_TIM, &TIM_TimeBaseInitStruct);

    // 配置编码器接口模式 - 使用双边沿检测获得最高分辨率
    TIM_EncoderInterfaceConfig(ENCODER1_TIM, 
                               TIM_EncoderMode_TI12,
                               TIM_ICPolarity_BothEdge,
                               TIM_ICPolarity_BothEdge);

    // 配置输入捕获滤波器
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(ENCODER1_TIM, &TIM_ICInitStruct);
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER1_TIM, &TIM_ICInitStruct);

    TIM_Cmd(ENCODER1_TIM, ENABLE);
    TIM_SetCounter(ENCODER1_TIM, 0);
}

/**
  * @brief  获取电机1当前计数值（只读不修改）
  * @retval 计数值
  */
int16_t Encoder1_GetCount(void)
{
    return (int16_t)TIM_GetCounter(ENCODER1_TIM);
}

/**
  * @brief  清零电机1计数器
  */
void Encoder1_ClearCount(void)
{
    TIM_SetCounter(ENCODER1_TIM, 0);
}

/**
  * @brief  获取电机1速度 (10ms脉冲数) 并清零（保持原功能）
  * @retval 速度值
  */
int16_t Encoder1_GetSpeed_Clear(void)
{
    int16_t count = (int16_t)TIM_GetCounter(ENCODER1_TIM);
    TIM_SetCounter(ENCODER1_TIM, 0);
    return count;
}


// --------------------------
// 电机2：TIM4 (PB6/PB7) - 最高分辨率配置
// --------------------------
void Encoder2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_ICInitTypeDef TIM_ICInitStruct = {0};

    // 使能时钟
    RCC_APB2PeriphClockCmd(ENCODER2_GPIO_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(ENCODER2_TIM_RCC, ENABLE);

    // 配置GPIO为上拉输入
    GPIO_InitStruct.GPIO_Pin = ENCODER2_CHA_PIN | ENCODER2_CHB_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(ENCODER2_GPIO_PORT, &GPIO_InitStruct);

    // 配置定时器基本参数
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER2_TIM, &TIM_TimeBaseInitStruct);

    // 配置编码器接口模式 - 使用双边沿检测获得最高分辨率
    TIM_EncoderInterfaceConfig(ENCODER2_TIM, 
                               TIM_EncoderMode_TI12,
                               TIM_ICPolarity_BothEdge,
                               TIM_ICPolarity_BothEdge);

    // 配置输入捕获滤波器
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(ENCODER2_TIM, &TIM_ICInitStruct);
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER2_TIM, &TIM_ICInitStruct);

    TIM_Cmd(ENCODER2_TIM, ENABLE);
    TIM_SetCounter(ENCODER2_TIM, 0);
}

/**
  * @brief  获取电机2当前计数值（只读不修改）
  * @retval 计数值
  */
int16_t Encoder2_GetCount(void)
{
    return (int16_t)TIM_GetCounter(ENCODER2_TIM);
}

/**
  * @brief  清零电机2计数器
  */
void Encoder2_ClearCount(void)
{
    TIM_SetCounter(ENCODER2_TIM, 0);
}

/**
  * @brief  获取电机2速度 (10ms脉冲数) 并清零（保持原功能）
  * @retval 速度值
  */
int16_t Encoder2_GetSpeed_Clear(void)
{
    int16_t count = (int16_t)TIM_GetCounter(ENCODER2_TIM);
    TIM_SetCounter(ENCODER2_TIM, 0);
    return count;
}

/**
  * @brief  同时读取两个电机的速度并自动清零
  * @param  speed1: 存储电机1速度的指针
  * @param  speed2: 存储电机2速度的指针
  */
void Encoder_GetBothSpeeds(int16_t *speed1, int16_t *speed2)
{
    *speed1 = (int16_t)TIM_GetCounter(ENCODER1_TIM);
    *speed2 = (int16_t)TIM_GetCounter(ENCODER2_TIM);
    
    // 读取后自动清零两个计数器
    TIM_SetCounter(ENCODER1_TIM, 0);
    TIM_SetCounter(ENCODER2_TIM, 0);
}
