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

//****************************************************************
//  FLASH MEMORY READ
//  needs 16 bit address pointer in address
//  returns 14 bit value from selected address
//
//****************************************************************

unsigned int _bl_flash_memory_read(unsigned int address) {
    EEADRL = ((address)&0xff);
    EEADRH = ((address) >> 8);
    CFGS = 0; // access FLASH program, not config
    EEPGD = 1;
    LWLO = 0;
    RD = 1;
    asm("NOP");
    asm("NOP");
    return ( (EEDATL) << 8 | (EEDATH));
}
//****************************************************************
//  FLASH MEMORY WRITE
//  needs 16 bit address pointer in address, 16 bit data pointer
//
//****************************************************************

void _bl_flash_memory_write(unsigned int address, unsigned char *data) {
    unsigned char wdi;
    EECON1 = 0;
    EEADRL = ((address)&0xff); // load address
    EEADRH = ((address) >> 8); // load address
    EEPGD = 1; // access program space FLASH memory
    CFGS = 0;
    ; // access FLASH program, not config
    WREN = 1; // allow program/erase
    LWLO = 1;
    for (wdi = 0; wdi < FLASH_BLOCK_BYTES; wdi += 2) {
        EEDATL = data[wdi];
        EEDATH = data[wdi + 1];
        EECON2 = 0x55;
        EECON2 = 0xAA;
        WR = 1; // set WR to begin write
        asm("NOP");
        asm("NOP");
        EEADR++;
    }
    EEADR--;
    LWLO = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1; // set WR to begin write
    asm("NOP");
    asm("NOP");
    WREN = 0; // disallow program/erase
}
//****************************************************************
//  FLASH MEMORY ERASE
//  Program memory can only be erased by rows.
//  A row consists of 32 words where the EEADRL<4:0> = 0000.
//
//****************************************************************

void _bl_flash_memory_erase(unsigned int address) {
    EEADRL = ((address)&0xff); // load address
    EEADRH = ((address) >> 8); // load address
    CFGS = 0; // access FLASH program, not config
    LWLO = 0;
    WREN = 1; // allow program/erase
    EEPGD = 1; // access program space FLASH memory
    FREE = 1; // perform an erase on next WR command, cleared by hardware
    EECON2 = 0x55; // required sequence
    EECON2 = 0xAA; // required sequence
    WR = 1; // set WR to begin erase cycle
    asm("NOP");
    asm("NOP");
    WREN = 0; // disallow program/erase
}
