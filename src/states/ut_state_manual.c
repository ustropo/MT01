/*
 * ut_state_manual.c
 *
 *  Created on: Dec 4, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"

#include "planner.h"

#define DEFAULT_UPDATE_TIMEOUT	1000
#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_AUTO_TITLE		"MODO AUTOMATICO"

/**
 * Update machine position
 * @param szTitle
 */
static void updatePosition(const char* szTitle)
{
	char text[MAX_COLUMN];
	float x; float y; float z;

	memset(text, 0, sizeof(text));

	/* Display is only cleared once to improve performance */
	if(szTitle)
	{
		ut_lcd_clear();
		/* Title */
		ut_lcd_drawString(0, 0, szTitle, false);
	}

	/* TODO: get position from machine */
	x = mp_get_runtime_absolute_position(0);
	y = mp_get_runtime_absolute_position(1);
	z = mp_get_runtime_absolute_position(2);

	sprintf(text, "X: %10.4f", x);
	ut_lcd_drawString(2, 0, text, false);
	sprintf(text, "Y: %10.4f", y);
	ut_lcd_drawString(3, 0, text, false);
	sprintf(text, "Z: %10.4f", z);
	ut_lcd_drawString(4, 0, text, false);

	/* Put it into screen */
	ut_lcd_output();
}

/**
 * Put machine into manual mode state.
 *
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_manual_mode(ut_context* pContext)
{
	uint32_t keyEntry;

	/* Clear display */
	updatePosition(DEFAULT_MANUAL_TITLE);

	while(true)
	{
		/* Wait for user interaction */
		keyEntry = 0;
		xQueueReceive( qKeyboard, &keyEntry, DEFAULT_UPDATE_TIMEOUT);

		/* Check which key */
		switch (keyEntry)
		{
		case KEY_ESC:
			return STATE_MAIN_MENU;

		/* TODO: operate machine - with other keys */
		default:
			break;
		}

		/* Update position */
		updatePosition(NULL);
	}

	return STATE_MAIN_MENU;
}

/**
 * Put machine into automatic mode.
 *
 * @param pContext Context object.
 * @return Main menu state
 */
ut_state ut_state_auto_mode(ut_context* pContext)
{
	uint32_t keyEntry;

	/* Clear display */
	updatePosition(DEFAULT_AUTO_TITLE);

	while(true)
	{
		/* Wait for user interaction */
		keyEntry = 0;
		xQueueReceive( qKeyboard, &keyEntry, DEFAULT_UPDATE_TIMEOUT);

		/* Check which key */
		switch (keyEntry)
		{
		case KEY_ESC:
			return STATE_MAIN_MENU;

			/* TODO: operate machine in auto mode*/
		default:
			break;
		}

		/* Update position */
		updatePosition(NULL);
	}

		return STATE_MAIN_MENU;

	return STATE_MAIN_MENU;
}
