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

char textXStr[MAX_COLUMN];
char textYStr[MAX_COLUMN];
char textZStr[MAX_COLUMN];
uint8_t gTitle;
extern uint16_t velocidadeJog;


#define DEFAULT_UPDATE_TIMEOUT	portMAX_DELAY
#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_AUTO_TITLE		"MODO AUTOMÁTICO"
#define DEFAULT_DESCOLA_TITLE	"DESLOCANDO"
#define DEFAULT_AVISO_MANUAL	"ENTER DISPARA/ESC VOLTA"
#define DEFAULT_AVISO_AUTO	    "ESC PARA A MAQUINA"
#define DEFAULT_AVISO_DESLOCA	"ESC VOLTA"
#define DEFAULT_LINHA1_MANUAL	"VELOCIDADE:         "
#define DEFAULT_LINHA1_AUTO	    ""
#define DEFAULT_LINHA1_DESLOCA	""

enum {
	MANUAL = 0,
	AUTO,
	DESLOCA,
};

static char gStrManual[3][24] =
{
	DEFAULT_MANUAL_TITLE,
	DEFAULT_AVISO_MANUAL,
};

static char gStrAuto[3][24] =
{
	DEFAULT_AUTO_TITLE,
	DEFAULT_AVISO_AUTO,
	""
};

static char gStrDesloca[3][24] =
{
	DEFAULT_DESCOLA_TITLE,
	DEFAULT_AVISO_DESLOCA,
	""
};

void vTimerUpdateCallback( TimerHandle_t pxTimer );
TimerHandle_t TimerUpdate;

/**
 * Update machine position
 * @param szTitle
 */
static void updatePosition(uint8_t menu)
{
	float x; float y; float z;
	char *lStr[3];
	/* Display is only cleared once to improve performance */

	switch(menu)
	{
		case MANUAL: lStr[0] = gStrManual[0];
					 lStr[1] = gStrManual[1];
			         sprintf(gStrManual[2], "VEL.: %d mm/s", velocidadeJog);
			         lStr[2] = gStrManual[2];
			         break;
		case AUTO:   lStr[0] = gStrAuto[0];
		 	 	 	 lStr[1] = gStrAuto[1];
                     lStr[2] = gStrAuto[2];
                     break;
		case DESLOCA: lStr[0] = gStrDesloca[0];
					  lStr[0] = gStrDesloca[1];
					  lStr[0] = gStrDesloca[2];
					  break;
	}
	/* TODO: get position from machine */
	x = mp_get_runtime_absolute_position(0);
	y = mp_get_runtime_absolute_position(1);
	z = mp_get_runtime_absolute_position(2);

	sprintf(textXStr, "X:%4.2f mm", x);
	sprintf(textYStr, "Y:%4.2f mm", y);
	sprintf(textZStr, "Z:%4.2f mm", z);

	/* Put it into screen */
	ut_lcd_output_manual_mode(TORCH,
			lStr,
			(const char *)textXStr,
			(const char *)textYStr,
			(const char *)textZStr);
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
	updatePosition(MANUAL);
	gTitle = MANUAL;
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
			iif_func_esc();
			return STATE_CONFIG_MANUAL_MODE;

		case KEY_ENTER:
			iif_func_enter();
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
	updatePosition(AUTO);
	gTitle = AUTO;
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
			return STATE_CONFIG_AUTO_MODE;
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

ut_state ut_state_deslocaZero_mode(ut_context* pContext)
{
	uint32_t keyEntry;

	/* Clear display */
	updatePosition(DESLOCA);
	gTitle = DESLOCA;
	iif_bind_idle();
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

void vTimerUpdateCallback( TimerHandle_t pxTimer )
{
	updatePosition(gTitle);
}
