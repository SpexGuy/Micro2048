#include "2048.h"
#include "renderer.h"
#include "animation.h"
#include <string.h>
#include <stdlib.h>
#include "UART.h"

#define TRANSLATE_RUN_TIME (SYSTICKS_PER_SECOND/8)
#define FADE_RUN_TIME (SYSTICKS_PER_SECOND/4)
#define MAX_ANIM_TIME (TRANSLATE_RUN_TIME + FADE_RUN_TIME)

Pixel colors[] = {
	{0x777777},
	{0xFFFF77},
	{0xFFFF00},
	{0xFF9900},
	{0xFF4A00},
	{0xFF0000},
	{0xFF0066},
	{0xFF00CC},
	{0xAA00FF},
	{0x0000FF},
	{0x00FFFF},
	{0x33FF99},
//padding, just in case
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF}
};

//--------------- Board Operations ----------------
void addTile(Board *b, uint8_t x, uint8_t y, uint8_t value, bool hidden) {
	Tile *tile = malloc(sizeof(Tile));
	tile->x = x;
	tile->y = y;
	tile->value = value;
	tile->hidden = hidden;
	b->tiles[y][x] = tile;
}

void setPos(Board *b, Tile *tile, uint8_t x, uint8_t y) {
	b->tiles[tile->y][tile->x] = NULL;
	b->tiles[y][x] = tile;
	tile->x = x;
	tile->y = y;
}

void removeTile(Board *b, Tile *tile) {
	b->tiles[tile->y][tile->x] = NULL;
	free(tile);
}

bool canTakeInput(Board *b) {
	return ((int64_t)Time - (int64_t)b->inputTime) > 0;
}

//-------------------- Animation and Callbacks ---------------
void showTile(void *param) {
//	uartTxPoll(UART0, "Show Tile\n\r");
	((Tile *)param)->hidden--;
}

void endMergeTranslate(void *param) {
	static Pixel white = {0xFFFFFF};
	Tile *tile = param;
//	uartTxPoll(UART0, "Start fade anim\n\r");
	tile->hidden++;
	tile->value++;
	scheduleAnimation(Time, FADE_RUN_TIME,
									  2*tile->x, 2*tile->x,
									  2*tile->y, 2*tile->y,
									  white, colors[tile->value],
									  showTile, tile);
}

void spawnMergeAnim(Board *b, Tile *last, Tile *tile, uint64_t time) {
//	uartTxPoll(UART0, "Start merge anim\n\r");
	scheduleAnimation(time, TRANSLATE_RUN_TIME,
									  2*tile->x, 2*last->x,
									  2*tile->y, 2*last->y,
									  colors[tile->value], colors[tile->value],
									  endMergeTranslate, last);
	removeTile(b, tile);
}

void spawnTranslateAnim(Board *b, Tile *tile, uint64_t time, uint8_t x, uint8_t y) {
//	uartTxPoll(UART0, "Start trans anim\n\r");
	tile->hidden++;
	scheduleAnimation(time, TRANSLATE_RUN_TIME,
									  2*tile->x, 2*x,
									  2*tile->y, 2*y,
									  colors[tile->value], colors[tile->value],
									  showTile, tile);
	setPos(b, tile, x, y);
}

//------------------- Header Functions -----------------------
void init2048(Board *b) {
	//fill board with null
	memset(b, 0, sizeof(*b));
}

void shiftUp(Board *b) {
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	uint8_t x, y, backY;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = 0;
		last = NULL;
		for (y = 0; y  < BOARD_HEIGHT; y++) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					b->inputTime = localTime+TRANSLATE_RUN_TIME+FADE_RUN_TIME;
					spawnMergeAnim(b, last, current, localTime);
					last = NULL;
				} else { //no match
					spawnTranslateAnim(b, current, localTime, x, backY++);
					last = current;
				}
			}
		}
	}
}

void shiftDown(Board *b) {
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	int8_t x, y, backY;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = BOARD_HEIGHT-1;
		last = NULL;
		for (y = BOARD_HEIGHT-1; y >= 0; y--) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					b->inputTime = localTime+TRANSLATE_RUN_TIME+FADE_RUN_TIME;
					spawnMergeAnim(b, last, current, localTime);
					last = NULL;
				} else { //no match
					spawnTranslateAnim(b, current, localTime, x, backY--);
					last = current;
				}
			}
		}
	}
}

void shiftLeft(Board *b) {
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	uint8_t x, y, backX;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = 0;
		last = NULL;
		for (x = 0; x  < BOARD_WIDTH; x++) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					b->inputTime = localTime+TRANSLATE_RUN_TIME+FADE_RUN_TIME;
					spawnMergeAnim(b, last, current, localTime);
					last = NULL;
				} else { //no match
					spawnTranslateAnim(b, current, localTime, backX++, y);
					last = current;
				}
			}
		}
	}
}

void shiftRight(Board *b) {
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	int8_t x, y, backX;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = BOARD_WIDTH-1;
		last = NULL;
		for (x = BOARD_WIDTH-1; x >= 0; x--) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					b->inputTime = localTime+TRANSLATE_RUN_TIME+FADE_RUN_TIME;
					spawnMergeAnim(b, last, current, localTime);
					last = NULL;
				} else { //no match
					spawnTranslateAnim(b, current, localTime, backX--, y);
					last = current;
				}
			}
		}
	}
}


void drawBoard(FrameBuffer *draw, Board *board) {
	int x, y;
	Tile *current;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			current = board->tiles[y][x];
			if (current && !current->hidden) {
				drawRect(draw, x*2, 2, y*2, 2, colors[current->value]);
			}
		}
	}
}
