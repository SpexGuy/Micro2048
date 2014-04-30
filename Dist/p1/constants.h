#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define _LITTLE_ENDIAN 1

#include <stdint.h>

extern void StartCritical(void);
extern void EndCritical(void);

#define DUTY_CYCLE_DELTA 					(4)
#define DUTY_CYCLE_MAX 						(256)
#define DUTY_CYCLE_GRADIATIONS 		(DUTY_CYCLE_MAX/DUTY_CYCLE_DELTA)
#define SCREEN_REFRESH_RATE				(120)//Hz
#define SCREEN_WIDTH							(8)
#define SCREEN_HEIGHT							(8)
#define CLOCK_FREQUENCY						(80000000)
// Sets the value that will be loaded into the SysTick Timers
// Load Register.
#define SYSTICK_COUNT   					(CLOCK_FREQUENCY/(DUTY_CYCLE_GRADIATIONS*SCREEN_HEIGHT*SCREEN_REFRESH_RATE))

typedef uint8_t byteFraction;
#define BF_1 255
#define BF_0 0

//get the fractional part of a/b
#define fract(a, b) ((byteFraction)((256*((uint32_t)(a)))/((uint32_t)(b))))
#define fract1(a, b) ((byteFraction)((255*((uint32_t)(a)))/((uint32_t)(b))))
//multiply by a byteFraction
#define bmult(a, b) (((a)*(b))>>8)
//equivalent of 1.0f-b for a byteFraction
#define bcomp(b) (255-(b))
//lerp with a byteFraction
#define blerp(a, b, z) (bmult(a, bcomp(z))+bmult(b, z))
//get the fractional part of a lerp with a byteFraction
#define blerpfrac(a, b, n, d) (fract((d)*(a)-(a)*(n)+(b)*(n), d))
#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))

#endif
