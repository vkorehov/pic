/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    sensor_average_every = 0;
    for(int i = 0; i < 2; i++) {
        sensor_values[i] = 0;
        sensor_values_averages[i] = 0;
    }
    pwm_timeout = 0xffffffff;
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
    while(1)
    {
        if(pwm_timeout > PWM_TIMEOUT) {
            pwm_duty(0, 0, 0, 0);
        } else {
            pwm_timeout++;
        }
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

