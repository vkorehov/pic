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

unsigned short hz100_bres; // Bresenham's Algorithm counter
unsigned short hz100_period = 10000; // meassured in uS
unsigned int syncperiod = 0;// measured in uS


unsigned char i2c_state; /*0 Idle, 1 Started receive, 2 Address matched **/
unsigned char i2c_error;
unsigned short i2c_collisions;
unsigned short i2c_response;
unsigned char i2c_command[4];

static void i2c_handle_error() {
    i2c_state = 0;
    i2c_error = 0;
    i2c_destroy();
    i2c_init();
}

void interrupt isr(void) {
    if (TMR0IE && TMR0IF) {
        TMR0IF = 0;
        hz100_bres += 16; // add 256 ticks to bresenham total// because we are running on 1/16 prescaler
        syncperiod += 16;
        if (hz100_bres >= hz100_period) // if reached period!
        {
            hz100_bres -= hz100_period; // subtract period, retain error
            hz100_clock();
        }
    }
    if (BCL1IE && BCL1IF) {
        BCL1IF = 0;
        i2c_error = 1;
        i2c_collisions++;
    }
    if (SSP1IE && SSP1IF) {
        SSP1IE = 0; // Disable interrupts. Everythong is synchronous here
        i2c_check_error();
        if (!i2c_error) {
            if (SSP1STATbits.S) {
                i2c_state = 1;
            } else if (SSP1STATbits.P) {
                i2c_state = 0;
            }
            if (SSP1STATbits.BF) {
                if (i2c_state == 0) {
                    volatile unsigned char unused = SSPBUF;
                } else if (i2c_state == 1) {
                    unsigned char addr = SSPBUF;
                    if(addr == 0x00) { // general call (Used for 50hz strobing)
                        i2c_state = 1;
                        hz100_sync();
                    } else if (addr == (I2C_MYADDR << 1)) { // write requesdt
                        i2c_state = 2;
                    } else if (addr == ((I2C_MYADDR << 1) + 1)) { // read request
                        i2c_state = 1;
                        i2c_Wait();
                        i2c_Write((i2c_response >> 8));
                        i2c_Wait();
                        if (i2c_error) {
                            i2c_handle_error();
                        } else {
                            i2c_Write(i2c_response);
                            i2c_Wait();
                            if (i2c_error) {
                                i2c_handle_error();
                            }
                        }
                    } else { // crazy shit
                        i2c_state = 0;
                    }
                } else if (i2c_state >= 2 && i2c_state <= 5) {
                    i2c_command[i2c_state - 2] = SSPBUF;
                    i2c_state++;
                    if (i2c_state >= 6) {
                        i2c_state = 1;
                        i2c_response = i2c_on_command(i2c_command);
                    }
                }
            }
        } else {
            i2c_handle_error();
        }
        SSP1IF = 0;
        SSP1IE = 1; // Reenable interrupts
    }
}


