#ifndef _RENDERER_H
#define _RENDERER_H 1

#include "screen.h"

void updateRow(uint8_t row, byteFraction duty);
void setCurrentBuffer(FrameBuffer *buffer);
FrameBuffer *getCurrentBuffer(void);
void setBrightness(uint32_t bright);
uint32_t getBrightness(void);

#define MAX_BRIGHTNESS 256

#endif
