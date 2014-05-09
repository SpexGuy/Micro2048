#include "communication.h"
#include "UART.h"
#include <alloca.h>

extern volatile uint64_t Time;

extern Board board;

uint8_t otherBoard = 0xFF;

uint8_t sentMove = 0xFF;
uint8_t getSentMove() {
	uint8_t move = sentMove;
	sentMove = 0xFF;
	return move;
}

uint64_t lastHeartbeatTime = 0;

void update(uint8_t uart) {
	uint8_t type = uartRx(uart, false);
	if (type == 0xFF || type == 0) return;
	otherBoard = uart;
	switch(type) {
		case TYPE_MOVE: {
			sentMove = uartRx(uart, true);
			debugPrintln("MOVE");
			break;
		}
		case TYPE_BOARD_REQ: {
			sendBoard(&board);
			break;
		}
		case TYPE_BOARD: {
			uint8_t boardData[BOARD_WIDTH * BOARD_HEIGHT];
			uartRxArray(uart, BOARD_WIDTH * BOARD_HEIGHT, boardData);
			debugPrintln("BOARD");
			clearBoard(&board);
			restoreBoard(&board, boardData);
			break;
		}
		case TYPE_TILE: {
			uint8_t x = uartRx(uart, true);
			uint8_t y = uartRx(uart, true);
			uint8_t val = uartRx(uart, true);
			debugPrintln("TILE");
			addTile(&board, x, y, val, true);
			break;
		}
		case TYPE_MOVE_READY: {
			debugPrintln("MOVE_READY");
			break;
		}
		case TYPE_HEARTBEAT: {
			WATCHDOG0_LOAD_R = WDT_LOAD_M;
			lastHeartbeatTime = Time;
			break;
		}
		default: {
			warningPrintln("UNKNOWN TYPE");
			break;
		}
	}
}

//****************** AI ********************//
void sendMove(uint8_t move) {
	uartTx(UART_GAME, TYPE_MOVE);
	uartTx(UART_GAME, move);
}

void requestBoard() {
	uartTx(UART_GAME, TYPE_BOARD_REQ);
}

//****************** Game **********************//
void sendBoard(Board *b) {
	uint8_t serial[BOARD_WIDTH * BOARD_HEIGHT];
	debugPrintln("Send Board");
	createBoardArray(b, serial);
	uartTx(UART_AI, TYPE_BOARD);
	uartTxArray(UART_AI, BOARD_WIDTH * BOARD_HEIGHT, serial);
}

void sendNewTile(Tile *tile) {
	uartTx(UART_AI, TYPE_TILE);
	uartTx(UART_AI, tile->x);
	uartTx(UART_AI, tile->y);
	uartTx(UART_AI, tile->value);
}

void requestMove(void) {
	sentMove = 0xFF;
	uartTx(UART_AI, TYPE_MOVE_READY);
}

void sendHeartbeat(void) {
	if (otherBoard == 0xFF) {
		uartTx(UART_AI, TYPE_HEARTBEAT);
		uartTx(UART_GAME, TYPE_HEARTBEAT);
	} else {
		uartTx(otherBoard, TYPE_HEARTBEAT);
	}
}

bool isConnected(void) {
	if (Time - lastHeartbeatTime > CONNECT_TIMEOUT) {
		infoPrintln("TIMEOUT");
		return false;
	}
	return true;
}

bool isAi(void) {
	return otherBoard == UART_GAME;
}

bool isGame(void) {
	return otherBoard == UART_AI;
}

bool isSinglePlayer(void) {
	return otherBoard == 0xFF;
}


//****** Receive ******//
bool receiveComs(void) {
	if (otherBoard == 0xFF) {
		update(UART_AI);
		update(UART_GAME);
		return otherBoard != 0xFF;
	} else {
		update(otherBoard);
		if (!isConnected()) {
			infoPrintln("CONNECTION LOST");
			otherBoard = 0xFF;
			return false;
		}
		return true;
	}
}
