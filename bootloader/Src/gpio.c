/*
 * gpio.c
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */

#include "gpio.h"

void gpio_init()
{
		//allow clock access to GPIOA
		RCC_AHBENR |= RCC_GPIOAEN;

	//set the alternate function mode(10) for GPIOA as we will be using PA2 as TX in GPIOA_AFRL
		GPIOA_MODER &= ~(1U<<4);
		GPIOA_MODER |= (1U<<5);


	//set the alternate function mode for GPIOA as we will be using PA3 as RX in GPIOA_AFRL
		GPIOA_MODER &= ~(1U<<6);
		GPIOA_MODER |= (1U<<7);

	//set alternate function for PA2 (AF7)
			GPIOA_AFRL |= (1U<<8);
			GPIOA_AFRL |= (1U<<9);
			GPIOA_AFRL |= (1U<<10);
			GPIOA_AFRL &= ~(1U<<11);

	//set alternate function for PA3 (AF7)
			GPIOA_AFRL |= (1U<<12);
			GPIOA_AFRL |= (1U<<13);
			GPIOA_AFRL |= (1U<<14);
			GPIOA_AFRL &= ~(1U<<15);
}
void led_delay()
{
	for(volatile int x=0;x<500000;x++);
}

