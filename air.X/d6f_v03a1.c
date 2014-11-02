/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#ifdef _12F1840
#define SSPIE SSP1IE
#define SSPIF SSP1IF
#endif

#define LERP_SIZE 6

unsigned int lerp_values[LERP_SIZE];
unsigned int lerp_deltas[LERP_SIZE-1];
const unsigned int lerp_multiplier = 75;
const unsigned int lerp_multiplier_lofi = 7;

void d6f_init(void) {
    lerp_values[0] = 500;
    lerp_values[1] = 700;
    lerp_values[2] = 1110;
    lerp_values[3] = 1580;
    lerp_values[4] = 2000;
    lerp_values[5] = 3000;

    lerp_deltas[0] = 200;
    lerp_deltas[1] = 410;
    lerp_deltas[2] = 470;
    lerp_deltas[3] = 420;
    lerp_deltas[4] = 1000;
}

unsigned int d6f_lerp(unsigned int voltage) {
    unsigned int value;
    // voltage is in mv units
    for(int i = 0; i < LERP_SIZE - 1; i++) {
        if(voltage > lerp_values[i] && voltage < lerp_values[i+1]) {
            // found range do factoring
            unsigned int offset = voltage - lerp_values[i];
            if(i != 4) {
                return lerp_multiplier * i + ((offset * lerp_multiplier) / lerp_deltas[i]);
            } else {
                // last LERP band causes overflow, so we need to make our calc imprecise
                return (lerp_multiplier * i + 10*((offset * lerp_multiplier_lofi) / lerp_deltas[i]));
            }
        }
    }
    // sensor error
    return 0;
}
