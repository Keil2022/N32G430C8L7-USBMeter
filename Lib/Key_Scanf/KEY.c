#include "KEY.h"

u16 KEY3_Time = 0;
u16 KEY2_Time = 0;
u16 KEY1_Time = 0;

u8 KEY3_Long_OK = 0;
u8 KEY3_Short_OK = 0;
u8 KEY2_Short_OK = 0;
u8 KEY1_Short_OK = 0;

u8 KEY3_Phase = 0;
u8 KEY2_Phase = 0;
u8 KEY1_Phase = 0;

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
	switch(KEY3_Phase)
	{
		//短按
		case 0:
			if(KEY3 == RESET)
			{
				KEY3_Time++;
				if(KEY3_Time >= Time_Long)
				{
					KEY3_Phase = 1;
					
					KEY3_Long_OK = 1;
				}
			}
			else
			{
				if(KEY3_Time >= Time_Short)
				{
					KEY3_Time = 0;
					KEY3_Short_OK = 1;
				}
			}
			break;
		
		//长按
		case 1:
			if(KEY3 == RESET)
			{
				KEY3_Time++;
				if(KEY3_Time >= Time_Loosen)
				{
					KEY3_Phase = 2;
				}
			}
			else
			{
				KEY3_Time = 0;
				KEY3_Phase = 0;
			}
			break;
		
		//等待松手
		case 2:
			if(KEY3 == SET)
			{
				KEY3_Time = 0;
				KEY3_Phase = 0;
			}
			break;
		
		default:
			KEY3_Phase = 0;
			KEY3_Time = 0;
			break;
	}
	
	switch(KEY2_Phase)
	{
		//短按
		case 0:
			if(KEY2 == RESET)
			{
				KEY2_Time++;
				if(KEY2_Time >= Time_Long)
				{
					KEY2_Phase = 1;
				}
			}
			else
			{
				if(KEY2_Time >= Time_Short)
				{
					KEY2_Time = 0;
					KEY2_Short_OK = 1;
				}
			}
			break;
		
		//长按
		case 1:
			if(KEY2 == RESET)
			{
				KEY2_Time++;
				if(KEY2_Time >= Time_Loosen)
				{
					KEY2_Phase = 2;
				}
			}
			else
			{
				KEY2_Time = 0;
				KEY2_Phase = 0;
			}
			break;
		
		//等待松手
		case 2:
			if(KEY2 == SET)
			{
				KEY2_Time = 0;
				KEY2_Phase = 0;
			}
			break;
		
		default:
			KEY2_Phase = 0;
			KEY2_Time = 0;
			break;
	}
	
	switch(KEY1_Phase)
	{
		//短按
		case 0:
			if(KEY1 == RESET)
			{
				KEY1_Time++;
				if(KEY1_Time >= Time_Long)
				{
					KEY1_Phase = 1;
				}
			}
			else
			{
				if(KEY1_Time >= Time_Short)
				{
					KEY1_Time = 0;
					KEY1_Short_OK = 1;
				}
			}
			break;
		
		//长按
		case 1:
			if(KEY1 == RESET)
			{
				KEY1_Time++;
				if(KEY1_Time >= Time_Loosen)
				{
					KEY1_Phase = 2;
				}
			}
			else
			{
				KEY1_Time = 0;
				KEY1_Phase = 0;
			}
			break;
		
		//等待松手
		case 2:
			if(KEY1 == SET)
			{
				KEY1_Time = 0;
				KEY1_Phase = 0;
			}
			break;
		
		default:
			KEY1_Phase = 0;
			KEY1_Time = 0;
			break;
	}
}





