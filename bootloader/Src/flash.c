/*
 * flash.c
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */
#include "uart.h"
#include "flash.h"

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

//variable  declaration
uint32_t byte_per_page = 256;

/* ============================================================
 * Flash unlock / lock / erase / half-page write
 * ============================================================ */
void flash_unlock_all(void)
{
    if (FLASH_PECR & FLASH_PECR_PELOCK) {
        FLASH_PEKEYR = PEKEY1;
        FLASH_PEKEYR = PEKEY2;
    }
    if (FLASH_PECR & FLASH_PECR_PRGLOCK) {
        FLASH_PRGKEYR = PRGKEY1;
        FLASH_PRGKEYR = PRGKEY2;
    }
}

void flash_lock_all(void)
{
    FLASH_PECR |= FLASH_PECR_PRGLOCK;
    FLASH_PECR |= FLASH_PECR_PELOCK;
}

void flash_erase_page(uint32_t page_address)
{
    FLASH_PECR |= FLASH_PECR_ERASE;
    FLASH_PECR |= FLASH_PECR_PROG;

    *(volatile uint32_t *)page_address = 0x00000000UL;  /* trigger */

    while (FLASH_SR & FLASH_SR_BSY);

    FLASH_PECR &= ~FLASH_PECR_ERASE;
    FLASH_PECR &= ~FLASH_PECR_PROG;
}

/*void flash_write_halfpage(uint32_t address, uint32_t words[WORDS_PER_HALFPAGE])
{
    FLASH_PECR |= FLASH_PECR_FPRG;
    FLASH_PECR |= FLASH_PECR_PROG;

    while (FLASH_SR & FLASH_SR_BSY);

    for (uint32_t i = 0; i < WORDS_PER_HALFPAGE; i++) {
        *(volatile uint32_t *)(address + i * 4U) = words[i];
    }

    while (FLASH_SR & FLASH_SR_BSY);

    FLASH_PECR &= ~FLASH_PECR_FPRG;
    FLASH_PECR &= ~FLASH_PECR_PROG;
}*/

void flash_write_word(uint32_t address, uint32_t data)
{
    while (FLASH_SR & FLASH_SR_BSY);          /* 1. wait until the flash is idle   */

    *(volatile uint32_t *)address = data;     /* 2. write ONE word                 */

    while (FLASH_SR & FLASH_SR_BSY);          /* 3. wait until it is really stored */
}

/* ============================================================
 * Receive a 32-bit "file size" header, then the firmware bytes,
 * erasing and writing it into Flash as it arrives.
 * ============================================================ */

//uint32_t debug_capture[WORDS_PER_HALFPAGE];

  void receive_and_flash_application(void)
    {
         uint32_t write_addr    = APP_ADDRESS;
         uint32_t total_bytes   = uart_read_word();                     /* e.g. 756        */
         uint32_t word_needed   = total_bytes / 4U;                     /* 189             */
         uint32_t word_per_page = byte_per_page / 4U;                   /* 64              */
         uint32_t pages_needed  = word_needed / word_per_page;          /* 2 full pages    */
         uint32_t remainder     = word_needed - word_per_page * pages_needed;   /* 61     */

     /* 1. erase every page the application touches (rounded up) */
         uint32_t pages_to_erase = (total_bytes + 255U) / 256U;         /* 3               */
         for (uint32_t e = 0; e < pages_to_erase; e++) {
               flash_erase_page(write_addr + e * 256U);
                    }
         uart_write('A');                                     /* (1) erase done            */

        /* 2. full pages: 64 words each */
         uint32_t p = 0;
         while (p < pages_needed) {
             for (uint32_t i = 0; i < word_per_page; i++) {
                   uint32_t c = uart_read_word();               /* listen: one word          */
                   flash_write_word(write_addr + p * 256U + i * 4U, c);   /* write it        */
                   uart_write('A');                             /* (2) ready for the next    */
                       }
                   p++;
                       }
                     write_addr += 256U * p;                              /* -> 0x0800 4200            */

         /* 3. remaining words, in the last (partly filled) page */
              for (uint32_t i = 0; i < remainder; i++) {
                   flash_write_word(write_addr + i * 4U, uart_read_word());
                   uart_write('A');                                 /* (3) ready for the next    */
                            }
                        }
/* ============================================================
 * Manual jump into the application
 * ============================================================ */

//typedef void (*app_entry_t)(void);
void jump_to_application(void)
{
    uint32_t app_stack = *(volatile uint32_t *)(APP_ADDRESS + 0);
    uint32_t app_reset  = *(volatile uint32_t *)(APP_ADDRESS + 4);

    /* Relocate vector table so interrupts (if the app ever uses any)
       resolve correctly against the application's own vector table.
       Not strictly required for a purely polling-based application,
       but included as correct, defensive practice. */
    VTOR = APP_ADDRESS;

    __asm volatile ("MSR MSP, %0" : : "r" (app_stack));

    app_entry_t app_reset_handler = (app_entry_t)app_reset;
    app_reset_handler();
}
