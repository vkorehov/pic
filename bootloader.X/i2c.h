/* 
 * File:   i2c.h
 * Author: vlad
 *
 * Created on 12 October 2013, 00:50
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

    #ifndef _ADDRESS_TYPEDEF
#define _ADDRESS_TYPEDEF


typedef    struct
    {
        unsigned char byte0;
        unsigned char byte1;
    }int16;
typedef union _ADDRESS
{
	struct
	{
		int address;
	}word;
	struct
	{
        unsigned char byte_L;
        unsigned char byte_H;

 	}bytes;
}  ADDRESS;


#endif

// pickit serial communication states
#define I2C_NO_TRANSACTION					0
#define I2C_SLAVE_ADDRESS_RECEIVED			1
#define I2C_WORD_ADDRESS_RECEIVED			2
#define I2C_READ_ADDRESS					3
#define I2C_READ_DATA						4
#define I2C_MASTER_NACK						5

// slave address definition
#ifdef _12F1840
#define SLAVE_ADDR 0x11
#else
#define SLAVE_ADDR 0x10
#endif
// address typdef
extern ADDRESS	flash_addr_pointer;


// function prototypes
unsigned char  do_i2c_tasks(void);

// externs
extern unsigned char flash_buffer[];
extern unsigned char pksa_index;
extern unsigned char pksa_wd_address;
extern unsigned char pksa_status;
extern unsigned int timeout;

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

