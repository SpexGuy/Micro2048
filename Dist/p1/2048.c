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
void addBlock(Board *b, uint8_t x, uint8_t y, uint8_t value, bool hidden) {
	Block *block = malloc(sizeof(Block));
	block->x = x;
	block->y = y;
	block->value = value;
	block->hidden = hidden;
	b->blocks[y][x] = block;
}

void setPos(Board *b, Block *block, uint8_t x, uint8_t y) {
	b->blocks[block->y][block->x] = NULL;
	b->blocks[y][x] = block;
	block->x = x;
	block->y = y;
}

void removeBlock(Board *b, Block *block) {
	b->blocks[block->y][block->x] = NULL;
	free(block);
}

bool canTakeInput(Board *b) {
	return ((int64_t)Time - (int64_t)b->inputTime) > 0;
}

//-------------------- Animation and Callbacks ---------------
void showBlock(void *param) {
//	uartTxPoll(UART0, "Show Block\n\r");
	((Block *)param)->hidden--;
}

void endMergeTranslate(void *param) {
	static Pixel white = {0xFFFFFF};
	Block *block = param;
//	uartTxPoll(UART0, "Start fade anim\n\r");
	block->hidden++;
	block->value++;
	scheduleAnimation(Time, FADE_RUN_TIME,
									  2*block->x, 2*block->x,
									  2*block->y, 2*block->y,
									  white, colors[block->value],
									  showBlock, block);
}

void spawnMergeAnim(Board *b, Block *last, Block *block, uint64_t time) {
//	uartTxPoll(UART0, "Start merge anim\n\r");
	scheduleAnimation(time, TRANSLATE_RUN_TIME,
									  2*block->x, 2*last->x,
									  2*block->y, 2*last->y,
									  colors[block->value], colors[block->value],
									  endMergeTranslate, last);
	removeBlock(b, block);
}

void spawnTranslateAnim(Board *b, Block *block, uint64_t time, uint8_t x, uint8_t y) {
//	uartTxPoll(UART0, "Start trans anim\n\r");
	block->hidden++;
	scheduleAnimation(time, TRANSLATE_RUN_TIME,
									  2*block->x, 2*x,
									  2*block->y, 2*y,
									  colors[block->value], colors[block->value],
									  showBlock, block);
	setPos(b, block, x, y);
}

//------------------- Header Functions -----------------------
void init2048(Board *b) {
	//fill board with null
	memset(b, 0, sizeof(*b));
}

void shiftUp(Board *b) {
	uint64_t localTime = Time;
	Block *current;
	Block *last;
	uint8_t x, y, backY;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = 0;
		last = NULL;
		for (y = 0; y  < BOARD_HEIGHT; y++) {
			current = b->blocks[y][x];
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
	Block *current;
	Block *last;
	int8_t x, y, backY;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = BOARD_HEIGHT-1;
		last = NULL;
		for (y = BOARD_HEIGHT-1; y >= 0; y--) {
			current = b->blocks[y][x];
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
	Block *current;
	Block *last;
	uint8_t x, y, backX;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = 0;
		last = NULL;
		for (x = 0; x  < BOARD_WIDTH; x++) {
			current = b->blocks[y][x];
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
	Block *current;
	Block *last;
	int8_t x, y, backX;
	if (!canTakeInput(b)) return;
	b->inputTime = localTime+TRANSLATE_RUN_TIME;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = BOARD_WIDTH-1;
		last = NULL;
		for (x = BOARD_WIDTH-1; x >= 0; x--) {
			current = b->blocks[y][x];
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
	Block *current;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			current = board->blocks[y][x];
			if (current && !current->hidden) {
				drawRect(draw, x*2, 2, y*2, 2, colors[current->value]);
			}
		}
	}
}
