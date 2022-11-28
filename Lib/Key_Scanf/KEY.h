#ifndef KEY_H
#define KEY_H

#include "n32g430_gpio.h"

#define KEY_GPIO_PORT 	GPIOA
#define KEY_GPIO_PINS	(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2)

void Key_Configuration(void);





#endif

