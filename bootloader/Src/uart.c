/*
 * uart.c
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */

#include "uart.h"

/*enable bits*/
#define USART2_SR_TXE			(1U << 7)
#define USART2_SR_RXNE   		(1U << 5)
#define USART2_SR_ORE			(1U << 3)
#define USART2_CR1_UE     		(1U << 13)
#define USART2_CR1_RE     		(1U << 2)
#define USART2_CR1_TE     		(1U << 3)
#define RCC_APB1ENR_USART2EN	(1U << 17)
#define RCC_CR_HSION     		(1U << 0)
#define RCC_CR_HSIRDY    		(1U << 1)
#define RCC_CFGR_SW_MSK     	(3U << 0)   /* SW[1:0]  clock switch select   */
#define RCC_CFGR_SW_HSI     	(1U << 0)   /* 01 = HSI selected as SYSCLK    */
#define RCC_CFGR_SWS_MSK    	(3U << 2)   /* SWS[1:0] switch status         */
#define RCC_CFGR_SWS_HSI    	(1U << 2)
#define RCC_CFGR_HPRE_MSK    	(15U << 4)   /* AHB prescaler  bits [7:4]  */
#define RCC_CFGR_PPRE1_MSK   	(7U << 8)   /* APB1 prescaler bits [10:8] */
#define RCC_CFGR_PPRE2_MSK   	(7U << 11)  /* APB2 prescaler bits [13:11]*/
//#define FLASH_ACR_LATENCY   	(1 << 0)   /* 0 = 0 wait state, 1 = 1 wait state */


#define SYS_CLK								16000000
//define baud_rate							115200
#define baud_rate							9600

volatile uint32_t uart_overruns = 0;
volatile uint32_t uart_bytes_received = 0;
volatile uint32_t first_overrun_at = 0;

/*void Systemclkconfig()
{
    // 1. Turn HSI on, wait until it's stable
    //RCC_CR |= RCC_CR_HSION;
    //while (!(RCC_CR & RCC_CR_HSIRDY));

   //  2. Set Flash latency to 0 wait states — correct for HSI's 16 MHz,
          since 0 wait states is valid up to 16 MHz on this chip (per RM0038,
          verify the exact voltage-range/frequency table for your setup) */
    //FLASH_ACR &= ~FLASH_ACR_LATENCY;

    /* 3. Clear AHB/APB1/APB2 prescalers -> all three = "no division",
          i.e. HCLK = PCLK1 = PCLK2 = SYSCLK (whatever SYSCLK ends up being) */
    //RCC_CFGR &= ~RCC_CFGR_HPRE_MSK;
    //RCC_CFGR &= ~RCC_CFGR_PPRE1_MSK;
    //RCC_CFGR &= ~RCC_CFGR_PPRE2_MSK;

    /* 4. Select HSI as the SYSCLK source */
    //RCC_CFGR &= ~RCC_CFGR_SW_MSK;
    //RCC_CFGR |=  RCC_CFGR_SW_HSI;

    /* 5. Wait until hardware confirms HSI is actually driving SYSCLK now */
    //while ((RCC_CFGR & RCC_CFGR_SWS_MSK) != RCC_CFGR_SWS_HSI);
//}


void Systemclkconfig(void)
{
    /* 1. Flash: 1 wait state for 16 MHz in range 2 (RM0038 Table 13).
          Must be done BEFORE switching to the faster clock. */
    FLASH_ACR |= FLASH_ACR_ACC64;                 /* 64-bit access, on its own   */
    while (!(FLASH_ACR & FLASH_ACR_ACC64));       /* wait until accepted         */
    FLASH_ACR |= FLASH_ACR_PRFTEN;                /* prefetch on                 */
    FLASH_ACR |= FLASH_ACR_LATENCY;               /* 1 wait state                */
    while (!(FLASH_ACR & FLASH_ACR_LATENCY));     /* wait until accepted         */

    /* 2. HSI on, wait until stable */
    RCC_CR |= RCC_CR_HSION;
    while (!(RCC_CR & RCC_CR_HSIRDY));

    /* 3. AHB, APB1, APB2 prescalers = divide by 1 */
    RCC_CFGR &= ~RCC_CFGR_HPRE_MSK;
    RCC_CFGR &= ~RCC_CFGR_PPRE1_MSK;
    RCC_CFGR &= ~RCC_CFGR_PPRE2_MSK;

    /* 4. Select HSI as SYSCLK, wait until it is really in use */
    RCC_CFGR &= ~RCC_CFGR_SW_MSK;
    RCC_CFGR |=  RCC_CFGR_SW_HSI;
    while ((RCC_CFGR & RCC_CFGR_SWS_MSK) != RCC_CFGR_SWS_HSI);
}






void uart_init()
{
	//allow clock access to USART2 peripheral
	RCC_APB1ENR	|= RCC_APB1ENR_USART2EN;


	//set USART2 alternate function in gpio.c
	//AF7,PA2 = USART2_TX,PA3 = USART2_RX


	//configure BRR
	//usart2_set_baudrate(SYS_CLK,baud_rate);

	USART2_BRR = 0x0683;               /* baud rate divisor for system clock */

	//enable TX and RX transfer
	USART2_CR1 |= USART2_CR1_RE;
	USART2_CR1 |= USART2_CR1_TE;

	//enable USART2 enable in CR1
	USART2_CR1 |= USART2_CR1_UE;
}

//uint8_t uart_read_byte(void)
//{
  //  while (!(USART2_SR & USART2_SR_RXNE));   /* wait for RXNE flag */
   // return (uint8_t)(USART2_DR);
//}
uint8_t uart_read_byte()
{
	uint32_t sr;
	do
	{
		sr = USART2_SR;
		if(sr & USART2_SR_ORE)
		{
			uart_overruns++;
			if(first_overrun_at == 0)
			{
				first_overrun_at = uart_bytes_received + 1;
			}
		}
	}while(!(sr & USART2_SR_RXNE));

	uart_bytes_received++;
	return (uint8_t)(USART2_DR);
}

uint32_t uart_read_word(void)
{
    uint32_t b0 = uart_read_byte();
    uint32_t b1 = uart_read_byte();
    uint32_t b2 = uart_read_byte();
    uint32_t b3 = uart_read_byte();
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}
void uart_write(int ch)
{
	while(!(USART2_SR & USART2_SR_TXE))
		;
	  USART2_DR = (ch & 0xFF);
}

