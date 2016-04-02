/*
 * ut_state_manual.c
 *
 *  Created on: Dec 4, 2015
 *      Author: Fernando
 */

#include "tinyg.h"		// #1
#include "hardware.h"
#include "controller.h"
#include "macros.h"

#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"

#include "planner.h"
#include "plasma.h"

bool sim = false;
bool programEnd = false;
bool lstop = false;
extern bool simTorch;
char textXStr[MAX_COLUMN];
char textYStr[MAX_COLUMN];
char textZStr[MAX_COLUMN];
uint8_t gTitle;
extern float *velocidadeJog;


#define DEFAULT_AUTO_TITLE		"MODO AUTOMÁTICO"
#define STOP_AUTO_TITLE		    "MÁQUINA PARADA"
#define DEFAULT_LINHA1_AUTO	    ""
#define DEFAULT_AVISO_AUTO	    ""
#define STOP_AVISO_AUTO	        ""

#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_LINHA1_MANUAL	"VELOCIDADE:         "
#define DEFAULT_AVISO_MANUAL	"ENTER DISPARA/ESC VOLTA"

#define DEFAULT_DESCOLA_TITLE	"DESLOCAR A ZERO"
#define DEFAULT_LINHA1_DESLOCA	""
#define DEFAULT_AVISO_DESLOCA	"ESC VOLTA"

#define DEFAULT_SIM_TITLE		"MODO SIMULAÇÃO"
#define STOP_SIM_TITLE		    "MÁQUINA PARADA"
#define DEFAULT_LINHA1_SIM	    ""
#define DEFAULT_AVISO_SIM	    ""
#define STOP_AVISO_SIM	        ""


#define DEFAULT_UPDATE_TIMEOUT	portMAX_DELAY

