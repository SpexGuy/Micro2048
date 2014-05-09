#include "constants.h"
#include "circular_buffer.h"
#include "lock.h"
#include "uart.h"

#define NVIC_EN1_INT33 (1<<(33%32))
#define NVIC_EN1_INT61 (1<<(61%32))

CircularBuffer rxBuffer[2];
CircularBuffer txBuffer[2];
UART_PERIPH *uarts[2] = {(UART_PERIPH *)UART2, (UART_PERIPH *)UART5};
uint32_t rcgcuart[2] = {SYSCTL_RCGCUART_R2, SYSCTL_RCGCUART_R5};
uint32_t pruart[2] = {SYSCTL_PRUART_R2, SYSCTL_PRUART_R5};

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
			NVIC_PRI8_R |= ((NVIC_PRI8_R & 0xFFFF00FF) | 0x00000400);
			NVIC_EN1_R |= NVIC_EN1_INT33;
			break;
		case UART_ID_5 :
			NVIC_PRI15_R |= ((NVIC_PRI15_R & 0xFFFF00FF) | 0x00000400);
			NVIC_EN1_R |= NVIC_EN1_INT61;
			break;
		default:
			return false;
	}
	
	// Wait until the UART is avaiable
  while(!(SYSCTL_PRUART_R & pruart[uartIndex]));
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
	if (uart->Flag & UART_FR_TXFF || txBuffer[uartId].count > 0) {
		if (cBufAddChar(&txBuffer[uartId], (char)data) != 0) {
			debugPrint("Failed to add to SW queue\n\r");
		}
	} else {
		//put data in hw queue
		uart->Data = (char)data;
	}
	//enable tx interrupts
	uart->IntMask |= UART_IM_TXIM;
	EndCritical();
}

void uartTxArray(uint8_t uartId, uint8_t len, uint8_t *data) {
	uint8_t c;
	for (c = 0; c < len; c++) {
		uartTx(uartId, data[c]);
	}
}

void uartRxArray(uint8_t uartId, uint8_t len, uint8_t *data) {
	uint8_t c;
	for (c = 0; c < len; c++) {
		data[c] = uartRx(uartId, true);
	}
}

void UARTIntHandler(uint8_t uartId) {
	char toSend = 0;
	
#	ifdef _DEBUG_
	switch(uartId) {
		case UART_ID_5:
			debugPrint("U5: ");
			break;
		case UART_ID_2:
			debugPrint("U2: ");
			break;
		default:
			debugPrint("U?: ");
			break;
	}
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_CTSMIS)
		debugPrint("Clear to Send Modem:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_RXMIS)
		debugPrint("Receive:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_TXMIS)
		debugPrint("Transmit:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_RTMIS)
		debugPrint("Receive Time-Out:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_FEMIS)
		debugPrint("Framing Error:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_PEMIS)
		debugPrint("Parity Error:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_BEMIS)
		debugPrint("Break Error:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_OEMIS)
		debugPrint("Overrun Error:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_9BITMIS)
		debugPrint("9-Bit Mode:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_LMSBMIS)
		debugPrint("LIN Mode Sync Break:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_LMSBMIS)
		debugPrint("LIN Mode Edge 1:");
	if(uarts[uartId]->MaskedIntStatus & UART_MIS_LME5MIS)
		debugPrint("LIN Mode Edge 5:");
	if(uarts[uartId]->RxStatus & UART_RSR_FE)
		debugPrint("Framing Error:");
	if(uarts[uartId]->RxStatus & UART_RSR_PE)
		debugPrint("Parity Error:");
	if(uarts[uartId]->RxStatus & UART_RSR_BE)
		debugPrint("Break Error:");
	if(uarts[uartId]->RxStatus & UART_RSR_OE)
		debugPrint("Overrun Error:");
	debugPrint("\n\r");
#	endif
			
	StartCritical();
	//add characters until receive queue is empty
  while(!(uarts[uartId]->Flag & UART_FR_RXFE)) {
		cBufAddChar(&rxBuffer[uartId], uarts[uartId]->Data);
	}
	if(uarts[uartId]->RxStatus & UART_RSR_OE)
		print("Overrun Error\n\r");
	
	//check if uP has characters
	if (cBufGetChar(&txBuffer[uartId], &toSend) > 0) {
		//nothing to send - disable tx interrupts
		uarts[uartId]->IntMask &= ~UART_IM_TXIM;
	} else {
		//send characters until tx fifo is full or
		//tx buffer is empty
		do {
			uarts[uartId]->Data = toSend;
		} while(!(uarts[uartId]->Flag & UART_FR_TXFF) &&
						cBufGetChar(&txBuffer[uartId], &toSend) == 0);
	}

	EndCritical();
}

void UART2IntHandler() {
	UARTIntHandler(UART_ID_2);
}

void UART5IntHandler() {
	UARTIntHandler(UART_ID_5);
}
