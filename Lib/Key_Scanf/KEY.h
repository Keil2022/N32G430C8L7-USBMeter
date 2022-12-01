#ifndef KEY_H
#define KEY_H

#include "n32g430_gpio.h"

#define KEY_GPIO_PORT 	GPIOA
#define KEY_GPIO_PINS	(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2)

#define KEY1	GPIO_Input_Pin_Data_Get(GPIOA, GPIO_PIN_2)
#define KEY2	GPIO_Input_Pin_Data_Get(GPIOA, GPIO_PIN_1)
#define KEY3	GPIO_Input_Pin_Data_Get(GPIOA, GPIO_PIN_0)

#define Time_Short 		2
#define Time_Long 		30
#define	Time_Loosen		50

extern u16 KEY3_Time;
extern u16 KEY2_Time;
extern u16 KEY1_Time;

extern u8 KEY3_Long_OK;
extern u8 KEY3_Short_OK;

extern u8 KEY2_Long_OK;
extern u8 KEY2_Short_OK;

extern u8 KEY1_Long_OK;
extern u8 KEY1_Short_OK;

void Key_Configuration(void);
void Key_Scanf(void);

#endif

