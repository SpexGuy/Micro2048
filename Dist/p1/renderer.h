#ifndef _RENDERER_H
#define _RENDERER_H 1

#include "screen.h"

#define MAX_BRIGHTNESS 256

void updateRow(int row, int duty);
void setCurrentBuffer(FrameBuffer *buffer);
FrameBuffer *getCurrentBuffer(void);
void setBrightness(uint32_t bright);
uint32_t getBrightness(void);

#endif
