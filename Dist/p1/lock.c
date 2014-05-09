#include "lock.h"
#include "constants.h"
#include "UART.h"

extern void EnableInterrupts(void);
extern void DisableInterrupts(void);

uint8_t depth = 0;

void StartCritical() {
	DisableInterrupts();
	depth++;
}

void EndCritical() {
	if (depth == 0)
#		ifdef _debug_		
		while(1) 
			uartTxPoll(UART0, "TOO MANY END CRITICALS!!!!!\r\n");
#		else
		// Reset board
		NVIC_APINT_R = NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ;
#		endif
		
	depth--;
	if (depth == 0)
		EnableInterrupts();
}
