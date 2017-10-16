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

volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */
#ifdef _12F1840
#define SSPIF SSP1IF
#endif

#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char command;
static unsigned char counter;

void interrupt isr(void) {
    if (TMR1IF) {
        TMR1IF = 0;
        GO = 1;
        if (counter++ >= 60) {
            counter = 0;
            if (faucet_timeout != 0) {
                faucet_timeout--;
            }
        }
    }
    if (ADIF) {
        unsigned int reading;
        ADIF = 0;
        reading = ADRESL;
        reading |= (ADRESH << 8);
        if (ADCON0bits.CHS == 8) {
            ADCON0bits.CHS = 9;
            ADPREF0 = 1;
            ADPREF1 = 1;
            ADNREF = 0;
            // Initialize
            if (sensor1 == 0) {
                sensor1 = INIT_VECTOR;
            }
            // Slew Rate Limiter
            if (reading > sensor1)
                sensor1++;
            else if (sensor1 != 0) // avoid flipping
                sensor1--;

        } else if (ADCON0bits.CHS == 9) {
            ADCON0bits.CHS = 11;
            ADPREF0 = 0; // Vref+
            ADPREF1 = 1;
            ADNREF = 1; // Vref-

            // Initialize
            if (sensor2 == 0) {
                sensor2 = INIT_VECTOR;
            }
            // Slew Rate Limiter
            if (reading > sensor2)
                sensor2++;
            else if (sensor2 != 0) // avoid flipping
                sensor2--;

        } else {
            ADCON0bits.CHS = 8;
            ADPREF0 = 1;
            ADPREF1 = 1;
            ADNREF = 0;

            // Initialize
            if (sensor3 == 0) {
                sensor3 = INIT_VECTOR;
            }
            // Slew Rate Limiter
            if (reading > sensor3)
                sensor3++;
            else if (sensor3 != 0) // avoid flipping
                sensor3--;
        }
        write_tmr1(AD_DELAY);
        TMR1ON = 1;
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
                                //
                                if (rx_buffer[1] == 1) {
                                    faucet_on = 1;
                                    faucet_timeout = FAUCET_TIMEOUT;
                                } else {
                                    faucet_on = 0;
                                    faucet_timeout = 0;
                                }
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
                        crc = crc8_table[faucet_on];
                        crc = crc8_table[crc ^ 0x01];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = faucet_on;
                        rx_buffer[1] = crc;
                        break;
                    case 0x02: // read command
                        r = sensor1 & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x02];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x03: // read command
                        r = (sensor1 >> 8) & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x03];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x04: // read command
                        r = sensor2 & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x04];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x05: // read command
                        r = (sensor2 >> 8) & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x05];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x06: // read command
                        r = sensor3 & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x06];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = r;
                        rx_buffer[1] = crc;
                        break;
                    case 0x07: // read command
                        r = (sensor3 >> 8) & 0xFF;
                        crc = crc8_table[r];
                        crc = crc8_table[crc ^ 0x07];
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
