/*
 * MonoScreen.c
 *
 *  Created on: 2020年1月15日
 *      Author: Desktop-01
 */


#include "MonoScreen.h"
#include "bsp_delay.h"
#include "SoftI2c.h"

#define I2Cx                I2C1
#define I2Cx_SCL_PIN        GPIO_PIN_4
#define I2Cx_SDA_PIN        GPIO_PIN_5
#define GPIOx               GPIOA

#define swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) // No-temp-var swap operation

// 定义显示缓存
#define VRAM_SIZE SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8)//(SCREEN_WIDTH * SCREEN_HEIGHT) >> 3
static uint8_t vRam[VRAM_SIZE] __attribute__((aligned(4)));

FontDef *font;
uint8_t fontSizeX = 1, fontSizeY = 1;
volatile ScreenDrawingMode pixelDrawingMode = SCREEN_PSET;
volatile ScreenDrawingMode backgroundDrawingMode = SCREEN_PNON;
volatile ScreenRotation screenRotation = ROTATION_NONE;

// *********************************************************
// 使用硬件I2C时的数据传输函数
// *********************************************************

static int I2CMasterWrite(uint8_t deviceAddress, uint8_t memAddress, uint8_t* data, uint16_t dataLen, uint32_t maxTimeout){
	SoftI2c_MemWrite(deviceAddress, &memAddress, 1, data, dataLen);
    return 0;
}

static void WriteCommand(uint8_t cmd){

	I2CMasterWrite(SCREEN_I2C_ADDRESS, SCREEN_I2C_COMMAND, &cmd, 1, 0xFFFF);
}

static void TransmitCommands(uint8_t* cmd, uint16_t size){
	I2CMasterWrite(SCREEN_I2C_ADDRESS, SCREEN_I2C_COMMAND, cmd, size, 0xFFFF);
}

static void TransmitData(uint8_t* data, uint16_t size){
	I2CMasterWrite(SCREEN_I2C_ADDRESS, SCREEN_I2C_DATA, data, size, 0xFFFF);
}

static uint8_t width() {
#if !DISABLE_SCREEN_ROTATE
	switch (screenRotation) {
	case ROTATION_90:
	case ROTATION_270:
		return SCREEN_HEIGHT ;
	default:
		break;
	}
#endif
	return SCREEN_WIDTH ;
}

static uint8_t height() {
#if !DISABLE_SCREEN_ROTATE
	switch (screenRotation) {
	case ROTATION_90:
	case ROTATION_270:
		return SCREEN_WIDTH ;
	default:
		break;
	}
#endif
	return SCREEN_HEIGHT ;
}

static void drawFastHLineInternal( int16_t x, int16_t y, int16_t w, ScreenDrawingMode drawingMode) {
	if ((y >= 0) && (y < SCREEN_HEIGHT )) { // Y coord in bounds?
		if (x < 0) { // Clip left
			w += x;
			x = 0;
		}

		if ((x + w) > SCREEN_WIDTH) { // Clip right
			w = (SCREEN_WIDTH - x);
		}

		if (w > 0) { // Proceed only if width is positive
			uint8_t *pBuf = &vRam[(y / 8) * SCREEN_WIDTH + x], mask = 1 << (y & 7);
			switch(drawingMode) {
			case SCREEN_PSET:               while(w--) { *pBuf++ |= mask; }; break;
			case SCREEN_PRES: mask = ~mask; while(w--) { *pBuf++ &= mask; }; break;
			case SCREEN_PINV:               while(w--) { *pBuf++ ^= mask; }; break;
			default:break;
			}
		}
	}
}

