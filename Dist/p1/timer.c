#include "timer.h"
#include "constants.h"
#include "lm4f120h5qr.h"
#include "animation.h"
#include "UART.h"

extern volatile bool AlertDebounce;

void initTimer0(void) {
	uint8_t delay;
	UNUSED(delay);
		
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0;
	delay = SYSCTL_RCGC1_R;
	TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
	TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;
//	TIMER0_CFG_R = TIMER_CFG_16_BIT;
	TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER0_TAILR_R = TIMER0_COUNT;
	
	//TIMER0_TAPR_R = TIMER0_PRESCALE;
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	TIMER0_IMR_R |= TIMER_IMR_TATOIM;
	//Enable Interupt
	NVIC_PRI4_R = ((NVIC_PRI4_R & 0x00FFFFFF) | 0x40000000);
	NVIC_EN0_R = 0x00080000;
	TIMER0_CTL_R |= TIMER_CTL_TAEN;
}



void TIMER0IntHandler(void) {
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	AlertDebounce = true;
	//uartTxPoll(UART0,"H");
}

