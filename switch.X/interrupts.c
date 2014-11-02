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

static void write_i2c(unsigned char b) {
    // insert slight delay, otherwise raspbery pi reads first bit as zero i.e. 0x81 => 0x01
    for (int i = 0; i < 8; i++) {
        asm("nop");
    }
    do {
        SSPCONbits.WCOL = 0b0;
        SSPBUF = b;
    } while (SSPCONbits.WCOL);
}

void interrupt isr(void) {
    if (TMR2IF) {
        TMR2IF = 0;
        switch (dht22_state) {
            case 1:
                start_read_dht22_pullup();
                break;
            default:
                dht22_abort();
        }
    }
    if (IOCIF) {
        asm("MOVLW 0xff");
        asm("banksel IOCBF");
        asm("XORWF IOCBF, W");
        asm("ANDWF IOCBF, F");
        unsigned char t = TMR2;
        TMR2 = 0; // reset timeout
        if (PORTBbits.RB0 == 0) {// we care only about HI => LO transition
            if (dht22_state < 4) {
                dht22_state++;
            } else {
                if (dht22_bit_index > 7) {
                    dht22_bit_index = 0;
                    dht22_index++;
                }
                if (t > DHT22_CUTOFF_TIME) {
                    dht22_bits[dht22_index] |= (1 << (7 - dht22_bit_index));
                }
                dht22_bit_index++;
                if (dht22_bit_index == 8 &&
                        dht22_index >= (DHT22_MAX_BYTES - 1)) {
                    unsigned char sum = dht22_bits[0] + dht22_bits[1] + dht22_bits[2] + dht22_bits[3];
                    if (sum != dht22_bits[4]) {// checksum doesn't match up?
                        // indicate error
                        dht22_bits[0] = dht22_bits[1] = dht22_bits[2] = dht22_bits[3] = 0;
                    }
                    dht22_abort();
                }
            }
        }
    }

    if (SSPIF) {
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
                // do a dummy read
                rx_buffer[0] = SSPBUF;
                for (int i = 0; i < RX_SIZE; i++) {
                    rx_buffer[i] = 0;
                }
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                rx_buffer[rx_index++] = SSPBUF;
                if (rx_index == 1) {
                    command = rx_buffer[0]; // save command
                    // input
                    switch (command) {
                        case 0x78:
                            ENTER_BOOTLOADER = 1;
                            asm("pagesel 0x000");
                            asm("goto 0x000");
                            break;
                        case 0x01:
                            on();
                            break;
                        case 0x02:
                            start_read_dht22();
                            break;
                    }
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // output
                switch (command) {
                    case 0x03: // read humidity
                        rx_buffer[0] = dht22_bits[1];
                        rx_buffer[1] = dht22_bits[0];
                        break;
                    case 0x04: // // read temperature
                        rx_buffer[0] = dht22_bits[3];
                        rx_buffer[1] = dht22_bits[2];
                        break;
                    default:
                        rx_buffer[0] = rx_buffer[1] = 0;
                }
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }

}
