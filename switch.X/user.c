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
//#ifndef _12F1840
//    APFCONbits.CCP2SEL = 1; // RB3 to be CCP2/P2A
//#endif
    i2c_init();
    // Output pin
    TRISAbits.TRISA0 = 0;
    PORTAbits.RA0 = 0;

    switch_count = 0;
    dht22_state = 0;
    dht22_index = 0;
    dht22_bit_index = 0;
    for(int i = 0; i < DHT22_MAX_BYTES; i++) {
        dht22_bits[i] = 0;
    }

    ANSELB = 0;// stupid
    TRISBbits.TRISB0 = 1;

    // TIMER2
    TMR2ON = 0;
    TMR2 = 0x00;
    PR2 = 0xFF;
    T2CONbits.T2CKPS = 0b00; // 1:64
    T2CONbits.T2OUTPS = 0b0000;// 1:16 post scaler
    TMR2IF = 0;

    IOCBNbits.IOCBN0 = 0;
    IOCBPbits.IOCBP0 = 0;
    /* Enable interrupts */
    IOCBF = 0;
    IOCIE = 1;
    SSPIF = 0;
    /* Enable interrupts */
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}