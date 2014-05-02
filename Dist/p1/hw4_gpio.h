#ifndef __HW4_GPIO_H__
#define __HW4_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define BUTTON_NORTH 2
#define BUTTON_SOUTH 0
#define BUTTON_EAST 3
#define BUTTON_WEST 1


void initializeGpioPins(void);
void updateDisplay(void);
void updateButtons(void);
uint8_t getButton(uint8_t index);


#endif
