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
    // Initialise I2C MSSP
    SSPCON1  = 0b00110110; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address)
    SSPCON2  = 0b00000001; // Slave Clock stretching on rceive and send 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(1) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(1)
    SSPCON3  = 0b00001001; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(1)
    SSPSTAT = 0b11000000; // Slew rate disabled, SMBus compat. timings. SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = I2C_MYADDR << 1; // My address    
}

void InitApp(void)
{
    i2c_init();
    
#ifdef DEBUG  
    // UART
    TRISCbits.TRISC6 = 0;
    BAUDCONbits.BRG16 = 1;
    TXSTAbits.BRGH = 1;
    SPBRGL = 69; // 51.0 = 9600 //115200 68.4@32mhz  42@20mhz 
    SPBRGH = 0; 
    SCKP = 0;
    TXEN = 1;
    SYNC = 0;
    SPEN = 1;
#endif        
    
    ANSELAbits.ANSA0 = 0; //
    ANSELAbits.ANSA1 = 0; //
    ANSELAbits.ANSA2 = 0; //
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1;

    ANSELBbits.ANSB4 = 1; // AN11
    TRISBbits.TRISB4 = 1;
    ANSELBbits.ANSB2 = 0; // output 
    PORTBbits.RB2 = 0;
    TRISBbits.TRISB2 = 0;
    PORTBbits.RB2 = 0;
    
    ADPREF0=1; // FVR
    ADPREF1=1;    
    ADNREF=0; // GND
    
    ADFVR0 = 1; // 1.024V
    ADFVR1 = 0;
    FVREN = 1;
    while(!FVRRDY) {        
    }
    
    ADFM = 1;
    ADCON0bits.CHS = 11;
    ADCON1bits.ADCS = 0b110;
    
    ADIF = 0;
    ADIE = 1;
    
    ADON = 1;

    // Timer1
    TMR1GE = 0;
    TMR1CS0 = 0;
    TMR1CS1 = 0;
    T1CKPS0 = 1;
    T1CKPS1 = 1;
    nT1SYNC = 1;

    TMR1IF = 0;
    TMR1IE = 1;
    
    write_tmr1(AD_DELAY);
    TMR1ON = 1;
    
    SSPIF = 0;
    /* Enable interrupts */
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}
