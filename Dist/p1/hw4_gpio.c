#include "hw4_gpio.h"
#include "led_chars.h"
#include "UART.h"
#include "board_ports.h"
#include "renderer.h"

#define NUM_BUTTONS 5

extern GPIO_PORT *PortA ;
extern GPIO_PORT *PortB ;
extern GPIO_PORT *PortC ;
extern GPIO_PORT *PortD ;
extern GPIO_PORT *PortE ;
extern GPIO_PORT *PortF ;

extern volatile uint16_t RefreshRate;

volatile bool AlertDebounce;

bool buttonPress[NUM_BUTTONS] = {false, false, false, false, false};
bool buttonRelease[NUM_BUTTONS] = {false, false, false, false, false};

uint8_t getButtonPress(uint8_t index) {
	if (buttonPress[index]) {
		buttonPress[index] = false;
		return true;
	}
	return false;
}

uint8_t getButtonRelease(uint8_t index) {
	if (buttonRelease[index]) {
		buttonRelease[index] = false;
		return true;
	}
	return false;
}

//*****************************************************************************
// The ISR sets AlertDebounce to true if the buttons should be examined.  
// If AlertDebounce is false, simply return
//
// If the ISR indicates that the buttons should be examined, the routine will
// debounces SW300-303.  If a button press is detected, it will print out a 
// message indicating with button was pressed.  
//
// When a button is pressed, update set Color to the appropriate value
//      SW300 – Display Off
//      SW301 – Refresh rate displayed in RED
//      SW302 – Refresh rate displayed in BLUE
//      SW303 – Refresh rate displayed in GREEN
//*****************************************************************************
void updateButtons(void)
{
	//loop iterators
	int c, d;
	//histeresis values
	static bool last[NUM_BUTTONS][3];
	if (!AlertDebounce) return;
	AlertDebounce = false;
	//shift old values
	for (c = 0; c < NUM_BUTTONS; c++) {
		for (d = 2; d > 0; d--) {
			last[c][d] = last[c][d-1];
		}
	}
	//read new values
	last[0][0] = !(PortA->Data & PIN_6);
	last[1][0] = !(PortA->Data & PIN_7);
	last[2][0] = !(PortD->Data & PIN_2);
	last[3][0] = !(PortD->Data & PIN_3);
	last[4][0] = !(PortF->Data & PIN_1);
	//for each button
	for (c = 0; c < NUM_BUTTONS; c++) {
		//check for button press
		if (last[c][0] && last[c][1] && !last[c][2]) {
			buttonPress[c] = true;
		}
		if (!last[c][0] && !last[c][1] && last[c][2]) {
			buttonRelease[c] = true;
		}
	}
}

//*****************************************************************************
// Initializes all of the GPIO pins used for the LED matrix, push 
// buttons, and the left potentiometer.
//*****************************************************************************
void initializeGpioPins(void)
{
	bool locked;
	int delay;
	UNUSED(delay);
	
	//Port A - SW2 and SW3
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
	delay = SYSCTL_RCGCGPIO_R;
	PortA->DigitalEnable |= PA6_SW2 | PA7_SW3;
	PortA->Direction &= ~(PA6_SW2 | PA7_SW3);
	PortA->PullUpSelect |= PA6_SW2 | PA7_SW3;
	
	//Port B - Data out
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	delay = SYSCTL_RCGCGPIO_R;
	PortB->DigitalEnable = 0xFF;
	PortB->Direction = 0xFF;
	
  //Port C - Output Select and JTAG interface
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTC_DEN_R   = 0xFF;
  GPIO_PORTC_DIR_R   = 0xF0;
  GPIO_PORTC_PUR_R   = 0x0F;
  GPIO_PORTC_AFSEL_R = 0x0F;
  GPIO_PORTC_PCTL_R  = (GPIO_PCTL_PC0_TCK | GPIO_PCTL_PC1_TMS | GPIO_PCTL_PC2_TDI |GPIO_PCTL_PC3_TDO);
	
	//Port D - UART2, SW4, and SW5
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
	delay = SYSCTL_RCGCGPIO_R;
	PortD->DigitalEnable |= PD2_SW4 | PD3_SW5 | PD6_U2_RX | PD7_U2_TX;
	PortD->Direction &= ~(PD2_SW4 | PD3_SW5);
	//PortD->Direction &= ~(PD6_U2_RX);
	//PortD->Direction |= PD7_U2_TX;
	PortD->PullUpSelect |= PD2_SW4 | PD3_SW5;
	PortD->AlternateFunctionSelect |= PD6_U2_RX | PD7_U2_TX;
	PortD->PortControl |= GPIO_PCTL_PD6_U2RX | GPIO_PCTL_PD7_U2TX;
	
	//Port E - ADC0 and UART5
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	delay = SYSCTL_RCGCGPIO_R;
	PortE->Direction &= ~PE3_ADC_0;
	//PortE->Direction &= ~(PE4_U5_RX);
	//PortE->Direction |= (PE5_U5_TX);
	PortE->DigitalEnable &= ~PE3_ADC_0;
	PortE->DigitalEnable |= PE4_U5_RX | PE5_U5_TX;
	PortE->AlternateFunctionSelect |= PE3_ADC_0 | PE4_U5_RX | PE5_U5_TX;
	PortE->AnalogSelectMode |= PE3_ADC_0;
	PortE->PortControl |= GPIO_PCTL_PE4_U5RX | GPIO_PCTL_PE5_U5TX;


	//Port F - /OE and SW6
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	delay = SYSCTL_RCGCGPIO_R;
	//port F may be locked
	locked = PortF->Lock;
	if (locked)
		PortF->Lock = 0x4C4F434B;
	PortF->DigitalEnable |= PF4_OUT_EN_B | PF1_SW6;
	PortF->Direction |= PF4_OUT_EN_B;
	PortF->Direction &= ~PF1_SW6;
	PortF->PullUpSelect |= PF1_SW6;
	//restore lock to original status
	if (locked)
		PortF->Lock = 0x4C4F434B;
}

