#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
void Motor1_SetSpeed(int16_t Speed);
void Motor2_SetSpeed(int16_t Speed);
void Motor_SetSpeed(int16_t Speed1, int16_t Speed2);
void Motor_Stop(void);
void Motor1_SetSpeed_Compat(int8_t Speed);
void Motor2_SetSpeed_Compat(int8_t Speed);

#endif
