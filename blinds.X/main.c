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
inline void write_tmr1(unsigned int val) {
    TMR1L = ((val)&0xff);
    TMR1H = ((val) >> 8);
}
inline unsigned int read_tmr1() {
    return ((unsigned int)TMR1H << 8) | (unsigned int)TMR1L;
}

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
inline void ccv(void) {
        RA0 = 0;
        RA4 = 1;    
}
inline void cv(void) {
        RA0 = 1;
        RA4 = 0;        
}
inline void stop(void) {
        RA0 = 0;
        RA4 = 0;        
}

static unsigned char ticker = 0;

void main(void)
{
    unsigned char last_dir = 0;
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    ticker = 0;
    
    /* Initialize I/O and Peripherals for application */
    InitApp();
    //next_position = position - 20;
    while(1)
    {
        //ccv();
        unsigned int tmr1 = read_tmr1();
        if (next_position > position && position + tmr1 > next_position) {
            stop();
            position = position + tmr1;
            next_position = position;
            TMR1H = 0;
            TMR1L = 0;
            eeprom_write(0x00, position & 0xFF);
            eeprom_write(0x01, (position >> 8) & 0xFF);    
            last_dir = 0;
        } else if (next_position < position && (tmr1 > position || position - tmr1 < next_position)) {
            stop();
            position = position - tmr1;
            next_position = position;
            TMR1H = 0;
            TMR1L = 0;            
            eeprom_write(0x00, position & 0xFF);
            eeprom_write(0x01, (position >> 8) & 0xFF);             
            last_dir = 1;
        } else if(next_position > position){
            ccv();
        } else if(next_position < position){
            cv();
        } else if (tmr1 > 0) {
            // inertia
            if (last_dir == 0) {
                position = position + tmr1;
                next_position = position;
                TMR1H = 0;
                TMR1L = 0;
                eeprom_write(0x00, position & 0xFF);
                eeprom_write(0x01, (position >> 8) & 0xFF);                    
            } else {
                position = position - tmr1;
                next_position = position;
                TMR1H = 0;
                TMR1L = 0;            
                eeprom_write(0x00, position & 0xFF);
                eeprom_write(0x01, (position >> 8) & 0xFF);                             
            }
        } else if(tmr1 == 0) {
            // idle
            // update next_position here only
            if(command_position != 0) {
                next_position = command_position;
                command_position = 0;
            }
        }
    }
}

