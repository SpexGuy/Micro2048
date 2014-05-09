#ifndef __HW4_GPIO_H__
#define __HW4_GPIO_H__

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

#define BUTTON_NORTH 2
#define BUTTON_SOUTH 0
#define BUTTON_EAST 3
#define BUTTON_WEST 1
#define BUTTON_AUX 4


void initializeGpioPins(void);
void updateDisplay(void);
void updateButtons(void);
void clearButtons(void);
uint8_t getButtonPress(uint8_t index);
uint8_t getButtonRelease(uint8_t index);

#endif
