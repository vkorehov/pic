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

/* <Initialize variables in user.h and insert code for user algorithms.> */
void i2c_destroy() {
    // Clear  SSP1CON1
    SSP1CON1 = 0b00000000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(0) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    BCL1IE = 0; // Disable I2C Bus Collision Interrupts
    BCL1IF = 0; // Clear I2C Bus Collision Flag
    SSP1IE = 0; // Disable interrupt on ACK
    SSP1IF = 0; // Clear interrupt on ACK flag

}
void i2c_init() {
    // Initialise I2C MSSP
    SSP1CON1 = 0b00101000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    SSP1CON2  = 0b01000000; // 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSP1CON3  = 0b00000000; // 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE() 2:AHEN() 1:DHEN(0)
    // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    SSPADD = 9;    		// 100Khz @ 4Mhz Fosc
    SSPSTAT = 0b11000000; 	// Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    BCL1IF = 0; // Clear I2C Bus Collision Flag
    BCL1IE = 1; // Enable I2C Bus Collision Interrupts
    SSP1IF = 0; // Clear interrupt on ACK flag
    SSP1IE = 1; // Enable interrupt on ACK
}

void InitApp(void)
{
    TRISA   = 0b00010111; // CPS0_RA0(p7) CPS3_RA4(p3)
                          //RA1(p6):SCL RA2(p5):SDA
    i2c_init();

    ANSELA  = 0b00010001; // RA4,RA0 = Analog

    CPSCON0 = 0b10001100; // 8:CPSON(1) 7:CPSRM(0) 3..4:CPSRNG(11 High Range) 2:CPSOUT(0) 1:T0XCS(0)
    CPSCON1 = 0b00000011; // 1..2:CPSCH(00 CPS0, 11 CPS3)


    //TIMER0 SETUP
    OPTION_REG = 0b11000111; // 8:WPUEN(1) 7:INTEDG(1) 6:TMR0CS(0) 5:TMR0SE(0) 4:PSA(0 Timer0) 1..3:PS(111 1:256)
    //TMR0IF = 0; // clear TMR0 interrupt flag
    //TMR0IE = 0; // enable TMR0 interrupt

    //TIMER1 SETUP
    T1CON  = 0b11000101; //  7..8:TMR1CS(11 Capacitive Sensing Oscillator) 5..6:T1CKPS(00 Disabled) 4:T1OSCEN(0) 3:T1SYNC(1) 1:TMR1ON(1)
    T1GCON = 0b11100001; // 8:TMR1GE(1) 7:T1GPOL(1) 6:T1GTM(1 pass signal thru flipflop) 5:T1GSPM(0 no 'single pulse' mode) 4:T1GGO(0 no single pulse) 3:T1GVAL(0 Gate value) 1..2:T1GSS(01 From Timer0 OF)
    TMR1GIF = 0; // Clear Gate Interrupt Flag
    TMR1GIE = 1; // Enable Gate Interrupt

    /* Enable interrupts */
    PEIE = 1;
    GIE  = 1;
}

