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
// slave address definition
#define SLAVE_ADDR 0x37
    
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



extern ADDRESS	_bl_flash_addr_pointer;
extern unsigned char _bl_flash_buffer[];
extern unsigned char _bl_pksa_wd_address;
extern unsigned int _bl_timeout;

// function prototypes
void _bl_do_i2c_tasks(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

