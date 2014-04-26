#include "hw4_gpio.h"
#include "led_chars.h"
#include "UART.h"
#include "board_ports.h"
#include "renderer.h"

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

volatile uint8_t DutyPos = 0;
volatile bool AlertDebounce;
volatile bool AlertRowUpdate;
char Color = 'R';
volatile uint8_t Row = 0;


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
void examineButtons(void)
{
	//loop iterators
	int c, d;
	//map from button to color
	static char *colors = " RBG";
	//string for printing color
	static char colorPrint[2];
	//histeresis values
	static bool last[4][3];
	if (!AlertDebounce) return;
	AlertDebounce = false;
	//shift old values
	for (c = 0; c < 4; c++) {
		for (d = 2; d > 0; d--) {
			last[c][d] = last[c][d-1];
		}
	}
	//read new values
	last[0][0] = !(PortA->Data & PIN_6);
	last[1][0] = !(PortA->Data & PIN_7);
	last[2][0] = !(PortD->Data & PIN_2);
	last[3][0] = !(PortD->Data & PIN_3);
	//for each button
	for (c = 0; c < 4; c++) {
		//check for button press
		if (last[c][0] && last[c][1] && !last[c][2]) {
			//set color
			Color = colors[c];
			//print color
			colorPrint[0] = Color;
			colorPrint[1] = '\0';
			uartTxPoll(UART0, "Button ");
			uartTxPoll(UART0, colorPrint);
			uartTxPoll(UART0, " was pressed.\r\n");
			break;
		}
	}
}

//*****************************************************************************
// The ISR sets AlertRowUpdate to true if the display should be updated.
// The routine will call getLCDRow() (led_chars.c) to determine  the 8-bit 
// data value that will be written out to PORTB.
//
// If AlertRowUpdate is false, simply return
//
//*****************************************************************************
void updateDisplay(void) {
	if (!AlertRowUpdate) return;
	AlertRowUpdate = false;
	
	updateRow(Row, DutyPos);
	//increment row
	Row = (Row+1) % 8;
}

//*****************************************************************************
// Initializes all of the GPIO pins used for the LED matrix, push 
// buttons, and the left potentiometer.
//*****************************************************************************
void initializeGpioPins(void)
{
	int delay;
	bool locked;
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
	PortF->DigitalEnable |= PIN_4;
	PortF->Direction |= PIN_4;
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
	PortD->DigitalEnable |= PIN_2 | PIN_3;
	PortD->Direction &= ~(PIN_2 | PIN_3);
	PortD->PullUpSelect |= PIN_2 | PIN_3;
	
	//configure ADC0 input
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	delay = SYSCTL_RCGCGPIO_R;
	PortE->Direction &= ~PIN_3;
	PortE->AlternateFunctionSelect |= PIN_3;
	PortE->DigitalEnable &= ~PIN_3;
	PortE->AnalogSelectMode |= PIN_3;
	
	
}

