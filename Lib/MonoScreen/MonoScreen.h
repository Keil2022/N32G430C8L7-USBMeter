/*
 * MonoScreen.h
 *
 * �������������ϳ����ĵ�ɫ��ʾ����
 *
 *  Created on: 2020��1��15��
 */

#ifndef __MONOSCREEN_H__
#define __MONOSCREEN_H__

#include "main.h"
#include "fonts.h"


// ********************************************************
// ��Ļ��������������
// ********************************************************
#define SCREEN_I2C_ADDRESS        0x78   // ���ߵ�ַ��Ĭ��ֵ��SSD1306/0x78
#define SCREEN_I2C_DATA           0x40   // ���ݵ�ַ
#define SCREEN_I2C_COMMAND        0x00   // �����ַ

#define SCREEN_DEFAULT_CONTRAST   0xCF    // Ĭ�϶Աȶ��趨
#define COM_PINS_HARDWARE_CONFIG  0x02    // ������COM�ӿ������趨��һ��32�ߵ�Ϊ0x02,64�ߵ�Ϊ0x12

// ********************************************************
// ��Ļ��������
// ********************************************************

#define SCREEN_WIDTH       128 // ��Ļ�����ȣ����أ�
#define SCREEN_HEIGHT      32  // ��Ļ����߶ȣ����أ�

// ********************************************************
// ��ʾ����
// ********************************************************

#define SCREEN_X_OFFSET    (int8_t)0    // Xƫ��
#define SCREEN_Y_OFFSET    (int8_t)0    // Yƫ��

typedef enum{
	ROTATION_NONE,
	ROTATION_90,
	ROTATION_180,
	ROTATION_270,
} ScreenRotation;

// ********************************************************
// ��Ϊ����
// ********************************************************

// ���ػ��Ʒ�ʽ
typedef enum {
	SCREEN_PSET = 0x00, // ��λ����
	SCREEN_PRES = 0x01, // ��λ����
	SCREEN_PINV = 0x02, // ��ת����
	SCREEN_PNON = 0x03  // ��������
} ScreenDrawingMode;

// ********************************************************
// ��������
// ********************************************************

// �Ƿ������Ļ��ת���������Ҫʹ����Ļ��ת������Խ�������ø��õ�����
#define DISABLE_SCREEN_ROTATE false

// ********************************************************
// ��������
// ********************************************************

void MonoScreen_Init(void);
void MonoScreen_SetPixelDrawingMode(ScreenDrawingMode mode);
void MonoScreen_SetBackgroundDrawingMode(ScreenDrawingMode mode);
void MonoScreen_SetFont(FontDef *font);
/**
 * ����Ĭ������Ĵ�С������������Զ������壬����Ч��
 */
void MonoScreen_setFontSize(uint8_t xSize, uint8_t ySize);
void MonoScreen_SetRotation(ScreenRotation rotation);
void MonoScreen_FillScreen(uint8_t pattern);
void MonoScreen_ClearScreen(void);

uint8_t MonoScreen_GetWidth(void);
uint8_t MonoScreen_GetHeight(void);

void MonoScreen_DrawPixel(int16_t x, int16_t y);

void MonoScreen_DrawHLine(int16_t x, int16_t y, int16_t w);
void MonoScreen_DrawVLine(int16_t x, int16_t y, int16_t h);
void MonoScreen_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

void MonoScreen_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h);
void MonoScreen_FillRect(int16_t x, int16_t y, int16_t w, int16_t h);

uint8_t MonoScreen_DrawChar(int16_t x, int16_t y, unsigned char c);
uint8_t MonoScreen_DrawColoredChar(int16_t x, int16_t y, unsigned char c, ScreenDrawingMode fg, ScreenDrawingMode bg);
uint16_t MonoScreen_DrawString(int16_t x, int16_t y, char *str);
uint16_t MonoScreen_DrawConstString(int16_t x, int16_t y, const char *str);

void MonoScreen_Flush(void);

#endif /* MONOSCREEN_H_ */
