/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "flash.h"         
#include "i2c.h"


/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/
unsigned char flash_buffer[FLASH_BLOCK_BYTES];
unsigned char pksa_wd_address;
unsigned char pksa_index;
unsigned char pksa_status;
ADDRESS flash_addr_pointer;
unsigned int timeout = 0x1000;
void interrupt isr(void) {
}

#asm
PSECT intentry
GOTO 0x204
#endasm

void main(void) {
    // Configure to 16 MHz
    OSCCON = 0b01101000; // 8:SPLLEN(0 by confb) 4..7:IRCF(1101 4 MHz) 1..2:SCS(00 use FOSC in confbits)
    SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    SSPADD = (unsigned char) (SLAVE_ADDR << 1); // Slave address
    SSPCON1 = 0b00110110; // 8:WCOL(0) 7:SSPOV(0) 6:SSPEN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address))
    SSPCON2 = 0b00000001; // 8:GCEN(0) 7:ACKSTAT(0) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
    SSPCON3 = 0b01101000; // 8:ACKTIM(0) 7:PCIE(1) 6:SCIE(1) 5:BOEN(1) 4:SDAHT(1) 3:SBCDE() 2:AHEN(0) 1:DHEN(1)
    timeout = 0x2000;
    OPTION_REG = 0x00;// Enable timer 0
    // main program loop
    while (1) {
        do_i2c_tasks();
    }
}