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

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

// The bootloader code does not use any interrupts.
// However, the downloaded code may use interrupts.
// The interrupt vector on a PIC16F1937 is located at
// address 0x0004. The following function will be located
// at the interrupt vector and will contain a jump to
// 0x0204
void interrupt serrvice_isr()
{
	#asm
		GOTO	0x204;
	#endasm
}
