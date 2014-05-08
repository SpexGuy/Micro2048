#include "hw4_gpio.h"
#include "led_chars.h"
#include "UART.h"
#include "board_ports.h"
#include "renderer.h"

#define NUM_BUTTONS 5

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
extern void uartTxPoll(uint32_t base, char *data);
extern void StartCritical(void);
extern void EndCritical(void);

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
	int delay;
	bool locked;
	UNUSED(delay); //delay is for delaying a clock cycle
	//configure data registers
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	delay = SYSCTL_RCGCGPIO_R;
	PortB->DigitalEnable = 0xFF;
	PortB->Direction = 0xFF;
	
	//configure /OE
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
	delay = SYSCTL_RCGCGPIO_R;
	//port F may be locked
	locked = PortF->Lock;
	if (locked)
		PortF->Lock = 0x4C4F434B;
	PortF->DigitalEnable |= PIN_4 | PIN_1;
	PortF->Direction |= PIN_4;
	PortF->Direction &= ~PIN_1;
	PortF->PullUpSelect |= PIN_1;
	//restore lock to original status
	if (locked)
		PortF->Lock = 0x4C4F434B;
	
	//configure switches
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
	delay = SYSCTL_RCGCGPIO_R;
	PortA->DigitalEnable |= PIN_6 | PIN_7;
	PortA->Direction &= ~(PIN_6 | PIN_7);
	PortA->PullUpSelect |= PIN_6 | PIN_7;
	
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
	delay = SYSCTL_RCGCGPIO_R;
	PortD->DigitalEnable |= PIN_2 | PIN_3 | PD6_U2_RX | PD7_U2_TX;
	PortD->Direction &= ~(PIN_2 | PIN_3);
	PortD->PullUpSelect |= PIN_2 | PIN_3;
	PortD->AlternateFunctionSelect |= PD6_U2_RX | PD7_U2_TX;
	PortD->PortControl |= GPIO_PCTL_PD6_U2RX | GPIO_PCTL_PD7_U2TX;
	
	//configure ADC0 input
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	delay = SYSCTL_RCGCGPIO_R;
	PortE->Direction &= ~PIN_3;
	PortE->DigitalEnable &= ~PIN_3;
	PortE->DigitalEnable |= PE4_U5_RX | PE5_U5_TX;
	PortE->AlternateFunctionSelect |= PIN_3 | PE4_U5_RX | PE5_U5_TX;
	PortE->AnalogSelectMode |= PIN_3;
	PortE->PortControl |= GPIO_PCTL_PE4_U5RX | GPIO_PCTL_PE5_U5TX;
}

