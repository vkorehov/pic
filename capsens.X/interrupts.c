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

/* Baseline devices don't have interrupts. Unfortunately the baseline detection 
 * macro is named _PIC12 */
int raw;
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
#define PCT_ON 7
#define PCT_OFF 5 // 2% hyst
unsigned int percent;
unsigned char skip_cps;
unsigned char sequential_press[8];

void interrupt isr(void) {
    if (TMR1GIF) {
        unsigned char ch = CPSCON1;
        unsigned char low = TMR1L;
        unsigned char hi = TMR1H;

        raw = low + (unsigned int) (hi << 8);

        if (skip_cps < 256 - 16 && skip_cps > 0) { // skip first 16 measurments
            // initial average
            if (average[ch] == 0) {
                average[ch] = raw;
            } else {
                average[ch] = average[ch] + (raw - average[ch]) / 16;
            }
            skip_cps--;
        } else if (skip_cps == 0) {
            if (raw >= average[ch] || average[ch] == 0) {
                percent = 0;
            } else {
                percent = (average[ch] - raw);
                for(int a=0; a<7;a++) {
                    unsigned int next = percent << 1;
                    if (next > percent) { // no overflow
                        percent = next;
                    } else {
                        percent = 65000; // max out
                        break;
                    }
                }
                percent = percent / (unsigned int) average[ch];
            }
            if (--tmr2_ticks == 0) {
                tmr2_ticks = 50;
                debug_byte0 = 0;
                debug_byte1 = percent & 0xFF;
                debug_byte2 = average[0] >> 8;
                debug_byte3 = average[0] & 0xff;
                debug = 1;
            }

            if (percent < PCT_OFF) {
                sequential_press[ch] = 0; // reset sequential press
                //if (--average_every[ch] == 0) {
                    //average_every[ch] = 2;
                    // Average only during idle
                average[ch] = average[ch] + (raw - average[ch]) / 16;
                //}
                button_trigger_on[ch] = 0;
            } else if (percent > PCT_ON) {
                if (button_trigger_on[ch] == 0 && sequential_press[ch] > 1) {
                    beep = 1000;
                    // Turned on
                    if (ch == 4) {
                        PORTC = (PORTC | (LED4_BIT)) & ~(LED2_BIT | LED0_BIT);
                        //command_to_send = 0x03;
                    }
                    if (ch == 2) {
                        PORTC = (PORTC | (LED2_BIT)) & ~(LED4_BIT | LED0_BIT);
                        //command_to_send = 0x02;
                    }
                    if (ch == 0) {
                        PORTC = (PORTC | (LED0_BIT)) & ~(LED4_BIT | LED2_BIT);
                        //command_to_send = 0x01;      
                    }
                    button_trigger_on[ch] = 1;
                } else if(button_trigger_on[ch] == 0) {
                    sequential_press[ch]++;                               
                }
            }
        } else {
            skip_cps--;
        }
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

        TMR1L = 0x00;
        TMR1H = 0x00;
        TMR1ON = 1; // Restart Timer1
        TMR0 = 0;
        TMR0IF = 0;
        TMR1GIF = 0;
    }
    if (TMR2IF) {
        if (beep > 0) {
            if (beep == 1000) {
                dpot_increment(100); // this resets pot to minimum voltage!
                dpot_decrement(40);
            }
            PORTAbits.RA3 = ticks & 0b1;
            beep--;
            if (beep <= 0) {
                dpot_increment(100); // this resets pot to minimum voltage!
                dpot_decrement(20);
                PORTC = PORTC | (LED4_BIT | LED2_BIT | LED0_BIT);
            }
        } else {
            PORTAbits.RA3 = 0;
        }
        ticks++;
        TMR2IF = 0;
    }
    if (BCLIF) {
        i2c_error = 1;
        i2c_collisions++;
        BCLIF = 0;
    }
    if (SSPIF) {
        if (!ACKSTAT) {
            i2c_ack = 1;
        }
        SSPIF = 0;
    }
}



