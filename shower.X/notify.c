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

unsigned char notify_state;
#define NOTIFY_I2C_ADDR 0x40

void notify_destroy() {
    SSPCON1 = 0x00;
    i2c_init(); // re-initialize i2c slave
    BCLIE = 0; // Disable I2C Bus Collision Interrupts
    BCLIF = 0; // Clear I2C Bus Collision Flag
    SSPIE = 0; // Disable interrupt on ACK
    SSPIF = 0; // Clear interrupt on ACK flag

}
void notify_init() {
    i2c_master = 1; // master mode
    // Initialise I2C MSSP
    SSPCON1 = 0b00101000; // 8:WCOL(0) 7:SSP1OV(0) 6:SSP1EN(1) 5:CKP(0) 1..4:SSPM(1000 I2C Master mode, clock = FOSC / (4 * (SSPADD+1)))
    SSPCON2  = 0b01000000; // 8:GCEN(0) 7:ACKSTAT(1) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSPCON3  = 0b00000000; // 8:ACKTIM(0) 7:PCIE(0) 6:SCIE(0) 5:BOEN(0) 4:SDAHT(0) 3:SBCDE() 2:AHEN() 1:DHEN(0)
    // I2C Master mode, clock = FOSC/(4 * (SSPADD + 1))
    //SSPADD = 9;    		// 100Khz @ 4Mhz Fosc
    //SSPADD = 79;    		// 100Khz @ 32Mhz Fosc
    SSPADD = 79;
    SSPSTAT = 0b11000000; 	// Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    BCLIF = 0; // Clear I2C Bus Collision Flag
    BCLIE = 1; // Enable I2C Bus Collision Interrupts
    SSPIF = 0; // Clear interrupt on ACK flag
    SSPIE = 1; // Enable interrupt on ACK
}

void notify(unsigned char* data, unsigned char data_size) {
    switch(notify_state) {
        case 0:
                notify_init(); // initialize chip in master mode
                SEN = 1; // initiate start event
                notify_state = 1;
            break;
        case 1:

            break;
        case 2:

            break;
        default:
            
            break;
    }
}
