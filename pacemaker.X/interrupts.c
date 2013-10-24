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
#include "user.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* Baseline devices don't have interrupts. Unfortunately the baseline detection 
 * macro is named _PIC12 */
unsigned char i2c_ack;
unsigned char i2c_error;
unsigned short i2c_collisions;
unsigned int sync_every; // 1 second/10mS (50Hz half-period)


void interrupt isr(void) {
    if(TMR1IF && TMR1IE) {
       TMR1IF = 0;
       TMR1ON = 0; // turn off timer1
       write_tmr1(0);
       if(pacemaker_state == 2) {
           if(sync_every == 0 ) {
                sync_every = 100;
                i2c_command0();
           } else {
              sync_every--;
           }
       }
    }
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



