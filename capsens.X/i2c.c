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

static const unsigned char pec_lookup[256] =
{
0x00U, 0x07U, 0x0EU, 0x09U, 0x1CU, 0x1BU, 0x12U, 0x15U,
0x38U, 0x3FU, 0x36U, 0x31U, 0x24U, 0x23U, 0x2AU, 0x2DU,
0x70U, 0x77U, 0x7EU, 0x79U, 0x6CU, 0x6BU, 0x62U, 0x65U,
0x48U, 0x4FU, 0x46U, 0x41U, 0x54U, 0x53U, 0x5AU, 0x5DU,
0xE0U, 0xE7U, 0xEEU, 0xE9U, 0xFCU, 0xFBU, 0xF2U, 0xF5U,
0xD8U, 0xDFU, 0xD6U, 0xD1U, 0xC4U, 0xC3U, 0xCAU, 0xCDU,
0x90U, 0x97U, 0x9EU, 0x99U, 0x8CU, 0x8BU, 0x82U, 0x85U,
0xA8U, 0xAFU, 0xA6U, 0xA1U, 0xB4U, 0xB3U, 0xBAU, 0xBDU,
0xC7U, 0xC0U, 0xC9U, 0xCEU, 0xDBU, 0xDCU, 0xD5U, 0xD2U,
0xFFU, 0xF8U, 0xF1U, 0xF6U, 0xE3U, 0xE4U, 0xEDU, 0xEAU,
0xB7U, 0xB0U, 0xB9U, 0xBEU, 0xABU, 0xACU, 0xA5U, 0xA2U,
0x8FU, 0x88U, 0x81U, 0x86U, 0x93U, 0x94U, 0x9DU, 0x9AU,
0x27U, 0x20U, 0x29U, 0x2EU, 0x3BU, 0x3CU, 0x35U, 0x32U,
0x1FU, 0x18U, 0x11U, 0x16U, 0x03U, 0x04U, 0x0DU, 0x0AU,
0x57U, 0x50U, 0x59U, 0x5EU, 0x4BU, 0x4CU, 0x45U, 0x42U,
0x6FU, 0x68U, 0x61U, 0x66U, 0x73U, 0x74U, 0x7DU, 0x7AU,
0x89U, 0x8EU, 0x87U, 0x80U, 0x95U, 0x92U, 0x9BU, 0x9CU,
0xB1U, 0xB6U, 0xBFU, 0xB8U, 0xADU, 0xAAU, 0xA3U, 0xA4U,
0xF9U, 0xFEU, 0xF7U, 0xF0U, 0xE5U, 0xE2U, 0xEBU, 0xECU,
0xC1U, 0xC6U, 0xCFU, 0xC8U, 0xDDU, 0xDAU, 0xD3U, 0xD4U,
0x69U, 0x6EU, 0x67U, 0x60U, 0x75U, 0x72U, 0x7BU, 0x7CU,
0x51U, 0x56U, 0x5FU, 0x58U, 0x4DU, 0x4AU, 0x43U, 0x44U,
0x19U, 0x1EU, 0x17U, 0x10U, 0x05U, 0x02U, 0x0BU, 0x0CU,
0x21U, 0x26U, 0x2FU, 0x28U, 0x3DU, 0x3AU, 0x33U, 0x34U,
0x4EU, 0x49U, 0x40U, 0x47U, 0x52U, 0x55U, 0x5CU, 0x5BU,
0x76U, 0x71U, 0x78U, 0x7FU, 0x6AU, 0x6DU, 0x64U, 0x63U,
0x3EU, 0x39U, 0x30U, 0x37U, 0x22U, 0x25U, 0x2CU, 0x2BU,
0x06U, 0x01U, 0x08U, 0x0FU, 0x1AU, 0x1DU, 0x14U, 0x13U,
0xAEU, 0xA9U, 0xA0U, 0xA7U, 0xB2U, 0xB5U, 0xBCU, 0xBBU,
0x96U, 0x91U, 0x98U, 0x9FU, 0x8AU, 0x8DU, 0x84U, 0x83U,
0xDEU, 0xD9U, 0xD0U, 0xD7U, 0xC2U, 0xC5U, 0xCCU, 0xCBU,
0xE6U, 0xE1U, 0xE8U, 0xEFU, 0xFAU, 0xFDU, 0xF4U, 0xF3U
};


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
        if(i2c_error == 1) { // Collision detected
            i2c_destroy(); // Clear all flags
            i2c_own_bus = 0;
        }
        i2c_error = 0;
        __delay_us(I2C_RETRY_DELAY);
        i2c_init(); // Clear all flags
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

void i2c_dbg(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4) {
    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        if (handle_error(0)) continue;
        i2c_own_bus = 1;

        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;

        i2c_Write(b1); //
        if (handle_error(1)) continue;

        i2c_Write(b2); //
        if (handle_error(1)) continue;

        i2c_Write(b3); //
        if (handle_error(1)) continue;

        i2c_Write(b4); //
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

void i2c_command8(unsigned char command) {
    unsigned char current_command;
    unsigned char pec;
    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        pec = 0;
        if (handle_error(0)) continue;
        i2c_own_bus = 1;

        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ I2C_SLAVE];

        i2c_Write(I2C_MYADDR); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ I2C_MYADDR];

        current_command = i2c_command_id++;
        i2c_Write(current_command); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ current_command];

        i2c_Write(command); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ command];

        i2c_Write(pec); //
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
    unsigned char current_command;
    unsigned char pec;

    i2c_retry = I2C_RETRIES;
    i2c_own_bus = 0;
    while (i2c_retry-- > 0) {
        handle_restart();
        pec = 0;
        if (handle_error(0)) continue;
        i2c_own_bus = 1;

        i2c_Address(I2C_SLAVE, I2C_WRITE); // Send slave address - write operation
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ I2C_SLAVE];
        
        i2c_Write(I2C_MYADDR); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ I2C_MYADDR];

        current_command = i2c_command_id++;
        i2c_Write(current_command); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ current_command];

        i2c_Write((command >> 8)); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ (command >> 8)];

        i2c_Write(command); //
        if (handle_error(1)) continue;
        pec = pec_lookup[pec ^ ((unsigned char)command)];
        
        i2c_Write(pec); //
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

// i2c_Wait - wait for I2C transfer to finish
static void i2c_Wait(void) {
    //SSP1CON2: 8:GCEN 7:ACKSTAT 6:ACKDT 5:ACKEN 4:RCEN 3:PEN 2:RSEN 1:SEN
    //SSPSTAT:  8:SMP  7:CKE     6:D/A   5:P     4:S    3:R/W 2:UA   1:BF
    while ((SSPCON2 & 0b00011111) || (SSPSTAT & 0b00000100));
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
    SSPBUF = data;
    check_error();
}

// i2c_Address - Sends Slave Address and Read/Write mode
// mode is either I2C_WRITE or I2C_READ

static void i2c_Address(unsigned char address, unsigned char mode) {
    unsigned char l_address;
    l_address = address << 1;
    l_address += mode;
    i2c_Wait();
    SSPBUF = l_address;
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
