#ifndef _AI2048_H_
#define _AI2048_H_ 1

#include "2048.h"

// Returns an int 0-3 representing the direction to move. 
// 		0 is North, 3 is West, interpolate other directions
uint8_t getNextMove(Board *b);

#endif
