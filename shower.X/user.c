/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

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


void InitApp(void)
{
    // Setup FVR
    FVRCONbits.ADFVR = 0b11; // 4.096V
    FVRCONbits.FVREN = 1;
    while(!FVRCONbits.FVRRDY) {
    }

    i2c_init();
    // Motor1
    //PORTAbits.RA4 = 0;
    //PORTAbits.RA5 = 0;
    //PORTAbits.RA7 = 0;
    //PORTAbits.RA6 = 0;
    //TRISAbits.TRISA4 = 0;
    //TRISAbits.TRISA5 = 0;
    //TRISAbits.TRISA7 = 0;
    //TRISAbits.TRISA6 = 0;
    // Motor2
    PORTCbits.RC2 = 0;
    //PORTCbits.RC5 = 0;
    PORTCbits.RC6 = 0;
    //PORTCbits.RC7 = 0;
    TRISCbits.TRISC2 = 0;
    //TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC6 = 0;
    //TRISCbits.TRISC7 = 0;

    // Valve outputs
    PORTBbits.RB0 = 0;
    TRISBbits.TRISB0 = 0;
    ANSELBbits.ANSB0 = 0;
    PORTBbits.RB1 = 0;
    TRISBbits.TRISB1 = 0;
    ANSELBbits.ANSB1 = 0;
    PORTCbits.RC0 = 0;
    TRISCbits.TRISC0 = 0;
    PORTCbits.RC1 = 0;
    TRISCbits.TRISC1 = 0;

    // Counters
    TRISBbits.TRISB2 = 1;
    ANSELBbits.ANSB2 = 0; // important
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB3 = 0; // important
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 1;

    
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1; // AN0

    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 1; // AN11
    TRISBbits.TRISB5 = 1;
    ANSELBbits.ANSB5 = 1; // AN13

    // Vref pins
    TRISAbits.TRISA2 = 1;
    ANSELAbits.ANSA2 = 1; // AN2
    TRISAbits.TRISA3 = 1;
    ANSELAbits.ANSA3 = 1; // AN3

    // TIMER2
//    TMR2ON = 0;
//    TMR2 = 0x00;
//    PR2 = 0xFF;
//    T2CONbits.T2CKPS = 0b00; // 1:64
//    T2CONbits.T2OUTPS = 0b0000;// 1:16 post scaler
//    TMR2IF = 0;
    
    // IoC
    IOCBNbits.IOCBN0 = 0; //??
    IOCBPbits.IOCBP0 = 0; // ??
    IOCBPbits.IOCBP2 = 1;
    IOCBPbits.IOCBP3 = 1;
    IOCBPbits.IOCBP6 = 1;
    IOCBPbits.IOCBP7 = 1;
    //
    // ADC
    ADCON0bits.CHS = 0b01011; // AN11
    ADCON1bits.ADNREF = 0b1; // VREF-
    ADCON1bits.ADPREF = 0b10; // VREF+
    ADCON1bits.ADFM = 1; // right justify
    ADCON1bits.ADCS = 0b010; // Tad = 1us @32Mhz
    ADCON0bits.ADON = 1;
    
    // TMR0
    TMR0 = 0x00;
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.PS = 0b111; // Max pre-scaler
    OPTION_REGbits.PSA = 0;

    ADIF = 0;
    TMR0IF = 0;
    IOCBF = 0;
    /* Enable interrupts */
    IOCIE = 1;
    ADIE = 1;
    TMR0IE = 1;
    PEIE = 1;
    GIE  = 1;
    
    pwm_en();
    // Launch ADC
    ADCON0bits.GO = 1;
}
