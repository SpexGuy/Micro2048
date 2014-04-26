#ifndef _BOARD_PORTS_H
#define _BOARD_PORTS_H 1

#define NULL                  0

#define PIN_0     (1 << 0)
#define PIN_1     (1 << 1)
#define PIN_2     (1 << 2)
#define PIN_3     (1 << 3)
#define PIN_4     (1 << 4)
#define PIN_5     (1 << 5)
#define PIN_6     (1 << 6)
#define PIN_7     (1 << 7)

#define PA6_SW2       PIN_6
#define PA7_SW3       PIN_7
#define PD2_SW4       PIN_2
#define PD3_SW5       PIN_3
#define PF1_SW6       PIN_1


#define PA0_U0_RX  PIN_0
#define PA1_U0_TX  PIN_1

#define PA2_SPI0_CLK   PIN_2
#define PA3_SPI0_CS    PIN_3
#define PA4_SPI0_MISO  PIN_4
#define PA5_SPI0_MOSI  PIN_5

#define PB0_DATA0     PIN_0
#define PB1_DATA1     PIN_1
#define PB2_DATA2     PIN_2
#define PB3_DATA3     PIN_3
#define PB4_DATA4     PIN_4
#define PB5_DATA5     PIN_5
#define PB6_DATA6     PIN_6
#define PB7_DATA7     PIN_7

#define PC4_RED_EN    PIN_4
#define PC5_GRN_EN    PIN_5
#define PC6_BLU_EN    PIN_6
#define PC7_COL_EN    PIN_7
#define PF4_OUT_EN_B  PIN_4

#define DISPLAY_EN_MASK PC4_RED_EN|PC5_GRN_EN|PC6_BLU_EN|PC7_COL_EN

#define PE3_ADC_0     PIN_3
#define PE2_ADC_1     PIN_2

#define PC0_TCK       PIN_0
#define PC1_TMS       PIN_1
#define PC2_TDI       PIN_2
#define PC3_TDO       PIN_3

#endif
