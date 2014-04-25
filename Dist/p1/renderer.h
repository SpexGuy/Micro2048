#ifndef _RENDERER_H
#define _RENDERER_H 1

#include "screen.h"

void updateRow(int row);
void setCurrentBuffer(FrameBuffer *buffer);
FrameBuffer *getCurrentBuffer(void);

#endif
