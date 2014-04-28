#ifndef _SCREEN_H
#define _SCREEN_H 1

#include "pixel.h"
#include <stdbool.h>

typedef struct {
	Pixel raster[SCREEN_HEIGHT][SCREEN_WIDTH];
} FrameBuffer;

void clearFrame(FrameBuffer *buffer);

void drawAAPixel(	FrameBuffer *draw,
									uint8_t x, uint8_t xOffset,
									uint8_t y, uint8_t yOffset,
									Pixel color);

void drawAARect(FrameBuffer *draw,
								uint8_t x, uint8_t xOffset, uint8_t width,
								uint8_t y, uint8_t yOffset, uint8_t height,
								Pixel color);

void drawRect(FrameBuffer *draw,
							uint8_t x, uint8_t width,
							uint8_t y, uint8_t height,
							Pixel color);


#define validPixel(x, y) ((x) < SCREEN_WIDTH && (y) < SCREEN_HEIGHT)

#endif
