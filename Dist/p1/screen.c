#include "screen.h"
#include <string.h>

void clearFrame(FrameBuffer *buffer) {
	memset(buffer, 0, sizeof(*buffer));
}

void drawAACorners(	FrameBuffer *draw,
										uint8_t x, uint8_t xOffset, uint8_t xSpan,
										uint8_t y, uint8_t yOffset, uint8_t ySpan,
										Pixel color)
{
	Pixel nwp, nep, swp, sep;
	uint8_t nxOffset, nyOffset, nw, ne, sw, se;
	//calculate AA coefficients
	nxOffset = 255-xOffset;
	nyOffset = 255-yOffset;
	nw = (nxOffset*nyOffset)/255;
	ne = (xOffset*nyOffset)/255;
	sw = (nxOffset*yOffset)/255;
	se = (xOffset*yOffset)/255;
	//calculate AA colors
	nwp = multiply(color, nw);
	nep = multiply(color, ne);
	swp = multiply(color, sw);
	sep = multiply(color, se);
	//add to pixels
	if (validPixel(x, y))
		nAdd(&draw->raster[ y ][ x ], nwp);
	if (validPixel(x+xSpan, y))
		nAdd(&draw->raster[ y ][x+xSpan], nep);
	if (validPixel(x, y+ySpan))
		nAdd(&draw->raster[y+ySpan][ x ], swp);
	if (validPixel(x+xSpan, y+ySpan))
		nAdd(&draw->raster[y+ySpan][x+xSpan], sep);
}

void drawAAPixel(	FrameBuffer *draw,
									uint8_t x, uint8_t xOffset,
									uint8_t y, uint8_t yOffset,
									Pixel color)
{
	drawAACorners(draw, x, xOffset, 1, y, yOffset, 1, color);
}

void drawAAEdges(	FrameBuffer *draw,
									uint8_t x, uint8_t e, uint8_t width,
									uint8_t y, uint8_t s, uint8_t height,
									Pixel color)
{
	uint8_t xo, yo, n, w;
	Pixel np, ep, wp, sp;
	//calculate coefficients
	n = 255-s;
	w = 255-e;
	//calculate pixels
	np = multiply(color, n);
	ep = multiply(color, e);
	sp = multiply(color, s);
	wp = multiply(color, w);
	//draw y edges
	for (xo = x+1; xo < x+width && xo < SCREEN_WIDTH; xo++) {
		nAdd(&draw->raster[y][xo], np);
		if (y+height < SCREEN_HEIGHT)
			nAdd(&draw->raster[y+height][xo], sp);
	}
	//draw x edges
	for (yo = y+1; yo < y+height && yo < SCREEN_HEIGHT; yo++) {
		nAdd(&draw->raster[yo][x], wp);
		if (x+width < SCREEN_WIDTH)
			nAdd(&draw->raster[yo][x+width], ep);
	}
}

void drawRect(FrameBuffer *draw,
							uint8_t x, uint8_t width,
							uint8_t y, uint8_t height,
							Pixel color)
{
	uint8_t xo, yo;
	if (!validPixel(x, y)) return;
	for (xo = x; xo < x+width && xo < SCREEN_WIDTH; xo++) {
		for (yo = y; yo < y+height && yo < SCREEN_HEIGHT; yo++) {
			nAdd(&draw->raster[yo][xo], color);
		}
	}
}

void drawAARect(FrameBuffer *draw,
								uint8_t x, uint8_t e, uint8_t width,
								uint8_t y, uint8_t s, uint8_t height,
								Pixel color)
{
	//draw center
	drawRect(draw, x+1, width-1, y+1, height-1, color);
	//draw edges
	drawAAEdges(draw, x, e, width, y, s, height, color);
	//draw corners
	drawAACorners(draw, x, e, width, y, s, height, color);
}
