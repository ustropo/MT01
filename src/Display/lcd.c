/*
 * lcd.c
 *
 *  Created on: Oct 27, 2015
 *      Author: Fernando
 */

#include "FreeRTOS.h"

#include "u8g.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>

/* Check if background color is black/white */
static uint8_t gaboBackColor[MAX_COLUMN * MAX_ROW];
/* Frame buffer for lcd */
static char gacChar[MAX_COLUMN * MAX_ROW];

/**
 * Global variables
 */
static u8g_t main_u8g;


/**
 *
 */
static void u8g_prepare(void)
{
  u8g_SetFont(&main_u8g, u8g_font_6x10);
  u8g_SetFontRefHeightExtendedText(&main_u8g);
  u8g_SetDefaultForegroundColor(&main_u8g);
  u8g_SetFontPosTop(&main_u8g);
}

/**
 * Initialize components
 */
void ut_lcd_init()
{
	/* Prepare u8g lib */
	u8g_InitComFn(&main_u8g, &u8g_dev_st7920_128x64_hw_spi, u8g_com_rx_hw_spi_fn);
	u8g_prepare();

	/* Clean all */
	ut_lcd_clear();
}

/**
 * Clear screen frame buffer
 * @param screen_id
 */
void ut_lcd_clear()
{
	/* Clean all */
	memset(gaboBackColor, 0, sizeof(gaboBackColor));
	memset(gacChar, ' ', sizeof(gacChar));
}


/**
 *	Draw a string into internal buffer
 *
 * @param line		Line to be drawed (zero based index)
 * @param column	Column (zero based index)
 * @param text		String to draw.
 * @param invert	Which color to draw (0 -> white / 1 -> black)
 */
void ut_lcd_drawString(uint8_t line, uint8_t column, const char* text, uint8_t invert)
{
	/* Check limits */
	if(line >= MAX_ROW || column >= MAX_COLUMN) return;

	/* Copy each char individually */
	uint8_t index = line * MAX_COLUMN + column;
	while(*text && column++ < MAX_COLUMN)
	{
		gacChar[index] = *text++;
		gaboBackColor[index++] = invert;
	}
}


/**
 * Draw a single char into screen
 * @param x				X position
 * @param y				Y position
 * @param w				Char width
 * @param h				Char height
 * @param invert		Background color
 * @param glyph			Char
 */
static uint8_t ut_lcd_draw_glyph(uint8_t x, uint8_t y, uint8_t h, uint8_t invert, char glyph)
{
	/* char width */
	char szOnlyChar[2] = {0};
	szOnlyChar[0] = glyph;
	uint8_t w = u8g_GetStrWidth(&main_u8g, szOnlyChar);

	/* Set background box */
	u8g_SetColorIndex(&main_u8g, 1);
	if(invert)
	{
		u8g_DrawBox(&main_u8g, x, y, w, h);
		u8g_SetColorIndex(&main_u8g, 0);
	}
	/* Draw glyph */
	y += main_u8g.font_calc_vref(&main_u8g);
	u8g_draw_glyph(&main_u8g, x, y, glyph);
	/* Return to future info */
	return w;
}

/**
 * Picture loop
 */
void ut_lcd_output()
{
	uint8_t row, col, x, y, index;
	uint8_t h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;

	u8g_prepare();
	u8g_FirstPage(&main_u8g);

	/* Picture loop */
	do
	{
		/* Through all rows */
		index = 0;
		y = 0;
		for(row = 0; row < MAX_ROW; row++)
		{
			x = 0;
			/* Through all columns */
			for(col = 0; col < MAX_COLUMN; col++)
			{
				/* Draw glyph */
				x += ut_lcd_draw_glyph(x, y, h, gaboBackColor[index], gacChar[index]);
				/* Next position */
				index++;
			}

			/* Next position */
			y += h;
		}

	} while(u8g_NextPage(&main_u8g));
}
