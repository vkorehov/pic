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

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Unfortunately the baseline detection 
 * macro is named _PIC12 */
unsigned int raw[8];
unsigned int average[8];
unsigned int trip[8];
unsigned char state[8]; // 0: init // >0: Counted // 1: Unpressed // 2 : Pressed
unsigned char button_state[8];

unsigned char i2c_ack;
unsigned char i2c_error;
unsigned int i2c_collisions;
unsigned char avgIndex = 0;
unsigned char ticks;
unsigned short beep;

void interrupt isr(void) {
    if (TMR2IF) {
        TMR2IF = 0;
        if(beep) {
           PORTAbits.RA3 = ticks & 0b1;
           beep--;
        } else {
           PORTAbits.RA3 = 0;
        }
        ticks++;
    }
    
    if (TMR1GIF) {
        TMR1GIF = 0;
        unsigned char ch = CPSCON1;
        TMR1ON = 0; // Stop Timer1
        //
        raw[ch] = TMR1L + (unsigned int) (TMR1H << 8);
        //
        TMR1L = 0x00;
        TMR1H = 0x00;
        if (++CPSCON1 > 2) {
            CPSCON1 = 0;
        }
        TMR1ON = 1; // Restart Timer1


        if (state[ch] == 0) { // skip first iteration, no reading yet
            state[ch] = 1;
        } else {
            if (average[ch] == 0) {
                average[ch] = raw[ch] - trip[ch];
            }

            if (raw[ch] < (average[ch] - trip[ch])) {
                state[ch] = 2;
                // Turned on
                if (ch == 6) {
                    PORTCbits.RC5 = 1;
                }
                if (ch == 7) {
                    PORTCbits.RC6 = 1;
                }
                if (ch == 0) {
                    PORTCbits.RC7 = 1;
                }
                if(button_state[ch] == 0) {
                    beep = 1000;
                }
                button_state[ch] = 1;
            } else if (raw[ch] > (average[ch] - trip[ch] + 16)) {
                state[ch] = 1;
                // Turned off
                if (ch == 6) {
                    PORTCbits.RC5 = 0;
                }
                if (ch == 7) {
                    PORTCbits.RC6 = 0;
                }
                if (ch == 0) {
                    PORTCbits.RC7 = 0;
                }
                button_state[ch] = 0;                
                //
            }
            if (state[ch] == 1) { // Average only during idle
                average[ch] = average[ch] + (((long) raw[ch]-(long) average[ch]) >> 4);
            }
            // switch channel
            switch(ch) {
                case 0:
                    CPSCON1 = 6;
                    break;
                case 6:
                    CPSCON1 = 7;
                    break;
                case 7:
                    CPSCON1 = 0;
                    break;
                default:
                    CPSCON1 = 0;
                    break;                    
            }
        }
    }
    if (BCLIF) {
        BCLIF = 0;
        i2c_error = 1;
        i2c_collisions++;
    }
    if (SSPIF) {
        SSPIF = 0;
        if (!ACKSTAT) {
            i2c_ack = 1;
        }
    }
}



