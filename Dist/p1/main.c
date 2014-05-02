//*****************************************************************************
//
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lm4f120h5qr.h"
#include "doublebuffer.h"
#include "../team.h"
#include "initBoard.h"
#include "board_config.h"
#include "gpio.h"
#include "UART.h"
#include "led_chars.h"
#include "hw4_gpio.h"
#include "hw4_adc.h"
#include "animation.h"

extern void uartTxPoll(uint32_t base, char *data);

/******************************************************************************
 * Defines
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/
extern uint32_t Brightness;

void initAnimation(void);
void finalizeAnimation(void *);

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	FrameBuffer *drawBuffer;
	Pixel px, px2;
	uint32_t offset = 0;
  initBoard();
	initDoubleBuffers();
    
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"****** ECE353 ******\n\r");
  uartTxPoll(UART0,"HW4 Demo\n\r");
  uartTxPoll(UART0,teamNumber);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,teamMembers);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"********************\n\r");
  uartTxPoll(UART0,"\n\r");

	px.hex = 0x0000ff;
	px2.hex = 0x0000ff;
	
	initAnimation();

  while(1) {
    //examineButtons();
		//px.color.r = fract(abs((offset%6400)-3200),3200);
		//px.color.g = fract(abs((offset%9600)-4800),4800);
		//px.color.b = fract(abs((offset%16000)-8000),8000);
		clearDrawBuffer();
//		drawBuffer = getDrawBuffer();
//		drawAARect(	drawBuffer,
//								(offset>>2)/256, fract(offset>>2,256), 2,
//								3, 0, 2, px);
//		drawRect(	drawBuffer,
//								3, 2,
//								3, 2, px2);
//		swapBuffers();
//		offset++;
//		offset %= (512<<4);
			drawAnimations(getDrawBuffer());
			swapBuffers();
			update();
			updateRefreshRate();
  }
}

void initAnimation() {
	uint64_t startTime = Time;
	uint64_t runTime = 100000;
	uint8_t startX = 7;
	uint8_t finalX = 0;
	uint8_t startY = 3;
	uint8_t finalY = 3;
	Pixel* startColor = malloc(sizeof(Pixel));
	Pixel* finalColor = malloc(sizeof(Pixel));
	
	startColor->hex = 0xFF0000;
	finalColor->hex = 0xFF00FF;
	
	schedule(startTime, runTime, 
						startX, finalX, 
						startY, finalY, 
						*startColor, *finalColor, 
						finalizeAnimation, NULL);
}

void finalizeAnimation(void * param) {
	initAnimation();
}



