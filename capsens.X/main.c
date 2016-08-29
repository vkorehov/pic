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
        raw[i] = 0;
        average[i] = 0;
        trip[i] = 32;
        state[i] = 0;
        button_state[i] = 0;
    }
    beep = 1000;
    /* Initialize I/O and Peripherals for application */
    InitApp();

    //PORTA = 0b1110;
    unsigned int c = 0;
    unsigned char once = 0;
    //CPSCON0bits.CPSRNG = 0b00; // Noise detection
    while (1) {
        //asm("nop");
        if(once == 0 && (button_state[0] || button_state[6] || button_state[7])) {
            i2c_command8(0xBE);
            once = 1;
        }
        c++;
    }
}

