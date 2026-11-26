/*
 * gpio.c
 *
 *  Created on: Nov 21, 2026
 *      Author: DELL
 */



#include "gpio.h"

/*enable bits*/
#define RCC_GPIOAEN						(1U<<0)

#define LED_PIN							5U

#define LOOPS_PER_MS					1333U

void gpio_init(void)
{
	RCC_AHBENR |= RCC_GPIOAEN;

	//clear the two  bits ie 10,11 of the GPIOA_MODER reg
	GPIOA_MODER &= ~(3U << (LED_PIN * 2U));

	//Set the two bits ie 10,11 to General purpose output mode
	GPIOA_MODER |= (1U << (LED_PIN * 2U));


}

void led_on(void)
{
	GPIOA_ODR |= (1U << LED_PIN);
}

void led_off(void)
{
	GPIOA_ODR &= ~(1U << LED_PIN);
}

void led_delay(uint32_t ms)
{
	for (uint32_t m = 0;m < ms;m++)
	{
		for(volatile uint32_t i = 0; i < LOOPS_PER_MS; i++)
		{

		}
	}
}
