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

// #define IS_EEPROM_TEST 1

/******************************************************************************
 * Defines
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/

volatile bool heartbeat = false;

Board board;

void SinglePlayerUpdate(void);
void AIUpdate(void);
void HostUpdate(void);
#ifdef _DEBUG_
void printFrameRate(void);
#endif
#ifdef IS_EEPROM_TEST
void eepromTest(void);
#endif


//*****************************************************************************
//*****************************************************************************

void	printGreeting() {
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"****** ECE353 ******\n\r");
  uartTxPoll(UART0,"Micro 2018\n\r");
  uartTxPoll(UART0,teamNumber);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,teamMembers);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"********************\n\r");
  uartTxPoll(UART0,"\n\r");
}

bool checkAIInput(void) {
	static bool requested = false;
	uint8_t move;
	if(canTakeInput(&board)) {
		if (!requested) {
			requestMove();
			requested = true;
		} else {
			char buffer[40];
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
					sprintf(buffer, "AI Move Select Error: %d", move);
					uartTxPoll(UART0, buffer);
					return false;
			}
		}
	}
	return false;
}

bool checkUserInput(void) {
	static uint64_t save_press_time = 0;
	if (getButtonPress(BUTTON_NORTH)) {
		return shiftUp(&board);
	} else if (getButtonPress(BUTTON_SOUTH)) {
		return shiftDown(&board);
	} else if (getButtonPress(BUTTON_EAST)) {
		return shiftRight(&board);
	} else if (getButtonPress(BUTTON_WEST)) {
		return shiftLeft(&board);
	} else if (getButtonPress(BUTTON_AUX)) {
		save_press_time = Time;
	} else if (getButtonRelease(BUTTON_AUX)) {
		if (Time - save_press_time > SAVE_HOLD_THRESH) {
			clearBoard(&board);
			restoreGame(&board);
			if (isGame()) {
				sendBoard(&board);
			}
		} else {
			saveGame(&board);
		}
	}
	return false;
}

void sendUserInput(void) {
	if (getButtonPress(BUTTON_NORTH)) {
		sendMove(0);
		shiftUp(&board);
	} else if (getButtonPress(BUTTON_SOUTH)) {
		sendMove(1);
		shiftDown(&board);
	} else if (getButtonPress(BUTTON_EAST)) {
		sendMove(2);
		shiftRight(&board);
	} else if (getButtonPress(BUTTON_WEST)) {
		sendMove(3);
		shiftLeft(&board);
	}
}

int main(void) {
	uint8_t lastMode = 0xFF;
	initBoard();
	initDoubleBuffers();
	printGreeting();
	init2048(&board);
	addRandomTile(&board);
	addRandomTile(&board);
	uartTxPoll(UART0, "\n");
#	ifdef IS_EEPROM_TEST
	eepromTest();
#	endif
	while(1) {
		if (heartbeat) {
			sendHeartbeat();
			heartbeat = false;
		}
		if (receiveComs()) {
			if (isAi()) {
				if (lastMode != MODE_AI) {
					uartTxPoll(UART0, "AI\r\n");
					requestBoard();
				}
				AIUpdate();
				lastMode = MODE_AI;
			} else {
				if (lastMode != MODE_HOST) {
					uartTxPoll(UART0, "HOST\r\n");
				}
				HostUpdate();
				lastMode = MODE_HOST;
			}
		} else {
			if (lastMode != MODE_SP) {
				uartTxPoll(UART0, "SP\r\n");
				clearButtons();
			}
			SinglePlayerUpdate();
			lastMode = MODE_SP;
		}
	}
}

void AIUpdate() {
	FrameBuffer *drawBuffer;
	updateAnimations();
	clearDrawBuffer();
	drawBuffer = getDrawBuffer();
	drawBoard(drawBuffer, &board);
	drawAnimations(drawBuffer);
	swapBuffers();
	updateRefreshRate();
	updateButtons();
	sendUserInput();
}

void HostUpdate() {
	FrameBuffer *drawBuffer;
#	ifdef _DEBUG_
		printFrameRate();
#	endif
	updateAnimations();
	clearDrawBuffer();
	drawBuffer = getDrawBuffer();
	drawBoard(drawBuffer, &board);
	drawAnimations(drawBuffer);
	swapBuffers();
	updateRefreshRate();
	updateButtons();
	if (checkAIInput()) {
		Tile *t = addRandomTile(&board);
		sendNewTile(t);
	}
}

void SinglePlayerUpdate() {
	FrameBuffer *drawBuffer;
#	ifdef _DEBUG_
		printFrameRate();
#	endif
	updateAnimations();
	clearDrawBuffer();
	drawBuffer = getDrawBuffer();
	drawBoard(drawBuffer, &board);
	drawAnimations(drawBuffer);
	swapBuffers();
	updateRefreshRate();
	updateButtons();
	if (checkUserInput()) {
		addRandomTile(&board);
	}
}

#ifdef _DEBUG_
void	printFrameRate(void) {
	static uint64_t lastSecond = 0;
	static 	uint32_t frameCount = 0;
	frameCount++;
	lastSecond = Time;
	if (Time - lastSecond > SYSTICKS_PER_SECOND) {
		char buffer[40];
		sprintf(buffer, "\033[A%6d frames per second\n\r", frameCount);
		uartTxPoll(UART0, buffer);
		frameCount = 0;
		lastSecond = Time;
	}
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
#endif

#	ifdef IS_EEPROM_TEST
void eepromTest() {
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

