#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Menu.h"
#include <string.h>
#include "Encoder.h"

// 定义全局参数变量
int32_t led_speed = 0;
int32_t led_direction = 0;
int32_t kp_value = 0;
int32_t ki_value = 0;
int32_t kd_value = 0;
static uint8_t key_pressed = 0;
static uint32_t key1_press_start_time = 0;
static uint32_t key2_press_start_time = 0;
static uint8_t key1_long_press_active = 0;
static uint8_t key2_long_press_active = 0;

// 移除全局的 editing 变量，改为局部变量

void display_menu(struct option_class* option, uint8_t hasTitle, int8_t displayItems, int8_t currentLine, uint8_t editing) {
    
    if (hasTitle) {
        OLED_ShowString(1, 1, option[0].Name);
        for (int8_t i = 1; i < displayItems; i++) {
            OLED_ShowString(i + 1, 2, option[i].Name);
        }
    } else {
        for (int8_t i = 0; i < displayItems; i++) {
            OLED_ShowString(i + 1, 2, option[i].Name);
        }
    }
    
    // 显示参数值
    for (int8_t i = (hasTitle ? 1 : 0); i < displayItems; i++) {
        int8_t displayLine = hasTitle ? i + 1 : i + 1;
        
        if (strcmp(option[i].Name, "LED_speed") == 0) {
            OLED_ShowNum(displayLine, 14, led_speed, 1);
        }
        else if (strcmp(option[i].Name, "LED_dir") == 0) {
            OLED_ShowNum(displayLine, 14, led_direction, 1);
        }
        else if (strcmp(option[i].Name, "kp") == 0) {
			if (kp_value<0)
			{
				OLED_ShowString(displayLine, 13, "-");
				OLED_ShowNum(displayLine, 14, -kp_value / 10, 1);  // 整数部分
				OLED_ShowString(displayLine, 15, ".");            // 小数点
				OLED_ShowNum(displayLine, 16, -kp_value % 10, 1);  // 小数部分
			}
			else
			{
				OLED_ShowString(displayLine, 13, " ");
				OLED_ShowNum(displayLine, 14, kp_value / 10, 1);  // 整数部分
				OLED_ShowString(displayLine, 15, ".");            // 小数点
				OLED_ShowNum(displayLine, 16, kp_value % 10, 1);  // 小数部分
			}
        }
        else if (strcmp(option[i].Name, "ki") == 0) {
			if (ki_value<0)
			{
				OLED_ShowString(displayLine, 13, "-");
				OLED_ShowNum(displayLine, 14, -ki_value / 10, 1);
				OLED_ShowString(displayLine, 15, ".");
				OLED_ShowNum(displayLine, 16, -ki_value % 10, 1);
			}
			else
			{
				OLED_ShowString(displayLine, 13, " ");
				OLED_ShowNum(displayLine, 14, ki_value / 10, 1);
				OLED_ShowString(displayLine, 15, ".");
				OLED_ShowNum(displayLine, 16, ki_value % 10, 1);
			}
        }
        else if (strcmp(option[i].Name, "kd") == 0) {
			if (kd_value<0)
			{
				OLED_ShowString(displayLine, 13, "-");
				OLED_ShowNum(displayLine, 14, -kd_value / 10, 1);
				OLED_ShowString(displayLine, 15, ".");
				OLED_ShowNum(displayLine, 16, -kd_value % 10, 1);
			}
			else
			{
				OLED_ShowString(displayLine, 13, " ");
				OLED_ShowNum(displayLine, 14, kd_value / 10, 1);
				OLED_ShowString(displayLine, 15, ".");
				OLED_ShowNum(displayLine, 16, kd_value % 10, 1);
			}
        }
    }
    
    OLED_ShowString(currentLine, 1, ">");
    
    if (editing) {
        OLED_ShowString(1, 15, "E");
    }
	else
	{
		OLED_ShowString(1, 15, " ");
	}
}

