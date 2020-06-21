/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/


#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#ifdef UNITTEST
#include <stdio.h>
#endif
/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
unsigned int target = 750;
int integral = 0;
int last_error = 0;
int derivative = 0;
unsigned char last_ctrl = 0;

unsigned char kp = 32;
unsigned char kd = 0;
unsigned char ki = 0;
unsigned char pid_enabled = 0;

void pid(void) {
    long ctrl;
    int error = target - sensor_values[1];

    integral = integral + error;
    derivative = error - last_error;

    ctrl = ((long) (kp * error) + (long) (ki * integral) + (long) (kd * derivative)) >> 9;

    // limiting
    if (ctrl > 0x09) {
        ctrl = 0x09;
    } else if (ctrl < -0x09) {
        ctrl = -0x09;
    }

    last_error = error;
    last_ctrl = (0x5f - ctrl);

    // perform action
    pwm_duty(0x00, (unsigned char) (0x5f - ctrl), 0x00, 0x60);
}

#ifdef UNITTEST

void Serinit(void) {
    TRISC = 0xFF;
    RCSTA = 0xB0;
    TXSTA = 0x24;
    SPBRG = 0x12;
}

void putch(char data) {
    while (!TXIF);
    TXREG = data;
}

void main(void) {
    Serinit();
    target = 750;
    
    sensor_values[1] = 600;
    pid();    
    printf("result: 600 %04x\n", last_ctrl);
    
    sensor_values[1] = 630;
    pid();    
    printf("result: 630 %04x\n", last_ctrl);
    
    sensor_values[1] = 750;
    pid();    
    printf("result: 750 %04x\n", last_ctrl);    

    sensor_values[1] = 800;
    pid();    
    printf("result: 800 %04x\n", last_ctrl);    
    
    sensor_values[1] = 1000;
    pid();    
    printf("result: 1000 %04x\n", last_ctrl);    
    
}
#else

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    for (int i = 0; i < 2; i++) {
        sensor_values[i] = 0;
    }
    pwm_timeout = 0xffffffff;
    for (unsigned char i = 0; i < 4; i++) {
        switch_timeouts[i] = 0xffffffff;
    }
    for (unsigned char i = 0; i < 4; i++) {
        pulses[i] = 0;
        pulse_rates[i] = 0;
    }
    ticks = 0;
    /* Initialize I/O and Peripherals for application */
    InitApp();
    while (1) {
        if (pwm_timeout > PWM_TIMEOUT) {
            pwm_duty(0, 0, 0, 0);
        } else {
            pwm_timeout++;
        }
        for (unsigned char i = 0; i < 4; i++) {
            if (switch_timeouts[i] > SWITCH_TIMEOUT) {
                off(i);
            } else {
                switch_timeouts[i]++;
            }
        }
        // PID controller
        if (pid_enabled > 0 && sensor_values[1] > 100) {
            pid();
        }
    }
}
#endif