/******************************************************************************/
/*Files to Include                                                            */
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

volatile unsigned char ENTER_BOOTLOADER __at(0x30); /* flag in order to enter bootloader */
#ifdef _12F1840
#define SSPIF SSP1IF
#endif

#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char command;
static unsigned char counter;

void __interrupt () isr(void) {
    if (SSPIF) {
        SSPIF = 0;
        unsigned char tmp;
        unsigned char r = 0;
        unsigned char crc = 0;
        unsigned char i2c_state = SSPSTAT & 0b00100100;
        // 0b00100000 = D/nA
        // 0b00000100 = R/nW
        // prevent overflow
        if (rx_index >= RX_SIZE) {
            rx_index = 0;
        }
        switch (i2c_state) {
            case 0b00000000: // STATE1: Maser Write, Last Byte = Address
                rx_index = 0;
                command = 0;
                ACKDT = 0;
                for (int i = 0; i < RX_SIZE; i++) {
                    rx_buffer[i] = 0;
                }
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                // do a dummy read
                rx_buffer[0] = SSPBUF;
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                rx_buffer[rx_index++] = SSPBUF;
                if (rx_index == 1) {
                    command = rx_buffer[0]; // save command
                }
                if (rx_index == 2) {
                    // input
                    switch (command) {
                        case 0x78:
                            crc = crc8_table[crc ^ 0x78];
                            crc = crc8_table[crc ^ I2C_MYADDR];
                            if (crc == rx_buffer[1]) {
                                ENTER_BOOTLOADER = 1;
                                asm("pagesel 0x000");
                                asm("goto 0x000");
                            } else {
                                ACKDT = 1;
                            }
                            break;
                    }
                }
                if (rx_index == 3) {
                    // input
                    switch (command) {
                        case 0x01:
                            crc = crc8_table[rx_buffer[1]];
                            crc = crc8_table[crc ^ 0x01];
                            crc = crc8_table[crc ^ I2C_MYADDR];
                            if (crc == rx_buffer[2]) {
                                //
//                                if (rx_buffer[1] != 0x00) {
//                                    on(rx_buffer[1]);
//                                } else {
//                                    off();
//                                }
                            } else {
                                ACKDT = 1;
                            }
                            break;
                    }
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
                // output
                switch (command) {
                    case 0x01: // read command
                        crc = crc8_table[0xff];
                        crc = crc8_table[crc ^ 0x01];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = 0xff;
                        rx_buffer[1] = crc;
                        break;
                    default:
                        rx_buffer[0] = rx_buffer[1] = 0;
                }
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                // do a dummy read
                tmp = SSPBUF;
                if (SSPSTATbits.BF == 1) {
                    break;
                }
                SSPBUF = rx_buffer[rx_index++];
                break;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                if (SSPSTATbits.BF == 1) {
                    break;
                }
                if (rx_index >= RX_SIZE) { // prevent overflow
                    SSPBUF = 0;
                } else {
                    SSPBUF = rx_buffer[rx_index++];
                }
                break;
        }
        SSPCON1bits.CKP = 1;
    }

}
