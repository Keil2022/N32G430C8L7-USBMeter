/*
 * MonoScreen.h
 *
 * 用以驱动市面上常见的单色显示屏。
 *
 *  Created on: 2020年1月15日
 */

#ifndef __MONOSCREEN_H__
#define __MONOSCREEN_H__

#include "main.h"
#include "fonts.h"


// ********************************************************
// 屏幕控制器参数定义
// ********************************************************
#define SCREEN_I2C_ADDRESS        0x78   // 总线地址，默认值：SSD1306/0x78
#define SCREEN_I2C_DATA           0x40   // 数据地址
#define SCREEN_I2C_COMMAND        0x00   // 命令地址

#define SCREEN_DEFAULT_CONTRAST   0xCF    // 默认对比度设定
#define COM_PINS_HARDWARE_CONFIG  0x02    // 控制器COM接口物理设定，一般32高的为0x02,64高的为0x12

// ********************************************************
// 屏幕参数定义
// ********************************************************

#define SCREEN_WIDTH       128 // 屏幕物理宽度（像素）
#define SCREEN_HEIGHT      32  // 屏幕物理高度（像素）

// ********************************************************
// 显示定义
// ********************************************************

#define SCREEN_X_OFFSET    (int8_t)0    // X偏移
#define SCREEN_Y_OFFSET    (int8_t)0    // Y偏移

typedef enum{
	ROTATION_NONE,
	ROTATION_90,
	ROTATION_180,
	ROTATION_270,
} ScreenRotation;

// ********************************************************
// 行为定义
// ********************************************************

// 像素绘制方式
typedef enum {
	SCREEN_PSET = 0x00, // 置位像素
	SCREEN_PRES = 0x01, // 复位像素
	SCREEN_PINV = 0x02, // 反转像素
	SCREEN_PNON = 0x03  // 不做处理
} ScreenDrawingMode;

// ********************************************************
// 其他定义
// ********************************************************

// 是否禁用屏幕旋转，如果不需要使用屏幕旋转，则可以禁用来获得更好的性能
#define DISABLE_SCREEN_ROTATE false

// ********************************************************
// 函数定义
// ********************************************************

void MonoScreen_Init(void);
void MonoScreen_SetPixelDrawingMode(ScreenDrawingMode mode);
void MonoScreen_SetBackgroundDrawingMode(ScreenDrawingMode mode);
void MonoScreen_SetFont(FontDef *font);
/**
 * 设置默认字体的大小。如果设置了自定义字体，则无效。
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
