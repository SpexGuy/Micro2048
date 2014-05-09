#include "AI2048.h"
#include "UART.h"
#include <stdlib.h>
#include "lock.h"

#define IS_RANDOM 0

static const int MultiplyDeBruijnBitPosition[32] = 
{
  0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
  8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
};

// forwards
bool internalShiftUp(Board *b);
bool internalShiftDown(Board *b);
bool internalShiftLeft(Board *b);
bool internalShiftRight(Board *b);

int32_t evaluatePossibilities(Board *b);
int32_t getSmoothness(Board*);
uint32_t log2LT(uint32_t);
Tile* findFarthestPositionNext(Board*, Tile* , uint8_t, uint8_t);
uint8_t getRandomMove(Board *b);
uint8_t getBestMove(Board *b);
Board* copyBoard(Board *b);
void freeBoard(Board *b);

uint8_t getNextMove(Board *b) {
	if(IS_RANDOM) {
		return getRandomMove(b);
	} else {
		return getBestMove(b);
	}
}

uint8_t getBestMove(Board *b) {
	uint8_t c, minIndex = 0;
	int32_t min, evals[4];
	Board *op;

#ifdef _DEBUG_
	uartTxPoll(UART0, "Getting Best Move");
#endif
	
	op = copyBoard(b);
	if(internalShiftUp(op)) {
		evals[0] = evaluatePossibilities(op);
	} else {
		evals[0] = 0x7FFFFFFF;
	}
	freeBoard(op);
	
	op = copyBoard(b);
	if(internalShiftDown(op)) {
			evals[1] = evaluatePossibilities(op);
	} else {
		evals[1] = 0x7FFFFFFF;
	}
	freeBoard(op);
	
	op = copyBoard(b);
	if(internalShiftRight(op)) {
			evals[2] = evaluatePossibilities(op);
	} else {
		evals[2] = 0x7FFFFFFF;
	}
	freeBoard(op);
	
	op = copyBoard(b);
	if(internalShiftLeft(op)) {
		evals[3] = evaluatePossibilities(op);
	} else {
		evals[3] = 0x7FFFFFFF;
	}
	freeBoard(op);
	
	min = 0x7FFFFFFF;
	for(c = 0; c < 4; c++) {
		if(evals[c] <	min) {
			min = evals[c];
			minIndex = c;
		}
	}
#ifdef _DEBUG_
	uartTxPoll(UART0, "returning best move");
#endif
	return minIndex;
}

uint8_t getRandomMove(Board *b) {
	return rand() % 4;
}

int32_t evaluatePossibilities(Board *b) {
	uint8_t x, y;
	Tile* current;
	int32_t evaluation2, evaluation4;
	uint8_t open = 0;
	
	for(x = 0; x < BOARD_WIDTH; x++) {
		for(y = 0; y < BOARD_HEIGHT; y++) {
			current = b->tiles[y][x];
			if (current == NULL) {
				open++;
				addTile(b, x, y, 0, false);
				current = b->tiles[y][x];
				evaluation2 += (getSmoothness(b) * 9);
				current->value = 1;
				evaluation4 += (getSmoothness(b)*1);
				removeTile(b, current);
			}
		}
	}
	
	if(open > 0) {
		return (evaluation2 + evaluation4) / (10*open);
	} else {
#		ifdef _DEBUG_
		uartTxPoll(UART0, "Loss Detected inside AI");
#		endif
		return 0x7FFFFFFF;
	}
}

int32_t getSmoothness(Board *b) {
	int32_t smoothness = 0;
	uint8_t x, y, direction;
	Tile *current, *targetTile;
	int32_t value, targetValue;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			current = b->tiles[y][x];
			if (current != NULL) {
				value = (uint32_t)current->value;
				for (direction=0; direction<=1; direction++) {
          targetTile = findFarthestPositionNext(b, current, direction, 1-direction);
					if (targetTile->value != NULL) {
            targetValue = targetTile->value;
            smoothness -= absVal(value - targetValue);
          }
        }
			}
		}
	}
	return smoothness;
}


