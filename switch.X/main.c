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
unsigned char movement_on_dim;
unsigned char movement_state;

unsigned char ra4;
unsigned char ra5;

unsigned char shower_allow_on;
unsigned char shower_state;
unsigned int shower_timeout;

/******************************************************************************/
/* User Global Variable Declaration                                           */

/******************************************************************************/
inline void write_tmr1(unsigned int v) {
    TMR1L = ((v)&0xff);
    TMR1H = ((v) >> 8);
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
unsigned char switch_dur_mult;

unsigned char last_dimm;

void on(unsigned char dim) {
    // shutdown timer
    TMR1ON = 0;
    switch_dur_mult = 0;
    write_tmr1(0xffff - SWITCH_ON_DURATION);
    TMR1IF = 0;
    TMR1ON = 1;

    if (last_dimm == dim) {
        return;
    }
    last_dimm = dim;
    // Disable output
    TRISAbits.TRISA0 = 1;
    CCP1ASbits.CCP1ASE = 1;

    // change dim
    CCPR1L = dim;
    // Enable output
    TRISAbits.TRISA0 = 0;
    CCP1ASbits.CCP1ASE = 0;
}

void off(void) {
    last_dimm = 0;
    // Disable output
    TRISAbits.TRISA0 = 1;
    CCP1ASbits.CCP1ASE = 1;
}

static unsigned char ticker = 0;

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    movement_on_dim = 0xff;
    movement_state = 0;
    shower_state = 0;
    shower_timeout = 0;
    shower_allow_on = 0;
    last_dimm = 0;
    ticker = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while (1) {
#ifdef MOVEMENT_ENABLED
        if (PORTAbits.RA4) {
            if (!movement_state) {
                on(movement_on_dim);
            }
            movement_state = 1;
        } else {
            movement_state = 0;
        }
#endif
        if (PORTAbits.RA4) {
            ra4++;
        }
        if (PORTAbits.RA5) {
            ra5++;
        }
#ifdef SHOWER_ENABLED
        if (PORTAbits.RA4 == 0 && shower_allow_on == 0x01) {
            if (shower_state == 1 && shower_timeout == 0) {
                // timeout occured, force OFF
                off();
                shower_state = 0;
            } else {
                on(0xFF);
                if (shower_state == 0) {
                    shower_timeout = SHOWER_TIMEOUT;
                }
                shower_state = 1;
            }
        } else {
            off();
            shower_state = 0;
            shower_timeout = 0;
        }

        if (ticker++ >= 250) {
            ticker = 0;
            if (shower_timeout != 0) {
                shower_timeout--;
            }
        }

#endif        
    }
}

