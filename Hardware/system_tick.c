#include "system_tick.h"
#include "stm32f10x.h"

volatile uint32_t system_tick = 0;

void SysTick_Init(void) {
    // 检查时钟源是否就绪
    if((SystemCoreClock == 0) || (SystemCoreClock > 100000000)) {
        while(1); // 系统时钟异常
    }

    // 配置1ms中断
    if(SysTick_Config(SystemCoreClock / 1000)) {
        while(1); // 初始化失败
    }
    
    // 设置中断优先级（必须调用）
    NVIC_SetPriority(SysTick_IRQn, 0);
}

uint32_t Get_SystemTick(void) {
    return system_tick; // 原子访问（32位读是原子的）
}


