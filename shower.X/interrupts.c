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
unsigned int sensor_values[4];
unsigned int sensor_values_averages[4];
unsigned char move_direction[2];
unsigned int steps_to_move[2];
unsigned int current_steps[2];
unsigned int new_steps[2];
volatile unsigned char ENTER_BOOTLOADER @ 0x30; /* flag in order to enter bootloader */

#define RX_SIZE 4
static unsigned char rx_buffer[RX_SIZE];
static unsigned char rx_index;

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
        ADCON0bits.GO = 1;
    }
    if (TMR1IF) {
        TMR1IF = 0;
        TMR1ON = 0;
        bool moved = false;
        // reenable timer if still moving
        if (current_steps[0] < steps_to_move[0]) {
            moved = true;
            unsigned char state = current_steps[0] % 4;
            // A7 = Violet (_B)
            // A6 = Black (B)
            // A5 = Red (A)
            // A4 = Yellow (_A)
            switch (state) {
                case 0:
                    // ? _A _B : A _B
                    PORTA = move_direction[0] ? 0b10010000 : 0b10100000;
                    break;
                case 1:
                    // ? _A B : A B
                    PORTA = move_direction[0] ? 0b01010000 : 0b01100000;
                    break;
                case 2:
                    //? A B : _A B
                    PORTA = move_direction[0] ? 0b01100000 : 0b01010000;
                    break;
                case 3:
                    // ? A _B : _A _B
                    PORTA = move_direction[0] ? 0b10100000 : 0b10010000;
                    break;
            }
            current_steps[0]++;
        } else {
            current_steps[0] = 0;
            steps_to_move[0] = 0;
            PORTA = 0b00000000;
        }
        // reenable timer if still moving
        if (current_steps[1] < steps_to_move[1]) {
            moved = true;
            unsigned char state = current_steps[1] % 4;
            // C2 = Yellow (_A)
            // C7 = Red (A)
            // C6 = Violet (_B)
            // C5 = Black (B)
            switch (state) {
                case 0:
                    // ? _A _B : A _B
                    PORTC = move_direction[1] ? (0b01000100 | port_c_switches) : (0b11000000 | port_c_switches);
                    break;
                case 1:
                    // ? _A B : A B
                    PORTC = move_direction[1] ? (0b00100100 | port_c_switches) : (0b10100000 | port_c_switches);
                    break;
                case 2:
                    //? A B : _A B
                    PORTC = move_direction[1] ? (0b10100000 | port_c_switches) : (0b00100100 | port_c_switches);
                    break;
                case 3:
                    // ? A _B : _A _B
                    PORTC = move_direction[1] ? (0b11000000 | port_c_switches) : (0b01000100 | port_c_switches);
                    break;
            }
            current_steps[1]++;
        } else {
            current_steps[1] = 0;
            steps_to_move[1] = 0;
            PORTC = 0b00000000 | port_c_switches;
        }
        if(moved) {
            // start SLOW then MED then FAST
            if (current_steps[0] > STEPS_MED || current_steps[1] > STEPS_MED) {
                write_tmr1(WAIT_FAST);
            } else if (current_steps[0] > STEPS_SLOW || current_steps[1] > STEPS_SLOW) {
                write_tmr1(WAIT_MED);
            } else {
                write_tmr1(WAIT_SLOW);
            }
            TMR1ON = 1;
        }
    }
    if (ADIF) {
        ADIF = 0;
        unsigned int current = ((ADRESH) << 8 | (ADRESL));
        unsigned char sensor;
        switch (ADCON0bits.CHS) {
            case 0b01000: // AN8
                ADCON0bits.CHS = 0b01001;
                sensor = 0;
                break;
            case 0b01001: // AN9
                ADCON0bits.CHS = 0b01011;
                sensor = 1;
                break;
            case 0b01011: // AN11
                ADCON0bits.CHS = 0b01101;
                sensor = 2;
                break;
            case 0b01101: // AN13
                ADCON0bits.CHS = 0b01000;
                sensor = 3;
                break;
            default:
                ADCON0bits.CHS = 0b01000;
                sensor = 255;
                break;
        }
        if (sensor < 4) {
            sensor_values[sensor] = (sensor_values[sensor] * AVERAGE_SAMPLES_MINUS_ONE + current) >> AVERAGE_SAMPLES_SHIFT;
        }
        if (sensor_average_every++ > AVERAGE_SAMPLES_MINUS_ONE) {
            sensor_average_every = 0;
            for (int i = 0; i < 4; i++) {
                sensor_values_averages[i] = (sensor_values_averages[i] * AVERAGE_SAMPLES_MINUS_ONE + sensor_values[i]) >> AVERAGE_SAMPLES_SHIFT;
            }
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
                if (rx_index == 3 && rx_buffer[0] == 0x01) {
                    new_steps[0] = ((rx_buffer[2] << 8) | rx_buffer[1]) & ~0b11;
                }
                if (rx_index == 3 && rx_buffer[0] == 0x02) {
                    unsigned int stored_steps[2];
                    stored_steps[0] = (eeprom_read(1) << 8) | eeprom_read(0);
                    stored_steps[1] = (eeprom_read(3) << 8) | eeprom_read(2);
                    new_steps[1] = ((rx_buffer[2] << 8) | rx_buffer[1]) & ~0b11;
                    for (unsigned char i = 0; i < 2; i++) {
                        if (stored_steps[i] > new_steps[i]) {
                            steps_to_move[i] = stored_steps[i] - new_steps[i];
                            current_steps[i] = 0;
                            move_direction[i] = 0;
                            // write new location to EEPROM
                            eeprom_write((i<<1), new_steps[i] & 0xff);
                            eeprom_write((i<<1) + 1, new_steps[i] >> 8);
                            write_tmr1(WAIT_SLOW);
                            TMR1ON = 1;
                        } else if (stored_steps[i] < new_steps[i]) {
                            steps_to_move[i] = new_steps[i] - stored_steps[i];
                            current_steps[i] = 0;
                            move_direction[i] = 1;
                            // write new location to EEPROM
                            eeprom_write((i<<1), new_steps[i] & 0xff);
                            eeprom_write((i<<1) + 1, new_steps[i] >> 8);
                            write_tmr1(WAIT_SLOW);
                            TMR1ON = 1;
                        }
                    }
                }
                if (rx_index == 1 && rx_buffer[0] == 0x03) {
                   on(0);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x04) {
                   on(1);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x05) {
                   on(2);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x06) {
                   on(3);
                }
                if (rx_index == 1 && rx_buffer[0] == 0x59) { // recalibrate
                   steps_to_move[0] = 0x3000;
                   move_direction[0] = 1;
                   current_steps[0] = STEPS_MED;
                   eeprom_write(0, 0);
                   eeprom_write(1, 0);
                   write_tmr1(WAIT_FAST);
                   TMR1ON = 1;
                }
                if (rx_index == 1 && rx_buffer[0] == 0x67) {
                   steps_to_move[1] = 0x3000;
                   move_direction[1] = 1;
                   current_steps[1] = STEPS_MED;
                   eeprom_write(2, 0);
                   eeprom_write(3, 0);
                   write_tmr1(WAIT_FAST);
                   TMR1ON = 1;
                }
                break;
            case 0b00000100: // STATE3: Maser Read, Last Byte = Address
                rx_index = 0;
            case 0b00100100: // STATE4: Maser Read, Last Byte = Data
                // Diagnostics output
                rx_buffer[0] = sensor_values_averages[0] & 0xff;
                rx_buffer[1] = sensor_values_averages[0] >> 8;
                write_i2c(rx_buffer[rx_index++]);
                break;
        }
        SSPCON1bits.CKP = 1;
        SSPIF = 0;
    }
}


