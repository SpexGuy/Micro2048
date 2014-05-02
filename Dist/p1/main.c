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
#include "2048.h"

extern void uartTxPoll(uint32_t base, char *data);

/******************************************************************************
 * Defines
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/

//*****************************************************************************
//*****************************************************************************

bool drawSquare;

void startFadeAnim(void *param);

void startTranslateAnim(void *param) {
	const Pixel px = {0x0000ff};
	drawSquare = true;
	schedule(Time, SYSTICKS_PER_SECOND/4,
					 0, 3,				//move from 0 to 3 in x
					 3, 3,				//stay at 3 in y
					 px, px,			//stay at blue
					 startFadeAnim, param);
}

void startFadeAnim(void *param) {
	const Pixel startColor = {0xFFFFFF};
	const Pixel endColor = {0x0000FF};
	drawSquare = false;
	schedule(Time, SYSTICKS_PER_SECOND/2,
					 7, 7,				//don't move
					 7, 7,				//stay at 7 in x and y
					 startColor, endColor,			//fade to blue
					 startFadeAnim, param);
}

void stringify(char *buffer, uint8_t num) {
	buffer[0] = '0' + (num/100);
	buffer[1] = '0' + (num/10)%10;
	buffer[2] = '0' + (num%10);
	buffer[3] = '\0';
}

void printBLerpFracs() {
	byteFraction b = BF_0;
	char buffer[4];
	do {
		stringify(buffer, b);
		uartTxPoll(UART0, buffer);
		uartTxPoll(UART0, " -> ");
		stringify(buffer, blerp(2, 0, b, 256));
		uartTxPoll(UART0, buffer);
		uartTxPoll(UART0, ":");
		stringify(buffer, blerpfrac(2, 0, b, 256));
		uartTxPoll(UART0, buffer);
		uartTxPoll(UART0, "\n\r");
		b++;
	} while(b != BF_0);
}

int
main(void)
{
	Board board;
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
//	startFadeAnim(NULL);
	
	init2048(&board);
	addBlock(&board, 0, 0, 0, false);
	addBlock(&board, 0, 1, 0, false);
	addBlock(&board, 0, 2, 0, false);
	addBlock(&board, 0, 3, 0, false);

	addBlock(&board, 1, 1, 1, false);
	addBlock(&board, 1, 2, 0, false);
	addBlock(&board, 1, 3, 1, false);

	addBlock(&board, 2, 1, 1, false);
	addBlock(&board, 2, 2, 1, false);
	addBlock(&board, 2, 3, 0, false);

	addBlock(&board, 3, 2, 1, false);
	addBlock(&board, 3, 3, 1, false);

//	printBLerpFracs();

  while(1) {
    //examineButtons();
		offset = Time + 3*SYSTICKS_PER_SECOND;
		while(Time < offset) {
			update();
			clearDrawBuffer();
			drawBuffer = getDrawBuffer();
			drawBoard(drawBuffer, &board);
			drawAnimations(drawBuffer);
			swapBuffers();
			updateRefreshRate();
		}

		shiftUp(&board);
  }
}
