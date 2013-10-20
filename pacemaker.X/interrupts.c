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
unsigned char i2c_ack;
unsigned char i2c_error;
unsigned short i2c_collisions;

void interrupt isr(void) {
    
    if (BCL1IE && BCL1IF) {
        BCL1IF = 0;
        i2c_error = 1;
        i2c_collisions++;
    }
    if (SSP1IE && SSP1IF) {
        SSP1IF = 0;
        if (!ACKSTAT) {
            i2c_ack = 1;
        }
    }

}



