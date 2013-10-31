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

#define MAX_ADC_VALUE 0b100110010 // 0.3 * MAX(=0b1111111111) //

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

/* i.e. uint8_t <variable_name>; */
unsigned char running;
unsigned char state; // 0 = Peak Detected
                    // 1 = Zero Crossing1
                    // 2 = Zero Crossing2
unsigned char dim; // Dim level 0xff fully on 0x00 fully off
unsigned char stop;

inline unsigned int read_tmr1() {
  return ( (TMR1H) << 8 | (TMR1L));
}
inline void write_tmr1(unsigned int v) {
    TMR1L = ((v)&0xff);
    TMR1H = ((v) >> 8);
}
unsigned int adc_freq = 0;
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    state = 0;
    stop = 0;
    dim = 0x0;
    running = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    while(1)
    {
        do_i2c_tasks();
        if(adc_freq++ == 0) {
            GO = 1; // start ADC
        }
    }

}

