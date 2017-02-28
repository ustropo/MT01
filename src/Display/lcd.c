/*
 * lcd.c
 *
 *  Created on: Oct 27, 2015
 *      Author: Fernando
 */

#include "FreeRTOS.h"
#include "semphr.h"

#include "u8g.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include "ut_state.h"
#include "eeprom.h"

static void readline(const char *message, char *line, uint8_t *counter);
/* Check if background color is black/white */
static uint8_t gaboBackColor[MAX_COLUMN * MAX_ROW];
/* Frame buffer for lcd */
static char gacChar[MAX_COLUMN * MAX_ROW];

static const char* gacStr[MAX_ROW];
static uint8_t gaboBackColorStr[MAX_ROW];
static uint8_t gabolineMarked[MAX_ROW];
static const u8g_fntpgm_uint8_t *gaboFontStr[MAX_ROW];

/**
 * Global variables
 */
static u8g_t main_u8g;
xSemaphoreHandle rspi_semaphore = 0;

/**
 *
 */
static void u8g_prepare(const uint8_t* font)
{
  u8g_SetFont(&main_u8g, font);
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
	u8g_prepare(u8g_font_6x10);
	rspi_semaphore = xSemaphoreCreateMutex();
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
 * Clear screen frame buffer
 * @param screen_id
 */
void ut_lcd_clear_str()
{
	/* Clean all */
	memset(gaboBackColorStr, 0, sizeof(gaboBackColorStr));
	memset(gacStr, '\n', sizeof(gacStr));
	memset(gaboFontStr, NULL, sizeof(gaboFontStr));
	memset(gabolineMarked, 0, sizeof(gabolineMarked));
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
 *	Draw a string into internal buffer
 *
 * @param line		Line to be drawed (zero based index)
 * @param column	Column (zero based index)
 * @param text		String to draw.
 * @param invert	Which color to draw (0 -> white / 1 -> black)
 */
void ut_lcd_drawStr(uint8_t line, uint8_t column, const char* text, uint8_t invert, uint8_t lineMarked, const uint8_t* font)
{
	/* Check limits */
	if(line >= MAX_ROW || column >= MAX_COLUMN) return;

	/* Copy each char individually */
	gacStr[line] = text;
	gaboBackColorStr[line] = invert;
	gaboFontStr[line] = (const u8g_fntpgm_uint8_t *)font;
	gabolineMarked[line] = lineMarked;
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
		//u8g_DrawFrame(&main_u8g, x, y, w, h);
		u8g_SetColorIndex(&main_u8g, 0);
	}
	/* Draw glyph */
	y += main_u8g.font_calc_vref(&main_u8g);
	u8g_draw_glyph(&main_u8g, x, y, glyph);
	/* Return to future info */
	return w;
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
static uint8_t ut_lcd_draw_str(uint8_t x, uint8_t y, uint8_t h, uint8_t invert, uint8_t lineMarked, const char* str)
{
	uint8_t w = u8g_GetStrWidth(&main_u8g, str);

	/* Set background box */
	u8g_SetColorIndex(&main_u8g, 1);
	switch(invert)
	{
	case BACKGROUND_NORMAL:
		break;
	case BACKGROUND_INVERTED: u8g_DrawBox(&main_u8g, x, y+1, w, h-2);
							  u8g_SetColorIndex(&main_u8g, 0);
		break;
	case BACKGROUND_FRAMED: //u8g_DrawFrame(&main_u8g, x, y, 128, h);
							  u8g_DrawHLine(&main_u8g, x, h-1, 128);
		break;
	case BACKGROUND_FRAMED_2: //u8g_DrawFrame(&main_u8g, x, y, 128, h);
							  u8g_DrawHLine(&main_u8g, x, ((2*h)-1), 128);
		break;
	}
	/* Draw Str */
	u8g_DrawStr(&main_u8g, x, y, str);
	if(lineMarked)
	{
		x = u8g_GetStrWidth(&main_u8g,str);
		u8g_prepare(u8g_font_m2icon_7);
		u8g_DrawStr(&main_u8g, x + 5, y, "\x44\n");
	}

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
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_prepare(u8g_font_6x10);
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
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_output_str(ut_state state)
{
	uint8_t row, x, y;
	uint8_t h;
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_prepare(gaboFontStr[0]);
	h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
	//u8g_prepare();
	u8g_FirstPage(&main_u8g);

	/* Picture loop */
	do
	{
		/* Through all rows */
		y = 0;
		for(row = 0; row < MAX_ROW; row++)
		{
			u8g_prepare(u8g_font_6x10);
			u8g_SetFont(&main_u8g, gaboFontStr[row]);
			u8g_SetFontRefHeightExtendedText(&main_u8g);
			u8g_SetDefaultForegroundColor(&main_u8g);
			u8g_SetFontPosTop(&main_u8g);
		//	u8g_FirstPage(&main_u8g);
			x = 0;
			/* Through all columns */
				/* Draw glyph */
			x = ut_lcd_draw_str(x, y, h, gaboBackColorStr[row],gabolineMarked[row], gacStr[row]);
			/* Next position */
			y += h;
			if (state == STATE_MAIN_MENU)
			{
				u8g_prepare(u8g_font_5x8);
				u8g_DrawHLine(&main_u8g, x, 55, 128);
				if(configFlags[MODOMAQUINA])
					u8g_DrawStr(&main_u8g, 60, 56, "MODO OXICORTE");
				else
					u8g_DrawStr(&main_u8g, 65, 56, "MODO PLASMA");
			}
		}

	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap,const char* str)
{
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_prepare(u8g_font_5x8);
	u8g_FirstPage(&main_u8g);
	do{
		u8g_DrawXBMP(&main_u8g, x, y, w, h, bitmap);
		u8g_DrawStr(&main_u8g, x+w-80, y+h+1, str);
	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_AUTO_TITLE		"MODO AUTOMÁTICO"
#define DEFAULT_DESCOLA_TITLE	"RODANDO"
#define DEFAULT_AVISO_MANUAL	"ENTER DISPARA/ESC VOLTA"
#define DEFAULT_TOCHA			"TOCHA"
#define DEFAULT_ACESA			"ACESA"
#define DEFAULT_APAGADA			"APAGADA"

#define warning_width 25
#define warning_height 22
static unsigned char warning_bits[] = {
   0x00, 0x10, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00,
   0x00, 0x6c, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00,
   0x00, 0xbb, 0x01, 0x00, 0x00, 0x39, 0x01, 0x00, 0x80, 0x39, 0x03, 0x00,
   0xc0, 0x38, 0x02, 0x00, 0xc0, 0x38, 0x04, 0x00, 0x60, 0x38, 0x0c, 0x00,
   0x20, 0x38, 0x08, 0x00, 0x30, 0x38, 0x18, 0x00, 0x10, 0x38, 0x10, 0x00,
   0x18, 0x00, 0x30, 0x00, 0x0c, 0x38, 0x60, 0x00, 0x0c, 0x38, 0x60, 0x00,
   0x06, 0x38, 0xc0, 0x00, 0x02, 0x00, 0x80, 0x00, 0xff, 0xff, 0xff, 0x01,
   0xff, 0xff, 0xff, 0x01 };

void ut_lcd_output_mov_mode(bool torch, char* title[6],const char* textX,const char* textY,const char* textZ)
{
	uint8_t row, x, y;
	uint8_t h;
	const char* str;
//	u8g_prepare(u8g_font_6x10);
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_FirstPage(&main_u8g);
	h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
	/* Picture loop */
	do
	{
		/* Through all rows */
		y = 0;
		u8g_DrawVLine(&main_u8g, 90, 11, 43);
		u8g_prepare(u8g_font_5x8);
		str = DEFAULT_TOCHA;
		u8g_DrawStr(&main_u8g, 95, 38, str);
		if(!torch)
		{
			str = DEFAULT_ACESA;
			u8g_DrawXBMP(&main_u8g, 94, 13, warning_width, warning_height, warning_bits);
			u8g_DrawStr(&main_u8g, 95, 46, str);
			u8g_DrawStr(&main_u8g, 112, 12, title[3]);
		}
		else
		{
			str = DEFAULT_APAGADA;
			u8g_DrawStr(&main_u8g, 92, 46, str);
		}

		for(row = 0; row < MAX_ROW+1; row++)
		{
			x = 0;
			switch(row)
			{
				case 0: str = title[0];
						u8g_prepare(u8g_font_helvB08);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						u8g_DrawHLine(&main_u8g, x, h-1, 128);
						break;
				case 1: str = title[2];
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 3;
						break;
				case 5: str = "";
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) -1;
						break;
				case 6: str = title[1];
						u8g_prepare(u8g_font_5x8);
						u8g_DrawStr(&main_u8g, (u8g_GetStrWidth(&main_u8g,str) + 25), y, title[4]);
						u8g_DrawHLine(&main_u8g, x, y-1, 128);
						break;
				case 2: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "X:");
						str = textX;
						x = 12;
						u8g_prepare(u8g_font_6x10);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						break;
				case 3: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Y:");
						u8g_prepare(u8g_font_6x10);
						str = textY;
						x = 12;
						break;
				case 4: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Z:");
						u8g_prepare(u8g_font_6x10);
						str = textZ;
						x = 12;
						break;
			}
				/* Draw glyph */
			u8g_DrawStr(&main_u8g, x, y, str);

			/* Next position */
			y += h;
		}

	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_output_plasma_mode(bool torch, char* title[7],const char* textX,const char* textY,const char* textZ)
{
	uint8_t row, x, y;
	uint8_t h;
	const char* str;
//	u8g_prepare(u8g_font_6x10);
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_FirstPage(&main_u8g);
	h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
	/* Picture loop */
	do
	{
		/* Through all rows */
		y = 0;
		u8g_DrawVLine(&main_u8g, 90, 11, 38);
//		u8g_DrawHLine(&main_u8g, 0, 11+45, 128);
		u8g_prepare(u8g_font_4x6r);
		str = DEFAULT_TOCHA;
		u8g_DrawStr(&main_u8g, 95, 35, str);
		u8g_DrawStr(&main_u8g, 92, 12, title[6]);
		if(!torch)
		{
			str = DEFAULT_ACESA;
			u8g_DrawXBMP(&main_u8g, 94, 13, warning_width, warning_height, warning_bits);
			u8g_DrawStr(&main_u8g, 95, 42, str);
			u8g_DrawStr(&main_u8g, 112, 12, title[3]);
		}
		else
		{
			str = DEFAULT_APAGADA;
			u8g_DrawStr(&main_u8g, 92, 42, str);
		}
		for(row = 0; row < MAX_ROW+1; row++)
		{
			x = 0;
			switch(row)
			{
				case 0: str = title[0];
						u8g_prepare(u8g_font_helvB08);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						u8g_DrawHLine(&main_u8g, x, h-1, 128);
						break;
				case 1: str = title[2];
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						break;
				case 5: str = "";
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) -1;
						break;
				case 6: str = title[1];
						u8g_prepare(u8g_font_4x6r);

						u8g_DrawStr(&main_u8g, (u8g_GetStrWidth(&main_u8g,str) + 25), y-5, title[4]);
						u8g_DrawStr(&main_u8g, (u8g_GetStrWidth(&main_u8g,str) + 25), y+2, title[5]);
			//			h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						u8g_prepare(u8g_font_5x8);
						u8g_DrawHLine(&main_u8g, x, y-6, 128);
						y = y-2;
						break;
				case 2: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "X:");
						str = textX;
						x = 12;
						u8g_prepare(u8g_font_6x10);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						break;
				case 3: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Y:");
						u8g_prepare(u8g_font_6x10);
						str = textY;
						x = 12;
						break;
				case 4: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Z:");
						u8g_prepare(u8g_font_6x10);
						str = textZ;
						x = 12;
						break;
			}
				/* Draw glyph */
			u8g_DrawStr(&main_u8g, x, y, str);

			/* Next position */
			y += h;
		}

	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_output_manual_mode(bool torch, char* title[7],const char* textX,const char* textY,const char* textZ)
{
	uint8_t row, x, y;
	uint8_t h;
	const char* str;
//	u8g_prepare(u8g_font_6x10);
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_FirstPage(&main_u8g);
	h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
	/* Picture loop */
	do
	{
		/* Through all rows */
		y = 0;
		u8g_DrawVLine(&main_u8g, 90, 11, 38);
//		u8g_DrawHLine(&main_u8g, 0, 11+45, 128);
		u8g_prepare(u8g_font_4x6r);
		str = DEFAULT_TOCHA;
		u8g_DrawStr(&main_u8g, 95, 35, str);
		u8g_DrawStr(&main_u8g, 92, 12, title[6]);
		if(!torch)
		{
			str = DEFAULT_ACESA;
			u8g_DrawXBMP(&main_u8g, 94, 13, warning_width, warning_height, warning_bits);
			u8g_DrawStr(&main_u8g, 95, 42, str);
		}
		else
		{
			str = DEFAULT_APAGADA;
			u8g_DrawStr(&main_u8g, 92, 42, str);
		}
		u8g_DrawStr(&main_u8g, 112, 12, title[3]);
		for(row = 0; row < MAX_ROW+1; row++)
		{
			x = 0;
			switch(row)
			{
				case 0: str = title[0];
						u8g_prepare(u8g_font_helvB08);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						u8g_DrawHLine(&main_u8g, x, h-1, 128);
						break;
				case 1: str = title[2];
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						break;
				case 5: str = "";
						u8g_prepare(u8g_font_5x8);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) -1;
						break;
				case 6: str = title[1];
						u8g_prepare(u8g_font_4x6r);

						u8g_DrawStr(&main_u8g, (u8g_GetStrWidth(&main_u8g,str) + 25), y-5, title[4]);
						u8g_DrawStr(&main_u8g, (u8g_GetStrWidth(&main_u8g,str) + 25), y+2, title[5]);
			//			h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						u8g_prepare(u8g_font_5x8);
						u8g_DrawHLine(&main_u8g, x, y-6, 128);
						y = y-2;
						break;
				case 2: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "X:");
						str = textX;
						x = 12;
						u8g_prepare(u8g_font_6x10);
						h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
						break;
				case 3: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Y:");
						u8g_prepare(u8g_font_6x10);
						str = textY;
						x = 12;
						break;
				case 4: u8g_prepare(u8g_font_helvB08);
						u8g_DrawStr(&main_u8g, x, y, "Z:");
						u8g_prepare(u8g_font_6x10);
						str = textZ;
						x = 12;
						break;
			}
				/* Draw glyph */
			u8g_DrawStr(&main_u8g, x, y, str);

			/* Next position */
			y += h;
		}

	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_output_int_var(const char* title,const char* varStr, uint8_t blinkpos, bool blink)
{
	char str2Show[20];
	uint8_t h;
	uint8_t endPos = 0, pointPos = 0;

	strcpy(str2Show,varStr);
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_FirstPage(&main_u8g);

	while (str2Show[endPos] != ' ')
	{
		if (str2Show[endPos] == '.')
			pointPos = endPos;
		endPos++;
	}
	pointPos = endPos - pointPos;
	if (blink)
	{

		if (blinkpos >=  pointPos && pointPos < endPos)
			str2Show[endPos - blinkpos - 1] = 0x20;
		else
			str2Show[endPos - blinkpos] = 0x20;
	}
	/* Picture loop */
	do
	{
		u8g_DrawFrame(&main_u8g,3, 10, 120, 44);
		u8g_prepare(u8g_font_6x10);
		h = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
		u8g_DrawStr(&main_u8g, 0, 11, title);
		u8g_DrawHLine(&main_u8g, 3, h+11, 120);
		u8g_DrawStr(&main_u8g, 33, h+20, str2Show);
	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

void ut_lcd_output_warning(const char* Str)
{
	uint8_t h,hfont,x,y;
	uint8_t lineSize;
	uint8_t lineSizeMax = 0;
	char line[3][21];
	uint8_t count;
	const char *message = Str;
	count = 0;
	xSemaphoreTake(rspi_semaphore, portMAX_DELAY);
	u8g_FirstPage(&main_u8g);
	u8g_prepare(u8g_font_helvB08);
	hfont = u8g_GetFontAscent(&main_u8g) - u8g_GetFontDescent(&main_u8g) + 1;
	readline(message,line[0],&count);
	readline(message,line[1],&count);
	readline(message,line[2],&count);
	h = 3*hfont;
	if(line[2][0] == 0)
	{
		h -= hfont;
		if(line[1][0] == 0)
		{
			h -= hfont;
		}
	}

	/* Picture loop */
	do
	{
		lineSize = u8g_GetStrWidth(&main_u8g,line[0]);
		lineSizeMax = max(lineSizeMax,lineSize);
		x = (128 - lineSize)/2;
		y = (64-h)/2;
		u8g_DrawStr(&main_u8g, x, y, line[0]);

		lineSize = u8g_GetStrWidth(&main_u8g,line[1]);
		lineSizeMax = max(lineSizeMax,lineSize);
		x = (128 - lineSize)/2;
		y += hfont;
		u8g_DrawStr(&main_u8g, x, y, line[1]);

		lineSize = u8g_GetStrWidth(&main_u8g,line[2]);
		lineSizeMax = max(lineSizeMax,lineSize);
		x = (128 - lineSize)/2;
		y += hfont;
		u8g_DrawStr(&main_u8g, x, y, line[2]);

		x = (128 - lineSizeMax)/2;
		y = (64-h)/2;
		u8g_DrawFrame(&main_u8g,(x-3), (y-3), (lineSizeMax+6), (h+6));
	} while(u8g_NextPage(&main_u8g));
	xSemaphoreGive(rspi_semaphore);
}

static void readline(const char *message, char *line, uint8_t *counter){

    int index = 0;
	if(message[*counter] != '\0')
	{
		while(message[*counter] != '\n'){
			line[index] = message[*counter];
			(*counter)++;
			index ++;
		}
		(*counter)++;
	}
	line[index] = 0;
}
