#include "PID.h"

/**
  * @brief  PID控制器初始化
  * @param  pid: PID结构体指针
  * @param  kp: 比例系数
  * @param  ki: 积分系数
  * @param  kd: 微分系数
  * @param  out_min: 输出最小值
  * @param  out_max: 输出最大值
  * @retval 无
  */
void PID_Init(PID_TypeDef* pid, float kp, float ki, float kd, float out_min, float out_max)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->OutputMin = out_min;
    pid->OutputMax = out_max;
    
    // 初始化变量
    pid->Target = 0;
    pid->Speed = 0;
    pid->Output = 0;
    pid->Error_k = 0;
    pid->Error_k_1 = 0;
    pid->Error_k_2 = 0;
}

/**
  * @brief  PID计算（增量式）
  * @param  pid: PID结构体指针
  * @param  target: 目标值
  * @param  speed: 实际值
  * @retval PID输出值
  */
float PID_Calculate(PID_TypeDef* pid, float target, float speed)
{
    pid->Target = target;
    pid->Speed = speed;
    
    // 计算当前误差
    pid->Error_k = target - speed;
    
    // 增量式PID计算
    float deltaOutput = pid->Kp * (pid->Error_k - pid->Error_k_1) 
                      + pid->Ki * pid->Error_k 
                      + pid->Kd * (pid->Error_k - 2 * pid->Error_k_1 + pid->Error_k_2);
    
    // 累加输出
    pid->Output += deltaOutput;
    
    // 输出限幅
    pid->Output = Constrain(pid->Output, pid->OutputMin, pid->OutputMax);
    
    // 更新历史误差
    pid->Error_k_2 = pid->Error_k_1;
    pid->Error_k_1 = pid->Error_k;
    
    return pid->Output;
}

/**
  * @brief  重置PID控制器
  * @param  pid: PID结构体指针
  * @retval 无
  */
void PID_Reset(PID_TypeDef* pid)
{
    pid->Output = 0;
    pid->Error_k = 0;
    pid->Error_k_1 = 0;
    pid->Error_k_2 = 0;
}

/**
  * @brief  设置PID参数
  * @param  pid: PID结构体指针
  * @param  kp: 比例系数
  * @param  ki: 积分系数
  * @param  kd: 微分系数
  * @retval 无
  */
void PID_SetParams(PID_TypeDef* pid, float kp, float ki, float kd)
{
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
}

/**
  * @brief  设置输出限制
  * @param  pid: PID结构体指针
  * @param  min: 输出最小值
  * @param  max: 输出最大值
  * @retval 无
  */
void PID_SetOutputLimit(PID_TypeDef* pid, float min, float max)
{
    pid->OutputMin = min;
    pid->OutputMax = max;
}

/**
  * @brief  限制数值范围
  * @param  x: 输入值
  * @param  min: 最小值
  * @param  max: 最大值
  * @retval 限制后的值
  */
float Constrain(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
