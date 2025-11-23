#include "ir_sensor.h"

static IR_Sensor_TypeDef ir_sensor;

// 初始化红外传感器
void IR_Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = IR1_PIN | IR2_PIN | IR3_PIN | IR4_PIN | IR5_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_PORT, &GPIO_InitStructure);
    
    // 初始化传感器引脚数组
    ir_sensor.pins[0] = IR1_PIN;
    ir_sensor.pins[1] = IR2_PIN;
    ir_sensor.pins[2] = IR3_PIN;
    ir_sensor.pins[3] = IR4_PIN;
    ir_sensor.pins[4] = IR5_PIN;
    
    // 读取初始状态
    IR_Sensor_Read();
}

// 读取所有传感器状态
void IR_Sensor_Read(void)
{
    ir_sensor.all_sensors = 0;
    
    for(int i = 0; i < IR_SENSOR_COUNT; i++) {
        // 读取传感器状态（假设低电平表示检测到黑线）
        ir_sensor.states[i] = GPIO_ReadInputDataBit(IR_PORT, ir_sensor.pins[i]) ? IR_WHITE : IR_BLACK;
        
        // 组合所有传感器状态
        if(ir_sensor.states[i] == IR_BLACK) {
            ir_sensor.all_sensors |= (1 << i);
        }
    }
}

// 获取单个传感器状态
uint8_t IR_GetSensorState(uint8_t sensor_num)
{
    if(sensor_num >= IR_SENSOR_COUNT) return IR_WHITE;
    return ir_sensor.states[sensor_num];
}

// 获取所有传感器状态组合
uint8_t IR_GetAllSensors(void)
{
    return ir_sensor.all_sensors;
}

// 获取小车位置（用于PID控制）
uint8_t IR_GetPosition(void)
{
    // 根据传感器状态计算偏离中心的位置
    // 返回值范围：0-4，2为中心位置
    // 将二进制改为十六进制表示
    if(ir_sensor.all_sensors == 0x04) return 2;  // 完全居中 (00000100)
    if(ir_sensor.all_sensors == 0x02) return 1;  // 偏右 (00000010)
    if(ir_sensor.all_sensors == 0x01) return 0;  // 严重偏右 (00000001)
    if(ir_sensor.all_sensors == 0x08) return 3;  // 偏左 (00001000)
    if(ir_sensor.all_sensors == 0x10) return 4;  // 严重偏左 (00010000)
    
    // 多传感器同时检测到的情况
    if(ir_sensor.all_sensors & 0x06) return 1;    // 1-2传感器 (00000110)
    if(ir_sensor.all_sensors & 0x03) return 0;    // 0-1传感器 (00000011)
    if(ir_sensor.all_sensors & 0x0C) return 2;    // 2-3传感器 (00001100)
    if(ir_sensor.all_sensors & 0x18) return 3;    // 3-4传感器 (00011000)
    if(ir_sensor.all_sensors & 0x1C) return 2;    // 2-3-4传感器 (00011100)
    if(ir_sensor.all_sensors & 0x07) return 1;    // 0-1-2传感器 (00000111)
    
    return 2; // 默认返回中心
}
