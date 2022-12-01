#include "time6.h"

u32 BSTIMClockFrequency = 0;

void TIME6_Configuration(void)
{
	/* System Clocks Configuration */
	BSTIMClockFrequency = Common_BSTIM_RCC_Initialize(TIM6, RCC_HCLK_DIV16);
	
	/* NVIC Configuration */
    Common_TIM_NVIC_Initialize(TIM6_IRQn, ENABLE);
	
	/* Time base configuration£¬ period = 65535, prescaler = prescaler */
    Common_TIM_Base_Initialize(TIM6, 999, 0);
	
	/* Initializes the TIM reload mode */
	TIM_Base_Reload_Mode_Set(TIM6, TIM_PSC_RELOAD_MODE_IMMEDIATE);
	
	/* The timer interruption function was enabled */
	TIM_Interrupt_Enable(TIM6, TIM_INT_UPDATE);

	/* OPEN TIME6 */
    TIM_On(TIM6);
}