Tile* findFarthestPositionNext(Board* b, Tile* tile, uint8_t dx, uint8_t dy) {
	Tile* current;
	uint8_t x, y;
	current = tile;
	if (tile == NULL)
		return NULL;
  // Progress towards the vector direction until an obstacle is found
	for (x = tile->x, y = tile->y;
				x < BOARD_WIDTH && y < BOARD_HEIGHT;
			x+= dx, y += dy)
	{
		if (b->tiles[y][x] == NULL)
			return current;
		current = b->tiles[y][x];
	}
	return NULL;
}

//---------Helpers----------//


bool internalShiftUp(Board *b) {
	bool ret = false;
	Tile *current;
	Tile *last;
	uint8_t x, y, backY;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = 0;
		last = NULL;
		for (y = 0; y  < BOARD_HEIGHT; y++) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					// Merge
					current->value++;
					last = NULL;
					ret = true;
				} else { //no match
					if (backY != y) {
						ret = true;
						// Translate
						current->x = x;
						current->y = backY;
					}
					backY++;
					last = current;
				}
			}
		}
	}
	return ret;
}


bool internalShiftDown(Board *b) {
	bool ret = false;
	Tile *current;
	Tile *last;
	int8_t x, y, backY;
	for (x = 0; x < BOARD_WIDTH; x++) {
		backY = BOARD_HEIGHT-1;
		last = NULL;
		for (y = BOARD_HEIGHT-1; y >= 0; y--) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					// Merge
					current->value++;
					last = NULL;
					ret = true;
				} else { //no match
					if (backY != y) {
						ret = true;						
						// Translate
						current->x = x;
						current->y = backY;
					}
					backY--;
					last = current;
				}
			}
		}
	}
	return ret;
}


bool internalShiftLeft(Board *b) {
	bool ret = false;
	Tile *current;
	Tile *last;
	uint8_t x, y, backX;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = 0;
		last = NULL;
		for (x = 0; x  < BOARD_WIDTH; x++) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					// Merge
					current->value++;
					last = NULL;
					ret = true;
				} else { //no match
					if (backX != x) {
						ret = true;
						// Translate
						current->x = backX;
						current->y = y;
					}
					backX++;
					last = current;
				}
			}
		}
	}
	return ret;
}

bool internalShiftRight(Board *b) {
	bool ret = false;	
	Tile *current;
	Tile *last;
	int8_t x, y, backX;
	for (y = 0; y < BOARD_HEIGHT; y++) {
		backX = BOARD_WIDTH-1;
		last = NULL;
		for (x = BOARD_WIDTH-1; x >= 0; x--) {
			current = b->tiles[y][x];
			if (current) { //space is occupied
				if (last != NULL && current->value == last->value) { //matched
					// Merge
					current->value++;
					last = NULL;
					ret = true;
				} else { //no match
					if (backX != x) {
						ret = true;
						// Translate
						current->x = backX;
						current->y = y;
					}
					backX--;
					last = current;
				}
			}
		}
	}
	return ret;
}


Board* copyBoard(Board *b) {
	uint8_t x, y;
	Board* copy = new(Board);

	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			Tile *currentTile, *newTile;
			currentTile = b->tiles[y][x];
			newTile = NULL;
			if(currentTile != NULL) {
					newTile = new(Tile);
					newTile->x = currentTile->x;
					newTile->y = currentTile->y;
					newTile->value = currentTile->value;
					newTile->hidden = currentTile->hidden;
			}
			copy->tiles[y][x] = newTile;
		}
	}
	copy->inputTime = b->inputTime;
	copy->score = b->score;
	return copy;
}

void freeBoard(Board *b) {
	uint8_t x, y;
	StartCritical();
	for (y = 0; y < BOARD_HEIGHT; y++) {
		for (x = 0; x < BOARD_WIDTH; x++) {
			delete(b->tiles[y][x]);
		}
	}
	delete(b);
	EndCritical();
}

//Thanks to http://graphics.stanford.edu/~seander/bithacks.html for the log2
uint32_t log2LT(uint32_t v) {
	v |= v >> 1; // first round down to one less than a power of 2 
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}
