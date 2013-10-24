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


static unsigned char ra4_state;
unsigned char pacemaker_state;// 0 - timer had not been initialized
                        // 1 - timer is counting period  and tuning is enabled by moving average
                        // 2 - I2C sync out is enabled
#define MAX_TRAININGS 32
#define MAX_ADC_VALUE 0b1011001100 // 0.7* MAX(=0b1111111111) //

static unsigned char trainings_count; // tune our timers up to MAX_TRAININGS
static unsigned int trainings[MAX_TRAININGS];

static unsigned int high_len;

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/
inline unsigned int read_tmr1() {
  return ( (TMR1H) << 8 | (TMR1L));
}
inline void write_tmr1(unsigned int v) {
    TMR1L = ((v)&0xff);
    TMR1H = ((v) >> 8);
}


inline unsigned int read_adc(void) {
  ADGO = 1;
  while(ADGO == 1) {
      asm("nop");
  }
  return ( (ADRESH) << 8 | (ADRESL));
}

void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    i2c_error = 0;
    i2c_ack = 0;
    i2c_own_bus = 0;
    i2c_retry = 0;
    i2c_collisions = 0;
    
    /* Initialize I/O and Peripherals for application */
    InitApp();

    pacemaker_state = 0;
    ra4_state = 0;
    trainings_count = 0;
    high_len = 0;

    sync_every = 100; // 1 second/10mS (50Hz half-period)


    while (1) {
        unsigned char ra4 = 0;
        unsigned int a = read_adc();
        if (a > MAX_ADC_VALUE) {
            ra4 = 1;
        }
        if(ra4_state != ra4) {
            ra4_state = ra4;
            if(pacemaker_state == 2 && ra4_state == 1) {
                write_tmr1(0xffff - (high_len >> 1)); // generate interrupt on half of length!
                TMR1ON = 1;
            }
            if(pacemaker_state == 0 && ra4_state == 1) {
                write_tmr1(0);
                TMR1ON = 1;
                pacemaker_state = 1;
            }
            if(pacemaker_state == 0 || pacemaker_state == 1 && ra4_state == 1) { // start measuring high pulse period
                write_tmr1(0);
                TMR1ON = 1;
                pacemaker_state = 1;
            }
            if(pacemaker_state == 1 && ra4_state == 0) { // stop measuring high pulse period
                high_len = read_tmr1();
                write_tmr1(0);
                TMR1ON = 0;
                trainings[trainings_count] = high_len;
                if(trainings_count == MAX_TRAININGS - 1) {
                    unsigned int highest_index = 0;
                    unsigned int highest = 0;
                    unsigned int lowest_index = 0;
                    unsigned int lowest = 0xffff;
                    unsigned int invalid = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        if(trainings[i] < 0x1000 || trainings[i] > 0x3000) {
                            invalid++;
                            trainings[i] = 0;
                            continue;
                        }

                        if(highest < trainings[i]) {
                            highest = trainings[i];
                            highest_index = i;
                        }
                        if(lowest > trainings[i]) {
                            lowest = trainings[i];
                            lowest_index = i;
                        }
                    }
                    trainings[lowest_index] = 0;
                    trainings[highest_index] = 0;
                    // calculate average
                    long long average = 0;
                    unsigned int last = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        average += trainings[i];
                        last = trainings[i];
                    }
                    for(int i = 0; i < invalid + 2; i++) {
                        average += last; // increment with last twice to fill missing highest/lowest vals
                    }
                    high_len = average >> 5;
                    pacemaker_state = 2;
                }
                trainings_count++;
            }
        }
    }
}

