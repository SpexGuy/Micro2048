#ifndef _COMMUNICATION_H
#include "2048.h"

#define _COMMUNICATION_H 1

//****** AI **********//
void sendMove(uint8_t move);

//****** Game ************//
void sendBoard(Board *b);
void sendNewTile(Tile *tile);
void requestMove(void);

//****** Recieve ******//
void recieveComs(void);

#endif