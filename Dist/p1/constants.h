#ifndef _CONSTANTS_H
#define _CONSTANTS_H 1

#define _LITTLE_ENDIAN 1

#include <stdint.h>


// #define _DEBUG_ 1

#define DUTY_CYCLE_DELTA 					(8)
#define DUTY_CYCLE_MAX 						(256)
#define DUTY_CYCLE_GRADIATIONS 		(DUTY_CYCLE_MAX/DUTY_CYCLE_DELTA)
#define SCREEN_REFRESH_RATE				(80)//Hz
#define SCREEN_WIDTH							(8)
#define SCREEN_HEIGHT							(8)
#define CLOCK_FREQUENCY						(80000000)
// Sets the value that will be loaded into the SysTick Timers
// Load Register.
#define SYSTICKS_PER_SECOND				(DUTY_CYCLE_GRADIATIONS * SCREEN_HEIGHT * SCREEN_REFRESH_RATE)
#define TIMER0_TICKS_PER_SECOND		(56)
#define TIMER0_PRESCALE						(1)
#define SYSTICK_COUNT   					(CLOCK_FREQUENCY / SYSTICKS_PER_SECOND)
#define TIMER0_COUNT							(CLOCK_FREQUENCY / (TIMER0_TICKS_PER_SECOND * TIMER0_PRESCALE))

//button check every 18 mS
#define SYSTICKS_PER_BUTTON				((SYSTICKS_PER_SECOND * 18)/1000)
//adc check every 1 mS
#define SYSTICKS_PER_ADC					((SYSTICKS_PER_SECOND * 1)/1000)
#define SAVE_HOLD_THRESH					(SYSTICKS_PER_SECOND)

typedef uint8_t byteFraction;
#define BF_1 255
#define BF_0 0

//get the fractional part of a/b
#define fract(a, b) ((byteFraction)((256*((uint32_t)(a)))/((uint32_t)(b))))
#define fract1(a, b) ((byteFraction)((255*((uint32_t)(a)))/((uint32_t)(b))))
//multiply by a byteFraction
#define bmult(a, b) (((a)*(b))>>8)
//equivalent of 1.0f-b for a byteFraction
#define bcomp(b) (BF_1-(b))
//lerp with a byteFraction
#define blerp(a, b, n, d) (((d)*(a)-(a)*(n)+(b)*(n))/(d))
//get the fractional part of a lerp with a byteFraction
#define blerpfrac(a, b, n, d) (fract((d)*(a)-(a)*(n)+(b)*(n), d))

#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))
#define absVal(a) (((a)<0)?(-(a)):(a))
//remove unused variable errors for expr
#define UNUSED(expr) do{(void)(expr);}while(0)
//make this feel more like C++
#define new(x) ((x*)malloc(sizeof(x)))
#define delete(x) (free(x))

#endif
