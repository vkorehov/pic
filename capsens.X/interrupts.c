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
unsigned short cps0_value;
unsigned short cps1_value;
unsigned short cps2_value;
unsigned short cps3_value;
unsigned char i2c_ack;
unsigned char i2c_error;
unsigned short i2c_collisions;

void interrupt isr(void) {
    if (TMR1GIE && TMR1GIF) {
        TMR1ON = 0; // Stop Timer1
        TMR1GIF = 0;
        if (CPSCON1 == 3) {
            cps3_value = (TMR1H << 8) + TMR1L;
            // Switch to CPS0
            CPSCON1 = 0;
        } else if (CPSCON1 == 2) {
            cps2_value = (TMR1H << 8) + TMR1L;
        } else if (CPSCON1 == 1) {
            cps1_value = (TMR1H << 8) + TMR1L;
        } else if (CPSCON1 == 0) {
            cps0_value = (TMR1H << 8) + TMR1L;
            // Switch to CPS3
            CPSCON1 = 3;
        }
        TMR1L = 0x00; // Reset Timer1
        TMR1H = 0x00;
        TMR1ON = 1; // Restart Timer1
    }
    if (BCLIE && BCLIF) {
        BCLIF = 0;
        i2c_error = 1;
        i2c_collisions++;
    }
    if (SSPIE && SSPIF) {
        SSPIF = 0;
        if (!ACKSTAT) {
            i2c_ack = 1;
        }
    }

}