static void drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, ScreenDrawingMode drawingMode) {
	if ((x >= 0) && (x < SCREEN_WIDTH )) { // X coord in bounds?
		if (__y < 0) { // Clip top
			__h += __y;
			__y = 0;
		}
		if ((__y + __h) > SCREEN_HEIGHT) { // Clip bottom
			__h = (SCREEN_HEIGHT - __y);
		}
		if (__h > 0) { // Proceed only if height is now positive
			// this display doesn't need ints for coordinates,
			// use local byte registers for faster juggling
			uint8_t y = __y, h = __h;
			uint8_t *pBuf = &vRam[(y / 8) * SCREEN_WIDTH + x];

			// do the first partial byte, if necessary - this requires some masking
			uint8_t mod = (y & 7);
			if (mod) {
				// mask off the high n bits we want to set
				mod = 8 - mod;
				// note - lookup table results in a nearly 10% performance
				// improvement in fill* functions
				// uint8_t mask = ~(0xFF >> mod);
				static const uint8_t premask[8] = { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
				uint8_t mask = premask[mod];
				// adjust the mask if we're not going to reach the end of this byte
				if (h < mod) mask &= (0XFF >> (mod - h));

				switch (drawingMode) {
				case SCREEN_PSET:
					*pBuf |= mask;
					break;
				case SCREEN_PRES:
					*pBuf &= ~mask;
					break;
				case SCREEN_PINV:
					*pBuf ^= mask;
					break;
				default:
					break;
				}
				pBuf += SCREEN_WIDTH;
			}

			if (h >= mod) { // More to go?
				h -= mod;
				// Write solid bytes while we can - effectively 8 rows at a time
				if (h >= 8) {
					if (drawingMode == SCREEN_PINV) {
						// separate copy of the code so we don't impact performance of
						// black/white write version with an extra comparison per loop
						do {
							*pBuf ^= 0xFF;  // Invert byte
							pBuf += SCREEN_WIDTH; // Advance pointer 8 rows
							h -= 8;     // Subtract 8 rows from height
						} while (h >= 8);
					}
					else {
						// store a local value to work with
						uint8_t val = (drawingMode != SCREEN_PRES) ? 255 : 0;
						do {
							*pBuf = val;    // Set byte
							pBuf += SCREEN_WIDTH;  // Advance pointer 8 rows
							h -= 8;      // Subtract 8 rows from height
						} while (h >= 8);
					}
				}

				if (h) { // Do the final partial byte, if necessary
					mod = h & 7;
					// this time we want to mask the low bits of the byte,
					// vs the high bits we did above
					// uint8_t mask = (1 << mod) - 1;
					// note - lookup table results in a nearly 10% performance
					// improvement in fill* functions
					static const uint8_t postmask[8] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
					uint8_t mask = postmask[mod];
					switch (drawingMode) {
					case SCREEN_PSET:
						*pBuf |= mask;
						break;
					case SCREEN_PRES:
						*pBuf &= ~mask;
						break;
					case SCREEN_PINV:
						*pBuf ^= mask;
						break;
					default:
						break;
					}
				}
			}
		} // endif positive height
	} // endif x in bounds
}

static void DrawPixelInternal(int16_t x, int16_t y, ScreenDrawingMode mode) {

	if(mode == SCREEN_PNON){
		return;
	}

	x += SCREEN_X_OFFSET;
	y += SCREEN_Y_OFFSET;

	if (x > width() - 1 || y > height() - 1 || x < 0 || y < 0) {
		return;
	}

#if !DISABLE_SCREEN_ROTATE
	switch (screenRotation) {
	case ROTATION_90:
		swap(x, y);
		x = SCREEN_WIDTH - x - 1;
		break;
	case ROTATION_180:
		x = SCREEN_WIDTH - x - 1;
		y = SCREEN_HEIGHT - y - 1;
		break;
	case ROTATION_270:
		swap(x, y);
		y = SCREEN_HEIGHT - y - 1;
		break;
	default:
		break;
	}
#endif

	switch (mode) {
	case SCREEN_PSET:
		vRam[x + (y / 8) * SCREEN_WIDTH ] |= (1 << (y & 7));
		break;
	case SCREEN_PRES:
		vRam[x + (y / 8) * SCREEN_WIDTH ] &= ~(1 << (y & 7));
		break;
	case SCREEN_PINV:
		vRam[x + (y / 8) * SCREEN_WIDTH ] ^= (1 << (y & 7));
		break;
	default: break;
	}
}

void DrawVLineInternal(int16_t x, int16_t y, int16_t h, ScreenDrawingMode mode){

	if(mode == SCREEN_PNON){
		return;
	}

#if !DISABLE_SCREEN_ROTATE
	bool bSwap = false;
	switch (screenRotation) {
	case ROTATION_90:
		// 90 degree rotation, swap x & y for rotation,
		// then invert x and adjust x for h (now to become w)
		bSwap = true;
		swap(x, y);
		x = SCREEN_WIDTH - x - 1;
		x -= (h - 1);
		break;
	case ROTATION_180:
		// 180 degree rotation, invert x and y, then shift y around for height.
		x = SCREEN_WIDTH - x - 1;
		y = SCREEN_HEIGHT - y - 1;
		y -= (h - 1);
		break;
	case ROTATION_270:
		// 270 degree rotation, swap x & y for rotation, then invert y
		bSwap = true;
		swap(x, y);
		y = SCREEN_HEIGHT - y - 1;
		break;
	default:
		break;
	}

	if (bSwap)
		drawFastHLineInternal(x, y, h, mode);
	else
#endif
		drawFastVLineInternal(x, y, h, mode);
}

void FillRectInternal(int16_t x, int16_t y, int16_t w, int16_t h, ScreenDrawingMode mode) {
	for (int16_t i = x; i < x + w; i++) {
		DrawVLineInternal(i, y, h, mode);
	}
}


static uint8_t DrawCharInternal(int16_t x, int16_t y, unsigned char c, ScreenDrawingMode fg, ScreenDrawingMode bg, uint8_t xSize, uint8_t ySize){

	if (font == NULL) {
		if ((x >= width()) || // Clip right
				(y >= height()) || // Clip bottom
				((x + 6 * xSize - 1) < 0) || // Clip left
				((y + 8 * ySize - 1) < 0))   // Clip top
			return 0;

		for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
			uint8_t line = ScreenBuildInFont[c * 5 + i];
			for (int8_t j = 0; j < 8; j++, line >>= 1) {
				if (line & 1) {
					if (xSize == 1 && ySize == 1)
						DrawPixelInternal(x + i, y + j, fg);
					else
						FillRectInternal(x + i * xSize, y + j * ySize, xSize, ySize, fg);
				}
				else if (bg != fg && bg != SCREEN_PNON) {
					if (xSize == 1 && ySize == 1)
						DrawPixelInternal(x + i, y + j, bg);
					else
						FillRectInternal(x + i * xSize, y + j * ySize, xSize, ySize, bg);
				}
			}
		}
		if (bg != fg) { // If opaque, draw vertical line for last column
			if (xSize == 1 && ySize == 1){
				DrawVLineInternal(x + 5, y, 8, bg);
			}
			else{
				FillRectInternal(x + 5 * xSize, y, xSize, 8 * ySize, bg);
			}
		}

		return 6 * xSize;
	}

	int16_t pX;
	int16_t pY;
	uint8_t tmpCh;
	uint8_t bL;
	uint8_t tL = font->fontHeight;
	const uint8_t *pCh;

	// If the specified character code is out of bounds should substitute the code of the "unknown" character
	if (c < font->fontMinChar || c > font->fontMaxChar) c = font->fontUnknownChar;

	// Pointer to the first byte of character in font data array
	pCh = &font->fontData[(c - font->fontMinChar) * font->fontBPC];

	// Draw character
	// Vertical pixels order
	pX = x;
	while (pX < x + font->fontWidth) {
		pY = y;
		while (pY < y + font->fontHeight) {
			bL = tL > 8 ? 8 : tL;
			tmpCh = *pCh++;
			while (bL) {
				if (tmpCh & 0x01)
					DrawPixelInternal(pX, pY, fg);
				else
					DrawPixelInternal(pX, pY, bg);
				tmpCh >>= 1;
				pY++;
				bL--;
			}
			tL -= 8;
		}
		pX++;
		tL = font->fontHeight;
	}

	return font->fontWidth;
}

