#include "initBoard.h"
#include "systick.h"
#include "hw4_gpio.h"
#include "spi.h"
#include "timer.h"

extern GPIO_CONFIG portA_config;
extern GPIO_CONFIG portB_config;
extern GPIO_CONFIG portC_config;
extern GPIO_CONFIG portD_config;
extern GPIO_CONFIG portE_config;
extern GPIO_CONFIG portF_config;
extern UART_CONFIG UART0_config;

extern void PLL_Init(void);
extern bool uartInitPolling(uint32_t base);
extern void uartTxPoll(uint32_t base, char *data);
extern void initPortC(void);

/******************************************************************************
 * Global Variables
 *****************************************************************************/
 GPIO_PORT *PortA = (GPIO_PORT *)PORTA;
 GPIO_PORT *PortB = (GPIO_PORT *)PORTB;
 GPIO_PORT *PortC = (GPIO_PORT *)PORTC;
 GPIO_PORT *PortD = (GPIO_PORT *)PORTD;
 GPIO_PORT *PortE = (GPIO_PORT *)PORTE;
 GPIO_PORT *PortF = (GPIO_PORT *)PORTF;
 
/******************************************************************************
 * Initialize the PLL, PortC, GPIO Pins, ADC, and UART
 *****************************************************************************/
void initBoard(void)
{
   int i = 0;
  // Initialize the PLLs so the the main CPU frequency is 80MHz
  PLL_Init();
	StartCritical();
  initializeSysTick(SYSTICK_COUNT, true);
	initTimer0();
	initPortC();
	initializeGpioPins();
	uartInitPolling(UART0);
	//uart 2 and 5
  initializeADC();
	initializeSPI(SSI0, 1, 1);
	uartTxPoll(UART0, "ending critical\r\n");
	EndCritical();
	uartTxPoll(UART0, "ended critical\r\n");
  
  // Need to wait a while before the UART is functional
  while(i < 10000)
  {
    i++;
  }

//	if(!initializeSPI(SSI0, 0, 0)) {
//		uartTxPoll(UART0,"Failed to init SPI");
//	}
	i = 0;
	// Need to wait a while before the UART is functional
  while(i < 10000)
  {
    i++;
  }

}


