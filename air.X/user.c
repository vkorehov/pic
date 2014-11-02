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
    SSPCON3  = 0b00001000; //SDA Thold=300ns 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(1) 3:SBCDE(0) 2:AHEN(0) 1:DHEN(0)
    SSPSTAT = 0b11000000; // Slew rate disabled, SMBus compat. timings. SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = I2C_MYADDR << 1; // My address
}


void InitApp(void)
{
    i2c_init();
    // OMRON input voltage pin
    // A0 = AN0 is analog
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;

    ANSELBbits.ANSB0 = 0;
    TRISBbits.TRISB0 = 0;
    // FVR
    adc_band = ADC_MIN_BAND; // Start with minimum
    FVRCONbits.ADFVR = adc_band;
    FVRCONbits.FVREN = 1;
    while(!FVRCONbits.FVRRDY) {}
    //
    // ADC
    ADCON0bits.CHS = 0b00000; // ANO
    ADCON1bits.ADNREF = 0; // GND
    ADCON1bits.ADPREF = 0b11; // FVR Buffer 1
    ADCON1bits.ADFM = 1;// right justify
    ADCON1bits.ADCS = 0b010;// Tad = 1us @32Mhz
    ADCON0bits.ADON = 1;
    // TMR0
    TMR0 = 0x00;
    OPTION_REGbits.nWPUEN = 1;
    OPTION_REGbits.TMR0CS = 0;
    OPTION_REGbits.PS = 0b111; // Max pre-scaler
    OPTION_REGbits.PSA = 0;
    SSPIF = 0;
    ADIF = 0;
    TMR0IF = 0;
    /* Enable interrupts */
    TMR0IE = 1;
    ADIE = 1;
    SSPIE = 1;
    PEIE = 1;
    GIE  = 1;
}