void MonoScreen_FillScreen(uint8_t pattern){
	for(uint16_t i = 0; i < VRAM_SIZE; i++){
		vRam[i] = pattern;
	}
}

void MonoScreen_ClearScreen(){
	switch (pixelDrawingMode) {
	case SCREEN_PSET:
	case SCREEN_PNON:
		MonoScreen_FillScreen(0x00);
		return;
	default:
		MonoScreen_FillScreen(0xFF);
		return;
	}
}

void MonoScreen_SetPixelDrawingMode(ScreenDrawingMode mode){
	pixelDrawingMode = mode;
}

void MonoScreen_SetBackgroundDrawingMode(ScreenDrawingMode mode){
	backgroundDrawingMode = mode;
}

void MonoScreen_SetRotation(ScreenRotation rotation){
	screenRotation = rotation;
}

void MonoScreen_SetFont(FontDef *f){
	font = f;
}

void MonoScreen_setFontSize(uint8_t xSize, uint8_t ySize){
	fontSizeX = xSize;
	fontSizeY = ySize;
}

uint8_t MonoScreen_GetWidth() {
	return width();
}

uint8_t MonoScreen_GetHeight(){
	return height();
}

void MonoScreen_DrawPixel(int16_t x, int16_t y){
	DrawPixelInternal(x, y, pixelDrawingMode);
}

