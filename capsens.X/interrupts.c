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
unsigned int raw[6];
unsigned int average[6];
unsigned int trip[6];
unsigned char state[6]; // 0: init // >0: Counted // 1: Unpressed // 2 : Pressed

unsigned char i2c_ack;
unsigned char i2c_error;
unsigned int i2c_collisions;
unsigned char avgIndex = 0;

void interrupt isr(void) {
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
                if (ch == 0) {
                    //PORTA = 0b0000111;
                }
            } else if (raw[ch] > (average[ch] - trip[ch] + 4)) {
                state[ch] = 1;
                // Turned off
                if (ch == 0) {
                    //PORTA = 0b00000101;
                }
                //
            }
            if (state[ch] == 1) { // Average only during idle
                average[ch] = average[ch] + (((long) raw[ch]-(long) average[ch]) >> 4);
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



