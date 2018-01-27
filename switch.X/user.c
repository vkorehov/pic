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

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void i2c_init() {
    // Initialise I2C MSSP
    SSPCON1  = 0b00110110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address)
    SSPCON2  = 0b00000001; // Slave Clock stretching on rceive and send 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(1)
    SSPCON3  = 0b00011001; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(1) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(1)
    SSPSTAT = 0b11000000; // Slew rate disabled, SMBus compat. timings. SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = I2C_MYADDR << 1; // My address        
}


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

void InitApp(void)
{
//#ifndef _12F1840
//    APFCONbits.CCP2SEL = 1; // RB3 to be CCP2/P2A
//#endif
    i2c_init();
#ifdef MOVEMENT_ENABLED
    TRISAbits.TRISA4 = 1; // enable input
    ANSELAbits.ANSA4 = 0;
#endif
#ifdef SHOWER_ENABLED
    TRISAbits.TRISA4 = 1; // enable input
    ANSELAbits.ANSA4 = 0;
#endif
#ifdef DHT22_ENABLED
    dht22_state = 0;
    dht22_index = 0;
    dht22_bit_index = 0;
    for(int i = 0; i < DHT22_MAX_BYTES; i++) {
        dht22_bits[i] = 0;
    }
#endif
    // TIMER1
    switch_dur_mult= 0;
    TMR1GE = 0;
    T1CONbits.TMR1CS = 0b01; // Fosc
    T1CONbits.nT1SYNC = 0;
    T1CONbits.T1CKPS = 0b11; // 1:8 pre-scaler
    write_tmr1(0);
    TMR1IF = 0;

    pwm_init();
    
    TMR1IE = 1;
    SSPIF = 0;
    /* Enable interrupts */
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}
