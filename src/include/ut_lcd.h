/*
 * ut_lcd.h
 *
 *  Created on: Jan 19, 2016
 *      Author: Fernando
 */

#ifndef INCLUDE_UT_LCD_H_
#define INCLUDE_UT_LCD_H_

#include "FreeRTOS.h"
#include "task.h"

#define SCREEN_WIDTH	128
#define SCREEN_HEIGHT	64


/**
 * Initialize internal buffer.
 *
 * This function must be called in first place. Only once.
 */
extern void ut_lcd_buf_init();

/**
 * Clear internal buffer.
 */
extern void ut_lcd_buf_clear();

/**
 * Put internal buffer into the screen.
 */
extern void ut_lcd_buf_output();

/**
 * Draw a string into the buffer.
 *
 * @param line     Line to be drawn. Range 0 to 5.
 * @param column   Column to be drawn. Range 0 to 20.
 * @param text	   Text to be drawn.
 * @param invert   Which color to draw. 0 - White; 1 - Black;
 */
extern void ut_lcd_buf_drawString(uint8_t line, uint8_t column, const char* text, uint8_t invert);

/**
 * Draw a bitmap XMP image into the buffer.
 *
 * @param x			Left position. Max of 128.
 * @param y			Top position. Max of 64.
 * @param w			Width of image, in pixels.
 * @param h			Height of image, in pixels.
 * @param bitmap	Image pointer data.
 */
extern void ut_lcd_buf_drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);

/**
 * Draw a single pixel into internal buffer.
 *
 * @param x			Left position. Max of 128.
 * @param y			Top position. Max of 128.
 * @param color		Color of the pixel. 0 -> black; 1 -> white.
 */
extern void ut_lcd_buf_drawPixel(uint8_t x, uint8_t y, uint8_t color);

#endif /* INCLUDE_UT_LCD_H_ */
