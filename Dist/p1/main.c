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
#include "AI2048.h"
#include "spi.h"
#include "eeprom.h"

//#define isAI 1
//#define IS_EEPROM_TEST 1

extern void uartTxPoll(uint32_t base, char *data);

/******************************************************************************
 * Defines
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/

Board board;

//*****************************************************************************
//*****************************************************************************

bool drawSquare;

void startFadeAnim(void *param);

void startTranslateAnim(void *param) {
	const Pixel px = {0x0000ff};
	drawSquare = true;
	scheduleAnimation(Time, SYSTICKS_PER_SECOND/4,
									  0, 3,				//move from 0 to 3 in x
									  3, 3,				//stay at 3 in y
									  px, px,			//stay at blue
									  startFadeAnim, param);
}

void startFadeAnim(void *param) {
	const Pixel startColor = {0xFFFFFF};
	const Pixel endColor = {0x0000FF};
	drawSquare = false;
	scheduleAnimation(Time, SYSTICKS_PER_SECOND/2,
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

bool checkInput(void) {
#	ifdef isAI
		if(canTakeInput(&board)) {	
			switch(getNextMove(&board)) {
				case 0:
					uartTxPoll(UART0, "Up\n\r");
					return shiftUp(&board);
				case 1:
					uartTxPoll(UART0, "Down\n\r");
					return shiftDown(&board);
				case 2:
					uartTxPoll(UART0, "Right\n\r");
					return shiftRight(&board);
				case 3:
					uartTxPoll(UART0, "Left\n\r");
					return shiftLeft(&board);
				default:
					uartTxPoll(UART0, "AI Move Select Error");
					return false;
				}
		} else {
			return false;
		}
#	else
		if (getButton(BUTTON_NORTH))
			return shiftUp(&board);
		else if (getButton(BUTTON_SOUTH)) {
			restoreGame(&board);
			//return shiftDown(&board);
			return false;
		}
		else if (getButton(BUTTON_EAST))
			return shiftRight(&board);
		else if (getButton(BUTTON_WEST)) {
			uartTxPoll(UART0, "Before Save\n\r");
			saveGame(&board);
			uartTxPoll(UART0, "After Save\n\r");
			
		}//return shiftLeft(&board);
		return false;
#	endif
}

int main(void)
{
	FrameBuffer *drawBuffer;
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
	
	init2048(&board);
	addRandomTile(&board);
	addRandomTile(&board);
	
#	ifdef IS_EEPROM_TEST
	{
		uint32_t out;
		char printbuf[20];
		uint32_t data = 0xDEADCAFE;
		uint16_t address = 0x2;
		sprintf(printbuf, "write: %X\n\r", data);
		uartTxPoll(UART0, printbuf);
		spi_eeprom_write_array(address, (uint8_t *)&data, sizeof(data));
		spi_eeprom_read_array(address, (uint8_t *)&out, sizeof(out));
		sprintf(printbuf, "read: %X\n\r", out);
		uartTxPoll(UART0, printbuf);
	}
#	endif

	while(1) {
//		uartTxPoll(UART0,"M");
		updateAnimations();
		clearDrawBuffer();
		drawBuffer = getDrawBuffer();
		drawBoard(drawBuffer, &board);
		drawAnimations(drawBuffer);
		swapBuffers();
		updateRefreshRate();
		updateButtons();
		if (checkInput()) {
			addRandomTile(&board);
		}
	}
}
