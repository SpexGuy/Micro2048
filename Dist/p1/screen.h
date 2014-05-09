#ifndef _SCREEN_H
#define _SCREEN_H 1

#include "pixel.h"
#include <stdbool.h>

/**
 * Framebuffer - an array to store pixel information for the screen
 */
typedef struct {
	Pixel raster[SCREEN_HEIGHT][SCREEN_WIDTH];
} FrameBuffer;

/**
 * completely zeros the given buffer
 */
void clearFrame(FrameBuffer *buffer);

/**
 * Draws a single 1x1 pixel sprite of the given color
 * at the given position and subpixel offset
 */
void drawAAPixel(	FrameBuffer *draw,
									uint8_t x, byteFraction xOffset,
									uint8_t y, byteFraction yOffset,
									Pixel color);

/**
 * Draws a rectangle with anti-aliased edges
 */
void drawAARect(FrameBuffer *draw,
								uint8_t x, byteFraction xOffset, uint8_t width,
								uint8_t y, byteFraction yOffset, uint8_t height,
								Pixel color);

/**
 * Draws a rectangle at the given position in the given color
 */
void drawRect(FrameBuffer *draw,
							uint8_t x, uint8_t width,
							uint8_t y, uint8_t height,
							Pixel color);


/** macro to determine if a pixel is on the screen */
#define validPixel(x, y) ((x) < SCREEN_WIDTH && (y) < SCREEN_HEIGHT)

#endif
