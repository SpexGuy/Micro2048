#include "pixel.h"

Pixel multiply(Pixel pix, uint8_t fraction) {
	nMultiply(&pix, fraction);
	return pix;
}

void nMultiply(Pixel *pix, uint8_t fraction) {
	pix->color.r = (pix->color.r * fraction)/255;
	pix->color.g = (pix->color.g * fraction)/255;
	pix->color.b = (pix->color.b * fraction)/255;
}

Pixel add(Pixel pix, Pixel addition) {
	nAdd(&pix, addition);
	return pix;
}

void nAdd(Pixel *pix, Pixel addition) {
	pix->color.r += addition.color.r;
	pix->color.g += addition.color.g;
	pix->color.b += addition.color.b;
}

uint8_t bmod(uint32_t a, uint32_t b) {
	return (256*a)/b;
}
