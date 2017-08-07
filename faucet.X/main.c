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
unsigned char faucet_on;
unsigned int faucet_timeout;


#ifdef DEBUG
void putch(unsigned char data) {
    while (!PIR1bits.TXIF) // wait until the transmitter is ready
        continue;
    TXREG = data; // send one character
}
#endif


/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
inline void write_tmr1(unsigned int val) {
    unsigned int v = 0xffff - val;
    TMR1L = ((v)&0xff);
    TMR1H = ((v) >> 8);
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
unsigned char switch_dur_mult;


static unsigned char ticker = 0;
unsigned int sensor1 = 0;
unsigned int sensor2 = 0;

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    faucet_on = 0;
    faucet_timeout = 0;    
    ticker = 0;
    sensor1 = 0;
    sensor2 = 0;
    
    /* Initialize I/O and Peripherals for application */
    InitApp();
#ifdef DEBUG
    printf("Started App: OSTS:%d \n", OSCSTATbits.OSTS);
#endif    

    while(1)
    {
        //if(sensor1 > AD_CUTOFF || sensor2 > AD_CUTOFF) {
        printf("S1:%d S2:%d\n", sensor1, sensor2);            
        //}
    }
}

