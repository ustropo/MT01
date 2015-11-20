/*
 * ut_state_splash.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"

/**
 * Splash screen text
 */
static const char* gszSplash[MAX_ROW] =
{
		/* "12345678901234567890" */
		   "",
		   "      METALIQUE     ",
		   "",
		   "     PLASMA  CNC    ",
		   "    REVISAO  2.43   ",
		   "",
};

/**
 * Execute a splash screen and
 * do some other initialization.
 *
 * @param pContext Context pointer
 * @return Next state.
 */
ut_state ut_state_splash(ut_context* pContext)
{
	uint8_t i;

	ut_lcd_clear(SCREEN_MAIN_ID);
	ut_lcd_clear(SCREEN_FOOTER_ID);
	ut_lcd_clear(SCREEN_HEADER_ID);

	for(i = 0; i < MAX_ROW; i++)
	{
		ut_lcd_drawString(SCREEN_MAIN_ID, i, 0, gszSplash[i], false);
	}
	ut_lcd_output();

	/* Delay */
	vTaskDelay(2500 / portTICK_PERIOD_MS);

	/* Next state */
	return STATE_WARNING;
}

