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
    // Setup FVR
    FVRCONbits.ADFVR = 0b11; // 4.096V
    FVRCONbits.FVREN = 1;
    while(!FVRCONbits.FVRRDY) {
        asm("nop");
    }

    i2c_init();
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;
    PORTAbits.RA3 = 0;
    PORTAbits.RA4 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;

    TRISBbits.TRISB0 = 1;
    ANSELBbits.ANSB0 = 1; // AN12
    // ADC setup
    ADCON0bits.CHS = 0b01100;// AN12
    ADCON1bits.ADNREF = 0b0;
    ADCON1bits.ADPREF = 0b11; // FVR buffer1
    ADCON1bits.ADCS = 0b101;
    ADCON1bits.ADFM = 1;
    ADIF = 0;
    ADIE = 1;
    ADCON0bits.ADON = 1;
    
    // Timer1
    TMR1GE = 0;
    T1CONbits.TMR1CS = 0b01;
    T1CONbits.nT1SYNC = 0;
    T1CONbits.T1CKPS = 0b00;
    TMR1IF = 0;
    TMR1IE = 1;
    write_tmr1(WAIT);
    TMR1ON = 1;
    /* Enable interrupts */
    PEIE = 1;
    GIE  = 1;
}
