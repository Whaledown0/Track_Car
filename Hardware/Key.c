// Key.c

#include "Key.h"
#include "Delay.h" // 假设你有一个精准的 delay_ms 函数

// 定义按键的物理信息
// 注意：这里的顺序必须与你在 main 函数中调用 Key_GetEvent 的顺序一致
static const uint16_t KEY_PINS[KEY_COUNT] = {KEY1_PIN, KEY2_PIN};

// 定义按键判断的时长阈值 (单位：毫秒)
#define DEBOUNCE_TIME      20   // 消抖时间
#define LONG_PRESS_TIME    1000 // 长按判定时间
#define DOUBLE_CLICK_TIME  300  // 双击判定间隔

// 全局静态按键数组，每个按键对应一个结构体
static Key_TypeDef keys[KEY_COUNT];

// ========================================
// 新增：一个简单的软件定时器变量 (1ms基准)
// 这个变量需要在你的 SysTick 中断中递增
volatile uint32_t systick_ms = 0; 

// 你需要在 main.c 中实现 SysTick 中断
// 例如：
// void SysTick_Handler(void) {
//     systick_ms++;
// }
// ========================================


/**
  * @brief  初始化按键
  * @param  无
  * @retval 无
  */
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 使能 GPIOB 时钟 (根据你的 KEY_PORT 定义)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 配置按键引脚为上拉输入
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);

    // 初始化所有按键的状态
    for (int i = 0; i < KEY_COUNT; i++) {
        keys[i].state = KEY_RELEASED;
        keys[i].last_state = KEY_RELEASED;
        keys[i].press_time = 0;
        keys[i].click_count = 0;
        keys[i].event = KEY_NO_EVENT;
    }
}

/**
  * @brief  按键扫描函数，应在主循环中被频繁调用
  * @param  无
  * @retval 无
  */
void Key_Scan(void)
{
    uint32_t current_time = systick_ms; // 获取当前精确时间

    for (int i = 0; i < KEY_COUNT; i++) {
        // 1. 读取当前物理状态
        // GPIO是上拉输入，按键按下时为低电平(0)，读取为RESET，对应KEY_PRESSED
        uint8_t current_hw_state = (GPIO_ReadInputDataBit(KEY_PORT, KEY_PINS[i]) == Bit_RESET) ? KEY_PRESSED : KEY_RELEASED;

        // 2. 保存上一次的逻辑状态
        keys[i].last_state = keys[i].state;
        
        // 3. 更新当前逻辑状态
        keys[i].state = current_hw_state;

        // 4. 状态机判断
        // 4.1 检测到从释放变为按下（下降沿）
        if (keys[i].state == KEY_PRESSED && keys[i].last_state == KEY_RELEASED) {
            keys[i].press_time = current_time; // 记录按下时刻
            keys[i].click_count++;
        }
        // 4.2 检测到从按下变为释放（上升沿）
        else if (keys[i].state == KEY_RELEASED && keys[i].last_state == KEY_PRESSED) {
            uint32_t press_duration = current_time - keys[i].press_time;

            // 按键释放后，判断事件
            if (keys[i].click_count == 1) {
                if (press_duration > LONG_PRESS_TIME) {
                    keys[i].event = KEY_LONG_PRESS;
                } else {
                    keys[i].event = KEY_SHORT_PRESS;
                }
            } else if (keys[i].click_count >= 2) {
                keys[i].event = KEY_DOUBLE_CLICK;
            }
            keys[i].click_count = 0; // 重置点击计数
        }

        // 4.3 处理单击的超时确认
        // 如果点击次数为1，且超过了双击间隔时间，则确认此次点击为一次单击
        if (keys[i].click_count == 1 && (current_time - keys[i].press_time) > DOUBLE_CLICK_TIME) {
            // 必须在按键释放状态下才能触发短按事件，避免长按干扰
            if(keys[i].state == KEY_RELEASED) {
                 keys[i].event = KEY_SHORT_PRESS;
                 keys[i].click_count = 0; // 重置，防止重复触发
            }
        }
    }
}

/**
  * @brief  获取按键事件
  * @param  key_num: 按键编号 (0 代表 KEY1, 1 代表 KEY2)
  * @retval 按键事件类型
  */
uint8_t Key_GetEvent(uint8_t key_num)
{
    if (key_num >= KEY_COUNT) {
        return KEY_NO_EVENT;
    }

    uint8_t event = keys[key_num].event;
    
    // 事件获取后立即清除，确保每个事件只被处理一次
    if (event != KEY_NO_EVENT) {
        keys[key_num].event = KEY_NO_EVENT;
    }
    
    return event;
}

/**
  * @brief  检测按键是否处于物理按下状态
  * @param  key_num: 按键编号
  * @retval 1: 按下, 0: 释放
  */
uint8_t Key_IsPressed(uint8_t key_num)
{
    if (key_num >= KEY_COUNT) {
        return 0; // 0 表示 false
    }
    return (keys[key_num].state == KEY_PRESSED);
}
