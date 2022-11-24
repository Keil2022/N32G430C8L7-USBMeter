/*
 * SoftI2c.c
 *
 *  Created on: 2021年3月16日
 *      Author: Geralt
 */
#include "SoftI2c.h"

#define SetSCL()     (SOFTI2C_SCL_GPIO_PORT->PBSC = SOFTI2C_SCL_GPIO_PIN)
#define SetSDA()     (SOFTI2C_SDA_GPIO_PORT->PBSC = SOFTI2C_SDA_GPIO_PIN)
#define ResetSCL()   (SOFTI2C_SCL_GPIO_PORT->PBC  = SOFTI2C_SCL_GPIO_PIN)
#define ResetSDA()   (SOFTI2C_SDA_GPIO_PORT->PBC  = SOFTI2C_SDA_GPIO_PIN)
#define ReadSCL()    ((SOFTI2C_SCL_GPIO_PORT->PID) & SOFTI2C_SCL_GPIO_PIN)
#define ReadSDA()    ((SOFTI2C_SDA_GPIO_PORT->PID) & SOFTI2C_SDA_GPIO_PIN)

#if SOFTI2C_FAST_MODE
#define I2C_Delay()  {__NOP();__NOP();__NOP();}
#else
#define I2C_Delay()  SysTick_Delay_Us(5)
#endif

#define DelayMicroseconds(x) do{int loops = x; while(loops--) __NOP();}while(0)


void I2C_Init() {
	SetSCL();
	I2C_Delay();
	SetSDA();
	I2C_Delay();
}

/*
函数：I2C_Start()
功能：产生I2C总线的起始状态
说明：
 SCL处于高电平期间，当SDA出现下降沿时启动I2C总线
 不论SDA和SCL处于什么电平状态，本函数总能正确产生起始状态
 本函数也可以用来产生重复起始状态
 本函数执行后，I2C总线处于忙状态
*/
void I2C_Start() {
	SetSCL();
	I2C_Delay();
	SetSDA();
	DelayMicroseconds(5);
	ResetSDA();  //发送起始信号
	I2C_Delay();
	ResetSCL();  //钳住I2C总线，准备发送或接收数据
	DelayMicroseconds(20);
}

/*
函数：I2C_Stop()
功能：产生I2C总线的停止状态
说明：
 SCL处于高电平期间，当SDA出现上升沿时停止I2C总线
 不论SDA和SCL处于什么电平状态，本函数总能正确产生停止状态
 本函数执行后，I2C总线处于空闲状态
*/
void I2C_Stop() {

	ResetSDA();  //发送结束条件的数据信号
	I2C_Delay();

	SetSCL();    //发送结束条件的时钟信号
	I2C_Delay();
	SetSDA();    //发送I2C总线结束信号
	I2C_Delay();
	DelayMicroseconds(20);
}

/*
 函数：I2C_GetAck()
 功能：读取从机应答位
 返回：
 0：从机应答
 1：从机非应答
 说明：
 从机在收到每个字节的数据后，要产生应答位
 从机在收到最后1个字节的数据后，一般要产生非应答位
 */
uint8_t I2C_GetAck() {
	uint8_t ack;
	uint8_t Error_time = 255;

	I2C_Delay();
	SetSDA(); /*8位发送完后释放数据线，准备接收应答位 释放总线*/
	I2C_Delay();
	SetSCL(); /*接受数据*/
	I2C_Delay();
	do {
		ack = ReadSDA();
		Error_time--;
		if (Error_time == 0) {
			ResetSCL();
			I2C_Delay();
			return 1;
		}
	} while (ack);   //判断是否接收到应答信号

	ResetSCL();  //清时钟线，钳住I2C总线以便继续接收
	DelayMicroseconds(20);
	return 0;
}

/*
 函数：I2C_PutAck()
 功能：主机产生应答位或非应答位
 参数：
 ack=0：主机产生应答位
 ack=1：主机产生非应答位
 说明：
 主机在接收完每一个字节的数据后，都应当产生应答位
 主机在接收完最后一个字节的数据后，应当产生非应答位
 */
void I2C_PutAck(uint8_t ack) {

	ack ? SetSDA() : ResetSDA();
	I2C_Delay();
	SetSCL();   //应答
	I2C_Delay();

	ResetSCL();  //清时钟线，钳住I2C总线以便继续接收  ，继续占用
	DelayMicroseconds(20);
}

/*
函数：I2C_Write()
功能：向I2C总线写1个字节的数据
参数：
 dat：要写到总线上的数据
*/
void I2C_Write(uint8_t dat) {
	for (uint8_t t = 0; t < 8; t++) {
		(dat & 0x80) ? SetSDA() : ResetSDA();
		I2C_Delay();
		SetSCL();  //置时钟线为高，通知被控器开始接收数据位
		I2C_Delay();
		ResetSCL();
		I2C_Delay();
		dat <<= 1;
	}
}

/*
 函数：I2C_Read()
 功能：从从机读取1个字节的数据
 返回：读取的一个字节数据
 */
uint8_t I2C_Read() {
	uint8_t dat = 0;
	I2C_Delay();
	I2C_Delay();
	SetSDA(); //在读取数据之前，要把SDA拉高

	I2C_Delay();

	for (uint8_t t = 0; t < 8; t++) {
		ResetSCL();   //
		I2C_Delay();
		SetSCL();     // 置时钟线为高使数据线上升沿数据有效
		I2C_Delay();
		dat <<= 1;
		if(ReadSDA()) dat += 1;
	}
	ResetSCL();
	I2C_Delay();
	return dat;
}

SoftI2cStatus SoftI2c_Init(void);
SoftI2cStatus SoftI2c_WriteByte(uint8_t address, uint8_t data);
SoftI2cStatus SoftI2c_ReadByte(uint8_t address, uint8_t* data);

SoftI2cStatus SoftI2c_MemWrite(uint8_t address, uint8_t* reg, uint8_t regSize, uint8_t* data, uint16_t dataSize) {
	// 确保地址最后一位是0
	address &= 0xFE;
	// 发出启动信号
	I2C_Start();
	// 发送从机地址
	I2C_Write(address);
	// 等待ACK回应
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}
	// 发送寄存器地址
	while(regSize--){
		I2C_Write(*reg++);
		if (I2C_GetAck()) {
			I2C_Stop();
			return I2C_NO_ACK;
		}
	}
	// 发送数据
	while(dataSize--){
		I2C_Write(*data++);
		if (I2C_GetAck()) {
			I2C_Stop();
			return I2C_NO_ACK;
		}
	}

	return I2C_OK;
}

SoftI2cStatus SoftI2c_MemRead(uint8_t address, uint8_t* reg, uint8_t regSize, uint8_t* data, uint16_t dataSize) {
	// 确保地址最后一位是0
	address &= 0xFE;
	// 发出启动信号
	I2C_Start();
	// 发送从机地址
	I2C_Write(address);
	// 等待ACK回应
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}
	// 发送寄存器地址
	while (regSize--) {
		I2C_Write(*reg++);
		if (I2C_GetAck()) {
			I2C_Stop();
			return I2C_NO_ACK;
		}
	}
	// 发送重复起始条件
	address |= 0x01;
	I2C_Start();
	I2C_Write(address);
	// 等待ACK回应
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}

	//接收数据
	while(1) {
		*data++ = I2C_Read();
		if (--dataSize == 0) {
			I2C_PutAck(1);
			break;
		}
		I2C_PutAck(0);
	}

	I2C_Stop();
	return I2C_OK;
}
