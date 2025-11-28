#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

// =================================================================
// ================ 请根据你的硬件修改此处配置 ===================
// =================================================================
// 定义编码器每转的脉冲数
// 例如：对于带有霍尔编码器和减速电机的小车，这个值可能是：
// 电机内部霍尔极数 (如11极) * 减速箱减速比 (如30) = 330
// 对于光电编码器，直接看编码器标签 (如20线, 100线, 360线)
#define ENCODER1_PULSES_REV 624.0f  // !!! 修改电机1的编码器线数 !!!
#define ENCODER2_PULSES_REV 624.0f  // !!! 修改电机2的编码器线数 !!!

// --------------------------
// 电机1：TIM3 (PA6/PA7)
// --------------------------
#define ENCODER1_TIM                TIM3
#define ENCODER1_TIM_RCC            RCC_APB1Periph_TIM3
#define ENCODER1_GPIO_PORT          GPIOA
#define ENCODER1_GPIO_RCC           RCC_APB2Periph_GPIOA
#define ENCODER1_CHA_PIN            GPIO_Pin_6
#define ENCODER1_CHB_PIN            GPIO_Pin_7

// --------------------------
// 电机2：TIM4 (PB6/PB7)
// --------------------------
#define ENCODER2_TIM                TIM4
#define ENCODER2_TIM_RCC            RCC_APB1Periph_TIM4
#define ENCODER2_GPIO_PORT          GPIOB
#define ENCODER2_GPIO_RCC           RCC_APB2Periph_GPIOB
#define ENCODER2_CHA_PIN            GPIO_Pin_6
#define ENCODER2_CHB_PIN            GPIO_Pin_7

// =================================================================
// ================== 不建议修改下方内容 ==========================
// =================================================================

// 函数声明
void Encoder1_Init(void);
void Encoder2_Init(void);

/**
  * @brief  获取电机1在10ms中断周期内的脉冲计数值 (并内部清零计数器)
  * @param  None
  * @retval 10ms内的脉冲计数值 (int16_t类型，范围-32768~32767)
  * @note   负值表示反转，正值表示正转。
  */
int16_t Encoder1_GetSpeed(void);

/**
  * @brief  获取电机2在10ms中断周期内的脉冲计数值 (并内部清零计数器)
  * @param  None
  * @retval 10ms内的脉冲计数值 (int16_t类型，范围-32768~32767)
  * @note   负值表示反转，正值表示正转。
  */
int16_t Encoder2_GetSpeed(void);

/**
  * @brief  获取电机1当前计数值（不清零，用于调试）
  * @param  None
  * @retval 当前计数值
  */
int16_t Encoder1_GetCount(void);

/**
  * @brief  获取电机2当前计数值（不清零，用于调试）
  * @param  None
  * @retval 当前计数值
  */
int16_t Encoder2_GetCount(void);

#endif /* __ENCODER_H */
