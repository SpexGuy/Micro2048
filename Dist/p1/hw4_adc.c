#include "hw4_adc.h"

//******************************************************
// Global Variables provided by other files
// Do not modify
//******************************************************
extern GPIO_PORT *PortA ;
extern GPIO_PORT *PortB ;
extern GPIO_PORT *PortC ;
extern GPIO_PORT *PortD ;
extern GPIO_PORT *PortE ;
extern GPIO_PORT *PortF ;

//******************************************************
// Functions provided by ECE staff
// Do not modify
//******************************************************
extern void StartCritical(void);
extern void EndCritical(void);

volatile uint16_t RefreshRate = 0;
volatile bool AlertADC0 = false;

/****************************************************************************
 * Initialization code for the ADC
 * with SS3, Software trigger, 125KHz
 ****************************************************************************/
//variable the compiler can't optimize
volatile int c;
void initializeADC(void)
{
	//configure potentiometer 0
	SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_R0;
	for (c = 0; c < 1000; c++); //wait for initialization
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN3;
	//processor triggered
	ADC0_EMUX_R |= ADC_EMUX_EM3_PROCESSOR;
	//clear sample input select
	ADC0_SSMUX3_R &= ~ADC_SSMUX3_MUX0_M;
	//single-sample, interrupt-enabled.
	ADC0_SSCTL3_R = ADC_SSCTL3_END0 | ADC_SSCTL3_IE0;
	// Clear Averaging Bits
	ADC0_SAC_R &= !0x7;
	// Average 64 samples
	ADC0_SAC_R |= ADC_SAC_AVG_64X;
}

//*****************************************************************************
// Get the ADC value of a given ADC Channel
//*****************************************************************************
uint32_t GetADCval(uint32_t Channel)
{
  uint32_t result;

  ADC0_SSMUX3_R = Channel;      // Set the channel
  ADC0_ACTSS_R  |= ADC_ACTSS_ASEN3; // Enable SS3
  ADC0_PSSI_R = ADC_PSSI_SS3;     // Initiate SS3

  while(0 == (ADC0_RIS_R & ADC_RIS_INR3)); // Wait for END of conversion
  result = ADC0_SSFIFO3_R & 0x0FFF;     // Read the 12-bit ADC result
  ADC0_ISC_R = ADC_ISC_IN3;         // Acknowledge completion

  return result;
}

//*****************************************************************************
// The refresh rate will go from 0-99Hz.
//*****************************************************************************
void updateRefreshRate(void)
{
	uint32_t adc;
	if (!AlertADC0) return;
	AlertADC0 = false;
	//get left adc
	adc = GetADCval(1);
	//set refresh rate between 0 and 99
	RefreshRate = (adc * 100) / 4096;
}
