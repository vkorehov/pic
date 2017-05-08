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

#define AVERAGE_SAMPLES_SHIFT 4
#define AVERAGE_SAMPLES 16
#define AVERAGE_SAMPLES_MINUS_ONE 15
unsigned char sensor_average_every;
unsigned int sensor_values[2];
unsigned int sensor_values_averages[2];
unsigned char i2c_master;

volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

unsigned char pulses[4];
unsigned char pulse_rates[4];

#define RX_SIZE 8
unsigned short ticks;
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char dht22_start_timer;
static unsigned char command;
static unsigned char i2c_error;
static unsigned char i2c_collisions;
static bit i2c_ack;

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
    if (TMR0IF) {
        TMR0IF = 0;
        if(ticks >= 8) { // every second
           // update pulse rates
           pulse_rates[0] = pulses[0];
           pulse_rates[1] = pulses[1];
           pulse_rates[2] = pulses[2];
           pulse_rates[3] = pulses[3];
           pulses[0] = 0;
           pulses[1] = 0;
           pulses[2] = 0;
           pulses[3] = 0;
           ticks = 0;
        } else {
           ticks++;
        }

//        ADCON0bits.GO = 1;
//        if(dht22_start_timer++ == 0xff) {
//            start_read_dht22();
//        }
    }    
    if (IOCIF) {
        if (IOCBFbits.IOCBF2) {
            pulses[0]++; // holodnaja tualet
        }
        if (IOCBFbits.IOCBF3 == 1) {
            pulses[1]++; // gorjachaja tualet
        }
        if (IOCBFbits.IOCBF6 == 1) {
            pulses[2]++; // gorjachaja obshij
        }
        if (IOCBFbits.IOCBF7 == 1) {
            pulses[3]++; // holodnaja obshij
        }
        asm("MOVLW 0xff");
        asm("banksel IOCBF");
        asm("XORWF IOCBF, W");
        asm("ANDWF IOCBF, F");        
//        if (PORTBbits.RB5 == 0) {// we care only about HI => LO transition
//            unsigned char t = TMR2;
//            TMR2 = 0; // reset timeout            
//            if (dht22_state < 4) {
//                dht22_state++;
//            } else {
//                if (dht22_bit_index > 7) {
//                    dht22_bit_index = 0;
//                    dht22_index++;
//                }
//                if (t > DHT22_CUTOFF_TIME) {
//                    dht22_bits[dht22_index] |= (1 << (7 - dht22_bit_index));
//                }
//                dht22_bit_index++;
//                if (dht22_bit_index == 8 &&
//                        dht22_index >= (DHT22_MAX_BYTES - 1)) {
//                    unsigned char sum = dht22_bits[0] + dht22_bits[1] + dht22_bits[2] + dht22_bits[3];
//                    if (sum != dht22_bits[4]) {// checksum doesn't match up?
//                        // indicate error
//                        dht22_bits[0] = dht22_bits[1] = dht22_bits[2] = dht22_bits[3] = 0;
//                    }
//                    dht22_abort();
//                }
//            }
//        }
    }
    if (TMR2IF) {
        TMR2IF = 0;
//        switch (dht22_state) {
//            case 1:
//                start_read_dht22_pullup();
//                break;
//            default:
//                dht22_abort();
//        }
    }    
    if (ADIF) {
        ADIF = 0;
        unsigned int current = ((ADRESH) << 8 | (ADRESL));
        unsigned char sensor;
        switch (ADCON0bits.CHS) {
            case 0b01011: // AN11
                ADCON0bits.CHS = 0b01101;
                sensor = 0;
                break;
            case 0b01101: // AN13
                ADCON0bits.CHS = 0b01011;
                sensor = 1;
                break;
            default:
                ADCON0bits.CHS = 0b01011;
                sensor = 0;
                break;
        }
        if (sensor < 2) {
            sensor_values[sensor] = (sensor_values[sensor] * AVERAGE_SAMPLES_MINUS_ONE + current) >> AVERAGE_SAMPLES_SHIFT;
        }
        if (sensor_average_every++ > AVERAGE_SAMPLES_MINUS_ONE) {
            sensor_average_every = 0;
            for (int i = 0; i < 2; i++) {
                sensor_values_averages[i] = (sensor_values_averages[i] * AVERAGE_SAMPLES_MINUS_ONE + sensor_values[i]) >> AVERAGE_SAMPLES_SHIFT;
            }
        }
    }
    if (BCLIF) {
        i2c_error = 1;
        i2c_collisions++;
        BCLIF = 0;
    }
    if (SSPIF) {
        if (!ACKSTAT) {
            i2c_ack = 1;
        }
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
                if(rx_index == 1) {
                    command = rx_buffer[0];
                }
                if (rx_index == 1 && rx_buffer[0] == 0x78) {
                    ENTER_BOOTLOADER = 1;
                    asm("pagesel 0x000");
                    asm("goto 0x000");
                }
                if (rx_index == 5 && command == 0x01) {
                   pwm_duty(rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4]);
                }                                
                if (rx_index == 1 && rx_buffer[0] == 0x02) {
                   on(0);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x03) {
                   on(1);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x04) {
                   on(2);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x05) {
                   on(3);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x20) {
                   off(0);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x30) {
                   off(1);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x40) {
                   off(2);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x50) {
                   off(3);
                }                
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // output
                switch (command) {
                    case 0x06: // read humidity
                        rx_buffer[0] = dht22_bits[1];
                        rx_buffer[1] = dht22_bits[0];
                        break;
                    case 0x07: // // read temperature
                        rx_buffer[0] = dht22_bits[3];
                        rx_buffer[1] = dht22_bits[2];
                        break;
                    case 0x08: // // read pulses from flow meter
                        rx_buffer[0] = pulse_rates[1];
                        rx_buffer[1] = pulse_rates[0];
                        break;
                    case 0x09: // // read pulses from flow meter
                        rx_buffer[0] = pulse_rates[3];
                        rx_buffer[1] = pulse_rates[2];
                        break;
                    case 0x10: // // read pulses from flow meter
                        rx_buffer[0] = ticks >> 8;
                        rx_buffer[1] = (ticks & 0b0000000011111111);
                        break;                          
                    default:
                        rx_buffer[0] = rx_buffer[1] = 0;
                }
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }
}


