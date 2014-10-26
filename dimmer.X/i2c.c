
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

#include "system.h"
#include "user.h"

#ifdef _12F1840
#define SSPIF SSP1IF
#endif

volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

unsigned char pksa_status = 0;
unsigned char after=10;


void _WriteData(unsigned char data) {
    do {
        WCOL = 0;
        SSPBUF = data;
    } while (WCOL);
    CKP = 1;
}

void do_i2c_tasks(void) {
    unsigned int dat = 0;
    unsigned char temp, idx;
    if (SSPIF) {
        if (SSPSTATbits.S) {
            if (SSPSTATbits.D_nA && !SSPSTATbits.BF && CKP) { // Master NACK
                goto stop;
            } else if (!SSPSTATbits.R_nW && !SSPSTATbits.D_nA && SSPSTATbits.BF) { //MASTER WRITES ADDRESS STATE
                temp = SSPBUF;
                pksa_status = I2C_SLAVE_ADDRESS_RECEIVED;
                //if(temp == 0x00) general call
            } else if (!SSPSTATbits.R_nW && SSPSTATbits.D_nA && SSPSTATbits.BF) { // MWD: //MASTER WRITES DATA STATE
                temp = SSPBUF;
                if (pksa_status == I2C_SLAVE_ADDRESS_RECEIVED) { // first time we get the slave address, after that set to word address
                    if(temp == 0x78) { // enter bootloader command
                        ENTER_BOOTLOADER = 1;
                        asm("goto 0x000");
                    } else if (temp == 0x01) { // turn on command
                        //TMR0 = 0x00;
                        switch_count = 0;
                        //TMR0IE = 1;
                        PORTAbits.RA0 = 1;
                    }
                }
            } else if (SSPSTATbits.R_nW && !SSPSTATbits.D_nA) { //MASTER READS ADDRESS STATE
                _WriteData(I2C_MYADDR);
            } else if (SSPSTATbits.R_nW && SSPSTATbits.D_nA && !SSPSTATbits.BF) { //MASTER READS DATA STATE
                _WriteData(I2C_MYADDR);
            }
        }
        if (SSPSTATbits.P) { //STOP or NACK state
stop:
            // reset status register
            SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
            pksa_status = I2C_NO_TRANSACTION;
        }
        SSPIF = 0;
        SSPEN = 1;
        CKP = 1; //release clock
    }

}

