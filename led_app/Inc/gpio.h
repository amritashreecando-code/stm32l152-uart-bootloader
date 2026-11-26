/*
 * gpio.h
 *
 *  Created on: Nov 21, 2026
 *      Author: DELL
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

#include "base.h"

#define GPIOA_MODER_OFFSET			(0x00)
#define GPIOA_MODER					(*(volatile uint32_t*)(GPIOA_PERIPHERAL + GPIOA_MODER_OFFSET))

#define GPIOA_ODR_OFFSET			(0x14)
#define GPIOA_ODR					(*(volatile uint32_t*)(GPIOA_PERIPHERAL + GPIOA_ODR_OFFSET))

#define RCC_AHBENR_OFFSET			(0x1C)
#define RCC_AHBENR					(*(volatile uint32_t*)(RCC_PERIPHERAL + RCC_AHBENR_OFFSET))

//function prototypes
void gpio_init(void);
void led_on(void);
void led_off(void);
void led_delay(uint32_t ms);






#endif /* GPIO_H_ */
