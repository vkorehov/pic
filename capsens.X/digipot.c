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


void dpot_increment(unsigned char count) {
    // 0b<nCS><U/D><0>
    // Setup
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1;
    __delay_us(3);
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;   
    __delay_us(3);
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;       
    __delay_us(3);
    while(count--) {
        PORTAbits.RA0 = 1;
        PORTAbits.RA1 = 0;
        PORTAbits.RA2 = 0;           
        __delay_us(1);
        PORTAbits.RA0 = 0;
        PORTAbits.RA1 = 0;
        PORTAbits.RA2 = 0;           
        __delay_us(1);
    }
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;           
    __delay_us(3);
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1;           
    __delay_us(3);
}

void dpot_decrement(unsigned char count) {
    // 0b<nCS><U/D><0>
    // Setup
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1;     
    __delay_us(3);
    PORTAbits.RA0 = 0;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;     
    __delay_us(6);
    while(count--) {
        PORTAbits.RA0 = 1;
        PORTAbits.RA1 = 0;
        PORTAbits.RA2 = 0;         
        __delay_us(1);
        PORTAbits.RA0 = 0;
        PORTAbits.RA1 = 0;
        PORTAbits.RA2 = 0; 
        __delay_us(1);
    }
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 0;     
    __delay_us(3);
    PORTAbits.RA0 = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA2 = 1; 
    __delay_us(3);
}