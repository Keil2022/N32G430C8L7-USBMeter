#include "main.h"

#include "bsp_delay.h"
#include "bsp_iwdg.h"

#include "MonoScreen.h"

#include "KEY.h"

#define ADC_xBit		4096.0F
#define	ADC_Vref		3300.0F

#define VOLTAGE_H_RES		26.8F
#define VOLTAGE_L_RES		2.8F
#define	VOLTAGE_Gain		( (VOLTAGE_H_RES + VOLTAGE_L_RES) / VOLTAGE_L_RES )
#define	VOLTAGE_Coeffient	( (ADC_Vref / ADC_xBit) * VOLTAGE_Gain )

#define CURRENT_Gain		50
#define CURRENT_RES			0.01F
#define CURRENT_Coeffient	( ADC_Vref / ADC_xBit / CURRENT_Gain / CURRENT_RES )

#define VOLTAGE_FACTOR       1.0F        	// ʵ�ʵ�ѹ����Ե�ѹ�ı�ֵ
#define CURRENT_FACTOR       1.0F          	// ʵ�ʵ�������Ե����ı�ֵ

#define VOLTAGE_ADC_CHANNEL  ADC_Channel_16_PB14   // ��ѹ����ͨ�����
#define CURRENT_ADC_CHANNEL  ADC_Channel_15_PB13   // ��������ͨ�����

//double	VOLTAGE_Gain;
//double	VOLTAGE_Coeffient;
//double	CURRENT_Coeffient;

void BSP_ADC_Init(void);
uint16_t BSP_ADC_GetData(uint8_t ADC_Channel);

int main(void)
{
	// �ڵ���ʱ���ÿ��Ź�
	DBG_Peripheral_ON(DBG_IWDG_STOP);

	// ��ʼ������
	MonoScreen_Init();
	BSP_ADC_Init();

	// ������ʾ����ȫ������0.5�루����������ʾ����
	if (RCC_Flag_Status_Get(RCC_FLAG_IWDGRST) == RESET)
	{
		MonoScreen_FillScreen(0xFF);
		MonoScreen_Flush();
		SysTick_Delay_Ms(500);
		MonoScreen_FillScreen(0x0);
		MonoScreen_Flush();

		RCC_Reset_Flag_Clear();
	}

	// ���ÿ��Ź�
	IWDG_Config(IWDG_CONFIG_PRESCALER_DIV32, 0xFF0);
	
//	//�������
//	VOLTAGE_Gain = (VOLTAGE_H_RES + VOLTAGE_L_RES) / VOLTAGE_L_RES;
//	VOLTAGE_Coeffient = (ADC_Vref/ADC_xBit) * VOLTAGE_Gain;
//	
//	CURRENT_Coeffient = ADC_Vref / ADC_xBit / CURRENT_Gain / CURRENT_RES;
	
	
	// ����ADC��1LSB��ʵ�ʵ�ѹ�����ı�ֵ
	const float volFactor = VOLTAGE_Coeffient * VOLTAGE_FACTOR;
	const float curFactor = CURRENT_Coeffient * CURRENT_FACTOR;
	
	// �����ѹ�͵�����ADC����ֵ
	uint32_t volRaw = 0, curRaw = 0;
	// �����ѹ�͵���ֵ����λmV/mA
	uint16_t voltage = 0, current = 0;
	// ���幦��ֵ����λ����
	uint32_t power = 0;

	char strbuf[256];
	while (1)
	{
		// ������ѹ�͵�����ADCֵ��16��������
		volRaw = 0;
		curRaw = 0;
		for (int i = 0; i < 16; i++)
		{
			volRaw += BSP_ADC_GetData(VOLTAGE_ADC_CHANNEL);
			curRaw += BSP_ADC_GetData(CURRENT_ADC_CHANNEL);
			SysTick_Delay_Ms(20);
		}
		volRaw >>= 4;	//����4λ��������16����ƽ��ֵ
		curRaw >>= 4;

		// ����ʵ�ʵĵ�ѹ�������͹���
		voltage = volRaw * volFactor;
		current = curRaw * curFactor;
		power = current * voltage / 1000;

		// ����������ʾ����ʾ����
		MonoScreen_ClearScreen();
		MonoScreen_setFontSize(2, 2);

		sprintf(strbuf, "%u.%02uV", voltage / 1000, (voltage % 1000) / 10);
		MonoScreen_DrawString(0, 1, strbuf);

		sprintf(strbuf, "%u.%02uA", current / 1000, (current % 1000) / 10);
		MonoScreen_DrawString(0, 17, strbuf);

		// ������Ҫ�ж���ʾС�����λ��
		if (power <= 9999)
		{
			sprintf(strbuf, "%u.%02uW", power / 1000, (power % 1000) / 10);
		}
		else
		{
			sprintf(strbuf, "%u.%uW", power / 1000, (power % 1000) / 100);
		}
		MonoScreen_setFontSize(2, 4);
		MonoScreen_DrawString(67, 2, strbuf);

		// ˢ����Ļ��ʾ
		MonoScreen_Flush();
		// here we go:)
		IWDG_Feed();
	}
}

