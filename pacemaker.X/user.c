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
    SSPCON1 = 0b00000000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(0) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    BCL1IE = 0; // Disable I2C Bus Collision Interrupts
    BCL1IF = 0; // Clear I2C Bus Collision Flag
    SSP1IE = 0; // Disable interrupt on ACK
    SSP1IF = 0; // Clear interrupt on ACK flag

}
void i2c_init() {
    // Initialise I2C MSSP
    SSPCON1 = 0b00101000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    SSPCON2  = 0b01000000; // 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSPCON3  = 0b00000000; // 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE() 2:AHEN() 1:DHEN(0)
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
    TRISA   = 0b11111111; // RA4(p3): AN3
    i2c_init();

    ANSELA = 0b00010000;// RA4(p3): AN3
    ADCON1 = 0b11000000;// ADFM=1, ADCS = 100 (Fosc/4, TAD = 1uS), ADPREF = 00 (VDD)
    ADCON0 = 0b00001101;// CHS = 0b00011; // GO = 0,ADON = 1

    /* Timrt1 configuration */
    TMR1CS1 = 0; TMR1CS0 = 1; // System Clock (FOSC)
    /* Timer1 interrupt on overflow */
    TMR1GE = 0; // always count!
    TMR1ON = 0; // initially disable
    TMR1H = 0;
    TMR1L = 0;
    TMR1IF = 0;
    TMR1IE = 1;
    /* Enable interrupts */
    PEIE = 1;
    GIE  = 1;
}

