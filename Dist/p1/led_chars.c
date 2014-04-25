#include "led_chars.h"

// Look up table for 
static const uint8_t LED_LUT[16][8] = \
{
    {   // Zero
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON &            LED_0_ON, // ROW 3
        LED_2_ON &            LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // One
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
                              LED_0_ON, // ROW 7
    },
    {   // Two
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Three
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Four
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON &            LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON &            LED_0_ON, // ROW 7
    },
    {   // Five
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON                      , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   // Six
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON                      , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
     {   // Seven
                              LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
                              LED_0_ON, // ROW 4
                              LED_0_ON, // ROW 5
                              LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
     },
    {   //  Eight
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON &            LED_0_ON, // ROW 1
        LED_2_ON &            LED_0_ON, // ROW 2
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 3
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  Nine
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
                              LED_0_ON, // ROW 1
                              LED_0_ON, // ROW 2
                              LED_0_ON, // ROW 3
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 4
        LED_2_ON &            LED_0_ON, // ROW 5
        LED_2_ON &            LED_0_ON, // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  10
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  11
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  12
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  13
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  14
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
    {   //  15
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 0
        LED_2_ON                      , // ROW 1
        LED_2_ON                      , // ROW 2
        LED_2_ON & LED_1_ON           , // ROW 3
        LED_2_ON & LED_1_ON           , // ROW 4
        LED_2_ON                      , // ROW 5
        LED_2_ON                      , // ROW 6
        LED_2_ON & LED_1_ON & LED_0_ON, // ROW 7
    },
};

//*****************************************************************************
// Convert a decimal number to BCD.  
//*****************************************************************************
static bool decimalToBcd(int8_t decimal, uint8_t *tens, uint8_t *ones)
{
    char msg[2];
    if(decimal > 99)
        return false;
    
    sprintf(msg, "%02d", decimal);
    
    *tens = msg[0] - 0x30;
    *ones = msg[1] - 0x30;
    
    return true;
}

//*****************************************************************************
// Returns 8-bit data written to the active row.
// Arguments
//    herts:    current refresh rate
//    row:      active row
//    lcdData:  data to write out to the latches.
//*****************************************************************************
bool getLCDRow(int16_t hertz, int8_t row, uint8_t *lcdData)
{
	uint8_t tens, ones;
	//get tens and ones places
	if (!decimalToBcd(hertz, &tens, &ones))
		return false;
	
	//calculate LED bits with the look-up table
	*lcdData = ((LED_LUT[tens][row] << 4) | 0x0F) & LED_LUT[ones][row];
  return true;
}
