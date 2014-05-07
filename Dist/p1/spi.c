#include "spi.h"
#include "lm4f120h5qr.h"
#include "stdlib.h"
#include <stdint.h>
#include "gpio.h"

#define FSSI_CLK_BIT_RATE (80000000 / (SPI_CLK_CPSDVSR * 1))

SPI_PERIPH *spi;

// Forwards
void initializePortASpi0(void);

void spiTx(uint8_t *dataIn, int32_t size, uint8_t *dataOut) {
	uint16_t c;
	//Wait for not busy
	while(spi->SSISR == SSI_SR_BSY);
	for(c = 0; c < size; c++)
		spi->SSIDR = dataIn[c];
	
	spi->SSICR1 |= SSP_EN;
	
	while(spi->SSISR == SSI_SR_BSY);
	for(c = 0; c < size; c++)
		dataOut[c] = spi->SSIDR;
}

bool initializeSPI( uint32_t base, uint8_t phase, uint8_t polarity) {
  uint32_t delay;
	UNUSED(delay);
  // Turn on the Clock Gating Register
  switch (base) 
  {
    case SSI0 :
    {
      // Configure GPIO Port A to support SSI0/SPI0
      initializePortASpi0();
      SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R0;
        break;
    }
    default:
        return false;
  }
  
  delay = SYSCTL_RCGCSSI_R;

  // Disable the SSI interface
  spi->SSICR1 &= ~0x2;

  // Enable Master Mode
  spi->SSICR1 = SSI_MASTER_EN;
  
  // Assume that we hvae a 80MHz clock and want a 4MHz SPI clock
  // FSSIClk = FSysClk / (CPSDVSR * (1 + SCR))
  spi->SSICPSR = 1;
  spi->SSICR0  = FSSI_CLK_BIT_RATE;
  
  // Clear the phse and polarity bits
  spi->SSICR0  &=  ~(SSI_CR0_SPH | SSI_CR0_SPO);
  
  if (phase == 1)
      spi->SSICR0  = SSI_CR0_SPH;
  
  if (polarity ==1)
      spi->SSICR0  |= SSI_CR0_SPO;

  // Freescale SPI Mode with 8-Bit data (See line 2226 of lm4f120h5qr.h)
  spi->SSICR0  = SSI_CR0_FRF_MOTO | SSI_CR0_DSS_9;
  
  //Enable SSI
  spi->SSICR1 |= 0x2;

  return true;
}


//**********Helpers**********//

// *******************************************
// Configure GPIO PA5-PA2 as SPI
// *******************************************
void initializePortASpi0(void)
{
  GPIO_PORT *GpioPortA = (GPIO_PORT*)PORTA;
  uint32_t delay;
	UNUSED(delay);
	
  // Turn on the clock gating register for GPIO port A
  // Make sure not to turn of any of the other ports
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  // Delay while clock starts up
  delay = SYSCTL_RCGCGPIO_R;
	delay = 1;
	delay = 2;
	delay = 3;
  // Set the 4 pins used for the SPI interface in the Digital Enable Register
	GpioPortA->DigitalEnable |= (PIN_2 | PIN_3 | PIN_4 | PIN_5);
  // Set the 4 pins used for the SPI interface in the Alternate Function Register
  GpioPortA->AlternateFunctionSelect |=  (PIN_2 | PIN_3 | PIN_4 | PIN_5);
  // Set the Port Control Register ( See lm4f120h5qr.h starting at line 2045)
  GpioPortA->PortControl = (GPIO_PCTL_PA2_SSI0CLK | GPIO_PCTL_PA3_SSI0FSS | 
														GPIO_PCTL_PA4_SSI0RX | GPIO_PCTL_PA5_SSI0TX);
}
