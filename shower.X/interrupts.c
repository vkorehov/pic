/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "system.h"
#include "user.h"

unsigned int sensor_values[2];

volatile unsigned char ENTER_BOOTLOADER __at(0x30); /* flag in order to enter bootloader */

unsigned char pulses[4];
unsigned char pulse_rates[4];

#define RX_SIZE 8
unsigned short ticks;
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;
static unsigned char command;

void __interrupt () isr(void) {
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
            sensor_values[sensor] = current;
        }
        // Re-Launch ADC
        ADCON0bits.GO = 1;        
    }
    if (SSPIF) {
        SSPIF = 0;
        unsigned char tmp;
        unsigned char d = 0;
        unsigned char r = 0;
        unsigned char crc = 0;
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
                command = 0;
                ACKDT = 0;
                for (int i = 0; i < RX_SIZE; i++) {
                    rx_buffer[i] = 0;
                }
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                // do a dummy read
                rx_buffer[0] = SSPBUF;
                break;
            case 0b00100000: // STATE2: Maser Write, Last Byte = Data
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                rx_buffer[rx_index++] = SSPBUF;
                if (rx_index == 1) {
                    command = rx_buffer[0]; // save command
                }
                if (rx_index == 2) {
                    // input
                    switch (command) {
                        case 0x02:
                            crc = crc8_table[crc ^ 0x02];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                on(0);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;
                        case 0x20:
                            crc = crc8_table[crc ^ 0x20];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                off(0);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                            
                        case 0x03:
                            crc = crc8_table[crc ^ 0x03];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                on(1);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                            
                        case 0x30:
                            crc = crc8_table[crc ^ 0x30];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                off(1);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                        
                        case 0x04:
                            crc = crc8_table[crc ^ 0x04];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                on(2);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                        
                        case 0x40:
                            crc = crc8_table[crc ^ 0x40];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                off(2);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                                                    
                        case 0x05:
                            crc = crc8_table[crc ^ 0x05];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                on(3);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                                                    
                        case 0x50:
                            crc = crc8_table[crc ^ 0x50];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[1]) {
                                off(3);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                                                                                
                        case 0x78:
                            crc = crc8_table[crc ^ 0x78];
                            crc = crc8_table[crc ^ I2C_MYADDR];
                            if (crc == rx_buffer[1]) {
                                ENTER_BOOTLOADER = 1;
                                asm("pagesel 0x000");
                                asm("goto 0x000");
                            } else {
                                ACKDT = 1;
                            }
                            break;
                    }
                }
                if (rx_index == 3) {
                    // input
                    switch (command) {                                            
                        case 0x06:
                            crc = crc8_table[crc ^ rx_buffer[1]];                            
                            crc = crc8_table[crc ^ 0x06];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[2]) {
                                kp = rx_buffer[1];
                            } else {
                                ACKDT = 1;
                            }
                            break;
                        case 0x07:
                            crc = crc8_table[crc ^ rx_buffer[1]];                            
                            crc = crc8_table[crc ^ 0x07];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[2]) {
                                kd = rx_buffer[1];
                            } else {
                                ACKDT = 1;
                            }
                            break;                            
                        case 0x08:
                            crc = crc8_table[crc ^ rx_buffer[1]];                            
                            crc = crc8_table[crc ^ 0x08];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[2]) {
                                ki = rx_buffer[1];
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                        
                        case 0x09:
                            crc = crc8_table[crc ^ rx_buffer[1]];                            
                            crc = crc8_table[crc ^ 0x09];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[2]) {
                                target = rx_buffer[1] * 10;
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                                                    
                        case 0x10:
                            crc = crc8_table[crc ^ rx_buffer[1]];                            
                            crc = crc8_table[crc ^ 0x10];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[2]) {
                                pid_enabled = rx_buffer[1];
                            } else {
                                ACKDT = 1;
                            }
                            break;                                                                                                                
                    }
                }
                if (rx_index == 6) {
                    // input
                    switch (command) {
                        case 0x01:
                            crc = crc8_table[crc ^ rx_buffer[4]];                            
                            crc = crc8_table[crc ^ rx_buffer[3]];                            
                            crc = crc8_table[crc ^ rx_buffer[2]];                                                        
                            crc = crc8_table[crc ^ rx_buffer[1]];
                            crc = crc8_table[crc ^ 0x01];
                            crc = crc8_table[crc ^ I2C_MYADDR];                            
                            if (crc == rx_buffer[5]) {
                                pwm_duty(rx_buffer[1], rx_buffer[2], rx_buffer[3], rx_buffer[4]);                                
                            } else {
                                ACKDT = 1;
                            }
                            break;
                    }
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
                // output
                switch (command) {
                    case 0x06: // read command
                        d = sensor_values[1] & 0xff;
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x06];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;
                    case 0x07: // read command
                        d = (sensor_values[1] >> 8) & 0xff;
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x07];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;
                    case 0x08: // read command
                        d = last_error & 0xff;
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x08];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;
                    case 0x09: // read command
                        d = (last_error >> 8) & 0xff; 
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x09];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;                                                
                    case 0x10: // read command
                        crc = crc8_table[last_ctrl];
                        crc = crc8_table[crc ^ 0x10];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = last_ctrl;
                        rx_buffer[1] = crc;
                        break;             
                    case 0x11: // read command
                        crc = crc8_table[pulse_rates[0]];
                        crc = crc8_table[crc ^ 0x11];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = pulse_rates[0];
                        rx_buffer[1] = crc;
                        break;                                                                                                                        
                    case 0x12: // read command
                        crc = crc8_table[pulse_rates[1]];
                        crc = crc8_table[crc ^ 0x12];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = pulse_rates[1];
                        rx_buffer[1] = crc;
                        break;                                                                                                
                    case 0x13: // read command
                        crc = crc8_table[pulse_rates[2]];
                        crc = crc8_table[crc ^ 0x13];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = pulse_rates[2];
                        rx_buffer[1] = crc;
                        break;                                                                                                                        
                    case 0x14: // read command
                        crc = crc8_table[pulse_rates[3]];
                        crc = crc8_table[crc ^ 0x14];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = pulse_rates[3];
                        rx_buffer[1] = crc;
                        break;                                                                                                                                                
                    case 0x15: // read command
                        d = (ticks >> 8) & 0xff;
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x15];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;                                                                                                                                                
                    case 0x16: // read command
                        d = ticks & 0xff;
                        crc = crc8_table[d];
                        crc = crc8_table[crc ^ 0x16];
                        crc = crc8_table[crc ^ I2C_MYADDR];
                        rx_buffer[0] = d;
                        rx_buffer[1] = crc;
                        break;                                                                                                                                                                        
                }
                if (SSPSTATbits.BF == 0) {
                    break;
                }
                // do a dummy read
                tmp = SSPBUF;
                if (SSPSTATbits.BF == 1) {
                    break;
                }
                SSPBUF = rx_buffer[rx_index++];
                break;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                if (SSPSTATbits.BF == 1) {
                    break;
                }
                if (rx_index >= RX_SIZE) { // prevent overflow
                    SSPBUF = 0;
                } else {
                    SSPBUF = rx_buffer[rx_index++];
                }
                break;
        }
        SSPCON1bits.CKP = 1;
    }
}


