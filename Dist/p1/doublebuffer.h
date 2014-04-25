#ifndef _DOUBLEBUFFER_H
#define _DOUBLEBUFFER_H 1

#include "renderer.h"

void initDoubleBuffers(void);
void swapBuffers(void);
FrameBuffer *getDrawBuffer(void);
void clearDrawBuffer(void);

#endif
