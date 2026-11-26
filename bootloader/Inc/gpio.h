/*
 * gpio.h
 *
 *  Created on: Aug 26, 2026
 *      Author: DELL
 */

#ifndef GPIO_H_
#define GPIO_H_



#include "base.h"

/* ---------------- GPIOA (PA2=TX, PA3=RX) ---------------- */

#define GPIOA_MODER_OFFSET			(0x00)
#define GPIOA_MODER					(*(volatile unsigned int*)(GPIOA_PERIPHERAL + GPIOA_MODER_OFFSET))

#define GPIOA_AFRL_OFFSET			(0x20)
#define GPIOA_AFRL					(*(volatile unsigned int*)(GPIOA_PERIPHERAL + GPIOA_AFRL_OFFSET))

#define RCC_AHBENR_OFFSET			(0x1C)
#define RCC_AHBENR					(*(volatile unsigned int*)(RCC_PERIPHERAL + RCC_AHBENR_OFFSET))

/*enable bits*/
#define RCC_GPIOAEN						(1U<<0)

/*function declarations*/
void gpio_init(void);
void led_delay(void);



#endif /* GPIO_H_ */
