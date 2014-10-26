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
static void write_i2c(unsigned char b) {
    do {
        SSPCONbits.WCOL = 0b0;
        SSPBUF = b;
    } while (SSPCONbits.WCOL);
}

void interrupt isr(void) {
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
                if (rx_index == 1 && rx_buffer[0] == 0x02) {
                    allow();
                }
                if (rx_index == 1 && rx_buffer[0] == 0x03) {
                    release();
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // Diagnostics output
                rx_buffer[0] = state & 0xff;
                rx_buffer[1] = 0 >> 8;
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }
    if(IOCIF) {
        tick_count++;
        leak();
        IOCBF = 0x0;
    }
    if(TMR1IF) {
        TMR1IF = 0;
        if(timeout == 0 || timeout > ALLOW_TIMEOUT) {
            reset();
        } else {
            timeout--;
        }
    }
}
