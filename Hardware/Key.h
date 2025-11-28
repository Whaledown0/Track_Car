#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

// 按键定义
#define KEY_COUNT 2
#define KEY1_PIN GPIO_Pin_1
#define KEY2_PIN GPIO_Pin_11
#define KEY_PORT GPIOB

// 按键状态
typedef enum {
    KEY_RELEASED = 0,
    KEY_PRESSED = 1
} KeyState;

// 按键事件
typedef enum {
    KEY_NO_EVENT = 0,
    KEY_SHORT_PRESS,    // 短按
    KEY_LONG_PRESS,     // 长按
    KEY_DOUBLE_CLICK    // 双击
} KeyEvent;

// 按键结构体
typedef struct {
    uint8_t pin;
    KeyState state;
    KeyState last_state;
    uint32_t press_time;
    uint8_t click_count;
    uint8_t event;
} Key_TypeDef;

// 函数声明
void Key_Init(void);
void Key_Scan(void);
uint8_t Key_GetEvent(uint8_t key_num);
uint8_t Key_IsPressed(uint8_t key_num);

#endif
