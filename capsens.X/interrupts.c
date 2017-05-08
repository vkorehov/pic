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
#include <stdio.h>
#include "user.h"        /* For true/false definition */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

static unsigned int ticks;

#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char command;

static void write_i2c(unsigned char b) {
    // insert slight delay, otherwise raspbery pi reads first bit as zero i.e. 0x81 => 0x01
    for (int i = 0; i < 64; i++) {
        asm("nop");
    }
    do {
        SSPCONbits.WCOL = 0b0;
        SSPBUF = b;
    } while (SSPCONbits.WCOL);
}

inline void process_cps(unsigned char ch, unsigned int raw) {
    // Initialize
    if (readings[ch] == 0) {
        readings[ch] = raw;
    }
    // Slew Rate Limiter
    if(raw > readings[ch])
        readings[ch]++;
    else if(readings[ch] != 0) // avoid flipping
        readings[ch]--;
    readings_counter++; 
}

void interrupt isr(void) {    
    if (TMR1GIF) {
        TMR1GIF = 0;
        unsigned char ch = CPSCON1;
        unsigned char low = TMR1L;
        unsigned char hi = TMR1H;
        unsigned int raw = low + (unsigned int) (hi << 8);
        // switch channel
        switch (ch) {
            case 0:
                CPSCON1 = 2;
                break;
            case 2:
                CPSCON1 = 4;
                break;
            case 4:
                CPSCON1 = 0;
                break;
            default:
                printf("Should not happen\n");
                CPSCON1 = 0;
                break;
        }
        TMR1L = 0x00;
        TMR1H = 0x00;
        TMR1ON = 1; // Restart Timer1
        TMR0 = 0;        
        TMR0IF = 0;
        process_cps(ch, raw);        
    }
    if (TMR2IF) {
        TMR2IF = 0;
        if(beep > 0) {
            beep--;
            PORTAbits.RA3 = ticks & 0b1;
        }
        ticks++;
    }
    if (SSPIF) {
        SSPIF = 0;
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
                    }
                }
                if (rx_index == 2) {
                    // input
                    switch (command) {
                        case 0x01:
                            state = rx_buffer[1];
                            break;
                    }
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // output
                switch (command) {
                    case 0x01: // read command
                        rx_buffer[0] = state;// buttons pressed
                        rx_buffer[1] = 0;
                        break;
                    case 0x02: // read command
                        rx_buffer[0] = 2 & 0xff; // raw[2]
                        rx_buffer[1] = 1 >> 8; // raw[2]
                        break;
                    default:
                        rx_buffer[0] = rx_buffer[1] = 0;
                }
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
    }
}



