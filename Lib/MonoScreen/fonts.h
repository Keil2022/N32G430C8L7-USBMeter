#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

typedef struct {
	const uint8_t fontWidth;       // Width of character
	const uint8_t fontHeight;      // Height of character
	const uint8_t fontBPC;         // Bytes for one character
	const uint8_t fontMinChar;     // Code of the first known symbol
	const uint8_t fontMaxChar;     // Code of the last known symbol
	const uint8_t fontUnknownChar; // Code of the unknown symbol
	const uint8_t *fontData;       // Font data
} FontDef;

/* 内建默认字体(ASCII 6x8点阵字体) */
extern const uint8_t ScreenBuildInFont[];


#endif // __FONTS_H__
