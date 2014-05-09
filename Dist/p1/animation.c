#include "animation.h"
#include <stdlib.h>
#include "UART.h"
#include "lock.h"

Animation *additions = NULL;
Animation *head = NULL;

void noFunc(void *param) {}

void scheduleAnimation(uint64_t startTime, uint64_t runTime, 
											 uint8_t startX, uint8_t finalX, 
											 uint8_t startY, uint8_t finalY, 
											 Pixel startColor, Pixel finalColor, 
											 void (*onFinish)(void* param), void* param)
{
	Animation* newAnimation;
	StartCritical();
		newAnimation = new(Animation);
	EndCritical();
	
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
		newAnimation->next = additions;
		additions = newAnimation;
	EndCritical();
}

void updateAnimation(Animation* animation, uint64_t localTime) {
	animation->posX = blerp(animation->startX, animation->finalX,
													localTime - animation->startTime, animation->runTime); 
	animation->posY = blerp(animation->startY, animation->finalY,
													localTime - animation->startTime, animation->runTime); 
	animation->currentOffsetX = blerpfrac(animation->startX, animation->finalX, 
																				localTime - animation->startTime, animation->runTime); 
	animation->currentOffsetY = blerpfrac(animation->startY, animation->finalY, 
																				localTime - animation->startTime, animation->runTime); 
	animation->currentColor = mix(animation->startColor, animation->finalColor,
																localTime - animation->startTime, animation->runTime); 
}

void addNewAnimations() {
	if (additions) {
		Animation *curr = additions;
		while(curr->next) curr = curr->next;
		curr->next = head;
		head = additions;
		additions = NULL;
	}
}

void updateAnimations() {
#	ifdef _DEBUG_	
	uint32_t dTime;
#	endif
	uint64_t localTime;
	Animation *current, *previous, *next;
	addNewAnimations();
	localTime = Time;

	current = head;
	previous = NULL;
	while(current != NULL) {
		updateAnimation(current, localTime);
		next = current->next;
		
		if(localTime - current->startTime > current->runTime) {
			current->onFinish(current->param);
			if(current == head) {
				head = next;
			} else {
				previous->next = next;
			}
			StartCritical();
				delete(current);
			EndCritical();
		} else {
			previous = current;
		}
		current = next;
	}
#	ifdef _DEBUG_	
	dTime = Time - localTime;
	if (dTime > SYSTICKS_PER_SECOND/60) {
		char buffer[40];
		sprintf(buffer, "It took %3d ticks to update anims!", dTime);
		uartTxPoll(UART0, buffer);
	}
#	endif
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

void freeList(Animation *head) {
	Animation *next;
	while(head) {
		next = head->next;
		delete(head);
		head = next;
	}
}

void clearAnimations() {
	StartCritical();
	freeList(additions);
	additions = NULL;
	freeList(head);
	head = NULL;
	EndCritical();
}
