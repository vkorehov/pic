#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "system.h"
#include "user.h"

// MCP4011 digipot control
void delay(unsigned int cycles) {    
    for(unsigned int i = 0; i < cycles; i++) {
        asm("nop");
    }
}

void dpot_increment(unsigned char count) {
    // 0b<nCS><U/D><0>
    // Setup
    PORTAbits.RA0 = 1;
    PORTAbits.RA2 = 1;
    delay(4);
    PORTAbits.RA2 = 0;   
    delay(4);
    PORTAbits.RA0 = 0;
    delay(4);
    while(count--) {
        PORTAbits.RA0 = 1;
        delay(1);
        PORTAbits.RA0 = 0;
        delay(1);
    }
    PORTAbits.RA0 = 1;
    delay(4);
    PORTAbits.RA2 = 1;           
    delay(4);
}

void dpot_decrement(unsigned char count) {
    // 0b<nCS><U/D><0>
    PORTAbits.RA0 = 1;
    PORTAbits.RA2 = 1;     
    delay(4);    
    // Setup
    PORTAbits.RA0 = 0;
    delay(4);
    PORTAbits.RA2 = 0;     
    delay(8);
    while(count--) {
        PORTAbits.RA0 = 1;
        delay(1);
        PORTAbits.RA0 = 0;
        delay(1);
    }
    PORTAbits.RA0 = 1;
    delay(4);
    PORTAbits.RA2 = 1; 
    delay(4);
}