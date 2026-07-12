/*
 * led.c
 *
 *  Created on: Jul 11, 2026
 *      Author: jenas
 */

#include "led.h"

#define RCC_AHB1ENR (*(volatile uint32_t*) 0x40023830)
#define GPIOA_MODER (*(volatile uint32_t*) 0x40020000)
#define GPIOA_ODR (*(volatile uint32_t*) 0x40020014)


void LED_Init(void) {
    // turn on clock for port A
    RCC_AHB1ENR |= (1 << 0);
    // set PA5 to general purpose output
    GPIOA_MODER &= ~(3 << 10);
    GPIOA_MODER |= (1 << 10);
}

void LED_Toggle(void) {
    // toggle the led
    GPIOA_ODR ^= (1 << 5);
}
