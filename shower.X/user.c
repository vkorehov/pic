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
    SSPCON1  = 0b00110110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address)
    SSPCON2  = 0b00000001; // Slave Clock stretching on rceive and send 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(1)
    SSPCON3  = 0b00001000; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(0)
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
    PORTAbits.RA4 = 0;
    PORTAbits.RA5 = 0;
    PORTAbits.RA7 = 0;
    PORTAbits.RA6 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    TRISAbits.TRISA7 = 0;
    TRISAbits.TRISA6 = 0;
    // Motor2
    PORTCbits.RC2 = 0;
    PORTCbits.RC5 = 0;
    PORTCbits.RC6 = 0;
    PORTCbits.RC7 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;

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

    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1; // AN0

    TRISBbits.TRISB2 = 1;
    ANSELBbits.ANSB2 = 1; // AN8
    TRISBbits.TRISB3 = 1;
    ANSELBbits.ANSB3 = 1; // AN9
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 1; // AN11
    TRISBbits.TRISB5 = 1;
    ANSELBbits.ANSB5 = 1; // AN13

    // Vref pins
    TRISAbits.TRISA2 = 1;
    ANSELAbits.ANSA2 = 1; // AN2
    TRISAbits.TRISA3 = 1;
    ANSELAbits.ANSA3 = 1; // AN3

    // ADC
    ADCON0bits.CHS = 0b01000; // AN8
    ADCON1bits.ADNREF = 0b1; // VREF-
    ADCON1bits.ADPREF = 0b10; // VREF+
    ADCON1bits.ADFM = 1;// right justify
    ADCON1bits.ADCS = 0b010;// Tad = 1us @32Mhz
    ADCON0bits.ADON = 1;
    // TMR0
    TMR0 = 0x00;
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.PS = 0b111; // Max pre-scaler
    OPTION_REGbits.PSA = 0;

    // Timer1
    TMR1GE = 0;
    T1CONbits.TMR1CS = 0b00; // Fosc/4
    T1CONbits.nT1SYNC = 0;
    T1CONbits.T1CKPS = 0b11;
    TMR1ON = 0;

    ADIF = 0;
    TMR0IF = 0;
    TMR1IF = 0;
    /* Enable interrupts */
    ADIE = 1;
    TMR0IE = 1;
    TMR1IE = 1;
    PEIE = 1;
    GIE  = 1;
}
