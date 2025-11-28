#include "Encoder.h"

// --------------------------
// 电机1：TIM3 (PA6/PA7) - 已按方案一优化
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

    // 配置定时器基本参数 - 使用更大的周期
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;  // 保持最大值
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER1_TIM, &TIM_TimeBaseInitStruct);

    // 关键修改：使用双边沿检测获得最高分辨率
    TIM_EncoderInterfaceConfig(ENCODER1_TIM, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);

    // 配置输入滤波器减少噪声
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;  // 最大滤波
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(ENCODER1_TIM, &TIM_ICInitStruct);
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER1_TIM, &TIM_ICInitStruct);

    TIM_SetCounter(ENCODER1_TIM, 0);
    TIM_Cmd(ENCODER1_TIM, ENABLE);
}

/**
  * @brief  获取电机1速度 (10ms脉冲数) 并清零
  * @retval 速度值
  */
int16_t Encoder1_GetSpeed(void)
{
    // 读取当前计数值
    int16_t count = (int16_t)TIM_GetCounter(ENCODER1_TIM);
    // 读取后立即清零，为下一次10ms周期做准备
    TIM_SetCounter(ENCODER1_TIM, 0);
    return count;
}

/**
  * @brief  获取电机1当前计数值
  * @retval 计数值
  */
int16_t Encoder1_GetCount(void)
{
    // 只读取，不修改，用于调试时观察累积值
    return (int16_t)TIM_GetCounter(ENCODER1_TIM);
}


// --------------------------
// 电机2：TIM4 (PB6/PB7) - 修改为与电机1一致的方案一配置
// --------------------------
void Encoder2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_ICInitTypeDef TIM_ICInitStruct = {0};

    // 使能时钟
    RCC_APB2PeriphClockCmd(ENCODER2_GPIO_RCC, ENABLE);
    RCC_APB1PeriphClockCmd(ENCODER2_TIM_RCC, ENABLE);

    // 配置GPIO为上拉输入（与电机1保持一致）
    GPIO_InitStruct.GPIO_Pin = ENCODER2_CHA_PIN | ENCODER2_CHB_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ENCODER2_GPIO_PORT, &GPIO_InitStruct);

    // 配置定时器基本参数（与电机1保持一致）
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;  // 改为0xFFFF，与电机1一致
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(ENCODER2_TIM, &TIM_TimeBaseInitStruct);

    // 关键修改：改为双边沿检测获得最高分辨率（与电机1保持一致）
    TIM_EncoderInterfaceConfig(ENCODER2_TIM, TIM_EncoderMode_TI12,
                               TIM_ICPolarity_BothEdge, TIM_ICPolarity_BothEdge);  // 改为BothEdge

    // 配置输入捕获滤波器（与电机1保持一致）
    TIM_ICStructInit(&TIM_ICInitStruct);
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;  // 改为0x0F，与电机1保持一致
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(ENCODER2_TIM, &TIM_ICInitStruct);
    
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(ENCODER2_TIM, &TIM_ICInitStruct);

    // 清空计数器并启动计数
    TIM_SetCounter(ENCODER2_TIM, 0);
    TIM_Cmd(ENCODER2_TIM, ENABLE);
}

/**
  * @brief  获取电机2速度 (10ms脉冲数) 并清零
  * @retval 速度值
  */
int16_t Encoder2_GetSpeed(void)
{
    // 读取当前计数值
    int16_t count = (int16_t)TIM_GetCounter(ENCODER2_TIM);
    // 读取后立即清零，为下一次10ms周期做准备
    TIM_SetCounter(ENCODER2_TIM, 0);
    return count;
}

/**
  * @brief  获取电机2当前计数值
  * @retval 计数值
  */
int16_t Encoder2_GetCount(void)
{
    // 只读取，不修改，用于调试时观察累积值
    return (int16_t)TIM_GetCounter(ENCODER2_TIM);
}
