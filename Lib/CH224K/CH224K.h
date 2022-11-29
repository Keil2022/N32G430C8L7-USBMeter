#ifndef CH224K_H
#define CH224K_H

#include "n32g430_gpio.h"

#define CH224K_GPIO_PORT 	GPIOB
#define CH224K_GPIO_PINS	( GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 )

void CH224K_Configuration(void);
void USBOutputvoltage_5V(void);
void USBOutputvoltage_9V(void);
void USBOutputvoltage_12V(void);
void USBOutputvoltage_15V(void);
void USBOutputvoltage_20V(void);

#endif

