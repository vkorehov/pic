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

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void on(void) {
    PORTAbits.RA0 = 1;
    switch_count = 0;
}

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while(1)
    {
        //if(switch_count >= SWITCH_ON_DURATION) {
            PORTAbits.RA0 = 0;
        //} else {
        //    switch_count++;
        //}
    }
}

