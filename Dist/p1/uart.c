#include "constants.h"
#include "circular_buffer.h"
#include "lock.h"
#include "uart.h"

#define NVIC_EN1_INT33 (0x00000002)
#define NVIC_EN1_INT61 (0x20000000)

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
  delay = 1000;
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
	switch(uartIndex) {
		case UART_ID_2 :
			NVIC_EN1_R |= NVIC_EN1_INT33;
			break;
		case UART_ID_5 :
			NVIC_EN1_R |= NVIC_EN1_INT61;
			break;
		default:
			return false;
	}
	
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
	uartTxPoll(UART0, "U2:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_CTSMIS)
		uartTxPoll(UART0, "Clear to Send Modem:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_RXMIS)
		uartTxPoll(UART0, "Receive:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_TXMIS)
		uartTxPoll(UART0, "Transmit:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_RTMIS) {
		uartTxPoll(UART0, "Receive Time-Out:");
	}
	if(uarts[0]->MaskedIntStatus & UART_MIS_FEMIS)
		uartTxPoll(UART0, "Framing Error:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_PEMIS)
		uartTxPoll(UART0, "Parity Error:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_BEMIS)
		uartTxPoll(UART0, "Break Error:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_OEMIS)
		uartTxPoll(UART0, "Overrun Error:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_9BITMIS)
		uartTxPoll(UART0, "9-Bit Mode:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_LMSBMIS)
		uartTxPoll(UART0, "LIN Mode Sync Break:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_LMSBMIS)
		uartTxPoll(UART0, "LIN Mode Edge 1:");
	if(uarts[0]->MaskedIntStatus & UART_MIS_LME5MIS)
		uartTxPoll(UART0, "LIN Mode Edge 5:");
	if(uarts[0]->RxStatus & UART_RSR_FE) {
		uartTxPoll(UART0, "Framing Error:");
		//uarts[0]->RxStatus = 0x1;
	}
	if(uarts[0]->RxStatus & UART_RSR_PE)
		uartTxPoll(UART0, "Parity Error:");
	if(uarts[0]->RxStatus & UART_RSR_BE)
		uartTxPoll(UART0, "Break Error:");
	if(uarts[0]->RxStatus & UART_RSR_OE)
		uartTxPoll(UART0, "Overrun Error:");					
	uartTxPoll(UART0, "\n\r");
	
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
	uartTxPoll(UART0, "U5: ");
	if(uarts[1]->MaskedIntStatus & UART_MIS_CTSMIS)
		uartTxPoll(UART0, "Clear to Send Modem:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_RXMIS)
		uartTxPoll(UART0, "Receive:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_TXMIS)
		uartTxPoll(UART0, "Transmit:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_RTMIS) {
		uartTxPoll(UART0, "Receive Time-Out:");
		uarts[1]->IntClear |= UART_ICR_RTIC;
	}
	if(uarts[1]->MaskedIntStatus & UART_MIS_FEMIS)
		uartTxPoll(UART0, "Framing Error:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_PEMIS)
		uartTxPoll(UART0, "Parity Error:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_BEMIS)
		uartTxPoll(UART0, "Break Error:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_OEMIS)
		uartTxPoll(UART0, "Overrun Error:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_9BITMIS)
		uartTxPoll(UART0, "9-Bit Mode:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_LMSBMIS)
		uartTxPoll(UART0, "LIN Mode Sync Break:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_LMSBMIS)
		uartTxPoll(UART0, "LIN Mode Edge 1:");
	if(uarts[1]->MaskedIntStatus & UART_MIS_LME5MIS)
		uartTxPoll(UART0, "LIN Mode Edge 5:");
	if(uarts[1]->RxStatus & UART_RSR_FE) {
		uartTxPoll(UART0, "Framing Error:");
		//uarts[1]->RxStatus = 0x1;
	}
	if(uarts[1]->RxStatus & UART_RSR_PE)
		uartTxPoll(UART0, "Parity Error:");
	if(uarts[1]->RxStatus & UART_RSR_BE)
		uartTxPoll(UART0, "Break Error:");
	if(uarts[1]->RxStatus & UART_RSR_OE)
		uartTxPoll(UART0, "Overrun Error:");					
	uartTxPoll(UART0, "\n\r");
	
			
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
