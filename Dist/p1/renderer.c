#include "renderer.h"
#include "board_ports.h"
#include "gpio.h"

uint32_t brightness = MAX_BRIGHTNESS;
FrameBuffer *currentBuffer = NULL;

extern GPIO_PORT *PortA;
extern GPIO_PORT *PortB;
extern GPIO_PORT *PortC;
extern GPIO_PORT *PortD;
extern GPIO_PORT *PortE;
extern GPIO_PORT *PortF;

void updateRow(int row, int duty) {
	uint32_t red = 0, green = 0, blue = 0;
	int c;
	for (c = 0; c < 8; c++) {
		uint8_t mask = 1<<c;
		Pixel current = currentBuffer->raster[row][c];
		if ((current.color.b*brightness) / MAX_BRIGHTNESS > duty)
			blue |= mask;
		if ((current.color.g*brightness) / MAX_BRIGHTNESS > duty)
			green |= mask;
		if ((current.color.r*brightness) / MAX_BRIGHTNESS > duty)
			red |= mask;
	}
	
	//enable output
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	//clear all colors
	PortC->MaskedData[DISPLAY_EN_MASK] = PC4_RED_EN|PC5_GRN_EN|PC6_BLU_EN;
	PortB->Data = 0xFF;
	//set row
	PortC->MaskedData[DISPLAY_EN_MASK] = PC7_COL_EN;
	PortB->Data = ~(1<<row);
	//set colors
	PortC->MaskedData[DISPLAY_EN_MASK] = PC4_RED_EN;
	PortB->Data = ~red;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC5_GRN_EN;
	PortB->Data = ~green;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC6_BLU_EN;
	PortB->Data = ~blue;
	//disable output
	PortF->MaskedData[PF4_OUT_EN_B] = 1;
}

void setCurrentBuffer(FrameBuffer *buffer) {
	currentBuffer = buffer;
}

FrameBuffer *getCurrentBuffer() {
	return currentBuffer;
}

void setBrightness(uint32_t bright) {
	brightness = bright;
}

uint32_t getBrightness() {
	return brightness;
}
