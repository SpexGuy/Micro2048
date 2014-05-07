#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "systick.h"
#include "renderer.h"
#include "UART.h"

extern volatile bool AlertADC0;

volatile uint64_t Time = 0;

 /****************************************************************************
 * The SysTick Handler 
 ****************************************************************************/
void SYSTICKIntHandler(void)
{
	static int adcCount = 0;
	static int row = 0;
	static byteFraction dutyPos = 0;
	
	Time++;

	if (adcCount++ > SYSTICKS_PER_ADC) {
		AlertADC0 = true;
		adcCount = 0;
	}
	
	if (++row == 8) {
		row = 0;
		dutyPos += DUTY_CYCLE_DELTA;
	}
	updateRow(row, dutyPos);
}


/****************************************************************************
 * Initialize the SysTick timer to a given count.
 * Optionally turn on interrupts
 ****************************************************************************/
void initializeSysTick(uint32_t count, bool enableInterrupts)
{
	//disable sysTick
	NVIC_ST_CTRL_R = 0;
	//set reload register
	NVIC_ST_RELOAD_R = count - 1;
	//empty current register
	NVIC_ST_CURRENT_R = 0;
	//enable sysTick
	if (enableInterrupts)
		NVIC_ST_CTRL_R |= NVIC_ST_CTRL_INTEN;
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}
