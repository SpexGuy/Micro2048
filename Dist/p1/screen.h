#ifndef _SCREEN_H
#define _SCREEN_H 1

#include "pixel.h"

typedef struct {
	Pixel raster[8][8];
} FrameBuffer;

void clearFrame(FrameBuffer *buffer);

#endif
