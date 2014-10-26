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

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

unsigned char state;
unsigned char timeout;

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
inline unsigned int read_tmr1() {
  return ( (TMR1H) << 8 | (TMR1L));
}
inline void write_tmr1(unsigned int v) {
    TMR1L = ((v)&0xff);
    TMR1H = ((v) >> 8);
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

inline void allow(void) {
    IOCBNbits.IOCBN0 = 0;
    IOCIE = 0; // disable interrupt on change
    IOCBF = 0;
    // Stop timer
    TMR1ON = 0;
    TMR1IE = 0;
    write_tmr1(0x0);// set timeout
    timeout = ALLOW_TIMEOUT;
    // enable timer 1
    TMR1IF = 0;
    TMR1ON = 1;
    TMR1IE = 1;
    state = 2; // allowed
}

inline void leak(void) {
    PORTAbits.RA0 = 1;
    state = 1;// leak
}

inline void release(void) {
    PORTAbits.RA0 = 0;
    tick_count = 0;
    state = 0; // no leak
}

inline void reset(void) { // must be called from interrupt only
     // Stop timer
     TMR1ON = 0;
     TMR1IE = 0;


     IOCBF = 0x0; // reset flags
     IOCIE = 1; // enable interrupt on change
     IOCBNbits.IOCBN0 = 1;
     
     state = 0; // no leak
}

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    /* Initialize I/O and Peripherals for application */
    InitApp();
    tick_count = 0;
    state = 0; // no leak initially
    timeout = 0;
    while(1)
    {
    }
}

