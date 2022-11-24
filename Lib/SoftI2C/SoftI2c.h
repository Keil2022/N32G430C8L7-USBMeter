/*
 * SoftI2c.h
 *
 *      Author: Geralt
 */

#ifndef SOFTI2C_SOFTI2C_H_
#define SOFTI2C_SOFTI2C_H_

#include "main.h"
#include "bsp_delay.h"

#define SOFTI2C_SCL_GPIO_PORT  GPIOA
#define SOFTI2C_SCL_GPIO_PIN   GPIO_PIN_4
#define SOFTI2C_SDA_GPIO_PORT  GPIOA
#define SOFTI2C_SDA_GPIO_PIN   GPIO_PIN_5

#define SOFTI2C_FAST_MODE      1

typedef enum{
	I2C_OK,
	I2C_NO_ACK,
}SoftI2cStatus;

SoftI2cStatus SoftI2c_Init(void);
SoftI2cStatus SoftI2c_WriteByte(uint8_t address, uint8_t data);
SoftI2cStatus SoftI2c_ReadByte(uint8_t address, uint8_t* data);
SoftI2cStatus SoftI2c_MemWrite(uint8_t address, uint8_t* reg, uint8_t regSize, uint8_t* data, uint16_t dataSize);
SoftI2cStatus SoftI2c_MemRead(uint8_t address, uint8_t* reg, uint8_t regSize, uint8_t* data, uint16_t dataSize);


#endif /* SOFTI2C_SOFTI2C_H_ */
