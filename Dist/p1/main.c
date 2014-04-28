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
	Pixel px, px2;
	int c, d;
	uint32_t offset = 0;
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

	px.hex = 0x0000ff;
	px2.hex = 0x0000ff;

  while(1) {
    //examineButtons();
		//px.color.r = fract(abs((offset%6400)-3200),3200);
		//px.color.g = fract(abs((offset%9600)-4800),4800);
		//px.color.b = fract(abs((offset%16000)-8000),8000);
		clearDrawBuffer();
		drawBuffer = getDrawBuffer();
		drawAARect(	drawBuffer,
								(offset>>2)/256, fract(offset>>2,256), 2,
								3, 0, 2, px);
		drawRect(	drawBuffer,
								3, 2,
								3, 2, px2);
		swapBuffers();
		offset++;
		offset %= (512<<4);
    updateRefreshRate();
  }
}




