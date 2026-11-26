/*
 * base.h
 *
 *  Created on: Nov 21, 2026
 *      Author: DELL
 */

#ifndef BASE_H_
#define BASE_H_

#define PERIPHERAL_BASE						(0x40000000)

#define AHB_PERIPHERAL_OFFSET				(0x00020000)
#define AHB_PERIPHERAL						(PERIPHERAL_BASE + AHB_PERIPHERAL_OFFSET)

#define GPIOA_PERIPHERAL_OFFSET				(0x00000000)
#define GPIOA_PERIPHERAL					(AHB_PERIPHERAL + GPIOA_PERIPHERAL_OFFSET)

#define RCC_PERIPHERAL_OFFSET				(0x00003800)
#define RCC_PERIPHERAL						(AHB_PERIPHERAL + RCC_PERIPHERAL_OFFSET)








#endif /* BASE_H_ */
