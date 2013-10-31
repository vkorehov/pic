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

void interrupt isr(void) {
    if (TMR1IE && TMR1IF) {
        TMR1IF = 0;
        TMR1ON = 0;
        if (!stop) {
            running = 1;
            if (state == 0) {
                state = 1;
                PORTA = 0b00010010;
            } else if (state == 1) {
                state = 2;
                PORTA = 0b00001010;
            } else if (state == 2) {
                state = 3;
                PORTA = 0b00001100;
            } else if (state == 3) {
                state = 0;
                PORTA = 0b00010100;
            }
            write_tmr1(WAIT);
            TMR1ON = 1;
        } else {
            running = 0;
            PORTA = 0b00000000;
        }
    }
    if (ADIF && ADIE) {
        ADIF = 0;
        if (running) {
            unsigned int v = ((ADRESH) << 8 | (ADRESL));
            if (v > 0x2A7) { // > 2.8V
                stop = 1;
            }
        }
    }
}


