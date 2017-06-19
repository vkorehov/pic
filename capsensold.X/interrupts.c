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
#include "user.h"        /* For true/false definition */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/
volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

/* Baseline devices don't have interrupts. Unfortunately the baseline detection 
 * macro is named _PIC12 */
int raw[8];
int average[8];
unsigned char button_trigger_on[8];

unsigned char i2c_ack;
unsigned char i2c_error;
unsigned int i2c_collisions;
unsigned char avgIndex = 0;
unsigned char ticks;
unsigned short beep;
unsigned char command_to_send = 0;
unsigned char debug_byte0 = 0;
unsigned char debug_byte1 = 0;
unsigned char debug_byte2 = 0;
unsigned char debug_byte3 = 0;
unsigned char debug = 0;
unsigned int tmr2_ticks;

unsigned char average_every[8];
unsigned int percent[8];
unsigned char skip_cps;
unsigned char sequential_press[8];
unsigned char latching;
unsigned char percents_on[8];
unsigned char percents_off[8];
unsigned char bright_on;
unsigned char bright_off;

#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char command;
static unsigned char counter;
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

inline void process_cps(unsigned char ch) {
        if (skip_cps < 256 - 16 && skip_cps > 0) { // skip first 16 measurments
            // initial average
            if (average[ch] == 0) {
                average[ch] = raw[ch];
            } else {
                average[ch] = average[ch] + (raw[ch] - average[ch]) / 16;
            }
            skip_cps--;
        } else if (skip_cps == 0) {
            if (raw[ch] >= average[ch] || average[ch] == 0) {
                percent[ch] = 0;
            } else {
                percent[ch] = (average[ch] - raw[ch]);
                for (int a = 0; a < 7; a++) {
                    unsigned int next = percent[ch] << 1;
                    if (next > percent[ch]) { // no overflow
                        percent[ch] = next;
                    } else {
                        percent[ch] = 65000; // max out
                        break;
                    }
                }
                percent[ch] = percent[ch] / (unsigned int) average[ch];
            }
            if (percent[ch] < percents_off[ch]) {
                sequential_press[ch] = 0; // reset sequential press
                //if (--average_every[ch] == 0) {
                //average_every[ch] = 2;
                // Average only during idle
                average[ch] = average[ch] + (raw[ch] - average[ch]) / 16;
                //}
                button_trigger_on[ch] = 0;
            } else if (percent[ch] > percents_on[ch]) {
                if (button_trigger_on[ch] == 0 && sequential_press[ch] > 1) {
                    beep = 1000;
                    // Turned on
                    if (ch == 4) {
                        PORTC = (PORTC | (LED4_BIT)) & ~(LED2_BIT | LED0_BIT);
#ifdef LATCH_CH_4
                        latching = !latching;
                        if (latching) {
                            command_to_send = 0x03;
                        } else {
                            command_to_send = 0x30;
                        }
#else
                        command_to_send = 0x03;
#endif                                                
                    }
                    if (ch == 2) {
                        PORTC = (PORTC | (LED2_BIT)) & ~(LED4_BIT | LED0_BIT);
#ifdef LATCH_CH_2
                        latching = !latching;
                        if (latching) {
                            command_to_send = 0x02;
                        } else {
                            command_to_send = 0x20;
                        }
#else
                        command_to_send = 0x02;
#endif                        
                    }
                    if (ch == 0) {
                        PORTC = (PORTC | (LED0_BIT)) & ~(LED4_BIT | LED2_BIT);
#ifdef LATCH_CH_0
                        latching = !latching;
                        if (latching) {
                            command_to_send = 0x01;
                        } else {
                            command_to_send = 0x10;
                        }
#else
                        command_to_send = 0x01;
#endif                        
                    }
                    button_trigger_on[ch] = 1;
                } else if (button_trigger_on[ch] == 0) {
                    sequential_press[ch]++;
                }
            }
        } else {
            skip_cps--;
        }    
}

void interrupt isr(void) {
    if (TMR2IF) {
        TMR2IF = 0;        
        if (beep > 0) {
            if (beep == 1000) {
                    bright_on = 1;                
            }
            PORTAbits.RA3 = ticks & 0b1;
            beep--;
            if (beep <= 0) {
                if (!latching) {
                    bright_off = 1;
                }
                PORTC = PORTC | (LED4_BIT | LED2_BIT | LED0_BIT);
            }
        } else {
            PORTAbits.RA3 = 0;
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
                            counter++;
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
                        rx_buffer[0] = command_to_send;
                        rx_buffer[1] = 0;                        
                        break;
                    case 0x02: // read command
                        rx_buffer[0] = raw[2] & 0xff;
                        rx_buffer[1] = raw[2] >> 8;
                        break;                                                
                    default:
                        rx_buffer[0] = rx_buffer[1] = 0;
                }
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
    }
    if (TMR1GIF) { // it is important to have it last handled
        TMR1GIF = 0;        
        unsigned char ch = CPSCON1;
        unsigned char low = TMR1L;
        unsigned char hi = TMR1H;

        raw[ch] = low + (unsigned int) (hi << 8);
        process_cps(ch);
#ifdef TRI_BUTTONS        
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
                CPSCON1 = 0;
                break;
        }
#endif   
#ifdef CH0_CH2_BUTTONS        
        // switch channel
        switch (ch) {
            case 0:
                CPSCON1 = 2;
                break;
            case 2:
                CPSCON1 = 4;
                break;
            default:
                CPSCON1 = 0;
                break;
        }        
#endif        
#ifdef CH2_CH4_BUTTONS        
        // switch channel
        switch (ch) {
            case 2:
                CPSCON1 = 4;
                break;
            case 4:
                CPSCON1 = 2;
                break;
            default:
                CPSCON1 = 2;
                break;
        }
#endif
        TMR1L = 0x00;
        TMR1H = 0x00;
        TMR1ON = 1; // Restart Timer1
        TMR0 = 0;
        TMR0IF = 0;
    }    
}



