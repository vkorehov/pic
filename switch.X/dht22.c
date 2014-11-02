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
#include "system.h"
#include "user.h"

unsigned char dht22_state; // 0 = idle
                           // 1 = start fired
                           // 2 = start ended
                           // 3 = dht skip bit1
                           // 4 = dht skip bit2

unsigned char dht22_index;
unsigned char dht22_bit_index;
unsigned char dht22_bits[DHT22_MAX_BYTES];

void start_read_dht22() {
     IOCBPbits.IOCBP0 = 0;
     IOCBNbits.IOCBN0 = 0;

     dht22_index = 0;
     dht22_bit_index = 0;
  
     for(int i = 0; i < DHT22_MAX_BYTES; i++) {
         dht22_bits[i] = 0;
     }
     
     dht22_state = 1;
     TRISBbits.TRISB0 = 0;
     PORTBbits.RB0 = 0;

     TMR2 = 0x00;
     PR2 = 0xFF;
     T2CONbits.T2CKPS = 0b10; // 1:16 >500uS
     T2CONbits.T2OUTPS = 5;//
     TMR2IF = 0;
     TMR2IE = 1;
     TMR2ON = 1;
}

void start_read_dht22_pullup() {
     dht22_state = 2;
     TMR2ON = 0;

     PORTBbits.RB0 = 1;
     asm("nop");
     asm("nop");
     TRISBbits.TRISB0 = 1;
     asm("nop");
     asm("nop");

     IOCBPbits.IOCBP0 = 1;
     IOCBNbits.IOCBN0 = 1; // we are waiting for P => N transition from DHT22


     TMR2IE = 0;
     TMR2IF = 0;
     // adjust timer for timeout/counting
     TMR2 = 0x00;
     PR2 = 0xFF;
     T2CONbits.T2CKPS = 0b10; // 1:16
     T2CONbits.T2OUTPS = 5;// 1:6 post scaler

     TMR2IE = 1;
     TMR2ON = 1; // enable timeout

}


void dht22_abort() {
     dht22_state = 0;
     dht22_index = 0;
     dht22_bit_index = 0;
     TMR2ON = 0;
     TMR2IF = 0;
     TMR2IE = 0;
     IOCBNbits.IOCBN0 = 0;
     IOCBPbits.IOCBP0 = 0;
     IOCBF = 0;
     TRISBbits.TRISB0 = 1;
}