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

#define PEAK_TO_ZC1 (0xffff - 4500)
#define ZC1_TO_ZC2 (0xffff - 40500)


volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */
#ifdef _12F1840
#define SSPIF SSP1IF
#endif


#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
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
    if(TMR0IF) {
        TMR0IF = 0;
        TMR0IE = 0;
        PORTAbits.RA0 = 0;
    }
    if(TMR1IF) {
        TMR1IF = 0;
        TMR1ON = 0;
        if(state == 0) {
            state = 1;
            // Zero Crossing1
            if(dim > 0x01) {
                PORTAbits.RA0 = 1;
            } else {
                PORTAbits.RA0 = 0;
            }
            if(dim != 0xff) {
                TMR0 = 0xff - dim; // Start timer0 in order to turn off output pin!
                TMR0IF = 0;
                TMR0IE = 1;
            } else {
                TMR0IE = 0;
            }
            // Endof zero crossing
            write_tmr1(ZC1_TO_ZC2);
            TMR1ON = 1;
        } else if(state == 1) {
            // Zero Crossing2
            if(dim > 0x01) {
                PORTAbits.RA0 = 1;
            } else {
                PORTAbits.RA0 = 0;
            }
            if(dim != 0xff) {
                TMR0 = 0xff - dim; // Start timer0 in order to turn off output pin!
                TMR0IF = 0;
                TMR0IE = 1;
            } else {
                TMR0IE = 0;
            }
            // Endof zero crossing
        }
    }
    if (C1IF) {
        C1IF = 0;
        TMR1ON = 0;
        write_tmr1(PEAK_TO_ZC1);
        state = 0;
        TMR1ON = 1;
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
                // do a dummy read
                rx_buffer[0] = SSPBUF;
                for (int i = 0; i < RX_SIZE; i++) {
                    rx_buffer[i] = 0;
                }
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                rx_buffer[rx_index++] = SSPBUF;
                if (rx_index == 1 && rx_buffer[0] == 0x78) {
                    ENTER_BOOTLOADER = 1;
                    asm("pagesel 0x000");
                    asm("goto 0x000");
                }
                if (rx_index == 2 && rx_buffer[0] == 0x01) {
                    dim = rx_buffer[1];
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // Diagnostics output
                rx_buffer[0] = switch_count & 0xff;
                rx_buffer[1] = switch_count >> 8;
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }

}


