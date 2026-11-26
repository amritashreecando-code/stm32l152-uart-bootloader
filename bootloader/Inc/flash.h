/*
 * flash.h
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

#include "base.h"

#define FLASH_PECR_OFFSET   		        0x04
#define FLASH_PECR							(*(volatile unsigned int*)(FLASH_PERIPHERAL + FLASH_PECR_OFFSET))

#define FLASH_PEKEYR_OFFSET 				0x0C
#define FLASH_PEKEYR  						(*(volatile unsigned int*)(FLASH_PERIPHERAL + FLASH_PEKEYR_OFFSET))

#define FLASH_PRGKEYR_OFFSET 				0x10
#define FLASH_PRGKEYR 						(*(volatile unsigned int*)(FLASH_PERIPHERAL + FLASH_PRGKEYR_OFFSET))

#define FLASH_SR_OFFSET     				0x18
#define FLASH_SR							(*(volatile unsigned int*)(FLASH_PERIPHERAL + FLASH_SR_OFFSET))

#define VTOR                				(*(volatile unsigned int*)0xE000ED08UL)

/*Enable bits*/
#define FLASH_SR_BSY        				(1 << 0)
#define FLASH_PECR_PELOCK   				(1 << 0)
#define FLASH_PECR_PRGLOCK  				(1 << 1)
#define FLASH_PECR_PROG     				(1 << 3)
#define FLASH_PECR_ERASE    				(1 << 9)
#define FLASH_PECR_FPRG     				(1 << 10)

/* Unlock key values — verified against RM0038 */
#define PEKEY1   							0x89ABCDEFUL
#define PEKEY2   							0x02030405UL
#define PRGKEY1  							0x8C9DAEBFUL
#define PRGKEY2  							0x13141516UL

/* ---------------- Bootloader constants ---------------- */
#define APP_ADDRESS        					0x08004000UL   /* application start address        */
#define WORDS_PER_HALFPAGE 					32U            /* 32 words = 128 bytes, per RM0038 */

/*function declarations*/
void flash_unlock_all(void);
void flash_lock_all(void);
void flash_erase_page(uint32_t page_address);
//void flash_write_halfpage(uint32_t address, uint32_t words[WORDS_PER_HALFPAGE]);
void flash_write_word(uint32_t address, uint32_t data);
void receive_and_flash_application(void);
typedef void (*app_entry_t)(void);
void jump_to_application(void);


#endif /* FLASH_H_ */
