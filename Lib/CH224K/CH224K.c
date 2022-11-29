#include "CH224K.h"

void CH224K_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;
	
	GPIO_Structure_Initialize(&GPIO_InitStructure);		//填入缺省值
	GPIO_InitStructure.GPIO_Current = GPIO_DS_12MA;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT_PP;	//推挽输出模式
	GPIO_InitStructure.Pin = CH224K_GPIO_PINS;
	GPIO_Peripheral_Initialize(CH224K_GPIO_PORT, &GPIO_InitStructure);
}

void USBOutputvoltage_5V(void)
{
	GPIO_Pins_Set(CH224K_GPIO_PORT, GPIO_PIN_3);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_4);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_5);
}

void USBOutputvoltage_9V(void)
{
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_3);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_4);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_5);	
}

void USBOutputvoltage_12V(void)
{
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_3);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_4);
	GPIO_Pins_Set(CH224K_GPIO_PORT, GPIO_PIN_5);	
}

void USBOutputvoltage_15V(void)
{
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_3);
	GPIO_Pins_Set(CH224K_GPIO_PORT, GPIO_PIN_4);
	GPIO_Pins_Set(CH224K_GPIO_PORT, GPIO_PIN_5);	
}

void USBOutputvoltage_20V(void)
{
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_3);
	GPIO_Pins_Set(CH224K_GPIO_PORT, GPIO_PIN_4);
	GPIO_Pins_Reset(CH224K_GPIO_PORT, GPIO_PIN_5);	
}


