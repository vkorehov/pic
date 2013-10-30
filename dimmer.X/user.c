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

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void i2c_init() {
    // Initialise I2C MSSP
    SSPCON1  = 0b00110110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(1110 I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled)
    SSPCON2  = 0b00000001; // 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSPCON3  = 0b01101000; // 8:ACKTIM(0) 7:PCIE(1) 6:SCIE(1) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(0)

    SSPADD = I2C_MYADDR; // My address
    SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF

}


void InitApp(void)
{
    FVRCONbits.CDAFVR = 0b10; // 2V
    FVRCONbits.FVREN = 1;
    while(!FVRCONbits.FVRRDY) {
    }

//#ifndef _12F1840
//    APFCONbits.CCP2SEL = 1; // RB3 to be CCP2/P2A
//#endif
//    CCPTMRS0bits.C2TSEL = 0b00; // Timer2
//    PR2 = period; // period of 10mS (155)
//    CCP2CONbits.P2M = 0b00; // Use only P2A
//    CCP2CONbits.CCP2M = 0b1100; // PWM mode P2A active high!
//    // Load duty cycle value
//    // Range is: (1024/255) * 155 = 622.2 = 623
//    CCPR2L = (unsigned char)((command >> 2) & 0xff);
//    CCP2CONbits.DC2B = (command & 0b11);
    
//    TMR2IF = 0;
//    T2CONbits.T2CKPS = 0b11; // 1:64 pre-scaler
//    T2CONbits.TMR2ON = 1;
//    while(!TMR2IF) {
//    }
//    TMR2IF = 0;
//    TMR2IE = 1;
//    TRISBbits.TRISB3 = 1;
    i2c_init();
    ANSELAbits.ANSA0 = 1;
//    ADCON1 = 0b11000000;// ADFM=1, ADCS = 100 (Fosc/4, TAD = 1uS), ADPREF = 00 (VDD)
//    ADCON0 = 0b00000001;// AD0 = 0b00000; // GO = 0,ADON = 1

    CM1CON1bits.C1NCH = 0b00; // C1VN connects to C12IN0- pin
    CM1CON1bits.C1PCH = 0b10; // FVR
    CM1CON0bits.C1POL = 1;
    CM1CON0bits.C1SP = 1;
    CM1CON0bits.C1HYS = 1;
    CM1CON0bits.C1SYNC = 1;
    C1INTP = 1;
    C1IF = 0;
    C1IE = 1;
    CM1CON0bits.C1ON = 1;
    
    // Output pin
    PORTBbits.RB3 = 0;
    TRISBbits.TRISB3 = 0;

    // Timer0    
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b100; // 1:32

    // Timer1
    TMR1GE = 0;
    T1CONbits.TMR1CS = 0b01;
    T1CONbits.nT1SYNC = 0;
    T1CONbits.T1CKPS = 0b00;
    write_tmr1(0);
    TMR1IF = 0;
    TMR1IE = 1;
    /* Enable interrupts */
    PEIE = 1;
    GIE  = 1;
}
