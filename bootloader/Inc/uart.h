/*
 * uart.h
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "base.h"

#define USART2_SR_OFFSET				(0x00)
#define USART2_SR						(*(volatile unsigned int*)(USART2_PERIPHERAL + USART2_SR_OFFSET))

#define USART2_CR1_OFFSET				(0x0C)
#define USART2_CR1						(*(volatile unsigned int*)(USART2_PERIPHERAL + USART2_CR1_OFFSET))

#define USART2_BRR_OFFSET				(0x08)
#define USART2_BRR						(*(volatile unsigned int*)(USART2_PERIPHERAL + USART2_BRR_OFFSET))

#define USART2_DR_OFFSET				(0x04)
#define USART2_DR						(*(volatile unsigned int*)(USART2_PERIPHERAL + USART2_DR_OFFSET))

#define RCC_CR_OFFSET					(0x00)
#define RCC_CR							(*(volatile unsigned int*)(RCC_PERIPHERAL + RCC_CR_OFFSET))

#define RCC_APB1ENR_OFFSET				(0x24)
#define RCC_APB1ENR						(*(volatile unsigned int*)(RCC_PERIPHERAL + RCC_APB1ENR_OFFSET))

#define RCC_CFGR_OFFSET					(0x08)
#define RCC_CFGR						(*(volatile unsigned int*)(RCC_PERIPHERAL + RCC_CFGR_OFFSET))

#define FLASH_ACR_OFFSET				(0x00)
#define FLASH_ACR						(*(volatile unsigned int*)(FLASH_PERIPHERAL + FLASH_ACR_OFFSET))


/*enable bits*/
#define USART2_SR_TXE			(1U << 7)
#define USART2_SR_RXNE   		(1U << 5)
#define USART2_CR1_UE     		(1U << 13)
#define USART2_CR1_RE     		(1U << 2)
#define USART2_CR1_TE     		(1U << 3)
#define RCC_CR_HSION     		(1U << 0)
#define RCC_CR_HSIRDY    		(1U << 1)
#define RCC_CFGR_SW_MSK     	(3U << 0)   /* SW[1:0]  clock switch select   */
#define RCC_CFGR_SW_HSI     	(1U << 0)   /* 01 = HSI selected as SYSCLK    */
#define RCC_CFGR_SWS_MSK    	(3U << 2)   /* SWS[1:0] switch status         */
#define RCC_CFGR_SWS_HSI    	(1U << 2)
#define RCC_CFGR_HPRE_MSK    	(15U << 4)   /* AHB prescaler  bits [7:4]  */
#define RCC_CFGR_PPRE1_MSK   	(7U << 8)   /* APB1 prescaler bits [10:8] */
#define RCC_CFGR_PPRE2_MSK   	(7U << 11)  /* APB2 prescaler bits [13:11]*/
#define FLASH_ACR_LATENCY   	(1U << 0)   /* 0 = 0 wait state, 1 = 1 wait state */
#define FLASH_ACR_PRFTEN		(1U << 1)
#define FLASH_ACR_ACC64			(1U << 2)

/*variable declarations*/
//volatile uint32_t uart_overruns; //how many overruns appear
//volatile uint32_t uart_bytes_received;
//volatile uint32_t first_overrun_at;

/*function declarations*/
void uart_init(void);
void uart_write(int ch);
uint8_t uart_read_byte(void);
uint32_t uart_read_word(void);
void Systemclkconfig(void);
//void usart2_set_baudrate(uint32_t peripheral_clock,uint32_t baudrate);

#endif /* UART_H_ */
