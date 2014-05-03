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
} Tile;

typedef struct {
	Tile *tiles[BOARD_HEIGHT][BOARD_WIDTH];
	uint32_t score;
	uint64_t inputTime;
} Board;

void init2048(Board *b);
void addTile(Board *b, uint8_t x, uint8_t y, uint8_t value, bool animated);
void shiftUp(Board *b);
void shiftDown(Board *b);
void shiftLeft(Board *b);
void shiftRight(Board *b);
void addRandomTile(Board *b);
void drawBoard(FrameBuffer *draw, Board *board);

#endif