void MonoScreen_DrawHLine(int16_t x, int16_t y, int16_t w){
#if !DISABLE_SCREEN_ROTATE
	  bool bSwap = false;
	  switch(screenRotation) {
	   case ROTATION_90:
	    // 90 degree rotation, swap x & y for rotation, then invert x
	    bSwap = true;
	    swap(x, y);
	    x = SCREEN_WIDTH - x - 1;
	    break;
	   case ROTATION_180:
	    // 180 degree rotation, invert x and y, then shift y around for height.
	    x  = SCREEN_WIDTH  - x - 1;
	    y  = SCREEN_HEIGHT - y - 1;
	    x -= (w-1);
	    break;
	   case ROTATION_270:
	    // 270 degree rotation, swap x & y for rotation,
	    // then invert y and adjust y for w (not to become h)
	    bSwap = true;
	    swap(x, y);
	    y  = SCREEN_HEIGHT - y - 1;
	    y -= (w-1);
	    break;
	   default:break;
	  }

	  if(bSwap) drawFastVLineInternal(x, y, w,pixelDrawingMode);
	  else
#endif
	  drawFastHLineInternal(x, y, w, pixelDrawingMode);
}

void MonoScreen_DrawVLine(int16_t x, int16_t y, int16_t h) {
	DrawVLineInternal(x,y,h,pixelDrawingMode);
}

void MonoScreen_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
    int16_t steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep) {
    	swap(x1, y1);
    	swap(x2, y2);
    }

    if (x1 > x2) {
    	swap(x1, x2);
    	swap(y1, y2);
    }

    int16_t dx, dy;
    dx = x2 - x1;
    dy = abs(y2 - y1);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y1 < y2) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x1<=x2; x1++) {
        if (steep) {
            MonoScreen_DrawPixel(y1, x1);
        } else {
        	MonoScreen_DrawPixel(x1, y1);
        }
        err -= dy;
        if (err < 0) {
            y1 += ystep;
            err += dx;
        }
    }
}

void MonoScreen_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
	MonoScreen_DrawHLine(x, y, w);
	MonoScreen_DrawHLine(x, y + h - 1, w);
	MonoScreen_DrawVLine(x, y, h);
	MonoScreen_DrawVLine(x + w - 1, y, h);
}

void MonoScreen_FillRect(int16_t x, int16_t y, int16_t w, int16_t h) {
	for (int16_t i = x; i < x + w; i++) {
		MonoScreen_DrawVLine(i, y, h);
	}
}

uint8_t MonoScreen_DrawChar(int16_t x, int16_t y, unsigned char c) {
	return DrawCharInternal(x, y, c, pixelDrawingMode, backgroundDrawingMode, fontSizeX, fontSizeY);
}

uint8_t MonoScreen_DrawColoredChar(int16_t x, int16_t y, unsigned char c, ScreenDrawingMode fg, ScreenDrawingMode bg){
	return DrawCharInternal(x, y, c, fg, bg, fontSizeX, fontSizeY);
}

uint16_t MonoScreen_DrawString(int16_t x, int16_t y, char *str){

	uint8_t fontHeight, fontWidth;
	int16_t rx = x, ry = y;
	if(font == NULL){
		fontHeight = 8 * fontSizeY;
		fontWidth  = 6 * fontSizeX;
	}
	else{
		fontHeight = font->fontHeight;
		fontWidth  = font->fontWidth;
	}

	uint16_t length = 0;
	while (*str) {
		if (rx + fontWidth >= width()) {
			rx = 0;
			ry += fontHeight;
			if (ry + fontHeight >= height()) {
				break;
			}

			if (*str == ' ' || *str == '\r') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		if (*str == '\r') {
			str++;
			continue;
		}
		else if (*str == '\n') {
			str++;
			rx = 0;
			ry += fontHeight;
			if (ry + fontHeight >= height()) {
				break;
			}
			if (*str == ' ' || *str == '\r') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}

			continue;
		}

		length += MonoScreen_DrawChar(rx, ry, *str);
		rx += fontWidth;
		str++;
	}

	return length;
}

