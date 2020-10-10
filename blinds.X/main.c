/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */


unsigned int position = 0;
unsigned int next_position = 0;
unsigned int command_position = 0;

unsigned char state = 0;

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
inline void ccv(void) {
        RA0 = 0;
        RA4 = 1;    
        TRISAbits.TRISA0 = 0;
        TRISAbits.TRISA4 = 0;        
}
inline void cv(void) {
        RA0 = 1;
        RA4 = 0;
        TRISAbits.TRISA0 = 0;
        TRISAbits.TRISA4 = 0;                
}
inline void stop(void) {
        RA0 = 0;
        RA4 = 0;        
        TRISAbits.TRISA0 = 1;
        TRISAbits.TRISA4 = 1;                
}

static unsigned char ticker = 0;
static unsigned int original_position = 0;

void main(void)
{
    unsigned char last_dir = 0;
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    ticker = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while(1)
    {
        if (command_position > 0) {
            next_position = command_position;
            write_ticks(0);
            original_position = position;
            if (next_position > position) {
                state = STATE_CCV;
            } else if (next_position < position) {
                state = STATE_CV;
            } else {
                state = STATE_IDLE;               
            }
            command_position = 0;
        }
        if (state == STATE_CCV) {
            ccv();
            position = original_position + (read_ticks());
            if (next_position <= position) {
                stop();
                next_position = position; // TODO: take into account inertia
                state = STATE_IDLE;
            }
        }
        if (state == STATE_CV) {
            cv();
            position = original_position - (read_ticks());
            if (next_position >= position) {
                stop();
                next_position = position; // TODO: take into account inertia
                state = STATE_IDLE;
            }
        }        
    }
}

