#include "constants.h"
#include "circular_buffer.h"
#include "lock.h"
#include "uart.h"

CircularBuffer rxBuffer[2];
CircularBuffer txBuffer[2];
UART_PERIPH *uarts[2] = {(UART_PERIPH *)UART2, (UART_PERIPH *)UART5};
uint32_t rcgcuart[2] = {SYSCTL_RCGCUART_R2, SYSCTL_RCGCUART_R5};

bool initUART(uint8_t uartIndex, uint32_t baud) {
  uint32_t delay;
	uint32_t divisor, rawRemainder, result;
	UART_PERIPH *uart = uarts[uartIndex];
	
	if (baud != 9600 && baud != 115200)
		return false;
	//attempt to initialize buffers
	if (cBufInit(&rxBuffer[uartIndex], 16) != 0)
		return false;
	if (cBufInit(&txBuffer[uartIndex], 16) != 0)
		return false;

	StartCritical();  
  // *******************************
  // Set up the UART registers
  // *******************************
	
  // Enable the clock gating register
  // ( Not found in the UART_PERIPH struct)
  SYSCTL_RCGCUART_R |= rcgcuart[uartIndex];  // 5
  delay = SYSCTL_RCGCUART_R;
	UNUSED(delay);
  delay = 500;
  while(delay --> 0);
	//disable uart
	uart->UARTControl &= ~UART_CTL_UARTEN;
	
  // Set the baud rate
	divisor = 16 * baud;
  uart->IntegerBaudRateDiv = CLOCK_FREQUENCY / divisor;     // 6
	//calculate the remainder
	rawRemainder = CLOCK_FREQUENCY % divisor;
	//convert to 16-bit number, rounding
	result = (rawRemainder*64 + divisor/2) / divisor;
  uart->FracBaudRateDiv = result;                           // 7
	
	// Configure the Line Control for 8-n-1
  uart->LineControl |= UART_LCRH_WLEN_8 | UART_LCRH_FEN;    // 8
	// Enable both TX and RX
  uart->UARTControl = UART_CTL_RXE | UART_CTL_TXE;          // 9
	
	// Enable only rx queue interrupts (tx enabled on tx attempted)
	uart->IntMask |= UART_IM_TXIM | UART_IM_RXIM | UART_IM_RTIM;
	
	// rx and tx as fast as possible
	uart->IntFIFOLevelSel = UART_IFLS_RX1_8 | UART_IFLS_TX7_8;
	
	// Enable the uart
	uart->UARTControl |= UART_CTL_UARTEN;
  
	// Enable the uart interrupt
	NVIC_EN0_R |= NVIC_EN0_INT5;
	
	// Wait until the UART is avaiable
  while(!(SYSCTL_PRUART_R & SYSCTL_PRUART_R0));
  // Plus a little more
  delay = 500;
  while(delay --> 0);
			
	EndCritical();
  return true;
}

uint8_t uartRx(uint8_t uartId, bool block) {
	uint8_t recvd = 0xFF;
	StartCritical();
	while((cBufGetChar(&rxBuffer[uartId], (char *)&recvd) > 0) && block) {
		EndCritical();
		//give the interrupt a chance to run
		StartCritical();
	}
	EndCritical();
	return recvd;
}

void uartTx(uint8_t uartId, uint8_t data) {
	UART_PERIPH *uart = uarts[uartId];
	StartCritical();
	//check hardware queue
	if (uart->Flag & UART_FR_TXFF) {
		//wait for space in tx buffer
//		while(cBufGetFreeCount(&txBuffer[uartId]) == 0) {
//			EndCritical();
//			//give the interrupt a chance to run
//			StartCritical();
//		}
		if (cBufAddChar(&txBuffer[uartId], (char)data)) {
			cBufAddChar(&txBuffer[uartId], 'X');
		}
	} else {
		//put data in hw queue
		uart->Data = (char)data;
	}
	//enable tx interrupts
	uart->IntMask |= UART_IM_TXIM;
	EndCritical();
}

void UART2IntHandler() {
	char toSend = 0;
	StartCritical();
	uartTxPoll(UART0, "U2");
	//add characters until receive queue is empty
  while(!(uarts[0]->Flag & UART_FR_RXFE)) {
		cBufAddChar(&rxBuffer[0], uarts[0]->Data);
	}
	
	//check if uP has characters
	if (cBufGetChar(&txBuffer[0], &toSend) > 0) {
		//nothing to send - disable tx interrupts
		uarts[0]->IntMask &= ~UART_IM_TXIM;
	} else {
		//send characters until tx fifo is full or
		//tx buffer is empty
		do {
			uarts[0]->Data = toSend;
		} while(!(uarts[0]->Flag & UART_FR_TXFF) &&
						cBufGetChar(&txBuffer[0], &toSend) == 0);
	}

	EndCritical();
}

void UART5IntHandler() {
	char toSend = 0;
	StartCritical();
	uartTxPoll(UART0, "U5");
	//add characters until receive queue is empty
  while(!(uarts[1]->Flag & UART_FR_RXFE)) {
		cBufAddChar(&rxBuffer[1], uarts[1]->Data);
	}
	
	//check if uP has characters
	if (cBufGetChar(&txBuffer[1], &toSend) > 0) {
		//nothing to send - disable tx interrupts
		uarts[1]->IntMask &= ~UART_IM_TXIM;
	} else {
		//send characters until tx fifo is full or
		//tx buffer is empty
		do {
			uarts[1]->Data = toSend;
		} while(!(uarts[1]->Flag & UART_FR_TXFF) &&
						cBufGetChar(&txBuffer[1], &toSend) == 0);
	}

	EndCritical();
}
