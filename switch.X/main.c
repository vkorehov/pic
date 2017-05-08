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

void pwm_init(void) {
    // Make sure RA2 could be used by MSSP
    // Relocate its function to RA5
    APFCON0bits.CCP1SEL = 1;

    CCP1ASbits.CCP1AS = 0b000; // disable automatic shutdown
    // Shutdown by default
    CCP1ASbits.PSS1BD = 0b00;
    CCP1ASbits.CCP1ASE = 1;
    TRISAbits.TRISA0 = 1;

    // initialize to 0% PWM
    CCPR1L = 0;
    CCP1CONbits.DC1B = 0b11;

    // Configure PWM Enchanced mode steering options
    CCP1CONbits.CCP1M = 0b1100; // P1A = P1B = active high, PWM
    CCP1CONbits.P1M = 0b00;
    PSTR1CONbits.STR1A = 0b0;
    PSTR1CONbits.STR1B = 0b1;

    // TIMER2
    TMR2ON = 0;
    TMR2IF = 0;
    TMR2 = 0x00;
#ifdef PWM_32K    
    PR2 = 0xFF;
    T2CONbits.T2CKPS = 0b00; // 1:1 pre-scaler    
    T2CONbits.T2OUTPS = 0b0000;// 1:1 post scaler
#endif
#ifdef PWM_500H
    PR2 = 0xFF;
    T2CONbits.T2CKPS = 0b11; // 1:64 pre-scaler    
    T2CONbits.T2OUTPS = 0b0000;// 1:1 post scaler    
#endif
    TMR2ON = 1;
}

void on(unsigned char dim) {
    // Disable output
    TRISAbits.TRISA0 = 1;
    CCP1ASbits.CCP1ASE = 1;

    // shutdown timer
    TMR1ON = 0;
    switch_dur_mult = 0;
    write_tmr1(0xffff - SWITCH_ON_DURATION);
    TMR1IF = 0;
    TMR1ON = 1;

    // change dim
    CCPR1L = dim;
    // Enable output
    TRISAbits.TRISA0 = 0;
    CCP1ASbits.CCP1ASE = 0;
}

void off(void) {
    // Disable output
    TRISAbits.TRISA0 = 1;
    CCP1ASbits.CCP1ASE = 1;
}


void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    movement_on_dim = 0xff;
    movement_state = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while(1)
    {
#ifdef MOVEMENT_ENABLED        
        if(PORTAbits.RA4) {
            if(!movement_state) {
                on(movement_on_dim);
            }
            movement_state = 1;
        } else {
            movement_state = 0;            
        }
#endif        
    }
}

