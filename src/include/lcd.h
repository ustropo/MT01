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

#define MAX_ROW	6
#define MAX_COLUMN	20

#define SCREEN_HEADER_ID	0
#define SCREEN_MAIN_ID		1
#define SCREEN_FOOTER_ID	2

extern void ut_lcd_init();
extern void ut_lcd_clear();
extern void ut_lcd_drawString(uint8_t line, uint8_t column, const char* text, uint8_t invert);
extern void ut_lcd_output();
extern void ut_lcd_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);

#endif /* INCLUDE_LCD_H_ */
