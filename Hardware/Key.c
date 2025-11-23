#include "Key.h"
#include "Delay.h"

static Key_TypeDef keys[KEY_COUNT];

// 初始化按键
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
    
    // 初始化按键状态
    for(int i = 0; i < KEY_COUNT; i++) {
        keys[i].state = KEY_RELEASED;
        keys[i].last_state = KEY_RELEASED;
        keys[i].press_time = 0;
        keys[i].click_count = 0;
        keys[i].event = KEY_NO_EVENT;
    }
}

// 按键扫描（非阻塞）
void Key_Scan(void)
{
    static uint32_t last_scan_time = 0;
    uint32_t current_time = 0; // 你需要实现一个简单的毫秒计数器
    
    // 简单的时间基准（如果没有系统滴答定时器）
    static uint32_t tick = 0;
    tick++;
    current_time = tick;
    
    uint8_t key_pins[] = {KEY1_PIN, KEY2_PIN};
    
    for(int i = 0; i < KEY_COUNT; i++) {
        keys[i].last_state = keys[i].state;
        keys[i].state = GPIO_ReadInputDataBit(KEY_PORT, key_pins[i]);
        
        if(keys[i].state == KEY_PRESSED && keys[i].last_state == KEY_RELEASED) {
            // 按键按下
            keys[i].press_time = current_time;
            keys[i].click_count++;
        }
        else if(keys[i].state == KEY_RELEASED && keys[i].last_state == KEY_PRESSED) {
            // 按键释放
            uint32_t press_duration = current_time - keys[i].press_time;
            
            if(press_duration > 20 && press_duration < 1000) { // 短按
                keys[i].event = KEY_SHORT_PRESS;
            }
            else if(press_duration >= 1000) { // 长按
                keys[i].event = KEY_LONG_PRESS;
            }
            
            // 双击检测（简单实现）
            if(keys[i].click_count >= 2) {
                keys[i].event = KEY_DOUBLE_CLICK;
                keys[i].click_count = 0;
            }
        }
        
        // 单击确认（如果超时没有双击）
        if(keys[i].click_count == 1 && (current_time - keys[i].press_time) > 300) {
            if((current_time - keys[i].press_time) < 1000) {
                keys[i].event = KEY_SHORT_PRESS;
            }
            keys[i].click_count = 0;
        }
    }
}

// 获取按键事件
uint8_t Key_GetEvent(uint8_t key_num)
{
    if(key_num >= KEY_COUNT) return KEY_NO_EVENT;
    
    uint8_t event = keys[key_num].event;
    keys[key_num].event = KEY_NO_EVENT; // 清除事件
    return event;
}

// 检测按键是否按下
uint8_t Key_IsPressed(uint8_t key_num)
{
    if(key_num >= KEY_COUNT) return 0;
    return (keys[key_num].state == KEY_PRESSED);
}
