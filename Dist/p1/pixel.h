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
			uint8_t p;
#		else
			uint8_t p;
			uint8_t r;
			uint8_t g;
			uint8_t b;
#		endif
	} color;
} Pixel;

Pixel mix(Pixel a, Pixel b, byteFraction z);
void nMix(Pixel *a, Pixel b, byteFraction z);
Pixel complement(Pixel pix);
void nComplement(Pixel *pix);
Pixel multiply(Pixel pix, byteFraction fraction);
void nMultiply(Pixel *pix, byteFraction fraction);
Pixel add(Pixel pix, Pixel addition);
void nAdd(Pixel *pix, Pixel addition);
void percentAdd(Pixel *pix, Pixel addition, byteFraction percent);
void setColor(Pixel *pix, Pixel value);

#endif
