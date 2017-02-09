/*
 * lcd.h
 *
 * Define some methods to handle lcd drawing
 *
 *  Created on: Oct 27, 2015
 *      Author: Fernando
 */

#ifndef INCLUDE_LCD_H_
#define INCLUDE_LCD_H_

#include "FreeRTOS.h"
#include "u8g.h"

#define MAX_ROW	6
#define MAX_COLUMN	20

#define SCREEN_HEADER_ID	0
#define SCREEN_MAIN_ID		1
#define SCREEN_FOOTER_ID	2

#define BACKGROUND_NORMAL	0
#define BACKGROUND_INVERTED	1
#define BACKGROUND_FRAMED	2
#define BACKGROUND_FRAMED_2	3

extern void ut_lcd_init();
extern void ut_lcd_clear();
extern void ut_lcd_clear_str();
extern void ut_lcd_drawString(uint8_t line, uint8_t column, const char* text, uint8_t invert);
extern void ut_lcd_drawStr(uint8_t line, uint8_t column, const char* text, uint8_t invert, uint8_t lineMarked, const uint8_t* font);
extern void ut_lcd_output();
extern void ut_lcd_output_str();
extern void ut_lcd_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap,const char* str);
extern void ut_lcd_output_mov_mode(bool torch, char* title[6],const char* textX,const char* textY,const char* textZ);
extern void ut_lcd_output_plasma_mode(bool torch, char* title[6],const char* textX,const char* textY,const char* textZ);
extern void ut_lcd_output_manual_mode(bool torch, char* title[7],const char* textX,const char* textY,const char* textZ);
extern void ut_lcd_output_int_var(const char* title,const char* varStr, uint8_t blinkpos, bool blink);
extern void ut_lcd_output_warning(const char* Str);
#endif /* INCLUDE_LCD_H_ */
