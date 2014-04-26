#ifndef __BOARD_CONFIG__
#define __BOARD_CONFIG__

#include <stdint.h>
#include <stdio.h>
#include "lm4f120h5qr.h"
#include "inc/hw_types.h"
#include "gpio.h"
#include "UART.h"
#include "board_ports.h"

/******************************************************************************
 * Defines
 *****************************************************************************/
 

#define DISABLED  0
#define NONE      0

/******************************************************************************
 * Initialization Structures
 *****************************************************************************/
 
/******************************************************************************
 * PORT A
 *****************************************************************************/
 GPIO_CONFIG portA_config = {
  (
    PA0_U0_RX  | 
    PA1_U0_TX  |
    PA6_SW2    |
    PA7_SW3
   
  ),                                // DigitalEnable
  (
    PA6_SW2    |
    PA7_SW3
   ),                               // Input
  NONE,                             // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  (
    PA6_SW2    |
    PA7_SW3
   ),                               // PullUp
  DISABLED,                         // AnalogEnable
  (
    PA0_U0_RX | 
    PA1_U0_TX 
  ),                                // AlternateFunctionEnable
  (
    GPIO_PCTL_PA0_U0RX    |
    GPIO_PCTL_PA1_U0TX    
  )                                 // PortControl
};

 /******************************************************************************
 * PORT B
 *****************************************************************************/
 GPIO_CONFIG portB_config = {
  (
   PB0_DATA0 |
   PB1_DATA1 |
   PB2_DATA2 |
   PB3_DATA3 |
   PB4_DATA4 |
   PB5_DATA5 |
   PB6_DATA6 |
   PB7_DATA7
  ),                                // DigitalEnable
  NONE,                             // Input
  (
   PB0_DATA0 |
   PB1_DATA1 |
   PB2_DATA2 |
   PB3_DATA3 |
   PB4_DATA4 |
   PB5_DATA5 |
   PB6_DATA6 |
   PB7_DATA7
  ),                                // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  DISABLED,                         // PullUp
  DISABLED,                         // AnalogEnable
  DISABLED,                         // AlternateFunctionEnable
  PORT_CONTROL_DEFAULT              // PortControl
};




 /******************************************************************************
 * PORT C
 *****************************************************************************/
 GPIO_CONFIG portC_config = {
  (
    PC4_RED_EN    |
    PC5_GRN_EN    |
    PC6_BLU_EN    |
    PC7_COL_EN    |   
    PC0_TCK       |
    PC1_TMS       |
    PC2_TDI       |
    PC3_TDO
  ),                                // DigitalEnable
  NONE,                             // Input
  (
    PC4_RED_EN |
    PC5_GRN_EN |
    PC6_BLU_EN |
    PC7_COL_EN 
  ),                                // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  ( 
    PC0_TCK       |
    PC1_TMS       |
    PC2_TDI       |
    PC3_TDO
  ),                                // PullUp
  DISABLED,                         // AnalogEnable
  ( 
    PC0_TCK       |
    PC1_TMS       |
    PC2_TDI       |
    PC3_TDO
  ),                                // AlternateFunctionEnable
  (
    GPIO_PCTL_PC0_TCK   |
    GPIO_PCTL_PC1_TMS   |
    GPIO_PCTL_PC2_TDI   |
    GPIO_PCTL_PC3_TDO
    
  )                                 // PortControl
};

 /******************************************************************************
 * PORT D
 *****************************************************************************/
 GPIO_CONFIG portD_config = {
  (
     PD2_SW4 |
     PD3_SW5
  ),                                // DigitalEnable
  (
     PD2_SW4 |
     PD3_SW5
  ),                                // Input
  NONE,                             // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  (
     PD2_SW4 |
     PD3_SW5
  ),                                // PullUp
  DISABLED,                         // AnalogEnable
  DISABLED,                         // AlternateFunctionEnable
  PORT_CONTROL_DEFAULT              // PortControl
};

 /******************************************************************************
 * PORT E
 *****************************************************************************/
 GPIO_CONFIG portE_config = {
  NONE,                             // DigitalEnable
  NONE,                             // Input
  NONE,                             // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  DISABLED,                         // PullUp
  DISABLED,                         // AnalogEnable
  DISABLED,                         // AlternateFunctionEnable
  PORT_CONTROL_DEFAULT              // PortControl
};

/******************************************************************************
 * PORT F
 *****************************************************************************/
GPIO_CONFIG portF_config = {
  (
    PF1_SW6 |
    PF4_OUT_EN_B
  ),                                // DigitalEnable
  (
    PF1_SW6
  ),                                // Input
  (
    PF4_OUT_EN_B
  ),                                // Output
  DISABLED,                         // InterruptEnable
  DISABLED,                         // InterruptLevel
  DISABLED,                         // InterruptLevelActiveHigh
  DISABLED,                         // InterruptEdge
  DISABLED,                         // InterruptEdgeRising
  DISABLED,                         // InterruptEdgeBoth
  DISABLED,                         // PullDown
  (
    PF1_SW6
  ),                                // PullUp
  DISABLED,                         // AnalogEnable
  DISABLED,                         // AlternateFunctionEnable
  PORT_CONTROL_DEFAULT              // PortControl
};


/******************************************************************************
 * Initialization Structure for UART0 Comms
 *****************************************************************************/ 
UART_CONFIG UART0_config = {
  ENABLED,    				// UART
  ENABLED,                  // UARTRx
  ENABLED,    				// UARTTx
  _8BITS,                   // WordLen
  9600,                     // BaudRate
  ENABLED,                  // FIFO
  DISABLED,                 // _2Stop
  DISABLED,                 // Parity
  DISABLED,                 // EvenParity
  DISABLED,                 // HighSpeed
  RX_ONE_EIGHTH,            // RxIntFIFO
  TX_ONE_EIGHTH,            // TxIntFIFO
  ENABLED,                  // RxIntMask
  ENABLED,                  // TxIntMask
  DISABLED,                 // RxTimeOutIntMask
  DISABLED,                 // OvrrIntMask
  DISABLED,                 // ParityErrIntMask
  DISABLED                  // FramingErrIntMask  
};




#endif
