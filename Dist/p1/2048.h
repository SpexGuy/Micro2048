#ifndef _2048_H
#define _2048_H 1

#include "constants.h"
#include "screen.h"

#define BOARD_WIDTH 4
#define BOARD_HEIGHT 4

typedef struct {
	uint8_t value;
	uint8_t x;
	uint8_t y;
	uint8_t hidden;
} Block;

typedef struct {
	Block *blocks[BOARD_HEIGHT][BOARD_WIDTH];
	uint32_t score;
} Board;

void init2048(Board *b);
void addBlock(Board *b, uint8_t x, uint8_t y, uint8_t value, bool animated);
void shiftUp(Board *b);
void shiftDown(Board *b);
void shiftLeft(Board *b);
void shiftRight(Board *b);
void addRandomPos(Board *b);
void drawBoard(FrameBuffer *draw, Board *board);

#endif
