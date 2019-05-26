/*
 * File:   pwm.c
 * Author: vlad
 *
 * Created on August 17, 2016, 9:46 AM
 */
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

unsigned long pwm_timeout;

void pwm_duty(unsigned char duty1_l, unsigned char duty1_h, unsigned char duty2_l, unsigned char duty2_h) {
    if(duty2_h != CCPR3L)
        CCPR3L = duty2_h;
    if(duty1_h != CCPR1L)    
        CCPR1L = duty1_h;
    if((0b00000001 & duty2_l) != CCP3CONbits.DC3B0)
        CCP3CONbits.DC3B0 = (0b00000001 & duty2_l);
    if(((0b00000010 & duty2_l) >> 1) != CCP3CONbits.DC3B1)
        CCP3CONbits.DC3B1 = (0b00000010 & duty2_l) >> 1;
    if((0b00000001 & duty1_l) != CCP1CONbits.DC1B0)
        CCP1CONbits.DC1B0 = (0b00000001 & duty1_l);
    if(((0b00000010 & duty1_l) >> 1) != CCP1CONbits.DC1B1)
        CCP1CONbits.DC1B1 = (0b00000010 & duty1_l) >> 1;
    pwm_timeout = 0;
}

void pwm_en(void) {
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC2 = 1;
    
    // TIMER4
    TMR4ON = 0;    
    CCPTMRS0bits.C3TSEL = 0b01; // TIMER4
    CCPTMRS0bits.C1TSEL = 0b01; // TIMER4
    //
    CCP3CONbits.CCP3M0 = 0;
    CCP3CONbits.CCP3M1 = 0;
    CCP3CONbits.CCP3M2 = 1;
    CCP3CONbits.CCP3M3 = 1;
    CCP3CONbits.P3M0 = 0;
    CCP3CONbits.P3M1 = 0;
    //
    CCP1CONbits.CCP1M0 = 0;
    CCP1CONbits.CCP1M1 = 0;
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M3 = 1;
    CCP1CONbits.P1M0 = 0;
    CCP1CONbits.P1M1 = 0;
    
    // DUTY
    CCPR3L = 0;
    CCPR1L = 0;
     
    CCP3CONbits.DC3B0 = 0;
    CCP3CONbits.DC3B1 = 0;
    
    CCP1CONbits.DC1B0 = 0;
    CCP1CONbits.DC1B1 = 0;
    
    // TIMER4
    TMR4IF = 0;
    TMR4 = 0x00;
    PR4 = 0xFF; // 31.25khz
    T4CONbits.T4CKPS = 0b00; // 1:1
    T4CONbits.T4OUTPS = 0b0000;// 1:16 post scaler
    TMR4ON = 1;
    while(!TMR4IF) {}
    //
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC2 = 0;    
}
