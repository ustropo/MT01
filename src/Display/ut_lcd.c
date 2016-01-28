/*
 * ut_lcd.c
 *
 *  Created on: Jan 19, 2016
 *      Author: Fernando
 */

#include "FreeRTOS.h"
#include "task.h"
#include "u8g.h"

#include "ut_lcd.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************
 *  Definitions
 ****************************************************************************************/

#define SCREEN_BUFFER_LEN	SCREEN_HEIGHT * (SCREEN_WIDTH + 7 ) / 8

/* pointer to the start adress of the glyph, points to progmem area */
typedef void * u8g_glyph_t;

extern u8g_glyph_t u8g_GetGlyph(u8g_t *u8g, uint8_t requested_encoding);
extern uint8_t *u8g_font_GetGlyphDataStart(const void *font, u8g_glyph_t g);

/****************************************************************************************
 *  Variables
 ****************************************************************************************/

static uint8_t gabFrameBuffer[SCREEN_BUFFER_LEN];
static u8g_t ut_u8g;
static bool boIsInitialized = false;

/****************************************************************************************
 *  Private functions
 ****************************************************************************************/

/**
 * Prepare u8g library
 */
static void ut_lcd_prepare()
{
	u8g_SetFont(&ut_u8g, u8g_font_6x10);
	u8g_SetFontRefHeightExtendedText(&ut_u8g);
	u8g_SetDefaultForegroundColor(&ut_u8g);
	u8g_SetFontPosTop(&ut_u8g);
}

/**
 * Draw a single pixel into internal frame buffer.
 *
 * @param index 		Index of the pixel.
 * @param pixelColor	Pixel color. 0 -> black; 1 -> white.
 */
static void ut_lcd_buf_drawPixelPos(uint32_t index, uint8_t bit, uint8_t pixelColor)
{
	/* Set or erase the single bit */
	uint8_t bMask = (1 << bit);
	if(pixelColor) { gabFrameBuffer[index] |= bMask; }
	else		   { gabFrameBuffer[index] &= ~bMask;}
}

/**
 * Draw multiple pixels once.
 * @param index		Index in internal buffer.
 * @param pixels
 */
static void ut_lcd_buf_draw8Pixel(uint32_t index, uint8_t pixels)
{
	uint8_t i;
	/* If multiple of 8, copy directly */
	if(index % 8 == 0) { gabFrameBuffer[index / 8] = pixels; return; }
	/* Otherwise, copy one by one */
	for(i = 0; i < 8; i++)
	{
		ut_lcd_buf_drawPixelPos(index / 8, index % 8, pixels & 1);
		pixels >>= 1;
		index++;
	}
}

/***
 * Draw only a line of the image.
 *
 * @param x			Left position.
 * @param y			Top position.
 * @param w			Width.
 * @param bitmap	Bitmap pointer data.
 */
static void ut_lcd_buf_drawHImage(uint8_t x, uint8_t y, uint8_t w, const uint8_t* bitmap)
{
	uint32_t index = x + y*SCREEN_WIDTH;
	uint8_t data = *bitmap;

	/* Adjust to be multiple of 8 */
	while(w >= 8)
	{
		ut_lcd_buf_draw8Pixel(index, *bitmap);
		index += 8;
		w -= 8;
		bitmap++;
	}

	/* Check for leftovers */
	data = *bitmap;
	while(w-- > 0)
	{
		ut_lcd_buf_drawPixelPos(index / 8, index % 8, data & 1);
		data >>= 1;
		index++;
	}
}

/**
 * Draw a single char into frame buffer
 * @param x			Left position. Max of 128.
 * @param y			Top position.  Max of 64.
 * @param encoding	Character.
 */
static void ut_lcd_buf_drawGlyph(uint8_t x, uint8_t y, const uint8_t encoding)
{
	  const uint8_t *data;
	  uint8_t w, h;
	  uint8_t i, j;
	  uint32_t index;

	  {
	    u8g_glyph_t g = u8g_GetGlyph(&ut_u8g, encoding);
	    if ( g == NULL  )
	      return;
	    data = u8g_font_GetGlyphDataStart(ut_u8g.font, g);
	  }

	  w = ut_u8g.glyph_width;
	  h = ut_u8g.glyph_height;

	  x += ut_u8g.glyph_x;
	  y -= ut_u8g.glyph_y;
	  y--;

	  /* TODO: boundaries */

	  /* now, w is reused as bytes per line */
	  w += 7;
	  w /= 8;

	  //iy = y;
	  y -= h;
	  y++;

	  for( j = 0; j < h; j++ )
	  {
	    index = x + y*SCREEN_WIDTH;
	    for( i = 0; i < w; i++ )
	    {
	      ut_lcd_buf_draw8Pixel(index, *data);
	      data++;
	      index+=8;
	    }
	    y++;
	  }
	  return;
}

/****************************************************************************************
 *  Public functions
 ****************************************************************************************/

/**
 * See ut_lcd.h
 */
void ut_lcd_buf_init()
{
	if(!boIsInitialized)
	{
		/* Prepare u8g lib */
		u8g_InitComFn(&ut_u8g, &u8g_dev_st7920_128x64_hw_spi, u8g_com_rx_hw_spi_fn);
		ut_lcd_prepare();

		/* Clean all */
		ut_lcd_buf_clear();

		boIsInitialized = true;
	}

}

/**
 * See ut_lcd.h
 */
void ut_lcd_buf_clear()
{
	/* Clear internal buffer */
	memset(gabFrameBuffer, 0, sizeof(gabFrameBuffer));
}

/**
 * See ut_lcd.h
 */
void ut_lcd_buf_output()
{
	ut_lcd_prepare();
	u8g_FirstPage(&ut_u8g);
	/* Picture loop */
	do
	{
		u8g_DrawXBMP(&ut_u8g, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, gabFrameBuffer);
	} while(u8g_NextPage(&ut_u8g));
}

/**
 * See ut_lcd.h
 * @param line
 * @param column
 * @param text
 * @param invert
 */
void ut_lcd_buf_drawString(uint8_t line, uint8_t column, const char* text, uint8_t invert)
{
	line *= 10;
	line += ut_u8g.font_calc_vref(&ut_u8g);

	while(*text)
	{
		ut_lcd_buf_drawGlyph(column*7, line, *text++);
		column += 1;
	}
}

/**
 * See ut_lcd.h
 * @param x
 * @param y
 * @param w
 * @param h
 * @param bitmap
 */
void ut_lcd_buf_drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap)
{
	uint32_t b = w;
	b += 7;
	b /= 8;

	/* Check for bounds */
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;

	while(h > 0)
	{
		ut_lcd_buf_drawHImage(x, y, w, bitmap);
		bitmap += b;
		y++;
		h--;
	}
}

/**
 * See ut_lcd.h
 * @param x
 * @param y
 * @param color
 */
void ut_lcd_buf_drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	/* Check position */
	if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;
	/* Adjust index of the buffer */
	uint32_t byteIndex = x + (y * SCREEN_WIDTH);
	/* Draw into buffer */
	ut_lcd_buf_drawPixelPos(byteIndex / 8, byteIndex % 8, color);
}
