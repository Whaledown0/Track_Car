#include "ir_sensor.h"

/**
  * @brief  红外传感器初始化
  */
void IR_Sensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = IR_LEFT_PIN | IR_MIDDLE_PIN | IR_RIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  获取原始传感器状态
  */
IRSensorState IR_Sensor_GetRawState(void)
{
    IRSensorState state;
    
    // 读取传感器状态（0表示检测到黑线）
    state.left = GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_LEFT_PIN);
    state.middle = GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_MIDDLE_PIN);
    state.right = GPIO_ReadInputDataBit(IR_GPIO_PORT, IR_RIGHT_PIN);
    
    return state;
}

/**
  * @brief  获取循迹状态
  */
TrackState IR_Sensor_GetTrackState(void)
{
    IRSensorState state = IR_Sensor_GetRawState();
    
    if (state.middle == DETECT_BLACK) {
        // 中间传感器检测到黑线
        if (state.left == DETECT_WHITE && state.right == DETECT_WHITE) {
            return TRACK_ON_LINE;        // 010 - 直行
        }
        else if (state.left == DETECT_BLACK && state.right == DETECT_WHITE) {
            return TRACK_LEFT;           // 110 - 偏左
        }
        else if (state.left == DETECT_WHITE && state.right == DETECT_BLACK) {
            return TRACK_RIGHT;          // 011 - 偏右
        }
        else if (state.left == DETECT_BLACK && state.right == DETECT_BLACK) {
            return TRACK_CROSS;          // 111 - 十字路口
        }
    } else {
        // 中间传感器未检测到黑线
        if (state.left == DETECT_BLACK && state.right == DETECT_WHITE) {
            return TRACK_LEFT_OFF;       // 100 - 左偏出
        }
        else if (state.left == DETECT_WHITE && state.right == DETECT_BLACK) {
            return TRACK_RIGHT_OFF;      // 001 - 右偏出
        }
        else if (state.left == DETECT_WHITE && state.right == DETECT_WHITE) {
            return TRACK_STOP;           // 000 - 停止线（三个传感器都未检测到黑线时停车）
        }
    }
    
    return TRACK_LOST;  // 未知状态
}

/**
  * @brief  检查是否在线上（1为是）
  */
uint8_t IR_Sensor_IsOnLine(void)
{
    IRSensorState state = IR_Sensor_GetRawState();
    return (state.middle == DETECT_BLACK);  
}

/**
  * @brief  检查是否为十字路口（1为是）
  */
uint8_t IR_Sensor_IsCrossroad(void)
{
    IRSensorState state = IR_Sensor_GetRawState();
    return (state.left == DETECT_BLACK && state.middle == DETECT_BLACK && state.right == DETECT_BLACK);
}

/**
  * @brief  检查停止线（1为是）
  */
uint8_t IR_Sensor_IsStopLine(void)
{
    IRSensorState state = IR_Sensor_GetRawState();
    return (state.left == DETECT_WHITE && state.middle == DETECT_WHITE && state.right == DETECT_WHITE);
}

/**
  * @brief  调试打印函数
  */
void IR_Sensor_DebugPrint(void)
{
    IRSensorState state = IR_Sensor_GetRawState();
    // printf("L:%d M:%d R:%d\n", state.left, state.middle, state.right);
}
