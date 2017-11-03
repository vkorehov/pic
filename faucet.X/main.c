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
static unsigned int water = 0;
static unsigned int water_pause = 0;

static unsigned char state = 0;
void hot_on() {
    if(faucet_on == 0) {
        faucet_on = 1;
        faucet_timeout = FAUCET_TIMEOUT;
    }    
    PORTBbits.RB1 = 1;    
}
void cold_on() {
    if(faucet_on == 0) {
        faucet_on = 1;
        faucet_timeout = FAUCET_TIMEOUT;
    }
    PORTBbits.RB0 = 1;    
}
void off() {
    PORTBbits.RB1 = 0;    
    PORTBbits.RB0 = 0;        
}

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


unsigned int ticker = 0;
unsigned int sensor1 = 0;
unsigned int sensor2 = 0;
unsigned int sensor3 = 0; // temperature sensor

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    faucet_on = 0;
    faucet_timeout = 0;    
    ticker = 0;
    sensor1 = 0;
    sensor2 = 0;
    sensor3 = 0;
    water = 0;
    water_pause = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
#ifdef DEBUG
    printf("Started App: OSTS:%d \n", OSCSTATbits.OSTS);
#endif    

    while(1)
    {
#ifdef DEBUG
        printf("S1:%d S2:%d S3:%d\n", sensor1, sensor2, sensor3);            
#endif         
        if(water_pause != 0) {
            water_pause--;
        }
        if(sensor1 > (AD_WATER_CUTOFF1 - 20) && sensor1 < (AD_WATER_CUTOFF1 + 20) &&
                sensor2 > (AD_WATER_CUTOFF2 - 20) && sensor2 < (AD_WATER_CUTOFF2 + 20)){
            water++;
        } else {
            water = 0;
        }
        if((sensor1 > AD_CUTOFF1 || sensor2 > AD_CUTOFF2) && water_pause == 0) {
            if (faucet_on == 1 && faucet_timeout == 0 ) {
                // timeout occured force off
                off();
            } else {
                hot_on();
                if(sensor3 > AD_30DEG) {
                    cold_on();
                }
            }
        } else if((sensor1 < (AD_CUTOFF1 - 50) && sensor2 < (AD_CUTOFF2 - 50)) || water > 1000) {
            if(water > 1000) {
                water_pause = 5000;
            }
            off();
            faucet_on = 0;
            faucet_timeout = 0;                
        }
    }
}
