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
#include "timers.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"

#include "planner.h"

#define DEFAULT_UPDATE_TIMEOUT	portMAX_DELAY
#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_AUTO_TITLE		"MODO AUTOMÁTICO"

void vTimerUpdateCallback( TimerHandle_t pxTimer );
TimerHandle_t TimerUpdate;

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

	sprintf(text, "X: %10.4f mm", x);
	ut_lcd_drawString(2, 0, text, false);
	sprintf(text, "Y: %10.4f mm", y);
	ut_lcd_drawString(3, 0, text, false);
	sprintf(text, "Z: %10.4f mm", z);
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
	tg_set_primary_source(XIO_DEV_COMMAND);
	iif_bind_jog();
	TimerUpdate = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) 2,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	xTimerStart( TimerUpdate, 0 );

	while(true)
	{
		/* Wait for user interaction */
		keyEntry = 0;
		xQueueReceive( qKeyboard, &keyEntry, DEFAULT_UPDATE_TIMEOUT);

		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			iif_func_down();
			break;

		case KEY_UP:
			iif_func_up();
			break;

		case KEY_RIGHT:
			iif_func_right();
			break;

		case KEY_LEFT:
			iif_func_left();
			break;

		case KEY_Z_UP:
			iif_func_zup();
			break;

		case KEY_Z_DOWN:
			iif_func_zdown();
			break;

		case KEY_ESC:
			xTimerStop( TimerUpdate, 0 );
			iif_bind_idle();
			return STATE_CONFIG_MANUAL_MODE;

		case KEY_RELEASED:
			iif_func_released();
			break;
		/* TODO: operate machine - with other keys */
		default:
			break;
		}

		/* Update position */
	//	updatePosition(NULL);
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
	uint8_t lstop = 0;

	/* Clear display */
	updatePosition(DEFAULT_AUTO_TITLE);
	TimerUpdate = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) 2,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	xTimerStart( TimerUpdate, 0 );
	tg_set_primary_source(XIO_DEV_USBFAT);
	xio_close(cs.primary_src);
	xio_open(cs.primary_src,0,0);
	iif_bind_filerunning();

	while(true)
	{
		/* Wait for user interaction */
		keyEntry = 0;
		xQueueReceive( qKeyboard, &keyEntry, DEFAULT_UPDATE_TIMEOUT);

		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			iif_func_down();
			break;

		case KEY_UP:
			iif_func_up();
			break;

		case KEY_RIGHT:
			iif_func_right();
			break;

		case KEY_LEFT:
			iif_func_left();
			break;

		case KEY_Z_UP:
			iif_func_zup();
			break;

		case KEY_Z_DOWN:
			iif_func_zdown();
			break;

		case KEY_ENTER:
			if (lstop == 1)
			{
				iif_func_enter();
				lstop = 0;
			}
			break;

		case KEY_ESC:
			xTimerStop( TimerUpdate, 0 );
			iif_bind_idle();
			return STATE_MAIN_MENU;
			break;

		case KEY_RELEASED:
			iif_func_released();
			break;
		/* TODO: operate machine - with other keys */
		default:
			break;
		}

		/* Update position */
	//	updatePosition(NULL);
	}

	return STATE_MAIN_MENU;
}

void vTimerUpdateCallback( TimerHandle_t pxTimer )
{
	updatePosition(NULL);
}
