#ifndef __LCD_H__
#define __LCD_H__

#include "font_6x10.h"

#define LCD_WIDTH					240		// Display width
#define LCD_HEIGHT					320		// Display height
#define LCD_BACK_BUFFER				((uint32_t)0xD0000000) //LCD back buffer SDRAM address
#define LCD_FRONT_BUFFER				((uint32_t)0xD004B000) //LCD front buffer SDRAM address

void LCD_Init(void);
void LCD_LayerDefaultInit(uint32_t LayerIndex, uint32_t FB_Address);
void LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState state);
void LCD_Clear(uint32_t LayerIndex, uint32_t Color);
void LCD_CopyBuffers(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint32_t color);
void LCD_DrawLine(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t Color);
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint32_t color);
void LCD_PutChar(int x, int y, char c);
void LCD_Printf(int x, int y, char *string, ...);

#endif
