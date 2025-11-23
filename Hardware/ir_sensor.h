#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f10x.h"

// 红外传感器数量
#define IR_SENSOR_COUNT 5

// 传感器引脚定义（请根据实际连接修改）
#define IR1_PIN GPIO_Pin_3
#define IR2_PIN GPIO_Pin_4
#define IR3_PIN GPIO_Pin_5
#define IR4_PIN GPIO_Pin_6
#define IR5_PIN GPIO_Pin_7
#define IR_PORT GPIOA

// 传感器状态
typedef enum {
    IR_BLACK = 0,    // 检测到黑线
    IR_WHITE = 1     // 检测到白色背景
} IR_State;

// 传感器数据结构
typedef struct {
    uint8_t pins[IR_SENSOR_COUNT];
    IR_State states[IR_SENSOR_COUNT];
    uint8_t all_sensors;  // 所有传感器的状态组合
} IR_Sensor_TypeDef;

// 函数声明
void IR_Sensor_Init(void);
void IR_Sensor_Read(void);
uint8_t IR_GetSensorState(uint8_t sensor_num);
uint8_t IR_GetAllSensors(void);
uint8_t IR_GetPosition(void);  // 获取小车偏离中心的位置

#endif
