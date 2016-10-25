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
/* Main Program                                                               */

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();    
    for(int i = 0;i < 8;i++) {
        average_every[i] = 2;
        average[i] = 0;
        button_trigger_on[i] = 0;
        sequential_press[i] = 0;
    }
    beep = 3000;
    command_to_send = 0;
    skip_cps = 256;
    debug_byte0 = 0;
    debug_byte1 = 0;
    debug_byte2 = 0;
    debug_byte3 = 0;
    debug = 0;    
    tmr2_ticks = 1;
    /* Initialize I/O and Peripherals for application */
    InitApp();

    //PORTA = 0b1110;
    //CPSCON0bits.CPSRNG = 0b00; // Noise detection
    while (1) {
        if(debug > 0) {
            i2c_dbg(debug_byte0, debug_byte1, debug_byte2, debug_byte3);
            debug = 0;
        }
    }
}

