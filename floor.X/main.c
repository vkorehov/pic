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
unsigned char floor_on;
unsigned int floor_timer;
unsigned int floor_timeout;
unsigned int floor_recovery_timeout;
unsigned char timeout_count;
unsigned char enabled;
unsigned int reading;
unsigned int state;
unsigned int average;

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

inline void on(void) {
    PORTBbits.RB2 = 1;
    state = 1;
}

inline void off(void){
    PORTBbits.RB2 = 0;   
    state = 0;
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
static unsigned char ticker = 0;
unsigned int sensor1 = 0;

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    floor_on = 0;
    floor_recovery_timeout = 0;        
    floor_timeout = 0;    
    floor_timer = 0;        
    ticker = 0;
    sensor1 = 0;
    state = 0;
    timeout_count = 0;
    enabled = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    PORTAbits.RA1 = 1;
    PORTAbits.RA2 = 0;
#ifdef DEBUG
    printf("Started App: OSTS:%d \n", OSCSTATbits.OSTS);
#endif    
    sensor1 = INIT_VECTOR; // initialize to MAX ADC       
    
    
    while(1)
    {
        if (enabled == 0) {
            off();
            floor_timer = 0;            
            floor_timeout = 0;
            floor_recovery_timeout = 0;            
            continue;
        }
        if (sensor1 > AD_CUTOFF) {            
            off();
            floor_timer = 0;            
            floor_timeout = 0;
            floor_recovery_timeout = 0;            
        } else if (sensor1 < (AD_CUTOFF_RESET)) {
            if(floor_timer == 0) {
                floor_timer = 1;
                floor_timeout = FL_TIMEOUT;
                floor_recovery_timeout = FL_RECOVERY_TIMEOUT;
                on();
            }
        }
        if (floor_timer == 1 && floor_timeout == 0) {
            off();
        }
        if (floor_timer == 1 && floor_recovery_timeout == 0) {
            // reset timer
            floor_timer = 0;            
            floor_timeout = 0;
            floor_recovery_timeout = 0;
            timeout_count++;
        }
        
        //if(sensor1 > AD_CUTOFF || sensor2 > AD_CUTOFF) {
#ifdef DEBUG        
        printf("S1:%d\n", sensor1);            
#endif        
        //}
    }
}