void Menu_loop(struct option_class* option, uint8_t hasTitle)
{
    int8_t totalItems = 0;
    int8_t displayItems = 0;
    int8_t currentLine = 0;
    int8_t firstItemLine = 0;
    uint8_t editing = 0;  // 改为局部变量
	int8_t change_Encoder;
	
	Key_Tick();
	Update_LED_Params(led_speed,led_direction);
	LED_Tick();
    // 计算总项数
    while (option[totalItems].Name[0] != ' ' && option[totalItems].Name[0] != '\0') {
        totalItems++;
    }
    displayItems = totalItems;

    // 设置起始行
    if (hasTitle) {
        currentLine = 2;
        firstItemLine = 2;
    } else {
        currentLine = 1;
        firstItemLine = 1;
    }

    // 初始显示
    display_menu(option, hasTitle, displayItems, currentLine, editing);
    
    while (1)
    {
		Key_Tick();
		Update_LED_Params(led_speed,led_direction);
		LED_Tick();
        if (!editing) {
            // 正常模式
            if (Key_Check(KEY_1,KEY_DOWN)) { // 上移
                OLED_ShowString(currentLine, 1, " ");
                currentLine--;
                if (currentLine < firstItemLine) {
                    currentLine = firstItemLine + displayItems - (hasTitle ? 2 : 1);
                }
                OLED_ShowString(currentLine, 1, ">");
            }    
        
            if (Key_Check(KEY_2,KEY_DOWN)) { // 下移
                OLED_ShowString(currentLine, 1, " ");
                currentLine++;
                if (currentLine > firstItemLine + displayItems - (hasTitle ? 2 : 1)) {
                    currentLine = firstItemLine;
                }
                OLED_ShowString(currentLine, 1, ">");
            }
        
            if (Key_Check(KEY_3,KEY_DOWN)) { // 确认
				OLED_Clear();
                if (!hasTitle) {
                    int8_t arrayIndex = currentLine - firstItemLine;
                    if (arrayIndex < displayItems && option[arrayIndex].func != NULL) {
                        option[arrayIndex].func();
                        // 从子菜单返回后刷新显示
                        display_menu(option, hasTitle, displayItems, currentLine, editing);
                    }
                } else {
                    editing = 1;
                    display_menu(option, hasTitle, displayItems, currentLine, editing);
                }
				
            }
        
            if (Key_Check(KEY_4,KEY_DOWN)) { // 退出
				OLED_Clear();
                break;
            }
        } else {
            // 编辑模式
            int8_t arrayIndex = currentLine - (hasTitle ? 1 : 0);
            change_Encoder = Encoder_Get();
			
            if (Key_Check(KEY_1,KEY_DOWN)) {  // 增加数值
                if (strcmp(option[arrayIndex].Name, "LED_speed") == 0) {
                    led_speed = (led_speed + 1) % 3;
                }
                else if (strcmp(option[arrayIndex].Name, "LED_dir") == 0) {
                    led_direction = !led_direction;
                }
				Update_LED_Params(led_speed, led_direction);
				display_menu(option, hasTitle, displayItems, currentLine, editing);
            }
			else if (Key_Check(KEY_1,KEY_SINGLE)) {  
				if (strcmp(option[arrayIndex].Name, "kp") == 0) {
                    kp_value--;
                }
                else if (strcmp(option[arrayIndex].Name, "ki") == 0) {
                    ki_value--;
                }
                else if (strcmp(option[arrayIndex].Name, "kd") == 0) {
                    kd_value--;
                }
				display_menu(option, hasTitle, displayItems, currentLine, editing);
            }
            else if (Key_Check(KEY_2,KEY_DOWN)) {  // 减少数值
                if (strcmp(option[arrayIndex].Name, "LED_speed") == 0) {
                    led_speed = (led_speed - 1 + 3) % 3;
                }
                else if (strcmp(option[arrayIndex].Name, "LED_dir") == 0) {
                    led_direction = !led_direction;
                }
				Update_LED_Params(led_speed, led_direction);
				display_menu(option, hasTitle, displayItems, currentLine, editing);
            }
			else if (Key_Check(KEY_2,KEY_SINGLE)) {  
				if (strcmp(option[arrayIndex].Name, "kp") == 0) {
                    kp_value--;
                }
                else if (strcmp(option[arrayIndex].Name, "ki") == 0) {
                    ki_value--;
                }
                else if (strcmp(option[arrayIndex].Name, "kd") == 0) {
                    kd_value--;
                }
				display_menu(option, hasTitle, displayItems, currentLine, editing);
            }
			else if (Key_GetState(KEY_1) == 1) {
				uint32_t press_duration = Get_SystemTick() - key1_press_start_time;
				if (press_duration > 1000) {  // 长按超过1000ms
					key1_long_press_active = 1;
					static uint32_t last_increment_time = 0;
					if (Get_SystemTick() - last_increment_time >= 100) {  // 每100ms增加1
						last_increment_time = Get_SystemTick();
						if (strcmp(option[arrayIndex].Name, "kp") == 0) {
							kp_value++;
						}
						else if (strcmp(option[arrayIndex].Name, "ki") == 0) {
							ki_value++;
						}
						else if (strcmp(option[arrayIndex].Name, "kd") == 0) {
							kd_value++;
						}
						Update_LED_Params(led_speed, led_direction);
						display_menu(option, hasTitle, displayItems, currentLine, editing);
					}
				}
			}
			else if (Key_GetState(KEY_2) == 1) {
				uint32_t press_duration = Get_SystemTick() - key2_press_start_time;
				if (press_duration > 1000) {  // 长按超过1000ms
					key2_long_press_active = 1;
					static uint32_t last_decrement_time = 0;
					if (Get_SystemTick() - last_decrement_time >= 100) {  // 每100ms减少1
						last_decrement_time = Get_SystemTick();
						if (strcmp(option[arrayIndex].Name, "kp") == 0) {
							kp_value--;
						}
						else if (strcmp(option[arrayIndex].Name, "ki") == 0) {
							ki_value--;
						}
						else if (strcmp(option[arrayIndex].Name, "kd") == 0) {
							kd_value--;
						}
						Update_LED_Params(led_speed, led_direction);
						display_menu(option, hasTitle, displayItems, currentLine, editing);
					}
				}
			}
            else if (Key_Check(KEY_4,KEY_DOWN)) {  // 退出编辑模式
				editing = 0;
				display_menu(option, hasTitle, displayItems, currentLine, editing);
			}
			else if (change_Encoder != 0) {  // 直接检查是否有编码器变化
				
					if (strcmp(option[arrayIndex].Name, "kp") == 0) {
						kp_value += change_Encoder; // 直接使用累计变化量
						if (kp_value > 99) kp_value = 99;
					}
					else if (strcmp(option[arrayIndex].Name, "ki") == 0) {
						ki_value += change_Encoder;
						if (ki_value > 99) ki_value = 99;
					}
					else if (strcmp(option[arrayIndex].Name, "kd") == 0) {
						kd_value += change_Encoder;
						if (kd_value > 99) kd_value = 99;
					}
					display_menu(option, hasTitle, displayItems, currentLine, editing);
			}
		}
	}
}

