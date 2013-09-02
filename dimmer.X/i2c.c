
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

void i2c_check_error() {
    if (WCOL) {
        i2c_error = 2;
        WCOL = 0;
    }
    if (SSPOV) {
        i2c_error = 3;
        SSPOV = 0;
    }
}

unsigned short i2c_on_command(unsigned char* command) {
    unsigned short old = hz100_command;
    if (command[1] != 0xff) {
        hz100_command = *((unsigned short*) &command[2]);
    }
    return old;
}

void i2c_Wait(void) {
    //SSP1CON2: 8:GCEN 7:ACKSTAT 6:ACKDT 5:ACKEN 4:RCEN 3:PEN 2:RSEN 1:SEN
    //SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    while ((SSP1CON2 & 0b00011111) || (SSPSTAT & 0b00000100));
}

// i2c_Write - Sends one byte of data

void i2c_Write(unsigned char data) {
    i2c_Wait();
    SSP1BUF = data;
    i2c_check_error();
}

void i2c_Address(unsigned char address, unsigned char mode) {
    unsigned char l_address;
    l_address = address << 1;
    l_address += mode;
    i2c_Wait();
    SSP1BUF = l_address;
    i2c_check_error();
}

