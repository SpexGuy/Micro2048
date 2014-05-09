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
#include "communication.h"

// #define IS_AI 1
// #define IS_EEPROM_TEST 1

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
#	ifdef IS_AI
		static bool requested = false;
		uint8_t move;
		if(canTakeInput(&board)) {
			if (!requested) {
			requestMove();
			requested = true;
			} else {
				move = getSentMove();
				switch(move) {
					case 0:
						requested = false;
						uartTxPoll(UART0, "Up\n\r");
						return shiftUp(&board);
					case 1:
						requested = false;
						uartTxPoll(UART0, "Down\n\r");
						return shiftDown(&board);
					case 2:
						requested = false;
						uartTxPoll(UART0, "Right\n\r");
						return shiftRight(&board);
					case 3:
						requested = false;
						uartTxPoll(UART0, "Left\n\r");
						return shiftLeft(&board);
					case 0xFF:
						return false;
					default:
						uartTxPoll(UART0, "AI Move Select Error");
						return false;
				}
			}
		}
		return false;
#	else
		static uint64_t save_press_time = 0;
		if (getButtonPress(BUTTON_NORTH)) {
			sendMove(0);
			return shiftUp(&board);
		} else if (getButtonPress(BUTTON_SOUTH)) {
			sendMove(1);
			return shiftDown(&board);
		} else if (getButtonPress(BUTTON_EAST)) {
			sendMove(2);
			return shiftRight(&board);
		} else if (getButtonPress(BUTTON_WEST)) {
			sendMove(3);
			return shiftLeft(&board);
		} else if (getButtonPress(BUTTON_AUX)) {
			save_press_time = Time;
		} else if (getButtonRelease(BUTTON_AUX)) {
			if (Time - save_press_time > SAVE_HOLD_THRESH) {
				clearBoard(&board);
				restoreGame(&board);
				sendBoard(&board);
			} else {
				saveGame(&board);
			}
		}
		return false;
#	endif
}

int main(void)
{
	uint64_t lastSecond;
	uint32_t frameCount;
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
	
	uartTxPoll(UART0, "\n");
	lastSecond = Time;
	sendBoard(&board);
	while(1) {
		if (Time - lastSecond > SYSTICKS_PER_SECOND) {
			char buffer[40];
			sprintf(buffer, "\033[A%6d frames per second\n\r", frameCount);
			uartTxPoll(UART0, buffer);
			frameCount = 0;
			lastSecond = Time;
		}
		updateAnimations();
		clearDrawBuffer();
		drawBuffer = getDrawBuffer();
		drawBoard(drawBuffer, &board);
		drawAnimations(drawBuffer);
		swapBuffers();
		frameCount++;
		updateRefreshRate();
		updateButtons();
		receiveComs();
		if (checkInput()) {
			Tile *t = addRandomTile(&board);
			sendNewTile(t);
		}
	}
}
