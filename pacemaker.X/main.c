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
#define MAX_TRAININGS 16
#define MAX_ADC_VALUE 0b1011001100 // 0.7* MAX(=0b1111111111) //
#define T1_I2C_DELAY_COMP 40 // Compensate for I2C delay, so zero crossing will be on I2C ACK of Address packet!

static unsigned char trainings_hi_count; // tune our timers up to MAX_TRAININGS_HI
static unsigned int trainings_hi[MAX_TRAININGS];
static unsigned char trainings_lo_count; // tune our timers up to MAX_TRAININGS_HI
static unsigned int trainings_lo[MAX_TRAININGS];

static unsigned int high_len;
static unsigned int low_len;
static unsigned int period_offset;
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
    trainings_hi_count = 0;
    high_len = 0;
    trainings_lo_count = 0;
    low_len = 0;

    sync_every = 100; // 1 second/10mS (50Hz half-period)


    while (1) {
        unsigned char ra4 = 0;
        unsigned int a = read_adc();
        if (a > MAX_ADC_VALUE) {
            ra4 = 1;
        }
        if(ra4_state != ra4) {
            ra4_state = ra4;
            if(pacemaker_state == 2 && ra4_state == 0) {
                write_tmr1(0xffff - period_offset); // generate interrupt on half of length!
                TMR1ON = 1;
            }
            if(pacemaker_state == 0) { // start measuring pulse periods
                write_tmr1(0);
                TMR1ON = 1;
                pacemaker_state = 1;
                continue;
            }
            if(pacemaker_state == 1 && ra4_state == 0 && trainings_hi_count != MAX_TRAININGS) { // stop measuring high pulse period
                high_len = read_tmr1();
                trainings_hi[trainings_hi_count] = high_len;
                if(trainings_hi_count == MAX_TRAININGS - 1) {
                    unsigned int highest_index = 0;
                    unsigned int highest = 0;
                    unsigned int lowest_index = 0;
                    unsigned int lowest = 0xffff;
                    unsigned int invalid = 0;
                    unsigned int last = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        if(trainings_hi[i] < 0x1000 || trainings_hi[i] > 0x3000) {
                            invalid++;
                            trainings_hi[i] = 0;
                            continue;
                        }

                        if(highest < trainings_hi[i]) {
                            highest = trainings_hi[i];
                            highest_index = i;
                        }
                        if(lowest > trainings_hi[i]) {
                            lowest = trainings_hi[i];
                            lowest_index = i;
                        }
                        last = trainings_hi[i];
                    }
                    trainings_hi[lowest_index] = 0;
                    trainings_hi[highest_index] = 0;
                    // calculate average
                    long long average = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        average += trainings_hi[i];
                    }
                    for(int i = 0; i < invalid + 2; i++) {
                        average += last; // increment with last twice to fill missing highest/lowest vals
                    }
                    high_len = average >> 4;
                }
                trainings_hi_count++;
            }
            if(pacemaker_state == 1 && ra4_state == 1 && trainings_lo_count != MAX_TRAININGS) { // stop measuring high pulse period
                low_len = read_tmr1();
                trainings_lo[trainings_lo_count] = low_len;
                if(trainings_lo_count == MAX_TRAININGS - 1) {
                    unsigned int highest_index = 0;
                    unsigned int highest = 0;
                    unsigned int lowest_index = 0;
                    unsigned int lowest = 0xffff;
                    unsigned int invalid = 0;
                    unsigned int last = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        if(trainings_lo[i] < 0x7000 || trainings_lo[i] > 0x9000) {
                            invalid++;
                            trainings_lo[i] = 0;
                            continue;
                        }

                        if(highest < trainings_lo[i]) {
                            highest = trainings_lo[i];
                            highest_index = i;
                        }
                        if(lowest > trainings_lo[i]) {
                            lowest = trainings_lo[i];
                            lowest_index = i;
                        }
                        last = trainings_lo[i];
                    }
                    trainings_lo[lowest_index] = 0;
                    trainings_lo[highest_index] = 0;
                    // calculate average
                    long long average = 0;
                    for(unsigned int i = 0; i < MAX_TRAININGS;i++) {
                        average += trainings_lo[i];
                    }
                    for(int i = 0; i < invalid + 2; i++) {
                        average += last; // increment with last twice to fill missing highest/lowest vals
                    }
                    low_len = average >> 4;
                }
                trainings_lo_count++;
            }
            if(pacemaker_state == 1) {
                // reset timer on every transiation
                write_tmr1(0);
            }
            if(pacemaker_state == 1 && trainings_lo_count == MAX_TRAININGS && trainings_hi_count == MAX_TRAININGS){
                pacemaker_state = 2;
                TMR1ON = 0;
                write_tmr1(0);
                period_offset = (low_len >> 1);
            }
        }
    }
}

