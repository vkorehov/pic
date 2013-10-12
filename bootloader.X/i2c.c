#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>     8   /* HiTech General Include File */
#endif
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "flash.h"
#include "i2c.h"
static unsigned int timeout = 0xffffffffffffffff;
void _WriteData(unsigned char data) {
    do {
        WCOL = 0;
        SSPBUF = data;
    } while (WCOL);
    CKP = 1;
}

void do_i2c_tasks() {
    unsigned int dat = 0;
    unsigned char temp, idx;
    if (SSPIF) {
        if (SSPSTATbits.S) {
            if (SSPSTATbits.D_nA && !SSPSTATbits.BF && CKP) { // Master NACK
                goto stop;
            } else if (!SSPSTATbits.R_nW && !SSPSTATbits.D_nA && SSPSTATbits.BF) { //MASTER WRITES ADDRESS STATE
                temp = SSPBUF;
                pksa_status = I2C_SLAVE_ADDRESS_RECEIVED;
                timeout = 0; // Cancell timeout
            } else if (!SSPSTATbits.R_nW && SSPSTATbits.D_nA && SSPSTATbits.BF) { // MWD: //MASTER WRITES DATA STATE
                temp = SSPBUF;
                if (pksa_status == I2C_SLAVE_ADDRESS_RECEIVED) { // first time we get the slave address, after that set to word address
                    pksa_wd_address = temp;
                    pksa_index = 0;
                    pksa_status = I2C_WORD_ADDRESS_RECEIVED;
                } else if (pksa_status == I2C_WORD_ADDRESS_RECEIVED) { // second time we get the word address, so look into word address
                    if (pksa_wd_address == 0x01) { // 0x01 is buffer word address
                        if (pksa_index == 0) {
                            flash_addr_pointer.bytes.byte_L = temp;
                            pksa_index++;
                        } else if (pksa_index == 1) {
                            flash_addr_pointer.bytes.byte_H = temp;
                        }
                    } else if (pksa_wd_address == 0x02) { // 0x02 write data word address
                        flash_buffer[pksa_index] = temp;
                        pksa_index++;
                        if (pksa_index == 16)
                            pksa_index--;
                    }
                }
            } else if (SSPSTATbits.R_nW && !SSPSTATbits.D_nA) { //MASTER READS ADDRESS STATE
                timeout = 0; // Cancell timeout
                if (pksa_wd_address == 0x01) { // buffer word address
                    // Send first byte here, next byte will be send at MRD case, see below
                    _WriteData(flash_addr_pointer.bytes.byte_L);
                } else if (pksa_wd_address == 0x03) { // read data from flash memory
                    if (pksa_index == 0) {
                        // read data into flash_buffer
                        for (idx = 0; idx < 16; idx += 2) {
                            dat = flash_memory_read(flash_addr_pointer.word.address);
                            flash_buffer[idx ] = dat >> 8;
                            flash_buffer[idx + 1] = dat & 0xFF;
                            flash_addr_pointer.word.address++;
                        }
                        // send first byte, the rest will be sent at MRD, see below
                        _WriteData(flash_buffer[pksa_index]);
                        pksa_index++;
                        if (pksa_index == 16)
                            pksa_index--; // should never get here....
                    }
                } else if (pksa_wd_address == 0x04) {
                    // erase command, erases a row of 32 words
                    flash_memory_erase(flash_addr_pointer.word.address);
                    flash_addr_pointer.word.address += 32;
                    _WriteData(0xef);
                } else if (pksa_wd_address == 0x05) {
                    // write command. What's stored into flash_buffer is written
                    // to FLASH memory at the address pointed to by the address pointer.
                    // The address pointer automatically increments by 8 units.
                    flash_memory_write(flash_addr_pointer.word.address, flash_buffer);
                    flash_addr_pointer.word.address += 8;
                    _WriteData(0x00);
                } else if (pksa_wd_address == 0x06) {
                    // jump to appplication code
                    _WriteData(0xA0);
                    timeout = 1;
                }
            } else if (SSPSTATbits.R_nW && SSPSTATbits.D_nA && !SSPSTATbits.BF) { //MASTER READS DATA STATE
                if (pksa_wd_address == 0x01) // buffer word address
                {
                    _WriteData(flash_addr_pointer.bytes.byte_H);
                } else if (pksa_wd_address == 0x03) {
                    _WriteData(flash_buffer[pksa_index]);
                    pksa_index++;
                    if (pksa_index == 16)
                        pksa_index--;
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
    } else {
        if (timeout == 1) {
            // Disable what we have enabled
            TRISC = 0x00; //RC3(p14):SCL RC4(p15):SDA
            SSPSTAT = 0x00; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
            SSPADD = 0x00; // Slave address
            SSPCON1 = 0x00; // 8:WCOL(0) 7:SSP1OV(0) 6:SSPEN(1) 5:CKP(1) 1..4:SSPM(0110 I2C Slave mode, 7-bit address))
            SSPCON2 = 0x00; // 8:GCEN(0) 7:ACKSTAT(0) 6:ACKDT(0) 5:ACKEN(0) 4:RCEN(0) 3:PEN(0) 2:RSEN(0) 1:SEN(0)
            SSPCON3 = 0x00; // 8:ACKTIM(0) 7:PCIE(1) 6:SCIE(1) 5:BOEN(1) 4:SDAHT(1) 3:SBCDE() 2:AHEN(0) 1:DHEN(1)
#asm
            goto 0x200;
#endasm
        } else if (timeout) {
            //timeout--;
        }
    }
}
