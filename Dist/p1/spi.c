#include "spi.h"
#include "lm4f120h5qr.h"
#include "stdlib.h"
#include <stdint.h>
#include "gpio.h"
#include "board_ports.h"

#define FSSI_CLK_BIT_RATE (80000000 / (SPI_CLK_CPSDVSR * 1))

extern GPIO_PORT *PortA;

SPI_PERIPH *spi;

// Forwards
void initializePortASpi0(void);

void spiTx(uint8_t *dataIn, int32_t size, uint8_t *dataOut) {
	uint16_t c;

	//Wait for not busy
	while(spi->SSISR & SSI_SR_BSY);
	
	for(c = 0; c < size; c++) {
		//wait for transmit not full
		while(!(spi->SSISR & SSI_SR_TNF));
		//send data
		spi->SSIDR = dataIn[c];
	}

	for(c = 0; c < size; c++) {
		//wait for data in receive fifo
		while(!(spi->SSISR & SSI_SR_RNE));
		//read data
		dataOut[c] = spi->SSIDR;
	}
}

bool initializeSPI( uint32_t base, uint8_t phase, uint8_t polarity) {
  SPI_PERIPH *myPeriph = (SPI_PERIPH *)base;
  uint32_t delay;
	UNUSED(delay);
	spi = myPeriph;

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
  myPeriph->SSICR1 &= ~(SSI_CR1_SSE);          // ###04###

  // Enable Master Mode
  myPeriph->SSICR1 = 0;                   // ###05###
  
  // Assume that we hvae a 80MHz clock and want a 4MHz SPI clock
  // SSICPSR * (1 + SSICR0) = 80M / 4M
  myPeriph->SSICPSR = 20;                  // ###06###
  myPeriph->SSICR0  = 0;                  // ###07###
  
  // Clear the phse and polarity bits
  myPeriph->SSICR0  &=  ~(SSI_CR0_SPH | SSI_CR0_SPO);
  
  if (phase == 1)
      myPeriph->SSICR0  |= SSI_CR0_SPH;
  
  if (polarity ==1)
      myPeriph->SSICR0  |= SSI_CR0_SPO;

  // Freescale SPI Mode with 8-Bit data (See line 2226 of lm4f120h5qr.h)
  myPeriph->SSICR0 |= SSI_CR0_DSS_8;                  // ###08###
  
  //Enable SSI
  myPeriph->SSICR1 |= SSI_CR1_SSE;                   // ###09###

  return true;
}


//**********Helpers**********//

// *******************************************
// Configure GPIO PA5-PA2 as SPI
// *******************************************
void initializePortASpi0(void)
{
  uint32_t delay;
	UNUSED(delay);
  
  // Turn on the clock gating register for GPIO port A
  // Make sure not to turn of any of the other ports
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
  
  // Delay while clock starts up
  delay = SYSCTL_RCGCGPIO_R;
  
  // Set the 4 pins used for the SPI interface in the Digital Enable Register
  PortA->DigitalEnable |= PA2_SPI0_CLK  |
													PA3_SPI0_CS   |
													PA4_SPI0_MISO |
													PA5_SPI0_MOSI;    // ###01###
  
  // Set the 4 pins used for the SPI interface in the Alternate Function Register
  PortA->AlternateFunctionSelect |= PA2_SPI0_CLK  |
																		PA3_SPI0_CS   |
																		PA4_SPI0_MISO |
																		PA5_SPI0_MOSI;  // ###02###
  
  // Set the Port Control Register ( See lm4f120h5qr.h starting at line 2045)
  PortA->PortControl |= GPIO_PCTL_PA2_SSI0CLK |
												GPIO_PCTL_PA3_SSI0FSS |
												GPIO_PCTL_PA4_SSI0RX  |
												GPIO_PCTL_PA5_SSI0TX;  // ###03###
}
