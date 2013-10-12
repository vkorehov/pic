/* 
 * File:   flash.h
 * Author: vlad
 *
 * Created on 12 October 2013, 00:49
 */

#ifndef FLASH_H
#define	FLASH_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned int flash_memory_read (unsigned int address);
void flash_memory_write (unsigned int address, unsigned char *data);
void flash_memory_erase (unsigned int address);


#ifdef	__cplusplus
}
#endif

#endif	/* FLASH_H */

