#include "screen.h"
#include <string.h>

void clearFrame(FrameBuffer *buffer) {
	memset(buffer, 0, sizeof(*buffer));
}

void drawAACorners(	FrameBuffer *draw,
										uint8_t x, byteFraction xOffset, uint8_t xSpan,
										uint8_t y, byteFraction yOffset, uint8_t ySpan,
										Pixel color)
{
	Pixel nwp, nep, swp, sep;
	byteFraction nxOffset, nyOffset, nw, ne, sw, se;
	//calculate AA coefficients
	nxOffset = bcomp(xOffset);
	nyOffset = bcomp(yOffset);
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
		percentAdd(&draw->raster[ y ][ x ], nwp, nw);
	if (validPixel(x+xSpan, y))
		percentAdd(&draw->raster[ y ][x+xSpan], nep, ne);
	if (validPixel(x, y+ySpan))
		percentAdd(&draw->raster[y+ySpan][ x ], swp, sw);
	if (validPixel(x+xSpan, y+ySpan))
		percentAdd(&draw->raster[y+ySpan][x+xSpan], sep, se);
}

void drawAAPixel(	FrameBuffer *draw,
									uint8_t x, byteFraction xOffset,
									uint8_t y, byteFraction yOffset,
									Pixel color)
{
	drawAACorners(draw, x, xOffset, 1, y, yOffset, 1, color);
}

void drawAAEdges(	FrameBuffer *draw,
									uint8_t x, byteFraction e, uint8_t width,
									uint8_t y, byteFraction s, uint8_t height,
									Pixel color)
{
	uint8_t xo, yo;
	byteFraction n, w;
	Pixel np, ep, wp, sp;
	//calculate coefficients
	n = bcomp(s);
	w = bcomp(e);
	//calculate pixels
	np = multiply(color, n);
	ep = multiply(color, e);
	sp = multiply(color, s);
	wp = multiply(color, w);
	//draw y edges
	for (xo = x+1; xo < x+width && xo < SCREEN_WIDTH; xo++) {
		percentAdd(&draw->raster[y][xo], np, n);
		if (y+height < SCREEN_HEIGHT)
			percentAdd(&draw->raster[y+height][xo], sp, s);
	}
	//draw x edges
	for (yo = y+1; yo < y+height && yo < SCREEN_HEIGHT; yo++) {
		percentAdd(&draw->raster[yo][x], wp, w);
		if (x+width < SCREEN_WIDTH)
			percentAdd(&draw->raster[yo][x+width], ep, e);
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
			percentAdd(&draw->raster[yo][xo], color, BF_1);
		}
	}
}

void drawAARect(FrameBuffer *draw,
								uint8_t x, byteFraction e, uint8_t width,
								uint8_t y, byteFraction s, uint8_t height,
								Pixel color)
{
	//draw center
	drawRect(draw, x+1, width-1, y+1, height-1, color);
	//draw edges
	drawAAEdges(draw, x, e, width, y, s, height, color);
	//draw corners
	drawAACorners(draw, x, e, width, y, s, height, color);
}
