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
#define I2C1_CR1 (*(volatile uint32_t*)0x40005400)
#define I2C1_CR2 (*(volatile uint32_t*)0x40005404)
#define I2C1_CCR (*(volatile uint32_t*)0x4000541C)
#define I2C1_TRISE (*(volatile uint32_t*)0x40005420)
#define I2C1_SR1 (*(volatile uint32_t*)0x40005414)
#define I2C1_DR (*(volatile uint32_t*)0x40005410)
#define I2C1_SR2 (*(volatile uint32_t*)0x40005418)





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

	/* Initialize I2C Engine*/
	I2C_CR1 |= (1 << 15);
	I2C_CR1 &= ~(1 << 15); // software reset the I2C

	I2C_CR1 = (1 << 4); // set to 16 MHz

	// 100 KHz bus means 10 microsecond period. High time is .5(10) = 5 microseconds since duty cycle is 50/50.
	// peripheral clock period is 16 MHz which means 0.0625 microsecond period. CCR value = high time / peripheral period = 5 microseconds / 0.0625 microseconds = 80.
	I2C1_CCR = 80;

	// TRISE = (Max rise time / peripheral clock period) + 1 = (1 microsecond / 0.0625 microsecond) = 17.
	I2C_TRISE = 17;

	/* Set enable bit */
	I2C_CR1 |= (1 << 0);


uint8_t I2C_Read(uint8_t deviceAddr, uint8_t regAddr) {
	volatile uint32_t temp; // to clear flags, temp must be used as a dump.

	I2C_CR1 |= (1 << 8); // Start Condition
	while (!(I2C1_SR1 & (1 << 0))); // set when start condition is sent

	I2C1_DR = deviceAddr; // push sensor address onto DR and send

	while (!(I2C_SR1 & (1 << 1)));
	temp = I2C1_SR1;
	temp = I2C1_SR2;

	I2C1_DR = regAddr;
	while (!(I2C1_SR1 & (1 << 7)));

	I2C1_CR1 |= (1 << 8);
	while (!(I2C1_SR1 & (1 << 0)));

	I2C1_DR = deviceAddr | 1; // set to read
	while (!(I2C1_SR1 & (1 << 1)));

	I2C1_CR1 &= ~(1 << 10); // ACK enable
	temp = I2C1_SR1;
	temp = I2C1_SR2;

	I2C1_CR1 |= (1 << 9); // Stop
	while (!(I2C1_SR1 & (1 << 6))); // wait for data to transmit

	return (uint8_t)I2C1_DR;
}


void I2C_Write(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
	volatile uint32_t temp; // to clear flags, temp must be used as dump.
	// follow same setup as Read for deviceAddr and regAddr
	I2C_CR1 |= (1 << 8); /
	while (!(I2C1_SR1 & (1 << 0)));

	I2C1_DR = deviceAddr;

	while (!(I2C_SR1 & (1 << 1)));
	temp = I2C1_SR1;
	temp = I2C1_SR2;

	I2C1_DR = regAddr;
	while (!(I2C1_SR1 & (1 << 7)));

	I2C1_DR = data; // send data on DR
	while (!(I2C1_SR1 & (1 << 2))); // wait for byte transfer

	I2C1_CR1 |= (1 << 9); // stop condition

}







}
