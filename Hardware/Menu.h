#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "OLED.h"
#include "Key.h"

// 菜单状态
typedef enum {
    MENU_SPEED_SETTING = 0,   // 速度设置菜单（主菜单）
    MENU_AUTO_RUNNING         // 自动运行模式
} MenuState;

// 速度档位
typedef enum {
    SPEED_STOP = 0,           // 停止
    SPEED_LOW = 1,            // 低速
    SPEED_MEDIUM = 2,         // 中速
    SPEED_HIGH = 3            // 高速
} SpeedLevel;

// 菜单结构体
typedef struct {
    MenuState current_state;
    uint8_t current_selection; // 当前速度选择项 (0-3)
    uint8_t speed_level;       // 当前速度档位
    uint8_t is_auto_running;   // 是否正在自动运行
} Menu_TypeDef;

// 声明外部变量
extern Menu_TypeDef menu;

// 函数声明
void Menu_Init(void);
void Menu_HandleKeyEvent(uint8_t key_num, uint8_t event);
void Menu_UpdateDisplay(void);
void Menu_SetSpeedLevel(uint8_t level);
uint8_t Menu_GetSpeedLevel(void);
uint8_t Menu_IsAutoRunning(void);
void Menu_StartAutoMode(void);
void Menu_StopAutoMode(void);

#endif