enum {
	MANUAL = 0,
	AUTO,
	DESLOCA,
	SIM
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

static char gStrSim[3][28] =
{
	DEFAULT_SIM_TITLE,
	DEFAULT_AVISO_SIM,
	""
};

static char gStrDesloca[3][24] =
{
	DEFAULT_DESCOLA_TITLE,
	DEFAULT_AVISO_DESLOCA,
	""
};

static void vTimerUpdateCallback( TimerHandle_t pxTimer );
static TimerHandle_t TimerUpdate;

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
			         sprintf(gStrManual[2], "VEL.: %.0f mm/s", *velocidadeJog);
			         lStr[2] = gStrManual[2];
			         break;
		case AUTO:   lStr[0] = gStrAuto[0];
 	 	 	 	 	 sprintf(gStrAuto[1], "LINHA: %d",  cm_get_linenum(RUNTIME));
		 	 	 	 lStr[1] = gStrAuto[1];
			         sprintf(gStrAuto[2], "VEL.: %.0f mm/s",  mp_get_runtime_velocity());
                     lStr[2] = gStrAuto[2];
                     break;
		case SIM:    lStr[0] = gStrSim[0];
	 	 	 	 	 sprintf(gStrSim[1], "LINHA: %d",  cm_get_linenum(RUNTIME));
		 	 	 	 lStr[1] = gStrSim[1];
			         sprintf(gStrSim[2], "VEL.: %.0f mm/s",  mp_get_runtime_velocity());
                     lStr[2] = gStrSim[2];
                     break;
		case DESLOCA: lStr[0] = gStrDesloca[0];
					  lStr[1] = gStrDesloca[1];
				         sprintf(gStrDesloca[2], "VEL.: %.0f mm/s",  mp_get_runtime_velocity());
					  lStr[2] = gStrDesloca[2];
					  break;
	}
	/* TODO: get position from machine */
	x = mp_get_runtime_absolute_position(0);
	y = mp_get_runtime_absolute_position(1);
	z = mp_get_runtime_absolute_position(2);

	sprintf(textXStr, "X:%4.2f mm", x);
	sprintf(textYStr, "Y:%4.2f mm", y);
	sprintf(textZStr, "Z:%4.2f mm", z);

	if(cm.machine_state == MACHINE_PROGRAM_END && !programEnd && (menu == AUTO || menu == SIM))
	{
		xTimerStop( TimerUpdate, 0 );
		ut_lcd_output_warning("CORTE AUTOMÁTICO\nFINALIZADO\nPRESSIONE ESC\n");
		/* Delay */
		vTaskDelay(2000 / portTICK_PERIOD_MS);
		programEnd = true;
	}
	else
	{
	/* Put it into screen */
	ut_lcd_output_manual_mode(TORCH,
			lStr,
			(const char *)textXStr,
			(const char *)textYStr,
			(const char *)textZStr);
	}
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
//	tg_set_primary_source(XIO_DEV_COMMAND);
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
	bool ltorchBuffer = false;
	programEnd = false;
	lstop = false;
	cm.gmx.feed_rate_override_enable = true;
	cm.gmx.feed_rate_override_factor = 1;
	/* Clear display */
	if(!sim){
		updatePosition(AUTO);
		gTitle = AUTO;
	}
	else{
		updatePosition(SIM);
		gTitle = SIM;
	}
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
	macro_func_ptr = _command_dispatch;
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
			/* Clear display */
			if(lstop)
			{
				lstop = false;
				iif_bind_filerunning_stop(lstop);
				if(gTitle == AUTO){
					strcpy(gStrAuto[0],DEFAULT_AUTO_TITLE);
					strcpy(gStrAuto[1],DEFAULT_AVISO_AUTO);
				}else if(gTitle == SIM){
					strcpy(gStrSim[0],DEFAULT_SIM_TITLE);
					strcpy(gStrSim[1],DEFAULT_AVISO_SIM);
				}
				iif_func_enter();
				TORCH = ltorchBuffer;
			}
			else
			{
				if(sim){
					gTitle = AUTO;
					sim = false;
					if (simTorch)
					{
						TORCH = TRUE;
						pl_arcook_start();
						isCuttingSet(true);
					}
				}
			}
			break;

		case KEY_ESC:
			if (programEnd || lstop){
				xTimerStop( TimerUpdate, 0 );
				if(gTitle == AUTO){
					strcpy(gStrAuto[0],DEFAULT_AUTO_TITLE);
					strcpy(gStrAuto[1],DEFAULT_AVISO_AUTO);
				}else if(gTitle == SIM){
					strcpy(gStrSim[0],DEFAULT_SIM_TITLE);
					strcpy(gStrSim[1],DEFAULT_AVISO_SIM);
				}
				cm_request_feedhold();
				cm_request_queue_flush();
				xio_close(cs.primary_src);

				cm.probe_state = PROBE_FAILED;
				//cm_set_motion_mode(MODEL, MOTION_MODE_CANCEL_MOTION_MODE);
				//cm_cycle_end();
				state = 0;
				cm.cycle_state = CYCLE_OFF;
				pl_arcook_stop();
				isCuttingSet(false);
				iif_bind_idle();
				macro_func_ptr = command_idle;
				if (programEnd)
					return STATE_MANUAL_MODE;
				return STATE_CONFIG_AUTO_MODE;
			}
			if(!lstop){
				lstop = true;
				//iif_bind_filerunning_stop(lstop);
				cm_request_feedhold();
				if(gTitle == AUTO){
					strcpy(gStrAuto[0],STOP_AUTO_TITLE);
					strcpy(gStrAuto[1],STOP_AVISO_AUTO);
				}else if(gTitle == SIM){
					strcpy(gStrSim[0],STOP_SIM_TITLE);
					strcpy(gStrSim[1],STOP_AVISO_SIM);
				}
				ltorchBuffer = TORCH;
				TORCH = FALSE;
			}

			break;

		case KEY_RELEASED:
			iif_func_released();
			break;
		/* TODO: operate machine - with other keys */
		case ARCO_OK_FAILED:
//			xTimerStop( TimerUpdate, 0 );
//			cm_request_feedhold();
//			cm_request_queue_flush();
//			xio_close(cs.primary_src);
//
//			cm.probe_state = PROBE_FAILED;
//			state = 0;
//			cm.cycle_state = CYCLE_OFF;
//
//			iif_bind_idle();
//			pl_arcook_stop();
//			isCuttingSet(false);
//			TORCH = FALSE;
//			ut_lcd_output_warning("ERRO\nPLASMA NÃO\nTRANSFERIDO\n");
//
//			vTaskDelay(2000 / portTICK_PERIOD_MS);
//			macro_func_ptr = command_idle;
//			return STATE_CONFIG_AUTO_MODE;
			xTimerStop( TimerUpdate, 0 );
			ut_lcd_output_warning("PLASMA NÃO\nTRANSFERIDO\nPRESSIONE ESC\n");
			TORCH = FALSE;
			macro_func_ptr = command_idle;
			uint32_t qSend = KEY_ESC;
			xQueueSend( qKeyboard, &qSend, 0 );
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
	cm_request_queue_flush();
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
			return (ut_state)pContext->value[0];

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

static void vTimerUpdateCallback( TimerHandle_t pxTimer )
{
	if(gTitle == AUTO || gTitle == SIM){
		iif_func_esc();
	}
	updatePosition(gTitle);
}
