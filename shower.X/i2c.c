
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


unsigned char pksa_index = 0;
unsigned char pksa_wd_address = 0;
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
                if(temp == 0x00) { // general call = Sync request from pacemaker

                }
            } else if (!SSPSTATbits.R_nW && SSPSTATbits.D_nA && SSPSTATbits.BF) { // MWD: //MASTER WRITES DATA STATE
                temp = SSPBUF;
                if (pksa_status == I2C_SLAVE_ADDRESS_RECEIVED) { // first time we get the slave address, after that set to word address
                    pksa_wd_address = temp;
                    pksa_index = 0;
                    pksa_status = I2C_WORD_ADDRESS_RECEIVED;
                } else if (pksa_status == I2C_WORD_ADDRESS_RECEIVED) { // second time we get the word address, so look into word address
                    if (pksa_wd_address == 0x01) { // 0x01 is buffer word address
                        if (pksa_index == 0) {
                            // TODO
                            pksa_index++;
                        } else if (pksa_index == 1) {
                            // TODO
                        }
                    } else if (pksa_wd_address == 0x02) { // 0x02 write data word address
                        if (pksa_index < 50) {
                            // TODO
                        }
                        pksa_index++;
                    }
                }
            } else if (SSPSTATbits.R_nW && !SSPSTATbits.D_nA) { //MASTER READS ADDRESS STATE
                if (pksa_wd_address == 0x01) { // buffer word address
                    // Send first byte here, next byte will be send at MRD case, see below
                    // TODO
                    _WriteData(0x10);
                } else if (pksa_wd_address == 0x03) { // read data from flash memory
                    if (pksa_index == 0) {
                        // send first byte, the rest will be sent at MRD, see below
                        // TODO
                        _WriteData(0x20);
                        pksa_index++;
                    }
                } else if (pksa_wd_address == 0x04) {
                    // TODO
                    _WriteData(0x01);
                } else if (pksa_wd_address == 0x05) {
                    // TODO
                    _WriteData(0x00);
                } else if (pksa_wd_address == 0x06) {
                    // TODO
                    _WriteData(0xA0);
                }
            } else if (SSPSTATbits.R_nW && SSPSTATbits.D_nA && !SSPSTATbits.BF) { //MASTER READS DATA STATE
                if (pksa_wd_address == 0x01) // buffer word address
                {
                    // TODO
                    _WriteData(0x30);
                } else if (pksa_wd_address == 0x03) {
                    if (pksa_index < 50) {
                        // TODO
                        _WriteData(0x10);
                    }
                    pksa_index++;
                }
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

