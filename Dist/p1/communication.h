#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H 1
#include "2048.h"
#include "UART.h"

#define TYPE_MOVE 1
#define TYPE_BOARD 2
#define TYPE_TILE 3
#define TYPE_MOVE_READY 4
#define TYPE_HEARTBEAT 5
#define TYPE_BOARD_REQ 6

#define MODE_AI 1
#define MODE_SP 2
#define MODE_HOST 3

#define UART_GAME UART_ID_2
#define UART_AI UART_ID_5

#define CONNECT_TIMEOUT SYSTICKS_PER_SECOND/4

//****** AI **********//
void sendMove(uint8_t move);

//****** Game ************//
void sendBoard(Board *b);
void sendNewTile(Tile *tile);
void requestMove(void);
void sendHeartbeat(void);
void requestBoard(void);

bool isConnected(void);
bool isAi(void);
bool isGame(void);
bool isSinglePlayer(void);

uint8_t getSentMove(void);

//****** Receive ******//
bool receiveComs(void);

#endif
