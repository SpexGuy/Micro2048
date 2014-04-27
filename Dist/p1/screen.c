#include "screen.h"
#include <string.h>

void clearFrame(FrameBuffer *buffer) {
	memset(buffer, 0, sizeof(*buffer));
}

void drawAAPixel(FrameBuffer *draw, uint8_t x, uint8_t xOffset, uint8_t y, uint8_t yOffset, Pixel color) {
	Pixel nwp, nep, swp, sep;
	uint8_t nxOffset, nyOffset, nw, ne, sw, se;
	//calculate AA coefficients
	nxOffset = 255-xOffset;
	nyOffset = 255-yOffset;
	nw = (nxOffset*nyOffset)/255;
	ne = (xOffset*nyOffset)/255;
	sw = (nxOffset*yOffset)/255;
	se = (xOffset*yOffset)/255;
	nwp = multiply(color, nw);
	nep = multiply(color, ne);
	swp = multiply(color, sw);
	sep = multiply(color, se);
	if (validPixel(x, y))
		nAdd(&draw->raster[ y ][ x ], nwp);
	if (validPixel(x+1, y))
		nAdd(&draw->raster[ y ][x+1], nep);
	if (validPixel(x, y+1))
		nAdd(&draw->raster[y+1][ x ], swp);
	if (validPixel(x+1, y+1))
		nAdd(&draw->raster[y+1][x+1], sep);
}

bool validPixel(uint8_t x, uint8_t y) {
	return x < SCREEN_WIDTH && y < SCREEN_HEIGHT;
}
