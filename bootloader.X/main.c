/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "flash.h"         
#include "i2c.h"


/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/


unsigned char flash_buffer[16];

unsigned char pksa_wd_address;
unsigned char pksa_index;
unsigned char pksa_status;
ADDRESS	flash_addr_pointer;

void init ()
{
	TRISC   = 0b00011000; //RC3(p14):SCL RC4(p15):SDA
}

void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();
    init();
    i2c_init();

//	goto App;
//	// if we have any application loaded, jump to it
//	if (  flash_memory_read(0x1FFF)  == 0x3455)
//	{
//		#asm
//			goto 0x200;
//		#endasm
//	}
//App:

		// main program loop
		while (1)
		{
			do_i2c_tasks();
		}
}