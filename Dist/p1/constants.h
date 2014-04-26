#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define _LITTLE_ENDIAN 1


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


#endif
