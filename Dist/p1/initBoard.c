#include "initBoard.h"
#include "systick.h"
#include "hw4_gpio.h"
#include "spi.h"
#include "timer.h"
#include "lock.h"

extern GPIO_CONFIG portA_config;
extern GPIO_CONFIG portB_config;
extern GPIO_CONFIG portC_config;
extern GPIO_CONFIG portD_config;
extern GPIO_CONFIG portE_config;
extern GPIO_CONFIG portF_config;
extern UART_CONFIG UART0_config;

extern void PLL_Init(void);
extern bool uartInitPolling(uint32_t base);

void initializeWatchdog(void);


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
  // Initialize the PLLs so the the main CPU frequency is 80MHz
  PLL_Init();
	StartCritical();
	initializeWatchdog();
  initializeSysTick(SYSTICK_COUNT, true);
	initTimer0();
	initializeGpioPins();
	uartInitPolling(UART0);
	if(!initUART(UART_ID_5, 115200)) {
		errorPrint("Failed to init UART 5");
	}
	if(!initUART(UART_ID_2, 115200)) {
		errorPrint("Failed to init UART 2");
	}
  initializeADC();
	initializeSPI(SSI0, 1, 1);
	EndCritical();
}

void initializeWatchdog(void) {
	uint8_t delay;
	UNUSED(delay);
	SYSCTL_RCGCWD_R |= 0x1;
	delay = SYSCTL_RCGCWD_R;
	WATCHDOG0_LOAD_R = WDT_LOAD_M;
	WATCHDOG0_CTL_R |= WDT_CTL_RESEN;
	WATCHDOG0_CTL_R |= WDT_CTL_INTEN;
}

