#include "communication.h"
#include "UART.h"
#include <alloca.h>

uint8_t sentMove = 0xFF;
uint8_t getSentMove() {
	uint8_t move = sentMove;
	sentMove = 0xFF;
	return move;
}

void update(uint8_t uart) {
	uint8_t type = uartRx(uart, false);
	if (type == 0xFF || type == 0) return;
	switch(type) {
		case TYPE_MOVE: {
			sentMove = uartRx(uart, true);
			uartTxPoll(UART0, "MOVE\r\n");
			break;
		}
		case TYPE_BOARD: {
			uint8_t board[BOARD_WIDTH * BOARD_HEIGHT];
			uartRxArray(uart, BOARD_WIDTH * BOARD_HEIGHT, board);
			uartTxPoll(UART0, "BOARD\r\n");
			break;
		}
		case TYPE_TILE: {
			uint8_t x = uartRx(uart, true);
			uint8_t y = uartRx(uart, true);
			uint8_t val = uartRx(uart, true);
			uartTxPoll(UART0, "TILE\r\n");
		}
		case TYPE_MOVE_READY: {
			uartTxPoll(UART0, "MOVE_READY\r\n");
			break;
		}
		default: {
			uartTxPoll(UART0, "UNKNOWN TYPE\r\n");
			break;
		}
	}
}

//****************** AI ********************//
void sendMove(uint8_t move) {
	uartTx(UART_GAME, TYPE_MOVE);
	uartTx(UART_GAME, move);
}

//****************** Game **********************//
void sendBoard(Board *b) {
	uint8_t serial[BOARD_WIDTH * BOARD_HEIGHT];
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

//****** Receive ******//
void receiveComs(void) {
	update(UART_AI);
	update(UART_GAME);
}
