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

static unsigned char _bl_index;
static unsigned char _bl_command;
static unsigned char _bl_temp;

static void _bl_write_i2c(unsigned char b) {
    do {
        SSPCONbits.WCOL = 0b0;
        SSPBUF = b;
    } while (SSPCONbits.WCOL);
}

void _bl_do_i2c_tasks() {
    if (SSPIF) {
        unsigned char i2c_state = SSPSTAT & 0b00100100;
        // 0b00100000 = D/nA
        // 0b00000100 = R/nW
        // prevent overflow
        switch (i2c_state) {
            case 0b00000000: // STATE1: Maser Write, Last Byte = Address
                // Disable timeout
                _bl_timeout = 0xffff;
                _bl_index = 0;
                _bl_command = 0;
                // do a dummy read
                _bl_temp = SSPBUF;
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                _bl_temp = SSPBUF;
                if(_bl_index == 0) {
                    _bl_command = _bl_temp;
                    _bl_index++;
                    break;
                }
                switch(_bl_command) {
                    case 0x01:
                        switch(_bl_index) {
                            case 1:
                                _bl_flash_addr_pointer.bytes.byte_L = _bl_temp;
                                break;
                            case 2:
                                _bl_flash_addr_pointer.bytes.byte_H = _bl_temp;
                                break;
                        }
                        break;
                    case 0x02:
                        if (_bl_index <= FLASH_BLOCK_BYTES) {
                            _bl_flash_buffer[_bl_index-1] = _bl_temp;
                        }
                        break;
                }
                _bl_index++;
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                _bl_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                _bl_temp = 0x0;
                switch(_bl_command) {
                    case 0x01:
                        _bl_temp = _bl_index == 0 ? _bl_flash_addr_pointer.bytes.byte_L : _bl_flash_addr_pointer.bytes.byte_H;
                        break;
                    case 0x03:
                        if(_bl_index == 0) {
                            // read data into flash_buffer
                            for (unsigned char idx = 0; idx < FLASH_BLOCK_BYTES; idx += 2) {
                                unsigned int dat = _bl_flash_memory_read(_bl_flash_addr_pointer.word.address);
                                _bl_flash_buffer[idx ] = dat >> 8;
                                _bl_flash_buffer[idx + 1] = dat & 0xFF;
                                _bl_flash_addr_pointer.word.address++;
                            }
                        }
                        if (_bl_index < FLASH_BLOCK_BYTES) {
                            _bl_temp = _bl_flash_buffer[_bl_index];
                        }
                        break;
                    case 0x04:
                        if (_bl_index == 0) {
                            // erase command, erases a row of ERASE_BLOCK_WORDS words
                            _bl_flash_memory_erase(_bl_flash_addr_pointer.word.address);
                            _bl_flash_addr_pointer.word.address += ERASE_BLOCK_WORDS;
                        }
                        break;
                    case 0x05:
                        if(_bl_index == 0) {
                            // write command. What's stored into flash_buffer is written
                            // to FLASH memory at the address pointed to by the address pointer.
                            // The address pointer automatically increments by FLASH_BLOCK_WORDS units.
                            _bl_flash_memory_write(_bl_flash_addr_pointer.word.address, _bl_flash_buffer);
                            _bl_flash_addr_pointer.word.address += FLASH_BLOCK_WORDS;
                        }
                        break;
                    case 0x06:
                        _bl_write_i2c(0xA0);
                        asm("reset");
                        break;
                }
                _bl_write_i2c(_bl_temp);
                _bl_index++;
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }
}