void BSP_ADC_Init()
{
	// ��������ʱ��
	RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOB | RCC_AHB_PERIPH_ADC);	//Enables the AHB peripheral clock.
	ADC_Clock_Mode_Config(ADC_CKMOD_AHB, RCC_ADCHCLK_DIV16);					//Configures the ADCHCLK prescaler.
	RCC_ADC_1M_Clock_Config(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8); 			//Configures the ADCx 1M clock (ADC1MCLK).

	GPIO_InitType GPIO_InitStructure;

	// ��ADC����������ù���ģʽ����Ϊģ��ģʽ
	GPIO_Structure_Initialize(&GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
	GPIO_Peripheral_Initialize(GPIOB, &GPIO_InitStructure);

	ADC_InitType ADC_InitStructure;

	// ����ADC����
	ADC_InitStructure.MultiChEn = DISABLE;	//�رն�ͨ��
	ADC_InitStructure.ContinueConvEn = DISABLE;	//
	ADC_InitStructure.ExtTrigSelect = ADC_EXT_TRIGCONV_REGULAR_SWSTRRCH;
	ADC_InitStructure.DatAlign = ADC_DAT_ALIGN_R;
	ADC_InitStructure.ChsNumber = ADC_REGULAR_LEN_1;
	ADC_Initializes(&ADC_InitStructure);

	// ����ADC
	ADC_ON();
	// �ȴ�ADC����
	while (ADC_Flag_Status_Get(ADC_RD_FLAG, ADC_FLAG_AWDG, ADC_FLAG_RDY) == RESET)
	{
		__NOP();
	}

	// ����ADC��У׼
	ADC_Calibration_Operation(ADC_CALIBRATION_ENABLE);
	// �ȴ�ADC��У׼���
	while (ADC_Calibration_Operation(ADC_CALIBRATION_STS) == SET)
	{
		__NOP();
	}
}

uint16_t BSP_ADC_GetData(uint8_t ADC_Channel)
{
	// ���ò�������
	ADC_Channel_Sample_Time_Config(ADC_Channel, ADC_SAMP_TIME_55CYCLES5);
	ADC_Regular_Sequence_Conversion_Number_Config(ADC_Channel, ADC_REGULAR_NUMBER_1);

	// ����ADCת��
	ADC_Regular_Channels_Software_Conversion_Operation(ADC_EXTRTRIG_SWSTRRCH_ENABLE);
	// �ȴ�ADCת�����
	while (ADC_Flag_Status_Get(ADC_RUN_FLAG, ADC_FLAG_ENDC, ADC_FLAG_RDY) == 0)
	{
		__NOP();
	}
	ADC_Flag_Status_Clear(ADC_FLAG_ENDC);
	ADC_Flag_Status_Clear(ADC_FLAG_STR);
	
	// ��ȡADC����ֵ
	return ADC_Regular_Group_Conversion_Data_Get();
}

