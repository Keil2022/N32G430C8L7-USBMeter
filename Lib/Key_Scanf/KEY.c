#include "KEY.h"

void Key_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;
	
	GPIO_Structure_Initialize(&GPIO_InitStructure);	//����ȱʡֵ
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;	//����ģʽ
	GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;	//��������
	GPIO_InitStructure.Pin = KEY_GPIO_PINS;
	GPIO_Peripheral_Initialize(KEY_GPIO_PORT, &GPIO_InitStructure);
}





