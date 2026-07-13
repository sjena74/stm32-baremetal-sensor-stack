/*
 * i2c.h
 *
 *  Created on: Jul 11, 2026
 *      Author: jenas
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
/* initialize peripheral and PB6, PB7 pins */
void I2C_INIT(void);
/* Read from register */
uint8_t I2C_Read(uint8_t deviceAddress, uint8_t registerAddress);
/* Write to register */
void I2C_Write(uint8_t deviceAddress, uint8_t registerAddress, uint8_t data);

#endif /* I2C_H_ */
