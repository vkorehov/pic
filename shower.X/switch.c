/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

unsigned long switch_timeouts[4];

void on(unsigned char sw) {
    // B0 = V-1
    // B1 = V-2
    // C0 = V-3
    // C1 = V-4
    switch(sw) {
        case 0:
            PORTBbits.RB0 = 1;
            break;
        case 1:
            PORTBbits.RB1 = 1;
            break;
        case 2:
            PORTCbits.RC0 = 1;
            break;
        case 3:
            PORTCbits.RC1 = 1;
            break;
    }
    switch_timeouts[sw] = 0;
}

void off(unsigned char sw) {
    // B0 = V-1
    // B1 = V-2
    // C0 = V-3
    // C1 = V-4
    switch(sw) {
        case 0:
            PORTBbits.RB0 = 0;
            break;
        case 1:
            PORTBbits.RB1 = 0;
            break;
        case 2:
            PORTCbits.RC0 = 0;
            break;
        case 3:
            PORTCbits.RC1 = 0;
            break;
    }
}
