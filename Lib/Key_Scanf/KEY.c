#include "KEY.h"

void Key_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;
	
	GPIO_Structure_Initialize(&GPIO_InitStructure);	//填入缺省值
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;	//输入模式
	GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;	//上拉输入
	GPIO_InitStructure.Pin = KEY_GPIO_PINS;
	GPIO_Peripheral_Initialize(KEY_GPIO_PORT, &GPIO_InitStructure);
}





