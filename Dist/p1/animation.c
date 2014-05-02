#include "animation.h"
#include "stdlib.h"
#include "UART.h"

extern void uartTxPoll(uint32_t base, char *data);

Animation* head = NULL;

void schedule(uint64_t startTime, uint64_t runTime, 
							uint8_t startX, uint8_t finalX, 
							uint8_t startY, uint8_t finalY, 
							Pixel startColor, Pixel finalColor, 
							void (*onFinish)(void* param), void* param)
{
	Animation* newAnimation = (Animation*) malloc(sizeof(Animation));
	
	newAnimation->startTime = startTime;
	newAnimation->runTime = runTime;
	newAnimation->startX = startX;
	newAnimation->finalX = finalX;
	newAnimation->currentOffsetX = 0;
	newAnimation->startY = startY;
	newAnimation->finalY = finalY;
	newAnimation->currentOffsetY = 0;
	newAnimation->startColor = startColor;
	newAnimation->finalColor = finalColor;
	newAnimation->onFinish = onFinish;
	newAnimation->param = param;
	
	StartCritical();
		newAnimation->next = head;
		head = newAnimation;
	EndCritical();
}

void updateAnimation(Animation* animation, uint64_t localTime) {
	byteFraction dt = fract(localTime - animation->startTime, animation->runTime);
	animation->posX = blerp(animation->startX, animation->finalX, dt);
	animation->posY = blerp(animation->startY, animation->finalY, dt);
	
	animation->currentOffsetX = blerpfrac(animation->startX, animation->finalX, 
																				localTime - animation->startTime, animation->runTime); 
	animation->currentOffsetY = blerpfrac(animation->startY, animation->finalY, 
																				localTime - animation->startTime, animation->runTime); 
	animation->currentColor = mix(animation->startColor, animation->finalColor, dt);

}

void update() {
	uint64_t localTime = Time;

	Animation* current = head;
	Animation*  previous = NULL;
	Animation* next;
	while(current != NULL) {
		updateAnimation(current, localTime);
		next = current->next;
		
		if(localTime - current->startTime > current->runTime) {
			current->onFinish(current->param);
			if(previous == NULL) {
				head = next;
			} else {
				previous->next = next;
			}
			free(current);
		} else {
			previous = current;
		}
		current = next;
	}
}

void drawAnimation(FrameBuffer* frameBuffer, Animation* animation) {
	drawAARect(frameBuffer, animation->posX, animation->currentOffsetX, 2, animation->posY, animation->currentOffsetY, 2, animation->currentColor);
}

void drawAnimations(FrameBuffer* frameBuffer) {
	Animation* current = head;
	while(current != NULL) {
		drawAnimation(frameBuffer, current);
		current = current->next;
	}
}
