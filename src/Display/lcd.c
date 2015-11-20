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

typedef struct
{
	char invert;
	char line[MAX_COLUMN + 1];
} ut_frame_row;

/**
 * Global types
 */
typedef struct
{
	char enable_footer;
	char enable_header;
	ut_frame_row header;
	ut_frame_row footer;
	ut_frame_row buffer[MAX_ROW];
} ut_frame_buffer;

/**
 * Global variables
 */
static u8g_t main_u8g;
static ut_frame_buffer frames;

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
	/* Disable all */
	memset((void *)&frames, 0, sizeof(ut_frame_buffer));
}

/**
 * Clear screen frame buffer
 * @param screen_id
 */
void ut_lcd_clear(uint8_t screen_id)
{
	uint8_t i = 0;

	switch(screen_id)
	{
	case SCREEN_FOOTER_ID:
		memset(frames.footer.line, ' ', MAX_COLUMN);
		frames.footer.line[MAX_COLUMN] = 0;
		frames.footer.invert = false;
		break;
	case SCREEN_HEADER_ID:
		memset(frames.header.line, ' ', MAX_COLUMN);
		frames.footer.line[MAX_COLUMN] = 0;
		frames.header.invert = false;
		break;
	case SCREEN_MAIN_ID:
		for(i = 0; i < MAX_ROW; i++)
		{
			memset(&frames.buffer[i].line, ' ', MAX_COLUMN);
			frames.footer.line[MAX_COLUMN] = 0;
			frames.buffer[i].invert = false;
		}
		break;
	}
}

/**
 *
 * @param enable
 */
void ut_lcd_enableHeader(uint8_t enable)
{
	frames.enable_header = enable;
}

/**
 *
 * @param enable
 */
void ut_lcd_enableFooter(uint8_t enable)
{
	frames.enable_footer = enable;
}

/**
 *
 * @param screen_id
 * @param line
 * @param column
 * @param text
 */
void ut_lcd_drawString(uint8_t screen_id, uint8_t line, uint8_t column, const char* text, uint8_t invert)
{

	if(line >= MAX_ROW && column >= MAX_COLUMN) return;

	switch(screen_id)
	{
	case SCREEN_MAIN_ID:
		strncpy(&frames.buffer[line].line[column], text, MAX_COLUMN - column);
		frames.buffer[line].invert = invert;
		break;
	case SCREEN_FOOTER_ID:
		strncpy(&frames.footer.line[column], text, MAX_COLUMN - column);
		frames.footer.invert = invert;
		break;
	case SCREEN_HEADER_ID:
		strncpy(&frames.header.line[column], text, MAX_COLUMN - column);
		frames.header.invert = invert;
		break;
	}
}

/**
 * Draw header
 */
static void lcd_draw_header(uint8_t w, uint8_t h)
{
	u8g_SetColorIndex(&main_u8g, 1);
	if(frames.header.invert)
	{
		u8g_DrawBox(&main_u8g, 0, 0, w, h);
		u8g_SetColorIndex(&main_u8g, 0);
	}
	u8g_DrawStr(&main_u8g, 0, 0, frames.header.line);
}

/**
 *
 * @param w
 * @param h
 */
static void lcd_draw_footer(uint8_t w, uint8_t h)
{
	u8g_SetColorIndex(&main_u8g, 1);
	if(frames.footer.invert)
	{
		u8g_DrawBox(&main_u8g, 0, (MAX_ROW - 1) * h, w, h);
		u8g_SetColorIndex(&main_u8g, 0);
	}
	u8g_DrawStr(&main_u8g, 0, (MAX_ROW - 1) * h, frames.footer.line);
}

/**
 *
 * @param line
 * @param w
 * @param h
 */
static void lcd_draw_main(uint8_t offset, uint8_t line, uint8_t w, uint8_t h)
{
	u8g_SetColorIndex(&main_u8g, 1);
	if(frames.buffer[line].invert)
	{
		u8g_DrawBox(&main_u8g, 0, (line + offset) * h, w, h);
		u8g_SetColorIndex(&main_u8g, 0);
	}
	u8g_DrawStr(&main_u8g, 0, (line + offset) * h, frames.buffer[line].line);
}

/**
 * Picture loop
 */
void ut_lcd_output()
{
	uint8_t w = u8g_GetWidth(&main_u8g);
	uint8_t offset = (frames.enable_header) ? 1 : 0;
	uint8_t max = (frames.enable_footer) ? MAX_ROW - 1 : MAX_ROW;
	uint8_t i;
	uint8_t h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;

	u8g_prepare();
	u8g_FirstPage(&main_u8g);

	/* Picture loop */
	do
	{
		/* Header */
		if(frames.enable_header) lcd_draw_header(w, h);

		/* Footer */
		if(frames.enable_footer) lcd_draw_footer(w, h);

		/* Main */
		for(i = 0; (i + offset) < max; i++)	lcd_draw_main(offset, i, w, h);

	} while(u8g_NextPage(&main_u8g));
}
