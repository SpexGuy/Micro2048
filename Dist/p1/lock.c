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
		while(1)
			uartTxPoll(UART0, "TOO MANY END CRITICALS!!!!!\r\n");
		
	depth--;
	if (depth == 0)
		EnableInterrupts();
}
