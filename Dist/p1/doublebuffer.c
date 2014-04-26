#include "doublebuffer.h"
#include <string.h> //for memset

FrameBuffer buffers[2];
uint8_t draw = 0;

void initDoubleBuffers() {
	memset(buffers, 0, sizeof(buffers));
	swapBuffers();
}

void swapBuffers() {
	setCurrentBuffer(getDrawBuffer());
	draw = 1-draw;
}

FrameBuffer *getDrawBuffer() {
	return &buffers[draw];
}

void clearDrawBuffer() {
	clearFrame(getDrawBuffer());
}
