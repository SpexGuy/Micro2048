#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H 1
#include "2048.h"
#include "UART.h"

#define TYPE_MOVE 1
#define TYPE_BOARD 2
#define TYPE_TILE 3
#define TYPE_MOVE_READY 4

#define UART_GAME UART_ID_2
#define UART_AI UART_ID_5

//****** AI **********//
void sendMove(uint8_t move);

//****** Game ************//
void sendBoard(Board *b);
void sendNewTile(Tile *tile);
void requestMove(void);

uint8_t getSentMove(void);

//****** Receive ******//
void receiveComs(void);

#endif
