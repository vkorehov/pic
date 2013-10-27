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

#ifdef _12F1840
#define SSPIF SSP1IF
#endif

void __bootloader__WriteData(unsigned char data) {
    do {
        WCOL = 0;
        SSPBUF = data;
    } while (WCOL);
    CKP = 1;
}

void __bootloader_do_i2c_tasks() {
    unsigned int dat = 0;
    unsigned char temp, idx;
    if (SSPIF) {
        if (SSPSTATbits.S) {
            if (SSPSTATbits.D_nA && !SSPSTATbits.BF && CKP) { // Master NACK
                goto __bootloader_stop;
            } else if (!SSPSTATbits.R_nW && !SSPSTATbits.D_nA && SSPSTATbits.BF) { //MASTER WRITES ADDRESS STATE
                // Disable timeout
                OPTION_REG = 0b11111111;
                temp = SSPBUF;
                __bootloader_pksa_status = I2C_SLAVE_ADDRESS_RECEIVED;
            } else if (!SSPSTATbits.R_nW && SSPSTATbits.D_nA && SSPSTATbits.BF) { // MWD: //MASTER WRITES DATA STATE
                temp = SSPBUF;
                if (__bootloader_pksa_status == I2C_SLAVE_ADDRESS_RECEIVED) { // first time we get the slave address, after that set to word address
                    __bootloader_pksa_wd_address = temp;
                    __bootloader_pksa_index = 0;
                    __bootloader_pksa_status = I2C_WORD_ADDRESS_RECEIVED;
                } else if (__bootloader_pksa_status == I2C_WORD_ADDRESS_RECEIVED) { // second time we get the word address, so look into word address
                    if (__bootloader_pksa_wd_address == 0x01) { // 0x01 is buffer word address
                        if (__bootloader_pksa_index == 0) {
                            __bootloader_flash_addr_pointer.bytes.byte_L = temp;
                            __bootloader_pksa_index++;
                        } else if (__bootloader_pksa_index == 1) {
                            __bootloader_flash_addr_pointer.bytes.byte_H = temp;
                        }
                    } else if (__bootloader_pksa_wd_address == 0x02) { // 0x02 write data word address
                        if (__bootloader_pksa_index < FLASH_BLOCK_BYTES) {
                            __bootloader_flash_buffer[__bootloader_pksa_index] = temp;
                        }
                        __bootloader_pksa_index++;
                    }
                }
            } else if (SSPSTATbits.R_nW && !SSPSTATbits.D_nA) { //MASTER READS ADDRESS STATE
                // Disable timeout
                OPTION_REG = 0b11111111;
                if (__bootloader_pksa_wd_address == 0x01) { // buffer word address
                    // Send first byte here, next byte will be send at MRD case, see below
                    __bootloader__WriteData(__bootloader_flash_addr_pointer.bytes.byte_L);
                } else if (__bootloader_pksa_wd_address == 0x03) { // read data from flash memory
                    if (__bootloader_pksa_index == 0) {
                        // read data into flash_buffer
                        for (idx = 0; idx < FLASH_BLOCK_BYTES; idx += 2) {
                            dat = __bootloader_flash_memory_read(__bootloader_flash_addr_pointer.word.address);
                            __bootloader_flash_buffer[idx ] = dat >> 8;
                            __bootloader_flash_buffer[idx + 1] = dat & 0xFF;
                            __bootloader_flash_addr_pointer.word.address++;
                        }
                        // send first byte, the rest will be sent at MRD, see below
                        __bootloader__WriteData(__bootloader_flash_buffer[__bootloader_pksa_index]);
                        __bootloader_pksa_index++;
                    }
                } else if (__bootloader_pksa_wd_address == 0x04) {
                    // erase command, erases a row of ERASE_BLOCK_WORDS words
                    __bootloader_flash_memory_erase(__bootloader_flash_addr_pointer.word.address);
                    __bootloader_flash_addr_pointer.word.address += ERASE_BLOCK_WORDS;
                    __bootloader__WriteData(0x00);
                } else if (__bootloader_pksa_wd_address == 0x05) {
                    // write command. What's stored into flash_buffer is written
                    // to FLASH memory at the address pointed to by the address pointer.
                    // The address pointer automatically increments by FLASH_BLOCK_WORDS units.
                    __bootloader_flash_memory_write(__bootloader_flash_addr_pointer.word.address, __bootloader_flash_buffer);
                    __bootloader_flash_addr_pointer.word.address += FLASH_BLOCK_WORDS;
                    __bootloader__WriteData(0x00);
                } else if (__bootloader_pksa_wd_address == 0x06) {
                    // jump to appplication code
                    __bootloader__WriteData(0xA0);
__bootloader_appjmp:
                    asm("reset");
                }
            } else if (SSPSTATbits.R_nW && SSPSTATbits.D_nA && !SSPSTATbits.BF) { //MASTER READS DATA STATE
                if (__bootloader_pksa_wd_address == 0x01) // buffer word address
                {
                    __bootloader__WriteData(__bootloader_flash_addr_pointer.bytes.byte_H);
                } else if (__bootloader_pksa_wd_address == 0x03) {
                    if (__bootloader_pksa_index < FLASH_BLOCK_BYTES) {
                        __bootloader__WriteData(__bootloader_flash_buffer[__bootloader_pksa_index]);
                    }
                    __bootloader_pksa_index++;
                }
            }
        }
        if (SSPSTATbits.P) { //STOP or NACK state
__bootloader_stop:
            // reset status register
            SSPSTAT = 0b11000000; // Slew rate disabled SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
            __bootloader_pksa_status = I2C_NO_TRANSACTION;
        }
        SSPIF = 0;
        SSPEN = 1;
        CKP = 1; //release clock
    } else if(TMR0IF) {
        if(!(--__bootloader_timeout)) {
            goto __bootloader_appjmp;
        }
        TMR0IF = 0;
    }
}