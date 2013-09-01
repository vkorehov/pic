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
    SSP1CON1  = 0b00111110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(1110 I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled)
    SSP1CON2  = 0b11100000; // 8:GCEN(1) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSP1CON3  = 0b01100100; // 8:ACKTIM(0) 7:PCIE(1) 6:SCIE(1) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE(1) 2:AHEN(0) 1:DHEN(0)
    // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    SSPADD = I2C_MYADDR; // My address
    SSP1MSK = 0b11111110;// Match every single address
    SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    BCL1IF = 0; // Clear I2C Bus Collision Flag
    BCL1IE = 1; // Enable I2C Bus Collision Interrupts
    SSP1IF = 0; // Clear interrupt on ACK flag
    SSP1IE = 1; // Enable interrupt on ACK
}


void InitApp(void)
{
    TRISA   = 0b00000111; // DACOUT_RA0(p7)
                          //RA1(p6):SCL RA2(p5):SDA
    i2c_init();

    ANSELA  = 0b00000001; // RA0 = Analog

    FVRCON = 0b11101000; // Setup FVR to 2.048V 8:FVREN(1) 7:FVRRDY(1) 6:TSEN(0) 5:TSRNG(0) 3..4:CDAFVR(10 2.048V) 1..2:ADFVR(00 Off)
    // Setup DAC
    DACCON0 = 0b10101000; // 8:DACEN(1) 7:DACLPS(0) 6:DACOE(1) 3..4:DACPSS(10 = FVR Buffer2 output)
    DACCON1 = 0b00000000; // 1..5:DACR(00000 - 0V)

    //TIMER0 SETUP
    OPTION_REG = 0b11000011; // 8:WPUEN(1) 7:INTEDG(1) 6:TMR0CS(0) 5:TMR0SE(0) 4:PSA(0 Timer0) 1..3:PS(011 1:16)
    TMR0IF = 0; // clear TMR0 interrupt flag
    TMR0IE = 1; // enable TMR0 interrupt


    /* Enable interrupts */
    PEIE = 1;
    GIE  = 1;
}
