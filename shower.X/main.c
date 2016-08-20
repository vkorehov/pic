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
unsigned char notify_state = 0;

/* i.e. uint8_t <variable_name>; */

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
    notify_state = 0;
    sensor_average_every = 0;
    for(int i = 0; i < 2; i++) {
        sensor_values[i] = 0;
        sensor_values_averages[i] = 0;
    }
    for(unsigned char i = 0; i < 4; i++) {
        switch_timeouts[i] = 0xffffffff;        
    }
    for(unsigned char i = 0; i < 4; i++) {
        pulses[i] = 0;
        pulse_rates[i] = 0;        
    }
    ticks = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    pwm_en();
    //pwm_duty(0x00, 0x0, 0x00, 0x0);    
    while(1)
    {
        for(unsigned char i = 0; i < 4; i++) {
            if(switch_timeouts[i] > SWITCH_TIMEOUT) {
                off(i);
            }
            else {
                switch_timeouts[i]++;
            }
        }
    }

}

