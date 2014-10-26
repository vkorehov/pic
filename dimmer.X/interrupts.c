/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "system.h"
#include "user.h"

#define PEAK_TO_ZC1 (0xffff - 4500)
#define ZC1_TO_ZC2 (0xffff - 40500)

void interrupt isr(void) {
    if(TMR0IE && TMR0IF) {
        TMR0IF = 0;
        TMR0IE = 0;
        PORTAbits.RA0 = 0;
    }
    if(TMR1IE && TMR1IF) {
        TMR1IF = 0;
        TMR1ON = 0;
        if(state == 0) {
            state = 1;
            // Zero Crossing1
            if(dim > 0x01) {
                PORTAbits.RA0 = 1;
            } else {
                PORTAbits.RA0 = 0;
            }
            if(dim != 0xff) {
                TMR0 = 0xff - dim; // Start timer0 in order to turn off output pin!
                TMR0IF = 0;
                TMR0IE = 1;
            } else {
                TMR0IE = 0;
            }
            // Endof zero crossing
            write_tmr1(ZC1_TO_ZC2);
            TMR1ON = 1;
        } else if(state == 1) {
            // Zero Crossing2
            if(dim > 0x01) {
                PORTAbits.RA0 = 1;
            } else {
                PORTAbits.RA0 = 0;
            }
            if(dim != 0xff) {
                TMR0 = 0xff - dim; // Start timer0 in order to turn off output pin!
                TMR0IF = 0;
                TMR0IE = 1;
            } else {
                TMR0IE = 0;
            }
            // Endof zero crossing
        }
    }
    if (C1IF && C1IE) {
        C1IF = 0;
        TMR1ON = 0;
        write_tmr1(PEAK_TO_ZC1);
        state = 0;
        TMR1ON = 1;
    }
}


