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
unsigned int switch_count;
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
    SSPCON3  = 0b00001000; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(0)
    SSPSTAT = 0b11000000; // Slew rate disabled, SMBus compat. timings. SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = I2C_MYADDR << 1; // My address
}


void InitApp(void)
{
    FVRCONbits.CDAFVR = 0b10; // 2V
    FVRCONbits.FVREN = 1;
    while(!FVRCONbits.FVRRDY) {
    }

    i2c_init();
    ANSELAbits.ANSA4 = 1;//C1IN1- pin

    CM1CON1bits.C1NCH = 0b01; // C1VN connects to C1IN1- pin
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
    PORTAbits.RA0 = 0;
    TRISAbits.TRISA0 = 0;

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
    SSPIF = 0;
    /* Enable interrupts */
    TMR1IE = 1;
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}
