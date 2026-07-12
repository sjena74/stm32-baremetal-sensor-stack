/*
 * i2c.c
 *
 *  Created on: Jul 11, 2026
 *      Author: jenas
 */

#include "i2c.h"

/* Memory Registers */

/* Buses */
#define RCC_APB1ENR (*(volatile uint32_t*)0x40023840)
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830)

/* GPIO */
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_OTYPER (*(volatile uint32_t*)0x40020404)
#define GPIOB_PUPDR (*(volatile uint32_t*)0x4002040C)
#define GPIOx_AFRH (*(volatile uint32_t*)0x40020024)


/* I2C1 Configuration */



void I2C1_INIT(void) {
	/* Initialize buses */
	RCC_APB1ENR |= (1 << 21);
	RCC_AHB1ENR |= (1 << 1);

	/* Initialize GPIO */
	// PB8 = SCL, PB9 = SDA
	GPIOB_MODER &= ~((3 << 16) | (3 << 18)); // clear pins 8/9
	GPIOB_MODER |= ~((2 << 16) | (2 << 18)); // set pins 8/9 for alternate function

	GPIOB_OTYPER |= ((1 << 8) | (1 << 9)); // type is open-drain

	GPIOB_PUPDR &= ~((3 << 16) | (3 << 18)); // set both pins to be pull-up
	GPIOB_PUPDR &= ~((1 << 16) | (1 << 18));

	GPIOx_AFRH &= ~((15 << 0) | (15 << 4)); // set both pins to be Alternate Function 04, for I2C1 clock/data lines
	GPIOx_AFRH |= ((4 << 0) | (4 << 4));




}
