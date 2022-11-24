/*
 * SoftI2c.c
 *
 *  Created on: 2021��3��16��
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
������I2C_Start()
���ܣ�����I2C���ߵ���ʼ״̬
˵����
 SCL���ڸߵ�ƽ�ڼ䣬��SDA�����½���ʱ����I2C����
 ����SDA��SCL����ʲô��ƽ״̬��������������ȷ������ʼ״̬
 ������Ҳ�������������ظ���ʼ״̬
 ������ִ�к�I2C���ߴ���æ״̬
*/
void I2C_Start() {
	SetSCL();
	I2C_Delay();
	SetSDA();
	DelayMicroseconds(5);
	ResetSDA();  //������ʼ�ź�
	I2C_Delay();
	ResetSCL();  //ǯסI2C���ߣ�׼�����ͻ��������
	DelayMicroseconds(20);
}

/*
������I2C_Stop()
���ܣ�����I2C���ߵ�ֹͣ״̬
˵����
 SCL���ڸߵ�ƽ�ڼ䣬��SDA����������ʱֹͣI2C����
 ����SDA��SCL����ʲô��ƽ״̬��������������ȷ����ֹͣ״̬
 ������ִ�к�I2C���ߴ��ڿ���״̬
*/
void I2C_Stop() {

	ResetSDA();  //���ͽ��������������ź�
	I2C_Delay();

	SetSCL();    //���ͽ���������ʱ���ź�
	I2C_Delay();
	SetSDA();    //����I2C���߽����ź�
	I2C_Delay();
	DelayMicroseconds(20);
}

/*
 ������I2C_GetAck()
 ���ܣ���ȡ�ӻ�Ӧ��λ
 ���أ�
 0���ӻ�Ӧ��
 1���ӻ���Ӧ��
 ˵����
 �ӻ����յ�ÿ���ֽڵ����ݺ�Ҫ����Ӧ��λ
 �ӻ����յ����1���ֽڵ����ݺ�һ��Ҫ������Ӧ��λ
 */
uint8_t I2C_GetAck() {
	uint8_t ack;
	uint8_t Error_time = 255;

	I2C_Delay();
	SetSDA(); /*8λ��������ͷ������ߣ�׼������Ӧ��λ �ͷ�����*/
	I2C_Delay();
	SetSCL(); /*��������*/
	I2C_Delay();
	do {
		ack = ReadSDA();
		Error_time--;
		if (Error_time == 0) {
			ResetSCL();
			I2C_Delay();
			return 1;
		}
	} while (ack);   //�ж��Ƿ���յ�Ӧ���ź�

	ResetSCL();  //��ʱ���ߣ�ǯסI2C�����Ա��������
	DelayMicroseconds(20);
	return 0;
}

/*
 ������I2C_PutAck()
 ���ܣ���������Ӧ��λ���Ӧ��λ
 ������
 ack=0����������Ӧ��λ
 ack=1������������Ӧ��λ
 ˵����
 �����ڽ�����ÿһ���ֽڵ����ݺ󣬶�Ӧ������Ӧ��λ
 �����ڽ��������һ���ֽڵ����ݺ�Ӧ��������Ӧ��λ
 */
void I2C_PutAck(uint8_t ack) {

	ack ? SetSDA() : ResetSDA();
	I2C_Delay();
	SetSCL();   //Ӧ��
	I2C_Delay();

	ResetSCL();  //��ʱ���ߣ�ǯסI2C�����Ա��������  ������ռ��
	DelayMicroseconds(20);
}

/*
������I2C_Write()
���ܣ���I2C����д1���ֽڵ�����
������
 dat��Ҫд�������ϵ�����
*/
void I2C_Write(uint8_t dat) {
	for (uint8_t t = 0; t < 8; t++) {
		(dat & 0x80) ? SetSDA() : ResetSDA();
		I2C_Delay();
		SetSCL();  //��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ
		I2C_Delay();
		ResetSCL();
		I2C_Delay();
		dat <<= 1;
	}
}

/*
 ������I2C_Read()
 ���ܣ��Ӵӻ���ȡ1���ֽڵ�����
 ���أ���ȡ��һ���ֽ�����
 */
uint8_t I2C_Read() {
	uint8_t dat = 0;
	I2C_Delay();
	I2C_Delay();
	SetSDA(); //�ڶ�ȡ����֮ǰ��Ҫ��SDA����

	I2C_Delay();

	for (uint8_t t = 0; t < 8; t++) {
		ResetSCL();   //
		I2C_Delay();
		SetSCL();     // ��ʱ����Ϊ��ʹ������������������Ч
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
	// ȷ����ַ���һλ��0
	address &= 0xFE;
	// ���������ź�
	I2C_Start();
	// ���ʹӻ���ַ
	I2C_Write(address);
	// �ȴ�ACK��Ӧ
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}
	// ���ͼĴ�����ַ
	while(regSize--){
		I2C_Write(*reg++);
		if (I2C_GetAck()) {
			I2C_Stop();
			return I2C_NO_ACK;
		}
	}
	// ��������
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
	// ȷ����ַ���һλ��0
	address &= 0xFE;
	// ���������ź�
	I2C_Start();
	// ���ʹӻ���ַ
	I2C_Write(address);
	// �ȴ�ACK��Ӧ
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}
	// ���ͼĴ�����ַ
	while (regSize--) {
		I2C_Write(*reg++);
		if (I2C_GetAck()) {
			I2C_Stop();
			return I2C_NO_ACK;
		}
	}
	// �����ظ���ʼ����
	address |= 0x01;
	I2C_Start();
	I2C_Write(address);
	// �ȴ�ACK��Ӧ
	if (I2C_GetAck()) {
		I2C_Stop();
		return I2C_NO_ACK;
	}

	//��������
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
