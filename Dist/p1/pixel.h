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


#endif
