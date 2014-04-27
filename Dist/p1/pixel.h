#ifndef _PIXEL_H
#define _PIXEL_H 1

#include "constants.h"
#include <stdint.h>

typedef union {
	uint32_t hex;
	struct {
#		ifdef _LITTLE_ENDIAN
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t _;
#		else
			uint8_t _;
			uint8_t r;
			uint8_t g;
			uint8_t b;
#		endif
	} color;
} Pixel;

Pixel multiply(Pixel pix, uint8_t fraction);
void nMultiply(Pixel *pix, uint8_t fraction);
Pixel add(Pixel pix, Pixel addition);
void nAdd(Pixel *pix, Pixel addition);

uint8_t bmod(uint32_t a, uint32_t b);

#endif
