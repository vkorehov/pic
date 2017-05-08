/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations and to compiler documentation for macro details. */
void ConfigureOscillator(void)
{
    // Configure to 32 MHz
    // 1110 = 8MHZ (+ 4xPLL = 32Mhz)
//    OSCCONbits.SPLLEN = 0; // xtal wont supoport PLL
//    OSCCONbits.SCS = 0b10; // by IRFC
//    OSCCONbits.IRCF = 0b1110;
    //OSCCON = 0b11110000; // 8:SPLLEN(1 Yes) 4..7:IRCF(1110 8/32 MHz) 1..2:SCS(00 use FOSC in confbits)
}
