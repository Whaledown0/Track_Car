#ifndef __SYSTEM_TICK_H
#define __SYSTEM_TICK_H

#include <stdint.h>

void SysTick_Init(void);
uint32_t Get_SystemTick(void);

#endif
