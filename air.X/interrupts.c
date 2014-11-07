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

volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

#ifdef _12F1840
#define SSPIF SSP1IF
#endif


#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
unsigned int d6f_val;
unsigned char adc_band;

#define AVERAGE_SAMPLES_SHIFT 7
#define AVERAGE_SAMPLES 128
#define AVERAGE_SAMPLES_MINUS_ONE 127

static void write_i2c(unsigned char b) {
    // insert slight delay, otherwise raspbery pi reads first bit as zero i.e. 0x81 => 0x01
    for (int i = 0; i < 8; i++) {
        asm("nop");
    }
    do {
        SSPCONbits.WCOL = 0b0;
        SSPBUF = b;
    } while (SSPCONbits.WCOL);
}

void interrupt isr(void) {
    if(TMR0IF) {
        TMR0IF = 0;
        ADCON0bits.GO = 1;
        PORTBbits.RB0 = 1;
    }
    if(ADIF) {
        unsigned int temp;
        PORTBbits.RB0 = 1;
        ADIF = 0;
        temp = (ADRESH) << 8 | (ADRESL) ;
        if(adc_band > ADC_MIN_BAND && temp < (0x1ff - ADC_BAND_SWITCH_HISTERESIS)) { // 0.5 * MAX
            // go LOW only if it is possible to measure more precize
            adc_band--;
            FVRCONbits.ADFVR = adc_band;
        } else if(adc_band < ADC_MAX_BAND && temp > (0x3ff - ADC_BAND_SWITCH_HISTERESIS)) {
            // go UP only if you have to
            adc_band++;
            FVRCONbits.ADFVR = adc_band;
        } else {
            // get pure voltage and then apply lerp
            unsigned int current = d6f_lerp((temp << (adc_band - 1)) - (4 << (adc_band - 1)) + 8);
            d6f_val = (d6f_val*AVERAGE_SAMPLES_MINUS_ONE + current) >> AVERAGE_SAMPLES_SHIFT;
        }
    }
    if (SSPIF) {
        unsigned char i2c_state = SSPSTAT & 0b00100100;
        // 0b00100000 = D/nA
        // 0b00000100 = R/nW
        // prevent overflow
        if (rx_index >= RX_SIZE) {
            rx_index = 0;
        }
        switch (i2c_state) {
            case 0b00000000: // STATE1: Maser Write, Last Byte = Address
                rx_index = 0;
                // do a dummy read
                rx_buffer[0] = SSPBUF;
                for (int i = 0; i < RX_SIZE; i++) {
                    rx_buffer[i] = 0;
                }
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                rx_buffer[rx_index++] = SSPBUF;
                if (rx_index == 1 && rx_buffer[0] == 0x78) {
                    ENTER_BOOTLOADER = 1;
                    asm("pagesel 0x000");
                    asm("goto 0x000");
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // OMRON output
                rx_buffer[0] = d6f_val & 0xff;
                rx_buffer[1] = d6f_val >> 8;
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }
}
