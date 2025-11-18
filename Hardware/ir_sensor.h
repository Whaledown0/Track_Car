#ifndef __IR_SENSOR_H
#define __IR_SENSOR_H

#include "stm32f10x.h"

#define IR_LEFT_PIN     GPIO_Pin_11
#define IR_MIDDLE_PIN   GPIO_Pin_12
#define IR_RIGHT_PIN    GPIO_Pin_15
#define IR_GPIO_PORT    GPIOA

#define DETECT_BLACK    0  
#define DETECT_WHITE    1

typedef enum {
    TRACK_LOST = 0,   
    TRACK_ON_LINE,      
    TRACK_LEFT,      
    TRACK_RIGHT,     
    TRACK_LEFT_OFF,    
    TRACK_RIGHT_OFF,    
    TRACK_CROSS,        
    TRACK_STOP         
} TrackState;

typedef struct {
    uint8_t left;
    uint8_t middle;
    uint8_t right;
} IRSensorState;

void IR_Sensor_Init(void);
TrackState IR_Sensor_GetTrackState(void);
IRSensorState IR_Sensor_GetRawState(void);
uint8_t IR_Sensor_IsOnLine(void);
uint8_t IR_Sensor_IsCrossroad(void);
uint8_t IR_Sensor_IsStopLine(void);
void IR_Sensor_DebugPrint(void);

#endif /* __IR_SENSOR_H */
