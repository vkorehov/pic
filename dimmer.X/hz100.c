
/******************************************************************************/
/* Files to Include                                                           */
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

#define INITIAL_ADJ_COUNT 5
#define PHASE_REPLACE_PROXIMITY 500

unsigned char initial_adjustments = INITIAL_ADJ_COUNT; // initial adjustments while clock is stabilizing
unsigned int sync_averages[5]; // measured in uS
unsigned int sync_average; // moving average
unsigned char adj_period_every; // 1:256 for period adjustment

unsigned char hz100_command;

/* Clock from zero crossing detection **/
void hz100_clock() {
    if (!initial_adjustments) {
        // Ok start to perform commands. i.e dimming.
        
    }
}

/* Received every second network syncronization from Raspbery PI */
void hz100_sync() {
    if (initial_adjustments) {
        if (initial_adjustments == INITIAL_ADJ_COUNT) { // Initially simply reset period measure
            syncperiod = 0;
            return;
        } else if (initial_adjustments == INITIAL_ADJ_COUNT - 1) {
            sync_average = sync_averages[0] = sync_averages[1] = sync_averages[2] = sync_averages[3] = sync_averages[4] = syncperiod;
        }
        initial_adjustments--;
    }
    for (int k = 4; k > 0; k--) {
        sync_averages[k] = sync_averages[k - 1];
    }
    sync_averages[0] = syncperiod;
    // recalculate average
    sync_average = sync_average - (sync_averages[4] >> 2) + (sync_averages[0] >> 2);
    // ADjust our period accordingly
    if(!adj_period_every) {
        hz100_period = syncperiod / 100;
    }
    adj_period_every++;
    // Phase adjustment
    TMR0IE = 0; // Disable interrupts from timer
    // How close we were from triggering?
    if(hz100_bres < hz100_period && hz100_period - hz100_bres < PHASE_REPLACE_PROXIMITY) {
       hz100_clock();
    }
    hz100_bres = 8;// 8 is best guess for range 0..16 (1/16 prescaler)
    TMR0IE = 1; // Reenable interrupts from timer
    syncperiod = 0;
}