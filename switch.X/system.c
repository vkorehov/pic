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
    OSCCON = 0b11110000; // 8:SPLLEN(0 No) 4..7:IRCF(1101 4 MHz(32Mhz)) 1..2:SCS(00 use FOSC in confbits)
}
