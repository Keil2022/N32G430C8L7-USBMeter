#include "KEY.h"

u16 KEY3_Time = 0;
u16 KEY2_Time = 0;
u16 KEY1_Time = 0;

void Key_Configuration(void)
{
	GPIO_InitType GPIO_InitStructure;
	
	GPIO_Structure_Initialize(&GPIO_InitStructure);	//填入缺省值
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;	//输入模式
	GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;	//上拉输入
	GPIO_InitStructure.Pin = KEY_GPIO_PINS;
	GPIO_Peripheral_Initialize(KEY_GPIO_PORT, &GPIO_InitStructure);
}

void Key_Handle(void)
{
	if(KEY3 == RESET)
	{
		KEY3_Time++;
		if(KEY3_Time >= Time_Long)
		{
			KEY3_Time = 0;
			
		}
	}
	else
	{
		if(KEY3_Time >= Time_Short)
		{
			
		}
	}
	
	if(KEY2 == RESET)
	{
		KEY2_Time++;
		if(KEY2_Time >= Time_Long)
		{
			//KEY2_Time = 0;
		}
	}
	else
	{
		if(KEY2_Time >= Time_Short)
		{
			
		}
	}
	
	if(KEY1 == RESET)
	{
		KEY1_Time++;
		if(KEY1_Time >= Time_Long)
		{
			//KEY1_Time = 0;
		}		
	}
	else
	{
		if(KEY1_Time >= Time_Short)
		{
			
		}
	}
}