// 修改其他菜单函数，确保从子菜单返回后能正常显示
void LED_Speed(void){
    // 这里可以添加三级菜单逻辑，或者保持为空
}
void LED_Dir(void){

}
	
void LED_Control(void)
{
    struct option_class option[]=
    {
        {"LED Control", NULL},
        {"LED_speed", LED_Speed},
        {"LED_dir", LED_Dir},
        {" ", NULL}
    };
    Menu_loop(option, 1);
}

void PID_Kp(void) {}
void PID_Ki(void) {}
void PID_Kd(void) {}

void PID(void)
{
    struct option_class option[]=
    {
        {"PID Settings", NULL},
        {"kp", PID_Kp},
        {"ki", PID_Ki},
        {"kd", PID_Kd},
        {" ", NULL}
    };
    Menu_loop(option, 1);
}

void Image(void)
{
    struct option_class option[]=
    {
        {"Image", NULL},
        {"Image", NULL},
        {" ", NULL}
    };
    Menu_loop(option, 1);
}

void Angle(void)
{
    struct option_class option[]=
    {
        {"Angle", NULL},
        {"Angle", NULL},
        {" ", NULL}
    };
    Menu_loop(option, 1);
}

void Main_Menu(void)
{
    struct option_class option[]=
    {
        {"LED_Control", LED_Control},
        {"PID", PID},
        {"Image", Image},
        {"Angle", Angle},
        {" ", NULL}
    };
    Menu_loop(option, 0);
}
