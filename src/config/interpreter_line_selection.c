/*
 * interpreter_jog_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "tinyg.h"
#include "platform.h"
#include "planner.h"
#include "interpreter_if.h"
#include "controller.h"
#include "macros.h"
#include "keyboard.h"
#include "config_SwTimers.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum{
	NONE = 0,
	DOWN,
	UP,
};

static uint8_t timeNum = NONE;
static void iif_line_down(void);
static void iif_line_up(void);
static void iif_line_released(void);
void vTimerLineCallback( TimerHandle_t pxTimer );

void iif_bind_line_selection(void)
{
	timeNum = NONE;
	if(swTimers[LINE_SELECTION_TIMER] == NULL)
	{
		swTimers[LINE_SELECTION_TIMER] = xTimerCreate
						   ( "Timer 1",
							 ( 250 ),
							 pdTRUE,
							 ( void * ) LINE_SELECTION_TIMER,
							 vTimerLineCallback
						   );
	}
	iif_func_enter = &iif_idle;
	iif_func_esc = &iif_line_released;
	iif_func_down = &iif_line_down;
	iif_func_up = &iif_line_up;
	iif_func_left = &iif_idle;
	iif_func_right = &iif_idle;
	iif_func_zdown = &iif_idle;
	iif_func_zup = &iif_idle;
	iif_func_released = &iif_line_released;
	iif_func_cycleStop = &iif_idle;
}

static void iif_line_down(void)
{
	  timeNum = DOWN;
	  xTimerStart( swTimers[LINE_SELECTION_TIMER], 0 );
}

static void iif_line_up(void)
{
	  timeNum = UP;
	  xTimerStart( swTimers[LINE_SELECTION_TIMER], 0 );
}

static void iif_line_released(void)
{
	  timeNum = NONE;
	  xTimerStop( swTimers[LINE_SELECTION_TIMER], 0 );
}

void vTimerLineCallback( TimerHandle_t pxTimer )
{
	uint32_t key = 0;
	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );

	switch(timeNum)
	{
		case DOWN:	key = KEY_DOWN; xQueueSend( qKeyboard, &key, 0 ); break;
		case UP:	key = KEY_UP; xQueueSend( qKeyboard, &key, 0 ); break;
	}
}
