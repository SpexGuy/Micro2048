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
	{0x777777}, //2
	{0xFFFF77}, //4
	{0xFFFF00}, //8
	{0xFF9900}, //16
	{0xFF4A00}, //32
	{0xFF0000}, //64
	{0xFF0066}, //128
	{0xFF00CC}, //256
	{0xAA00FF}, //512
	{0x0000FF}, //1024
	{0x00FFFF}, //2048
	{0x33FF99}, //4096
//padding, just in case
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF},
	{0xFFFFFF}
};

void showTile(void *param);

//--------------- Board Operations ----------------
uint8_t countEmpty(Board *b) {
	uint8_t x, y, count = 0;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			if (b->tiles[y][x] == NULL) {
				count++;
			}
		}
	}
	return count;
}

void getNthEmptyTile(Board *b, uint8_t n, uint8_t *xPos, uint8_t *yPos) {
	uint8_t x, y, count = 0;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			if (b->tiles[y][x] == NULL) {
				if (count == n) {
					*xPos = x;
					*yPos = y;
					return;
				}
				count++;
			}
		}
	}
	*xPos = (uint8_t)(-1);
	*yPos = (uint8_t)(-1);
}

void fadeToColor(void *param) {
	static Pixel white = {0xFFFFFF};
	Tile *tile = param;
	scheduleAnimation(Time, FADE_RUN_TIME,
									  2*tile->x, 2*tile->x,
									  2*tile->y, 2*tile->y,
									  white, colors[tile->value],
									  showTile, tile);
}

void addTile(Board *b, uint8_t x, uint8_t y, uint8_t value, bool animated) {
	static Pixel black = {0x000000};
	static Pixel white = {0xFFFFFF};
	uint64_t localTime = Time;
	Tile *tile = malloc(sizeof(Tile));
	tile->x = x;
	tile->y = y;
	tile->value = value;
	tile->hidden = 0;
	if (animated) {
		tile->hidden++;
		scheduleAnimation(localTime, FADE_RUN_TIME/2,
											2*x, 2*x, 2*y, 2*y,
											black, white,
											fadeToColor, tile);
		b->inputTime = localTime + FADE_RUN_TIME + FADE_RUN_TIME/2;
	}
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
	Tile *tile = param;
//	uartTxPoll(UART0, "Start fade anim\n\r");
	tile->hidden++;
	tile->value++;
	fadeToColor(tile);
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

bool shiftUp(Board *b) {
	bool ret = false;
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	uint8_t x, y, backY;
	if (!canTakeInput(b)) return false;
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
					ret = true;
				} else { //no match
					if (backY != y) {
						ret = true;
						spawnTranslateAnim(b, current, localTime, x, backY);
					}
					backY++;
					last = current;
				}
			}
		}
	}
	return ret;
}

bool shiftDown(Board *b) {
	bool ret = false;
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	int8_t x, y, backY;
	if (!canTakeInput(b)) return false;
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
					ret = true;
				} else { //no match
					if (backY != y) {
						ret = true;
						spawnTranslateAnim(b, current, localTime, x, backY);
					}
					backY--;
					last = current;
				}
			}
		}
	}
	return ret;
}

bool shiftLeft(Board *b) {
	bool ret = false;
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	uint8_t x, y, backX;
	if (!canTakeInput(b)) return false;
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
					ret = true;
				} else { //no match
					if (backX != x) {
						ret = true;
						spawnTranslateAnim(b, current, localTime, backX, y);
					}
					backX++;
					last = current;
				}
			}
		}
	}
	return ret;
}

bool shiftRight(Board *b) {
	bool ret = false;
	uint64_t localTime = Time;
	Tile *current;
	Tile *last;
	int8_t x, y, backX;
	if (!canTakeInput(b)) return false;
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
					ret = true;
				} else { //no match
					if (backX != x) {
						ret = true;
						spawnTranslateAnim(b, current, localTime, backX, y);
					}
					backX--;
					last = current;
				}
			}
		}
	}
	return ret;
}

void addRandomTile(Board *b) {
	uint8_t x, y;
	uint8_t value = (rand()%10 == 0);
	uint8_t emptyCount = countEmpty(b);
	uint8_t selection = rand()%emptyCount;
	getNthEmptyTile(b, selection, &x, &y);
	if (x == (uint8_t)(-1)) return;
	addTile(b, x, y, value, true);
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
