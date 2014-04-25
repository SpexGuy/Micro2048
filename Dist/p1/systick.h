#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>

void SYSTICKIntHandler(void);
void initializeSysTick(uint32_t count, bool enableInterrupts);

#endif
