/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic12f1840.h>        /* For true/false definition */
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

unsigned int ticks = 0;
bool prev_state = false;

inline void write_ticks(unsigned int val) {
    ticks = val;
}
inline unsigned int read_ticks() {
    return ticks >> 2;
}

inline unsigned int read_ra5() {
    return PORTAbits.RA5;
}

unsigned int debounce_state = 0; // Current debounce status
inline bool debounce_ra5() {
    debounce_state = (debounce_state<<1) | read_ra5() | 0xe000;
    if(debounce_state == 0xe000)return true;
    return false;
}

void __interrupt () isr(void) {
    if (TMR1IF) {
        TMR1IF = 0;
        bool current_state = debounce_ra5();
        if(current_state != prev_state) {
            prev_state = current_state;
            ticks++;
        }
    }
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
                                command_position = (unsigned int)rx_buffer[1];
                                eeprom_write(0x00, command_position & 0xFF);
                                eeprom_write(0x01, (command_position >> 8) & 0xFF);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;
                        case 0x02:
                            crc = crc8_table[rx_buffer[1]];
                            crc = crc8_table[crc ^ 0x02];
                            crc = crc8_table[crc ^ I2C_MYADDR];
                            if (crc == rx_buffer[2]) {
                                position = next_position = (unsigned int)rx_buffer[1];
                                write_ticks(0);
                                eeprom_write(0x00, position & 0xFF);
                                eeprom_write(0x01, (position >> 8) & 0xFF);
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
                        crc = crc8_table[state];
                        crc = crc8_table[crc ^ 0x01];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = state;
                        rx_buffer[1] = crc;
                        break;
                    case 0x02: // read command
                        r =  eeprom_read(0);
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x02];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x03: // read command
                        r = eeprom_read(1);
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x03];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x04: // read command
                        r = read_ticks() & 0xff;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x04];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x05: // read command
                        r = read_ticks() >> 8;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x05];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x06: // read command
                        r = position & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x06];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x07: // read command
                        r = (position >> 8) & 0xff;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x07];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x08: // read command
                        r = next_position & 0xff;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x08];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x09: // read command
                        r = (next_position >> 8) & 0xff;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x09];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x0a: // read command
                        r = command_position & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x0a];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x0b: // read command
                        r = (command_position >> 8) & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x0b];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
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
