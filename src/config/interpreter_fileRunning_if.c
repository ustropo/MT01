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
static void iif_released_filerunning(void);

bool zpbutton = false;
void vTimerCallback( TimerHandle_t pxTimer );
TimerHandle_t xTimers;

void iif_enter_filerunning(void)
{
	cm_request_cycle_start();
}

void iif_esc_filerunning(void)
{
	cm_request_feedhold();
}

void iif_down_filerunning(void) {}
void iif_up_filerunning(void)
{
	  xTimers = xTimerCreate
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
	  xTimerStart( xTimers, 0 );
}
void iif_left_filerunning(void){}
void iif_right_filerunning(void) {}
void iif_released_filerunning(void)
{
	zpbutton = false;
	xTimerStop(xTimers, 0 );
}

void iif_bind_filerunning(void)
{
	iif_func_enter = &iif_enter_filerunning;
	iif_func_esc = &iif_esc_filerunning;
	iif_func_down = &iif_down_filerunning;
	iif_func_up = &iif_up_filerunning;
	iif_func_left = &iif_left_filerunning;
	iif_func_right = &iif_right_filerunning;
	iif_func_released = &iif_released_filerunning;
}

void vTimerCallback( TimerHandle_t pxTimer )
{
	zpbutton = true;
	nop();
}
