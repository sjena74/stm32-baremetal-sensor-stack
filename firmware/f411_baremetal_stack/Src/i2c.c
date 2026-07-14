/*
 * practice.c
 *
 *  Created on: Jul 13, 2026
 *      Author: jenas
 */

#include <stdint.h>

/* Configure buses via RCC*/
#define RCC_AHB1ENR (*(volatile uint32_t*)0x40023830)
#define RCC_APB1ENR (*(volatile uint32_t*)0x40023840)

/* Memory Mapped I/O Registers */

// need PB8, PB9 from port B
#define GPIOB_MODER (*(volatile uint32_t*)0x40020400)
#define GPIOB_OTYPER (*(volatile uint32_t*)0x40020404)
#define GPIOB_PUPDR (*(volatile uint32_t*)0x4002040C)
#define GPIOB_AFRH (*(volatile uint32_t*)0x40020424)

/* Configure I2C defines */
// 0x4000 5400 base
#define I2C_CR1 (*(volatile uint32_t*)0x40005400) // ACK
#define I2C_CR2 (*(volatile uint32_t*)0x40005404) // FREQ
#define I2C_DR (*(volatile uint32_t*)0x40005410) // addresses + data sent on
#define I2C_SR1 (*(volatile uint32_t*)0x40005414)
#define I2C_SR2 (*(volatile uint32_t*)0x40005418)
#define I2C_CCR (*(volatile uint32_t*)0x4000541C)
#define I2C_TRISE (*(volatile uint32_t*)0x40005420)

void I2C_INIT(void) {

	RCC_AHB1ENR |= (1 << 1); // set PortB clock enable
	RCC_APB1ENR |= (1 << 21); // set I2C1 clock enable


	GPIOB_MODER &= ~(3 << 16); // clear MODER8
	GPIOB_MODER |= (2 << 16); // set MODER8 to alternate function

	GPIOB_MODER &= ~(3 << 18); //clear MODER 8
	GPIOB_MODER |= (2 << 18);  //set MODER 8 to alternate function

	GPIOB_OTYPER &= ~(1 << 8); // clear type bit 8 (reset)
	GPIOB_OTYPER |= (1 << 8); // set type bit 8 to open-drain

	GPIOB_OTYPER &= ~(1 << 9); // clear type bit 9 (reset)
	GPIOB_OTYPER |= (1 << 9); // set type bit 9 to open-drain

	GPIOB_PUPDR &= ~(3 << 16); // clear PUPDR8
	GPIOB_PUPDR |= (1 << 16); // set PUPDR8 to pull up

	GPIOB_PUPDR &= ~(3 << 18); // clear PUPDR9
	GPIOB_PUPDR |= (1 << 18); // set PUPDR9 to pull up

	GPIOB_AFRH &= ~(15 << 0); // clear AFRH8
	GPIOB_ARFH |= (4 << 0);	  // set pin 8 to SCL

	GPIOB_AFRH &= ~(15 << 4); // clear AFRH9
	GPIOB_AFRH |= (4 << 4);   // set pin 9 to SDA

	/* I2C Configuration */
	// start i2c engine
	I2C_CR1 |= (1 << 15);
	I2C_CR1 &= ~(1 << 15);

	I2C_CR2 |= 16; // set 16Mhz for peripheral clock bus

	I2C_CCR = 80; // calculated from 100KHz high time of 5 microseconds divided by period of peripheral clock bus. 5 / 0.0625 = 80
	I2C_TRISE = 17; // calculated from 100Khz max low-to-high time which is 1ns / 0.0625(period of peripheral clock bus) = 17

	I2C_CR1 |= (1 << 0); // enable peripheral

}


void I2C_WRITE(uint8_t deviceAddr, uint8_t regAddr, uint8_t data) {
	volatile uint32_t temp;

	I2C_CR1 |= (1 << 8); // start generation..pull SDA line low while SCL is high
	while(!(I2C_SR1 & (1 << 0))); // wait until hardware knows bus is transitioned

	I2C_DR = (deviceAddr << 1); // the LSB must be 0 since writing to device
	while(!(I2C_SR1 & (1 << 1))); // wait until address is received and ACK is sent!
	temp = I2C_SR1;
	temp = I2C_SR2; // complete clearing sequence after previous step as told in reference manual

	I2C_DR = regAddr;
	while (!(I2C_SR1 & (1 << 7))); // wait for TxE
	// code up to here is used in both I2C_WRITE and I2C_READ

	I2C_DR = data;
	while (!(I2C_SR1 & (1 << 2))); // wait for byte transfer

	I2C_CR1 |= (1 << 9); // set stop condition
}

uint8_t I2C_READ(uint8 deviceAddr, uint8_t regAddr) {
	volatile uint32_t temp;

	I2C_CR1 |= (1 << 8); // start generation..pull SDA line low while SCL is high
	while(!(I2C_SR1 & (1 << 0))); // wait until hardware knows bus is transitioned

	I2C_DR = (deviceAddr << 1); // the LSB must be 0 since writing to device
	while(!(I2C_SR1 & (1 << 1))); // wait until address is received and ACK is sent!
	temp = I2C_SR1;
	temp = I2C_SR2; // complete clearing sequence after previous step as told in reference manual

	I2C_DR = regAddr;
	while (!(I2C_SR1 & (1 << 2))); // wait for BTF in READ

	I2C_CR1 |= (1 << 8); // Start Condition again
	while (!(I2C_SR1 & (1 << 0))); // same seq

	I2C1_DR = (deviceAddr << 1) | 1; // set to read
	while (!(I2C_SR1 & (1 << 1))); // wait till ACK

	I2C_CR1 &= ~(1 << 10); // ACK enable
	temp = I2C_SR1;
	temp = I2C_SR2;

	I2C_CR1 |= (1 << 9); // Stop
	while (!(I2C_SR1 & (1 << 6))); // wait for data to transmit

	return (uint8_t)I2C_DR;
}
