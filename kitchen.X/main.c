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

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

static unsigned char ticker = 0;

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    ticker = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while (1) {
        if (PORTAbits.RA3) {
            TRISAbits.TRISA0 = 1;
            PORTAbits.RA0 = 0b0;
        } else {
            TRISAbits.TRISA0 = 0;
            PORTAbits.RA0 = 0b1;
        }
        asm("clrwdt");         
    }
}

