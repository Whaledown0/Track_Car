#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"

// PID结构体定义
typedef struct {
    float Target;           // 目标值
    float Speed;            // 实际值
    float Output;           // 控制输出
    float Kp, Ki, Kd;      // PID参数
    float Error_k;          // e(k)
    float Error_k_1;        // e(k-1)
    float Error_k_2;        // e(k-2)
    float OutputMin;        // 输出最小值
    float OutputMax;        // 输出最大值
} PID_TypeDef;

// 函数声明
void PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, float out_min, float out_max);
float PID_Calculate(PID_TypeDef* pid, float target, float speed);
void PID_Reset(PID_TypeDef* pid);
void PID_SetParams(PID_TypeDef* pid, float kp, float ki, float kd);
void PID_SetOutputLimit(PID_TypeDef* pid, float min, float max);

// 辅助函数
float Constrain(float x, float min, float max);

#endif
