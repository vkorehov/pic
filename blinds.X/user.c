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
#ifdef _12F1840
#define SSPIE SSP1IE
#define SSPIF SSP1IF
#endif

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

void i2c_init() {
    SSPCON1  = 0b00110110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address)
    SSPCON2  = 0b00000001; // Slave Clock stretching on rceive and send 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(1)
    SSPCON3  = 0b00011001; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(1) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(1)
    SSPSTAT = 0b11000000; // Slew rate disabled, SMBus compat. timings. SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = I2C_MYADDR << 1; // My address                
}

void InitApp(void)
{
    i2c_init();
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA4 = 0;
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA4 = 0;
    RA0 = 0;
    RA4 = 0;
    // input from encoder
    TRISAbits.TRISA5 = 1;
    // TIMER1
    TMR1CS1 = 0;    
    TMR1CS0 = 1;
    T1CKPS1 = 0;    
    T1CKPS0 = 0;
    nT1SYNC = 1;
    TMR1GE = 0;
    TMR1L = 0;
    TMR1H = 0;
    next_position = position = ((unsigned int)eeprom_read(0x01) << 8) | (unsigned int)eeprom_read(0x00);
    if(position == 0xFFFF) {// flash not initialied
        next_position = position = 128;
    }
    //next_position = position = 128;
    command_position = 0;
    
    TMR1ON = 1;

    SSPIF = 0;
    TMR1IF = 0;
    /* Enable interrupts */
    TMR1IE = 1;    
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}
