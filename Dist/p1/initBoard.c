#include "initBoard.h"
#include "systick.h"
#include "hw4_gpio.h"

extern GPIO_CONFIG portA_config;
extern GPIO_CONFIG portB_config;
extern GPIO_CONFIG portC_config;
extern GPIO_CONFIG portD_config;
extern GPIO_CONFIG portE_config;
extern GPIO_CONFIG portF_config;
extern UART_CONFIG UART0_config;

extern void PLL_Init(void);
extern bool uartInitPolling(uint32_t base);
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
  
  initPortC();
  
  uartInitPolling(UART0);
  
  // Initialize GPIO Pins
  initializeGpioPins();
  
  // Initialize ADC0
  initializeADC();
  
  // Need to wait a while before the UART is functional
  while(i < 10000)
  {
    i++;
  }

  // Initialize the SysTick Timer
  initializeSysTick(SYSTICK_COUNT, true);
}
