#include "Menu.h"
#include "Delay.h"
#include <string.h>

// 菜单显示文本
char speed_menu_items[][15] = {
    "0",
    "1", 
    "2",
    "3"
};

Menu_TypeDef menu;

// 初始化菜单
void Menu_Init(void)
{
    menu.current_state = MENU_SPEED_SETTING;
    menu.current_selection = 0;
    menu.speed_level = SPEED_STOP;
    menu.is_auto_running = 0;
    
    Menu_UpdateDisplay();
}

void Menu_HandleKeyEvent(uint8_t key_num, uint8_t event)
{
    switch(menu.current_state) {
        case MENU_SPEED_SETTING:
            if(key_num == 0 && event == KEY_SHORT_PRESS) { // KEY1: 选择速度档位
                menu.current_selection = (menu.current_selection + 1) % 4;
                Menu_UpdateDisplay();
            }
            else if(key_num == 1 && event == KEY_SHORT_PRESS) { // KEY2: 启动自动运行
                if(menu.current_selection == SPEED_STOP) {
                    // 如果选择停止，则不启动
                    return;
                }
                menu.speed_level = menu.current_selection;
                menu.is_auto_running = 1;
                menu.current_state = MENU_AUTO_RUNNING;
                Menu_UpdateDisplay();
                
                
            }
            break;
            
        case MENU_AUTO_RUNNING:
            if(key_num == 1 && event == KEY_SHORT_PRESS) { // KEY2: 停止自动运行
                Menu_StopAutoMode();
            }
            else if(key_num == 0 && event == KEY_LONG_PRESS) { // KEY1长按：返回速度设置
                Menu_StopAutoMode();
            }
            break;
    }
}

// 更新显示
void Menu_UpdateDisplay(void)
{
    OLED_Clear();
    
    switch(menu.current_state) {
        case MENU_SPEED_SETTING:
            
            for(int i = 0; i < 4; i++) {
                if(i == menu.current_selection) {
                    OLED_ShowString(i+1, 1, ">");
                    OLED_ShowString(i+1, 2, speed_menu_items[i]);
                } else {
                    OLED_ShowString(i+1, 1, " ");
                    OLED_ShowString(i+1, 2, speed_menu_items[i]);
                }
            }
            
            break;
            
        case MENU_AUTO_RUNNING:
            OLED_ShowString(1, 1, "Auto Track");
            OLED_ShowString(2, 1, "RUNNING");
            
            OLED_ShowNum(3, 1, menu.current_selection , 2);
            
            break;
    }
}

// 设置速度档位
void Menu_SetSpeedLevel(uint8_t level)
{
    if(level <= SPEED_HIGH) {
        menu.speed_level = level;
        menu.current_selection = level;
    }
}

// 获取速度档位
uint8_t Menu_GetSpeedLevel(void)
{
    return menu.speed_level;
}

// 是否正在自动运行
uint8_t Menu_IsAutoRunning(void)
{
    return menu.is_auto_running;
}

// 启动自动模式
void Menu_StartAutoMode(void)
{
    if(menu.speed_level != SPEED_STOP) {
        menu.is_auto_running = 1;
        menu.current_state = MENU_AUTO_RUNNING;
        Menu_UpdateDisplay();
    }
}

// 停止自动模式
void Menu_StopAutoMode(void)
{
    menu.is_auto_running = 0;
    menu.current_state = MENU_SPEED_SETTING;
    Menu_UpdateDisplay();
}
