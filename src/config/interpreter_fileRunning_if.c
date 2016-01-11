/*
 * interpreter_fileRunning_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "platform.h"
#include "interpreter_if.h"

static void iif_enter_filerunning(void);
static void iif_esc_filerunning(void);
static void iif_down_filerunning(void);
static void iif_up_filerunning(void);
static void iif_left_filerunning(void);
static void iif_right_filerunning(void);
static void iif_zdown_filerunning(void);
static void iif_zup_filerunning(void);
static void iif_released_filerunning(void);

float zmove = 0;
void vTimerCallback( TimerHandle_t pxTimer );
TimerHandle_t xTimers[2];

void iif_enter_filerunning(void)
{
	cm_request_cycle_start();
}

void iif_esc_filerunning(void)
{
	cm_request_feedhold();
}

void iif_down_filerunning(void) {
	  xTimers[1] = xTimerCreate
	                   (  /* Just a text name, not used by the RTOS kernel. */
	                     "Timer 1",
	                     /* The timer period in ticks, must be greater than 0. */
	                     ( 100 ),
	                     /* The timers will auto-reload themselves when they
	                     expire. */
	                     pdTRUE,
	                     /* Assign each timer a unique id equal to its array
	                     index. */
	                     ( void * ) 1,
	                     /* Each timer calls the same callback when it expires. */
	                     vTimerCallback
	                   );
	  xTimerStart( xTimers[1], 0 );
}
void iif_up_filerunning(void)
{
	xTimers[0] = xTimerCreate
	                   (  /* Just a text name, not used by the RTOS kernel. */
	                     "Timer 0",
	                     /* The timer period in ticks, must be greater than 0. */
	                     ( 100 ),
	                     /* The timers will auto-reload themselves when they
	                     expire. */
	                     pdTRUE,
	                     /* Assign each timer a unique id equal to its array
	                     index. */
	                     ( void * ) 0,
	                     /* Each timer calls the same callback when it expires. */
	                     vTimerCallback
	                   );
	  xTimerStart( xTimers[0], 0 );
}
void iif_left_filerunning(void){}
void iif_right_filerunning(void) {}
void iif_zdown_filerunning(void){}
void iif_zup_filerunning(void){}
void iif_released_filerunning(void)
{
	zmove = 0;
	xTimerStop(xTimers[0], 0 );
	xTimerStop(xTimers[1], 0 );
}

void iif_bind_filerunning(void)
{
	iif_func_enter = &iif_enter_filerunning;
	iif_func_esc = &iif_esc_filerunning;
	iif_func_down = &iif_down_filerunning;
	iif_func_up = &iif_up_filerunning;
	iif_func_left = &iif_left_filerunning;
	iif_func_right = &iif_right_filerunning;
	iif_func_zdown = &iif_zdown_filerunning;
	iif_func_zup = &iif_zup_filerunning;
	iif_func_released = &iif_released_filerunning;
}

void vTimerCallback( TimerHandle_t pxTimer )
{
	long lArrayIndex;

	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );

	/* Which timer expired? */
	lArrayIndex = ( long ) pvTimerGetTimerID( pxTimer );
	switch (lArrayIndex)
	{
		case 0: zmove = 0.007;
		break;
		case 1: zmove = -0.007;
		break;
	}
}
