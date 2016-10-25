/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic16f1938.h>        /* For true/false definition */

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */
void i2c_destroy() {
    // Clear  SSP1CON1
    SSPCON1 = 0b00000000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(0) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    BCLIE = 0; // Disable I2C Bus Collision Interrupts
    BCLIF = 0; // Clear I2C Bus Collision Flag
    SSPIE = 0; // Disable interrupt on ACK
    SSPIF = 0; // Clear interrupt on ACK flag

}

void i2c_init() {
    // Initialise I2C MSSP
    SSPCON1 = 0b00101000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    SSPCON2 = 0b01000000; // 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSPCON3 = 0b00000000; // 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE() 2:AHEN() 1:DHEN(0)
    // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    //SSPADD = 9;    		// 100Khz @ 4Mhz Fosc
    //SSPADD = 79;    		// 100Khz @ 32Mhz Fosc
    SSPADD = 79;
    SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    BCLIF = 0; // Clear I2C Bus Collision Flag
    BCLIE = 1; // Enable I2C Bus Collision Interrupts
    SSPIF = 0; // Clear interrupt on ACK flag
    SSPIE = 1; // Enable interrupt on ACK

}

void InitApp(void) {
    //Setup FVR
    FVRCONbits.CDAFVR = 0b11; // 4.086V
    FVRCONbits.ADFVR = 0b01; // 1.024V   
    FVRCONbits.FVREN = 1;   
    while (!FVRCONbits.FVRRDY) {
    }    
    // setup references
    DACCON0bits.DACPSS = 0b10; // FVR BUFFER2
    DACNSS = 0b00; // GND
    DACCON0bits.DACOE = 0;
    DACCON1bits.DACR = 0; // 4.086 * (16/32)
    DACEN = 1;
    
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    //
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1;

    TRISAbits.TRISA0 = 0; // U/D as output
    TRISAbits.TRISA1 = 0; // EN as output
    TRISAbits.TRISA2 = 0; // nCS as output

    dpot_increment(100); // this resets pot to minimum voltage!
    dpot_decrement(20);
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 1;
    PORTAbits.RA2 = 1;

    // CPS0_RB0 CPS2_RB2 CPS4_RB4
    TRISBbits.TRISB0 = 1;
    ANSELBbits.ANSB0 = 1;
    TRISBbits.TRISB2 = 1;
    ANSELBbits.ANSB2 = 1;
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB4 = 1;

    //TRISC   = 0b00011000; //RC3(p14):SCL RC4(p15):SDA
    PORTC = PORTC | (LED4_BIT | LED2_BIT | LED0_BIT);
            
    //PORTCbits.RC5 = 0;
    //PORTCbits.RC6 = 0;
    //PORTCbits.RC7 = 0;    
    TRISCbits.TRISC3 = 1; // SCL
    TRISCbits.TRISC4 = 1; // SDA
    TRISCbits.TRISC5 = 0; // output
    TRISCbits.TRISC6 = 0; // output
    TRISCbits.TRISC7 = 0; // output
    
    // buzzer    
    ANSELAbits.ANSA3 = 0;
    PORTAbits.RA3 = 0;
    TRISAbits.TRISA3 = 0; // BUZZ as output
    
    i2c_init();


    // 8:CPSON(1) 7:CPSRM(1) 3..4:CPSRNG(11 High Range) 2:CPSOUT(0) 1:T0XCS(0)
    CPSCON0bits.CPSON = 1;
    CPSCON0bits.CPSRM = 1;
    CPSCON0bits.CPSRNG = 0b11; // High Range
    CPSCON0bits.CPSOUT = 0;
    CPSCON0bits.T0XCS = 0;
    
    // 1..2:CPSCH(00 CPS0, 11 CPS3)
    CPSCON1bits.CPSCH = 0b0000; // CPS0

    //TIMER0 used for CPS
    // 8:WPUEN(1) 7:INTEDG(1) 6:TMR0CS(0) 5:TMR0SE(0) 4:PSA(0 Timer0) 1..3:PS(111 1:256)
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.INTEDG = 1;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.TMR0SE = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111;
    
    //Timer2
    T2CONbits.T2CKPS = 0b11;
    T2CONbits.T2OUTPS = 0b0000;
    PR2 = 0x10;
    T2CONbits.TMR2ON = 1;
    TMR2IF = 0;
    TMR2IE = 1;    
    
    //TIMER1 SETUP
    //  7..8:TMR1CS(11 Capacitive Sensing Oscillator) 5..6:T1CKPS(00 Disabled) 4:T1OSCEN(0) 3:T1SYNC(1) 1:TMR1ON(1)    
    T1CONbits.TMR1CS = 0b11;
    T1CONbits.T1CKPS = 0b00;
    T1CONbits.T1OSCEN = 0;
    T1CONbits.nT1SYNC = 1;
    T1CONbits.TMR1ON = 1;
    
    // 8:TMR1GE(1) 7:T1GPOL(1) 6:T1GTM(1 pass signal thru flipflop) 5:T1GSPM(0 no 'single pulse' mode) 4:T1GGO(0 no single pulse) 3:T1GVAL(0 Gate value) 1..2:T1GSS(01 From Timer0 OF)
    T1GCONbits.TMR1GE = 1;
    T1GCONbits.T1GPOL = 0;
    T1GCONbits.T1GTM = 0;// skip flip flop
    T1GCONbits.T1GSPM = 0;
    T1GCONbits.T1GGO = 0;
    T1GCONbits.T1GVAL = 0;
    T1GCONbits.T1GSS = 0b01;
    TMR1GIF = 0; // Clear Gate Interrupt Flag
    TMR1GIE = 1; // Enable Gate Interrupt

    /* Enable interrupts */
    PEIE = 1;
    GIE = 1;
}

