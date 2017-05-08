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

unsigned int tripped_readings[8];
unsigned int last_readings[8];
unsigned int readings[8];
unsigned int readings_counter;
unsigned int beep;
unsigned char state[3];

#ifdef SIMULATOR

void putch(unsigned char data) {
    while (!PIR1bits.TXIF) // wait until the transmitter is ready
        continue;
    TXREG = data; // send one character
}

#else

void putch(unsigned char data) {
    while (!PIR1bits.TXIF) // wait until the transmitter is ready
        continue;
    TXREG = data; // send one character
}
#endif
/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    readings_counter = 0;
    for (int i = 0; i < 8; i++) {
        readings[i] = 0;
        last_readings[i] = 0;
        tripped_readings[i] = 0;       
    }
    beep = 0;
    for (int i = 0; i < 3; i++) {
        state[i] = 0;
    }    
    /* Initialize I/O and Peripherals for application */
    InitApp();
    printf("Started App\n");
    dpot_increment(100); // this resets pot to minimum voltage!
    dpot_decrement(30);
#ifdef SIMULATOR
    run_tests();
#endif
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 0;
    PORTCbits.RC5 = 0;
    //PORTA = 0b1110;
    //CPSCON0bits.CPSRNG = 0b00; // Noise detection
    unsigned char training = 64;
    while (1) {
        if (readings_counter >= SCANS_PER_DECODE) {
            readings_counter = 0;
            if (training == 0 && tripped_readings[4] == 0 && last_readings[4] > readings[4] && last_readings[4] - readings[4] > 6) {
                tripped_readings[4] = last_readings[4];
                state[0] = ~state[0];
                beep = 500;
            } else if (training == 0 && tripped_readings[4] != 0 && readings[4] + 4 > tripped_readings[4]) {
                tripped_readings[4] = 0;
            }
            if (training == 0 && tripped_readings[2] == 0 && last_readings[2] > readings[2] && last_readings[2] - readings[2] > 6) {
                tripped_readings[2] = last_readings[2];
                state[1] = ~state[1];
                beep = 500;                                
            } else if (training == 0 && tripped_readings[2] != 0 && readings[2] + 4 > tripped_readings[2]) {
                tripped_readings[2] = 0;
            }
            if (training == 0 && tripped_readings[0] == 0 && last_readings[0] > readings[0] && last_readings[0] - readings[0] > 6) {
                tripped_readings[0] = last_readings[0];
                state[2] = ~state[2];                
                beep = 500;                                                
            } else if (training == 0 && tripped_readings[0] != 0 && readings[0] + 4 > tripped_readings[0]) {
                tripped_readings[0] = 0;
            }

            if (training > 0) {
                training--;
            }
            last_readings[0] = readings[0];
            last_readings[2] = readings[2];
            last_readings[4] = readings[4];
            printf("%d %d %d\n", readings[0], readings[2], readings[4]);
        }
        PORTCbits.RC0 = state[0];
        PORTCbits.RC1 = state[1];
        PORTCbits.RC5 = state[2];
    }
}