uint16_t MonoScreen_DrawConstString(int16_t x, int16_t y, const char *str){

	uint8_t fontHeight, fontWidth;
	int16_t rx = x, ry = y;
	if(font == NULL){
		fontHeight = 8 * fontSizeY;
		fontWidth  = 6 * fontSizeY;
	}
	else{
		fontHeight = font->fontHeight;
		fontWidth  = font->fontWidth;
	}

	uint16_t length = 0;
	while (*str) {
		if (rx + fontWidth >= width()) {
			rx = 0;
			ry += fontHeight;
			if (ry + fontHeight >= height()) {
				break;
			}

			if (*str == ' ' || *str == '\r') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		if (*str == '\r') {
			str++;
			continue;
		}
		else if (*str == '\n') {
			str++;
			rx = 0;
			ry += fontHeight;
			if (ry + fontHeight >= height()) {
				break;
			}
			if (*str == ' ' || *str == '\r') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}

			continue;
		}

		length += MonoScreen_DrawChar(rx, ry, *str);
		rx += fontWidth;
		str++;
	}

	return length;
}

void MonoScreen_Flush(void){
	uint8_t cmds[] = {
			0x22,
			0x00,
			0xFF,
			0x21,
			0x00,
			SCREEN_WIDTH - 1
		};
	TransmitCommands(cmds, sizeof(cmds));
	TransmitData(vRam, VRAM_SIZE);
}

static void MonoScreen_BSP_Init(){
    GPIO_InitType GPIO_InitStructure;

    RCC_AHB_Peripheral_Clock_Enable(RCC_AHB_PERIPH_GPIOA);

    GPIO_Structure_Initialize(&GPIO_InitStructure);
    GPIO_InitStructure.Pin            = I2Cx_SCL_PIN | I2Cx_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_OUT_OD;
    GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST; 
    GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP; 
    GPIO_Peripheral_Initialize(GPIOx, &GPIO_InitStructure);

	GPIO_Pins_Set(GPIOA, I2Cx_SCL_PIN | I2Cx_SDA_PIN);
}

void MonoScreen_Init(void){
    
    MonoScreen_BSP_Init();

	// Waitting for power stable
	SysTick_Delay_Ms(200);

	// SSD1306 Initialization Sequence
	WriteCommand(0xAE);                   // Set Display Off
	WriteCommand(0xD5);                   // Display divide ratio/osc. freq. mode
	WriteCommand(0x80);                   // 115HZ
	WriteCommand(0xA8);                   // Multiplex ration mode:
	WriteCommand(SCREEN_HEIGHT - 1);

	WriteCommand(0xD3);                   // Set Display Offset
	WriteCommand(0x00);                   // No offset
	WriteCommand(0x40);                   // Set Display Start Line
	WriteCommand(0x8D);                   // DC-DC Control Mode Set

	WriteCommand(0x14);                   // DC-DC ON Mode Set

	WriteCommand(0x20);                   // Set Memory Addressing Mode
	WriteCommand(0x00);                   // Page Addressing Mode: Horizontal
	WriteCommand(0xA1);                   // Segment Remap
	WriteCommand(0xC8);                   // Set COM Output Scan Direction

	WriteCommand(0xDA);                   // Seg pads hardware: alternative
	WriteCommand(COM_PINS_HARDWARE_CONFIG);
	WriteCommand(0x81);                   // Contrast control
	WriteCommand(SCREEN_DEFAULT_CONTRAST);

	WriteCommand(0xD9);                   // Set pre-charge period
	WriteCommand(0xF1);

	WriteCommand(0xDB);                   // VCOMH deselect level mode
	WriteCommand(0x40);
	WriteCommand(0xA4);                   // Set Entire Display On/Off
	WriteCommand(0xA6);                   // Set Normal Display
	WriteCommand(0x2E);                   // Set Display On
	WriteCommand(0xAF);                   // Set Display On
}
