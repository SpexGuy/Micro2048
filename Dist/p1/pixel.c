#include "pixel.h"

Pixel mix(Pixel a, Pixel b, uint32_t n, uint32_t d) {
	nMix(&a, b, n, d);
	return a;
}

void nMix(Pixel *a, Pixel b, uint32_t n, uint32_t d) {
	a->color.r = blerp(a->color.r, b.color.r, n, d);
	a->color.g = blerp(a->color.g, b.color.g, n, d);
	a->color.b = blerp(a->color.b, b.color.b, n, d);
}

Pixel complement(Pixel a) {
	nComplement(&a);
	return a;
}

void nComplement(Pixel *a) {
	a->color.r = bcomp(a->color.r);
	a->color.g = bcomp(a->color.g);
	a->color.b = bcomp(a->color.b);
}

Pixel multiply(Pixel pix, byteFraction fraction) {
	nMultiply(&pix, fraction);
	return pix;
}

void nMultiply(Pixel *pix, byteFraction fraction) {
	pix->color.r = bmult(pix->color.r, fraction);
	pix->color.g = bmult(pix->color.g, fraction);
	pix->color.b = bmult(pix->color.b, fraction);
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

void percentAdd(Pixel *pix, Pixel addition, byteFraction percent) {
	static Pixel white = {0xFFFFFF};
	//calculate the total percentage
	uint16_t totalPercent = pix->color.p + percent;
	//get the overflow (signed arithmetic)
	byteFraction overflow = max(0, (int)totalPercent-255);
	if (overflow > 0) {
		//calculate fractions for the old and new parts
		byteFraction oldFract = fract1(pix->color.p, totalPercent);
		byteFraction newFract = fract1(percent, totalPercent);
		//calculate the percentaged pixels and add
		nMultiply(pix, oldFract);
		nMultiply(&addition, newFract);
		nAdd(pix, addition);
		//lerp to white
		nMix(pix, white, overflow, 255);
		pix->color.p = 255;
	} else {
		nAdd(pix, addition);
		pix->color.p = totalPercent;
	}
}

void setColor(Pixel *pix, Pixel value) {
	pix->color.r = value.color.r;
	pix->color.g = value.color.g;
	pix->color.b = value.color.b;
}
