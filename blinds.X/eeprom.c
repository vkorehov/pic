/*
 * File:   eeprom.c
 * Author: vlad
 *
 * Created on August 20, 2017, 1:07 AM
 */


#include <xc.h>

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"

unsigned char eeprom_read(unsigned char address) {
    EEADRL = address;
    CFGS = 0; // access config program, not FLASH
    EEPGD = 0;
    RD = 1;
    return EEDATL;
}
//****************************************************************
//  FLASH MEMORY WRITE
//  needs 16 bit address pointer in address, 16 bit data pointer
//
//****************************************************************

void eeprom_write(unsigned char address, unsigned char data) {
    EEADRL = address;
    EEDATL = data;
    EEPGD = 0; // access config program, not FLASH
    CFGS = 0;
    WREN = 1; // allow eeprom/erase
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1; // set WR to begin write
    while(WR) {
    }
    WREN = 0;
}
