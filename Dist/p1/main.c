//*****************************************************************************
//
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lm4f120h5qr.h"
#include "doublebuffer.h"
#include "../team.h"
#include "initBoard.h"
#include "board_config.h"
#include "gpio.h"
#include "UART.h"
#include "led_chars.h"
#include "hw4_gpio.h"
#include "hw4_adc.h"

extern void uartTxPoll(uint32_t base, char *data);

/******************************************************************************
 * Defines
 *****************************************************************************/

/******************************************************************************
 * Global Variables
 *****************************************************************************/
extern uint32_t Brightness;

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	FrameBuffer *drawBuffer;
	Pixel px;
	int c, d;
	int offset = 0;
  initBoard();
	initDoubleBuffers();
    
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"****** ECE353 ******\n\r");
  uartTxPoll(UART0,"HW4 Demo\n\r");
  uartTxPoll(UART0,teamNumber);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,teamMembers);
  uartTxPoll(UART0,"\n\r");
  uartTxPoll(UART0,"********************\n\r");
  uartTxPoll(UART0,"\n\r");

	px.hex = 0x00ffff;

  while(1) {
    //examineButtons();
		clearDrawBuffer();
		drawBuffer = getDrawBuffer();
		drawAARect(	drawBuffer,
								(offset/7000)%8, bmod(offset,7000), 3,
								(offset/5000)%8, bmod(offset,5000), 3, px);
		swapBuffers();
		offset++;
    updateRefreshRate();
  }
}




