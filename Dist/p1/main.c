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

	

  while(1) {
    //examineButtons();
		drawBuffer = getDrawBuffer();
		for (c = 0; c < 4; c++) {
			for (d = 0; d < 4; d++) {
				drawBuffer->raster[(c+offset/7000)%8][(d+offset/10000)%8].hex = 0xFF6600;
				drawBuffer->raster[(c+4+offset/7000)%8][(d+offset/10000)%8].hex = 0xFFFF00;
				drawBuffer->raster[(c+offset/7000)%8][(d+4+offset/10000)%8].hex = 0x11DDFF;
				drawBuffer->raster[(c+4+offset/7000)%8][(d+4+offset/10000)%8].hex = 0x66ff00;
			}
		}
		//offset++;
		swapBuffers();
    updateDisplay();
    updateRefreshRate();
  }
}




