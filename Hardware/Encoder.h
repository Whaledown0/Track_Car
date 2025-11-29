#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"

// --------------------------
// 电机1：TIM3 (PA6/PA7)
// --------------------------
#define ENCODER1_TIM            TIM3
#define ENCODER1_TIM_RCC        RCC_APB1Periph_TIM3
#define ENCODER1_GPIO_PORT      GPIOA
#define ENCODER1_GPIO_RCC       RCC_APB2Periph_GPIOA
#define ENCODER1_CHA_PIN        GPIO_Pin_6
#define ENCODER1_CHB_PIN        GPIO_Pin_7

// --------------------------
// 电机2：TIM4 (PB6/PB7)
// --------------------------
#define ENCODER2_TIM            TIM4
#define ENCODER2_TIM_RCC        RCC_APB1Periph_TIM4
#define ENCODER2_GPIO_PORT      GPIOB
#define ENCODER2_GPIO_RCC       RCC_APB2Periph_GPIOB
#define ENCODER2_CHA_PIN        GPIO_Pin_6
#define ENCODER2_CHB_PIN        GPIO_Pin_7

// 函数声明
void Encoder1_Init(void);
void Encoder2_Init(void);

// 修改：分别提供读取和清零的函数
int16_t Encoder1_GetCount(void);           // 只读不修改
int16_t Encoder2_GetCount(void);           // 只读不修改
void Encoder1_ClearCount(void);            // 只清零不读取
void Encoder2_ClearCount(void);            // 只清零不读取

// 新增：同时读取两个电机的速度（自动清零）
void Encoder_GetBothSpeeds(int16_t *speed1, int16_t *speed2);

// 新增：分别读取两个电机的速度（可选择是否清零）
int16_t Encoder1_GetSpeed_Clear(void);     // 读取并清零（保持原功能）
int16_t Encoder2_GetSpeed_Clear(void);     // 读取并清零（保持原功能）

#endif /* __ENCODER_H */
