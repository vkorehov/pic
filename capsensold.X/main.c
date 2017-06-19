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
#include <stdio.h>
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#ifdef SIMULATOR
void putch(unsigned char data) {
    while( ! PIR1bits.TXIF)          // wait until the transmitter is ready
        continue;
    TXREG = data;                     // send one character
}
#endif
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
        percent[i] = 0;
        percents_on[i] = PCT_ON;
        percents_off[i] = PCT_OFF;  
        raw[i] = 0;
    }
#ifdef PCT_4_ON   
    percents_on[4] = PCT_4_ON;
    percents_off[4] = PCT_4_OFF;
#endif    
#ifdef PCT_2_ON   
    percents_on[2] = PCT_2_ON;
    percents_off[2] = PCT_2_OFF;
#endif
#ifdef PCT_0_ON   
    percents_on[0] = PCT_0_ON;
    percents_off[0] = PCT_0_OFF;
#endif
    
    beep = 3000;
    command_to_send = 0;
    skip_cps = 256;
    tmr2_ticks = 1;
    latching = 0;
    bright_on = 0;
    bright_off = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    printf("Started App\n");

#ifdef SIMULATOR
    run_tests();
#endif    
    //PORTA = 0b1110;
    //CPSCON0bits.CPSRNG = 0b00; // Noise detection
    while (1) {
        if(bright_on) {
            dpot_increment(100); // this resets pot to minimum voltage!
            dpot_decrement(30);
            bright_on = 0;
        }
        if(bright_off) {            
            dpot_increment(100); // this resets pot to minimum voltage!
            dpot_decrement(LOW_LIGHT);
            bright_off = 0;
        }
        asm("CLRWDT");
    }
}

