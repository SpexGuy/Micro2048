#include <stdbool.h>
#include "lm4f120h5qr.h"
#include "systick.h"
#include "renderer.h"
#include "UART.h"
#include "communication.h"

extern volatile bool AlertADC0;
extern volatile bool heartbeat;

volatile uint64_t Time = 0;

 /****************************************************************************
 * The SysTick Handler 
 ****************************************************************************/
void SYSTICKIntHandler(void)
{
	static int adcCount = 0;
//	static int debugCount = 0;
	static int row = 0;
	static byteFraction dutyPos = 0;
	static uint32_t heartbeatCount = 0;
	
	Time++;
	if (adcCount++ > SYSTICKS_PER_ADC) {
		AlertADC0 = true;
		adcCount = 0;
	}
	if (heartbeatCount++ > SYSTICKS_PER_HEARTBEAT) {
		heartbeat = true;
		heartbeatCount = 0;
	}
	if (++row == 8) {
		row = 0;
		dutyPos += DUTY_CYCLE_DELTA;
	}
	updateRow(row, dutyPos);
	
	// Timer Debug
	//if (debugCount++ > SYSTICKS_PER_SECOND) {
		//debugPrint("C");
		//debugCount = 0;
	//}
	if (isSinglePlayer()) {
		WATCHDOG0_LOAD_R = WDT_LOAD_M;
	}
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
