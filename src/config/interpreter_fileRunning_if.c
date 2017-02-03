/*
 * interpreter_fileRunning_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "tinyg.h"
#include "config.h"
#include "canonical_machine.h"
#include "plan_arc.h"
#include "planner.h"
#include "macros.h"
#include "stepper.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "config_SwTimers.h"

#include "platform.h"
#include "interpreter_if.h"
#include "eeprom.h"
#include "ut_state_config_var.h"

extern bool sim;

static void iif_enter_filerunning(void);
static void iif_esc_filerunning(void);
static void iif_down_filerunning(void);
static void iif_up_filerunning(void);
static void iif_left_filerunning(void);
static void iif_right_filerunning(void);
static void iif_zdown_filerunning(void);
static void iif_zup_filerunning(void);
static void iif_released_filerunning(void);

static void iif_enter_warning(void);
static void iif_esc_warning(void);
static void iif_down_warning(void);
static void iif_up_warning(void);

float zmove = 0;
float feedratepercent = 0.05;
void vTimerCallback( TimerHandle_t pxTimer );
void vTimerCallbackVelocity( TimerHandle_t pxTimer );

void iif_enter_filerunning(void)
{
	cm_request_cycle_start();
}

void iif_esc_filerunning(void)
{
	cm_request_feedhold();
}

void iif_zdown_filerunning(void) {

	  xTimerStart( swTimers[ZDOWN_FILERUNNING_TIMER], 0 );
}
void iif_zup_filerunning(void)
{
	  xTimerStart( swTimers[ZUP_FILERUNNING_TIMER], 0 );
}
void iif_left_filerunning(void)
{
	xTimerStart( swTimers[LEFT_FILERUNNING_TIMER], 0 );

}
void iif_right_filerunning(void)
{
	xTimerStart( swTimers[RIGHT_FILERUNNING_TIMER], 0 );
}

void iif_down_filerunning(void)
{
#ifdef VEL_CHANGE
	  xTimerStart( swTimers[DOWN_FILERUNNING_TIMER], 0 );
#endif
}
void iif_up_filerunning(void)
{
#ifdef VEL_CHANGE
	  xTimerStart( swTimers[UP_FILERUNNING_TIMER], 0 );
#endif
}

void iif_released_filerunning(void)
{
	zmove = 0;
	xTimerStop(swTimers[ZDOWN_FILERUNNING_TIMER], 0 );
	xTimerStop(swTimers[ZUP_FILERUNNING_TIMER], 0 );
	xTimerStop(swTimers[LEFT_FILERUNNING_TIMER], 0 );
	xTimerStop(swTimers[RIGHT_FILERUNNING_TIMER], 0 );
#ifdef VEL_CHANGE
	xTimerStop(swTimers[DOWN_FILERUNNING_TIMER], 0 );
	xTimerStop(swTimers[UP_FILERUNNING_TIMER], 0 );
#endif
}

void iif_bind_filerunning(void)
{
	if(swTimers[ZUP_FILERUNNING_TIMER] == NULL)
	{
	swTimers[ZUP_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 500 ),
	                     pdTRUE,
	                     ( void * ) ZUP_FILERUNNING_TIMER,
	                     vTimerCallback
	                   );
	}
	if(swTimers[ZDOWN_FILERUNNING_TIMER] == NULL)
	{
	swTimers[ZDOWN_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 500 ),
	                     pdTRUE,
	                     ( void * ) ZDOWN_FILERUNNING_TIMER,
	                     vTimerCallback
	                   );
	}
	if(swTimers[RIGHT_FILERUNNING_TIMER] == NULL)
	{
	swTimers[RIGHT_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 150 ),
	                     pdTRUE,
	                     ( void * ) RIGHT_FILERUNNING_TIMER,
	                     vTimerCallback
	                   );
	}
	if(swTimers[LEFT_FILERUNNING_TIMER] == NULL)
	{
	swTimers[LEFT_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 150 ),
	                     pdTRUE,
	                     ( void * ) LEFT_FILERUNNING_TIMER,
	                     vTimerCallback
	                   );
	}
#ifdef VEL_CHANGE
	swTimers[DOWN_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 500 ),
	                     pdTRUE,
	                     ( void * ) DOWN_FILERUNNING_TIMER,
						 vTimerCallbackVelocity
	                   );
	swTimers[UP_FILERUNNING_TIMER] = xTimerCreate
	                   ( "Timer 1",
	                     ( 500 ),
	                     pdTRUE,
	                     ( void * ) UP_FILERUNNING_TIMER,
						 vTimerCallbackVelocity
	                   );
#endif
	iif_func_enter = &iif_enter_filerunning;
	iif_func_esc = &iif_idle;
	iif_func_down = &iif_down_filerunning;
	iif_func_up = &iif_up_filerunning;
	iif_func_left = &iif_left_filerunning;
	iif_func_right = &iif_right_filerunning;
	iif_func_zdown = &iif_zdown_filerunning;
	iif_func_zup = &iif_zup_filerunning;
	iif_func_released = &iif_released_filerunning;
	iif_func_cycleStop = &iif_idle;
}

void iif_bind_filerunning_stop(bool stop)
{
	if(stop)
	{
		iif_func_esc = &iif_esc_filerunning;
	}
	else
	{
		iif_func_esc =  &iif_idle;
	}
}

void iif_bind_warning(void)
{
	iif_func_enter = &iif_enter_warning;
	iif_func_esc = &iif_esc_warning;
	iif_func_down = &iif_down_warning;
	iif_func_up = &iif_up_warning;
	iif_func_left = &iif_idle;
	iif_func_right = &iif_idle;
	iif_func_zdown = &iif_idle;
	iif_func_zup = &iif_idle;
	iif_func_released = &iif_idle;
	iif_func_cycleStop = &iif_idle;
}

static void iif_enter_warning(void)
{

}
static void iif_esc_warning(void)
{

}
static void iif_down_warning(void)
{

}
static void iif_up_warning(void)
{

}


void vTimerCallback( TimerHandle_t pxTimer )
{
	long lArrayIndex;

	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );

	/* Which timer expired? */
	lArrayIndex = ( long ) pvTimerGetTimerID( pxTimer );
	if(configFlags[MODOMAQUINA] == MODO_OXICORTE)
	{
		switch (lArrayIndex)
		{
			case RIGHT_FILERUNNING_TIMER:
				if(isDwell)
				{
					st_set_dwell_elapsed_time(1);
					configVarOx[tempoDwell] += 1;
				}
			break;
			case LEFT_FILERUNNING_TIMER:
				if(isDwell)
				{
					st_set_dwell_elapsed_time(-1);
					configVarOx[tempoDwell] -= 1;
				}
			break;
		}
	}

	if (configFlags[MODOMAQUINA] == MODO_OXICORTE || sim){
		switch (lArrayIndex)
		{
			case ZUP_FILERUNNING_TIMER: zmove = 0.005;
			break;
			case ZDOWN_FILERUNNING_TIMER: zmove = -0.005;
			break;
		}
	}
	else
	{
		switch (lArrayIndex)
		{
			case ZUP_FILERUNNING_TIMER: configVarPl[PL_CONFIG_TENSAO_THC] += 1;
										if(configVarPl[PL_CONFIG_TENSAO_THC] > THC_VMAX)
											configVarPl[PL_CONFIG_TENSAO_THC] = THC_VMAX;
			break;
			case ZDOWN_FILERUNNING_TIMER: configVarPl[PL_CONFIG_TENSAO_THC] -= 1;
										  if(configVarPl[PL_CONFIG_TENSAO_THC] < THC_VMIN)
											  configVarPl[PL_CONFIG_TENSAO_THC] = THC_VMIN;
			break;
		}
	}
}

#ifdef VEL_CHANGE
void vTimerCallbackVelocity( TimerHandle_t pxTimer )
{
	long lArrayIndex;
	mpBuf_t *bf = mp_get_run_buffer();
	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );
	if (bf->gm.motion_mode == MOTION_MODE_STRAIGHT_FEED ||
		bf->gm.motion_mode == MOTION_MODE_CW_ARC ||
		bf->gm.motion_mode == MOTION_MODE_CCW_ARC)
	{
		if(mr.section == SECTION_BODY)
		{
			/* Which timer expired? */
			lArrayIndex = ( long ) pvTimerGetTimerID( pxTimer );
			switch (lArrayIndex)
			{
				case DOWN_FILERUNNING_TIMER: cm.gmx.feed_rate_override_factor -= feedratepercent;
				break;
				case UP_FILERUNNING_TIMER: cm.gmx.feed_rate_override_factor += feedratepercent;
				break;
			}
			mp_plan_feedrateoverride_callback(mp_get_run_buffer());
		}
	}

}
#endif
