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
    // Configure to HS
    OSCCONbits.SPLLEN = 0; // xtal wont supoport PLL
#if I2C_MYADDR == 0x67
    OSCCONbits.SCS = 0b10; // by CONFIG (bootloader set to HS)
#else
    OSCCONbits.SCS = 0b00; // by CONFIG (bootloader set to HS)    
#endif
    OSCCONbits.IRCF = 0b1111;    
}
