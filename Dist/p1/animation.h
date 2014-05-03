#ifndef _ANIMATION_H
#define _ANIMATION_H 1

#include "constants.h"
#include "pixel.h"
#include "renderer.h"

extern volatile uint64_t Time;

typedef struct animation {
	struct animation * next;
	uint64_t startTime;
	uint64_t runTime;
	uint8_t startX;
	uint8_t finalX;
	uint8_t startY;
	uint8_t finalY;
	uint8_t posX;
	uint8_t posY;
	byteFraction currentOffsetX;
	byteFraction currentOffsetY;
	Pixel startColor;
	Pixel finalColor;
	Pixel currentColor;
	void (*onFinish)(void* param);
	void* param;
} Animation;

void noFunc(void *param);

void scheduleAnimation(uint64_t startTime, uint64_t runTime, 
							uint8_t startX, uint8_t finalX, 
							uint8_t startY, uint8_t finalY, 
							Pixel startColor, Pixel finalColor, 
							void (*onFinish)(void* param), void* param);
							
void updateAnimations(void);

void drawAnimations(FrameBuffer* frameBuffer);

#endif
