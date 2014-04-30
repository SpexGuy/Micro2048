#include "renderer.h"
#include "board_ports.h"
#include "gpio.h"

uint32_t brightness = 256;
FrameBuffer *currentBuffer = NULL;

extern GPIO_PORT *PortA;
extern GPIO_PORT *PortB;
extern GPIO_PORT *PortC;
extern GPIO_PORT *PortD;
extern GPIO_PORT *PortE;
extern GPIO_PORT *PortF;

uint32_t gammaCorrect(uint32_t val) {
	return (val*val)/255;
}

void updateRow(uint8_t row, byteFraction duty) {
	uint32_t red = 0, green = 0, blue = 0;
	int c;
	for (c = 0; c < 8; c++) {
		uint8_t mask = 1<<c;
		Pixel current = currentBuffer->raster[row][c];
		if (((current.color.b*current.color.b*brightness)>>16) > duty)
			blue |= mask;
		if (((current.color.g*current.color.g*brightness)>>16) > duty)
			green |= mask;
		if (((current.color.r*current.color.r*brightness)>>16) > duty)
			red |= mask;
	}
	
	//clear all colors
	PortC->MaskedData[DISPLAY_EN_MASK] = PC4_RED_EN|PC5_GRN_EN|PC6_BLU_EN;
	PortB->Data = 0xFF;
	//push value
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	PortB->Data = 0xFF;
	PortF->MaskedData[PF4_OUT_EN_B] = 1;
	//set row
	PortC->MaskedData[DISPLAY_EN_MASK] = 0;
	PortB->Data = 0xFF;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC7_COL_EN;
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	PortB->Data = ~(1<<row);
	PortF->MaskedData[PF4_OUT_EN_B] = 1;
	//set colors
	//green
	PortC->MaskedData[DISPLAY_EN_MASK] = 0;
	PortB->Data = 0xFF;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC5_GRN_EN;
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	PortB->Data = ~green;
	PortF->MaskedData[PF4_OUT_EN_B] = 1;
	//red
	PortC->MaskedData[DISPLAY_EN_MASK] = 0;
	PortB->Data = 0xFF;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC4_RED_EN;
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	PortB->Data = ~red;
	PortF->MaskedData[PF4_OUT_EN_B] = 1;
	//blue
	PortC->MaskedData[DISPLAY_EN_MASK] = 0;
	PortB->Data = 0xFF;
	PortC->MaskedData[DISPLAY_EN_MASK] = PC6_BLU_EN;
	PortF->MaskedData[PF4_OUT_EN_B] = 0;
	PortB->Data = ~blue;
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
