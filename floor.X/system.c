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
    // Configure to 32MHZ
    OSCCON = 0b11110000; // 8:SPLLEN(0 by confb) 4..7:IRCF(1110 8MHz) 1..2:SCS(10 use IRCF in confbits)
}
