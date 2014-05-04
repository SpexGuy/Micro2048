#include "AI2048.h"
#include <stdlib.h>


uint8_t getNextMove(Board *b) {
	return rand() % 4;
}


