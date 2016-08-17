/*
 * File:   pwm.c
 * Author: vlad
 *
 * Created on August 17, 2016, 9:46 AM
 */
#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

void pwm1_en(unsigned short duty) {
    TRISCbits.TRISC6 = 1;
    // TIMER4
    TMR4ON = 0;    
    CCPTMRS0bits.C3TSEL = 0b01; // TIMER4
    //
    CCP3CONbits.CCP3M0 = 0;
    CCP3CONbits.CCP3M1 = 0;
    CCP3CONbits.CCP3M2 = 1;
    CCP3CONbits.CCP3M3 = 1;
    CCP3CONbits.P3M0 = 0;
    CCP3CONbits.P3M1 = 0;
    //
    // DUTY
    CCPR3L = 0xa0;
    CCP3CONbits.DC3B0 = 0;
    CCP3CONbits.DC3B1 = 0;
    // TIMER4
    TMR4IF = 0;
    TMR4 = 0x00;
    PR4 = 0xFF; // 31.25khz
    T4CONbits.T4CKPS = 0b00; // 1:1
    T4CONbits.T4OUTPS = 0b0000;// 1:16 post scaler
    TMR4ON = 1;    
    TRISCbits.TRISC6 = 0;    
}
