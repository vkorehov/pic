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

#define I2C_MYADDR 0x10                    // This device I2C address
#define I2C_SLAVE 0x40                    // Slave device I2C address
#define I2C_RETRY_DELAY  713
#define I2C_RETRIES 3
unsigned char i2c_command_id;
unsigned char i2c_own_bus; //
unsigned char i2c_retry;

static void i2c_Start(void);
static void i2c_Restart(void);
static void i2c_Stop(void);
static void i2c_Wait(void);
static void i2c_Write(unsigned char data);
static void i2c_Address(unsigned char address, unsigned char mode);
static unsigned char i2c_Read(unsigned char ack);

static void check_error() {
    if (WCOL) {
        i2c_error = 2;
        WCOL = 0;
    }
}

static void handle_restart() {
    if (!i2c_own_bus) {
        i2c_Start();
    } else {
        i2c_Restart();
    }
}

static unsigned char handle_error(unsigned char waitAck) {
    i2c_ack = 0;
    i2c_Wait(); // Wait first for last operation completion!
    if (i2c_error) {
        i2c_destroy();
        i2c_own_bus = 0;
        i2c_error = 0;
        __delay_us(I2C_RETRY_DELAY);
        i2c_init();
        return 1;
    }
    if (waitAck) {
        unsigned char maxwait = 10;
        while (maxwait-- > 0) {
            if (i2c_ack) {
                return 0;
            }
            __delay_us(1);
        }
        i2c_init(); // Clear all flags
        __delay_us(I2C_RETRY_DELAY);
        return 2; // Error no ACK received!
    }
    return 0;
}

void i2c_command8(unsigned char command) {
    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        if (handle_error(0)) continue;
        i2c_own_bus = 1;
        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;
        i2c_Write(I2C_MYADDR); //
        if (handle_error(1)) continue;
        i2c_Write(++i2c_command_id); //
        if (handle_error(1)) continue;
        i2c_Write(command); //
        if (handle_error(1)) continue;
        i2c_Stop(); // send Stop
        i2c_own_bus = 0;
        if (handle_error(0)) continue;
        return; // Success
    }
    if (i2c_own_bus) {
        i2c_Stop();
        i2c_own_bus = 0;
    }
}

void i2c_command16(unsigned short command) {
    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        if (handle_error(0)) continue;
        i2c_own_bus = 1;
        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;
        i2c_Write(I2C_MYADDR); //
        if (handle_error(1)) continue;
        i2c_Write(++i2c_command_id); //
        if (handle_error(1)) continue;
        i2c_Write((command >> 8)); //
        if (handle_error(1)) continue;
        i2c_Write(command); //
        if (handle_error(1)) continue;
        i2c_Stop(); // send Stop
        i2c_own_bus = 0;
        if (handle_error(0)) continue;
        return; // Success
    }
    if (i2c_own_bus) {
        i2c_Stop();
        i2c_own_bus = 0;
    }
}

// Read a char from servo ic

unsigned char i2c_command8_and_read(unsigned char command) {
    unsigned char read_byte;
    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        if (handle_error(0)) continue;
        i2c_own_bus = 1;
        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;
        i2c_Write(command); //
        if (handle_error(1)) continue;
        i2c_Restart(); // Restart
        if (handle_error(1)) continue;
        i2c_Address(I2C_SLAVE, I2C_READ); // Send slave address - read operation
        if (handle_error(1)) continue;
        i2c_Write(I2C_MYADDR); //
        if (handle_error(1)) continue;
        i2c_Write(++i2c_command_id); //
        if (handle_error(1)) continue;
        read_byte = i2c_Read(0); // Read one byte
        if (handle_error(0)) continue;
        // If more than one byte to be read, (0) should
        // be on last byte only
        // e.g.3 bytes= i2c_Read(1); i2c_Read(1); i2c_Read(0);
        i2c_Stop(); // send Stop
        i2c_own_bus = 0;
        if (handle_error(0)) continue;
        return read_byte; // return byte.
        // If reading more than one byte
        // store in an array
    }
    if (i2c_own_bus) {
        i2c_Stop();
        i2c_own_bus = 0;
    }
    return 0xff;
}


// i2c_Wait - wait for I2C transfer to finish

static void i2c_Wait(void) {
    //SSP1CON2: 8:GCEN 7:ACKSTAT 6:ACKDT 5:ACKEN 4:RCEN 3:PEN 2:RSEN 1:SEN
    //SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    while ((SSP1CON2 & 0b00011111) || (SSPSTAT & 0b00000100));
}

// i2c_Start - Start I2C communication

static void i2c_Start(void) {
    i2c_Wait();
    SEN = 1;
    check_error();
}

// i2c_Restart - Re-Start I2C communication

static void i2c_Restart(void) {
    i2c_Wait();
    RSEN = 1;
    check_error();
}

// i2c_Stop - Stop I2C communication

static void i2c_Stop(void) {
    i2c_Wait();
    PEN = 1;
    check_error();
}

// i2c_Write - Sends one byte of data

static void i2c_Write(unsigned char data) {
    i2c_Wait();
    SSP1BUF = data;
    check_error();
}

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ

static void i2c_Address(unsigned char address, unsigned char mode) {
    unsigned char l_address;
    l_address = address << 1;
    l_address += mode;
    i2c_Wait();
    SSP1BUF = l_address;
    check_error();
}

// i2c_Read - Reads a byte from Slave device

static unsigned char i2c_Read(unsigned char ack) {
    // Read data from slave
    // ack should be 1 if there is going to be more data read
    // ack should be 0 if this is the last byte of data read
    unsigned char i2cReadData;

    i2c_Wait();
    RCEN = 1;
    i2c_Wait();
    check_error();
    i2cReadData = SSPBUF;
    i2c_Wait();
    if (i2c_error) {
        return 0xff;
    }
    if (ack) ACKDT = 0; // Ack
    else ACKDT = 1; // NAck
    ACKEN = 1; // send acknowledge sequence
    check_error();
    if (i2c_error) {
        return 0xff;
    }
    return ( i2cReadData);
}