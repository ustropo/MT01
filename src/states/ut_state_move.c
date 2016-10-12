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
#include "stepper.h"
#include "spindle.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "config_SwTimers.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"

#include "planner.h"
#include "plasma.h"
#include "eeprom.h"

extern TaskHandle_t xCncTaskHandle;
bool sim = false;
bool programEnd = false;
bool lstop = false;
extern bool simTorch;
char textXStr[MAX_COLUMN];
char textYStr[MAX_COLUMN];
char textZStr[MAX_COLUMN];
uint8_t gTitle;
extern float *velocidadeJog;
extern bool intepreterRunning;

#define DEFAULT_AUTO_TITLE		"MODO AUTOM햀ICO"
#define STOP_AUTO_TITLE		    "M핽UINA PAUSADA"
#define DEFAULT_LINHA1_AUTO	    ""
#define DEFAULT_AVISO_AUTO	    ""
#define STOP_AVISO_AUTO	        ""

#define DEFAULT_MANUAL_TITLE	"MODO MANUAL"
#define DEFAULT_LINHA1_MANUAL	"VELOCIDADE:         "
#define DEFAULT_AVISO_MANUAL	"ENTER DISPARA/ESC VOLTA"

#define DEFAULT_DESCOLA_TITLE	"DESLOCANDO"
#define DEFAULT_LINHA1_DESLOCA	""
#define DEFAULT_AVISO_DESLOCA	"ESC VOLTA"

#define DEFAULT_SIM_TITLE		"MODO SIMULA플O"
#define STOP_SIM_TITLE		    "M핽UINA PAUSADA"
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

static char gStrManual[4][24] =
{
	DEFAULT_MANUAL_TITLE,
	DEFAULT_AVISO_MANUAL,
	"",
	""
};

static char gStrAuto[6][24] =
{
	DEFAULT_AUTO_TITLE,
	DEFAULT_AVISO_AUTO,
	"",
	"",
	""
};

static char gStrSim[4][28] =
{
	DEFAULT_SIM_TITLE,
	DEFAULT_AVISO_SIM,
	"",
	""
};

static char gStrDesloca[4][24] =
{
	DEFAULT_DESCOLA_TITLE,
	DEFAULT_AVISO_DESLOCA,
	"",
	""
};



static void vTimerUpdateCallback( TimerHandle_t pxTimer );
void warm_stop(void);
static bool ltorchBuffer = false;
extern bool zinhibitor;

/**
 * Update machine position
 * @param szTitle
 */
static void updatePosition(uint8_t menu)
{
	float x; float y; float z;
	char *lStr[6];
	/* Display is only cleared once to improve performance */
	lStr[4] = "";
	switch(menu)
	{
		case MANUAL: lStr[0] = gStrManual[0];
					 lStr[1] = gStrManual[1];
			         sprintf(gStrManual[2], "VEL.: %.0f mm/min", *velocidadeJog);
			         lStr[2] = gStrManual[2];
                     if (arcoOkGet())
                    	 lStr[3] = "AOK";
                     else
                    	 lStr[3] = "";
			         break;
		case AUTO:   lStr[0] = gStrAuto[0];
					 currentLine = cm_get_linenum(RUNTIME);
 	 	 	 	 	 sprintf(gStrAuto[1], "LINHA: %d",  cm_get_linenum(RUNTIME));
		 	 	 	 lStr[1] = gStrAuto[1];
			         sprintf(gStrAuto[2], "VEL.: %.0f mm/min",  mp_get_runtime_velocity());
                     lStr[2] = gStrAuto[2];

						 if (arcoOkGet())
							 lStr[3] = "AOK";
						 else
							 lStr[3] = "";
					 if(configFlags[MODOMAQUINA] == MODO_PLASMA)
					 {
						 sprintf(gStrAuto[4], "THC SET: %.0f V",  configVarPl[PL_CONFIG_TENSAO_THC]);
						 lStr[4] = gStrAuto[4];
						 if(isCuttingGet()){
							 sprintf(gStrAuto[5], "THC REAL: %.0f V",  THC_realGet());
							 lStr[5] = gStrAuto[5];
						 }
						 else
						 {
							 lStr[5] = "THC REAL: --- V";
						 }
                     }
                     else
                     {
                    	 if(isDwell){
        			         sprintf(gStrAuto[4], "T: %.0f s",  st_get_dwell_elapsed_time());
                    		 lStr[4] = gStrAuto[4];
                    	 }
                     }
                     break;
		case SIM:    lStr[0] = gStrSim[0];
		 	 	 	 currentLine = cm_get_linenum(RUNTIME);
	 	 	 	 	 sprintf(gStrSim[1], "LINHA: %d",  cm_get_linenum(RUNTIME));
		 	 	 	 lStr[1] = gStrSim[1];
			         sprintf(gStrSim[2], "VEL.: %.0f mm/min",  mp_get_runtime_velocity());
                     lStr[2] = gStrSim[2];
                     break;
		case DESLOCA: lStr[0] = gStrDesloca[0];
					  lStr[1] = gStrDesloca[1];
				      sprintf(gStrDesloca[2], "VEL.: %.0f mm/min",  mp_get_runtime_velocity());
					  lStr[2] = gStrDesloca[2];
					  break;
	}
	/* TODO: get position from machine */
	x = mp_get_runtime_absolute_position(0);
	y = mp_get_runtime_absolute_position(1);
	z = mp_get_runtime_absolute_position(2);

	sprintf(textXStr, "%4.2f mm", x);
	sprintf(textYStr, "%4.2f mm", y);
	sprintf(textZStr, "%4.2f mm", z);

	if(cm.machine_state == MACHINE_PROGRAM_END && !programEnd && (menu == AUTO || menu == SIM))
	{
		xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
		ut_lcd_output_warning("CORTE AUTOM햀ICO\nFINALIZADO\nPRESSIONE ESC\n");
		/* Delay */
	//	vTaskDelay(2000 / portTICK_PERIOD_MS);
		currentLine = 0;
		intepreterRunning = false;
		programEnd = true;
	}
	else
	{
		/* Put it into screen */
		if(configFlags[MODOMAQUINA] || sim || menu == MANUAL || menu == DESLOCA){
			ut_lcd_output_mov_mode(TORCH,
					lStr,
					(const char *)textXStr,
					(const char *)textYStr,
					(const char *)textZStr);
		}
		else{
			ut_lcd_output_plasma_mode(TORCH,
					lStr,
					(const char *)textXStr,
					(const char *)textYStr,
					(const char *)textZStr);
		}
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
	restart_stepper();
	/* Clear display */
	updatePosition(MANUAL);
	gTitle = MANUAL;
//	tg_set_primary_source(XIO_DEV_COMMAND);
	iif_bind_jog();
	swTimers[AUTO_MENU_TIMER] = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) AUTO_MENU_TIMER,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
	macro_func_ptr = command_idle;
	xTaskNotifyGive(xCncTaskHandle);
	intepreterRunning = true;

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
			xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
			iif_func_esc();
			intepreterRunning = false;
			return STATE_CONFIG_MANUAL_MODE;

		case KEY_ENTER:
			iif_func_enter();
			break;

		case KEY_RELEASED:
			iif_func_released();
			break;

		case EMERGENCIA_SIGNAL:
			xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
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
	ltorchBuffer = false;
	uint32_t arco = 0;
//	stepper_init();
	restart_stepper();
	if (configFlags[MODOMAQUINA] == MODO_PLASMA)
		eepromReadConfig(CONFIGVAR_PL);
	else
		eepromReadConfig(CONFIGVAR_OX);
	lstop = false;
	cm.gmx.feed_rate_override_enable = true;
	cm.gmx.feed_rate_override_factor = 1;
	programEnd = false;
	cm.machine_state = MACHINE_READY;
	/* Clear display */
	if(!sim){
		updatePosition(AUTO);
		gTitle = AUTO;
	}
	else{
		updatePosition(SIM);
		gTitle = SIM;
	}

	if (swTimers[AUTO_MENU_TIMER] == NULL){
	swTimers[AUTO_MENU_TIMER] = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) AUTO_MENU_TIMER,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	}
	xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
	tg_set_primary_source(CNC_MEDIA);
	xio_close(cs.primary_src);
	macro_func_ptr = _command_dispatch;
	xio_open(cs.primary_src,0,0);
	xTaskNotifyGive(xCncTaskHandle);
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
				if(isDwell == true)
				{
					st_command_dwell(DWELL_RESTART);
				}
				if (arco == ARCO_OK_OFF)
				{
					arco = 0;
					if(isCuttingGet())
					{
						state = 0;
						isCuttingSet(false);
//						isCuttingSet(true);
//						pl_arcook_start();
				//		cm_set_feed_rate(configVarPl[PL_CONFIG_VELOC_CORTE]);
						xMacroArcoOkSync = true;
					}
					else
					{
						macro_func_ptr = M3_Macro;
						iif_func_enter();
					}
				}
				else{
					iif_func_enter();
				}
				TORCH = ltorchBuffer;
				if(sim && configFlags[MODOMAQUINA] == MODO_OXICORTE){
					gTitle = AUTO;
					sim = false;
					if (simTorch)
					{
						TORCH = TRUE;
					}
				}
			}
			else
			{
				if(isDwell == true)
				{
					st_command_dwell(DWELL_EXIT);
				}
				if(sim){
					gTitle = AUTO;
					sim = false;
					if (simTorch)
					{
						TORCH = TRUE;
						if(configFlags[MODOMAQUINA] == MODO_PLASMA){
							pl_arcook_start();
							isCuttingSet(true);
						}
					}
				}
			}
			break;

		case KEY_ESC:
			if (programEnd || lstop){
				uint32_t *value = configsVar->value;
				if(!programEnd){
					cm_request_feedhold();
					xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
					configsVar->currentState = STATE_AUTO_MODE;
					configsVar->type = UT_CONFIG_BOOL;
					configsVar->name = "DESEJA SAIR?";
					ut_state_config_var(pContext);
				}
				if(*value || programEnd){
//					xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
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
					if(isDwell == true)
					{
//						st_command_dwell(DWELL_EXIT);
//						st_command_dwell(DWELL_RESTART);
						st_command_dwell(DWELL_ZERO);
						mr.move_state = MOVE_OFF;
//						while(!st_runtime_isbusy());
						cm.queue_flush_requested = false;
						cm_queue_flush();
						cm.motion_state = MOTION_STOP;
					}
					state = 0;
					cm.cycle_state = CYCLE_OFF;
					pl_arcook_stop();
					isCuttingSet(false);
					iif_bind_idle();
					cm.gmx.feed_rate_override_enable = true;
					cm.gmx.feed_rate_override_factor = 1;
					while(cm.queue_flush_requested == true && !programEnd)
					{

					}
					cm.probe_state = PROBE_FAILED;
					TORCH = FALSE;
					macro_func_ptr = command_idle;
					intepreterRunning = false;
					sim = false;
					currentLine = 0;
					zinhibitor = false;
					if (programEnd)
						return STATE_MANUAL_MODE;
					return STATE_CONFIG_AUTO_MODE;
				}
				else
				{
					xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
					warm_stop();
				}

			}
			if(!lstop){
				if(arco == ARCO_OK_FAILED)
				{
					arco = ARCO_OK_OFF;
					lstop = true;
					xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
				}
				else
				{
					lstop = true;
					warm_stop();
				}
			}

			break;

		case KEY_RELEASED:
			iif_func_released();
			break;
		/* TODO: operate machine - with other keys */
		case MATERIAL_FAILED:
			if(!arco)
			{
				xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
				ut_lcd_output_warning("CHECAR SENSOR\nOHMICO\n");
				TORCH = FALSE;
				arco = ARCO_OK_FAILED;
				lstop = false;
				warm_stop();
				state = 0;
			}
			break;
		case ARCO_OK_FAILED:
			if(!arco)
			{
				xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
				ut_lcd_output_warning("PLASMA N홒\nTRANSFERIDO\n");
				TORCH = FALSE;
				pl_arcook_stop();
				arco = ARCO_OK_FAILED;
				lstop = false;
				warm_stop();
				ltorchBuffer = TRUE;
			}
			break;

		case USB_DISCONNECTED:
			xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
			cm_request_feedhold();
			cm_request_queue_flush();
			xio_close(cs.primary_src);
			cm.probe_state = PROBE_FAILED;
			state = 0;
			cm.cycle_state = CYCLE_OFF;
			pl_arcook_stop();
			isCuttingSet(false);
			iif_bind_idle();
			TORCH = FALSE;
			cm.gmx.feed_rate_override_enable = true;
			cm.gmx.feed_rate_override_factor = 1;
			macro_func_ptr = command_idle;
			ut_lcd_output_warning("PEN DRIVE\nDESCONECTADO\n");
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			return STATE_CONFIG_AUTO_MODE;
		break;

		case EMERGENCIA_SIGNAL:
			warm_stop();
			if(!sim){
				updatePosition(AUTO);
				gTitle = AUTO;
			}
			else{
				updatePosition(SIM);
				gTitle = SIM;
			}
			xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
			break;

		default: break;
		}



		/* Update position */
	//	updatePosition(NULL);
	}

	return STATE_MAIN_MENU;
}

ut_state ut_state_deslocaZero_mode(ut_context* pContext)
{
	uint32_t keyEntry;
//	stepper_init();
	restart_stepper();
	cm_request_queue_flush();
	/* Clear display */
	updatePosition(DESLOCA);
	gTitle = DESLOCA;
	iif_bind_deslocar();
	swTimers[AUTO_MENU_TIMER] = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) AUTO_MENU_TIMER,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
	xTaskNotifyGive(xCncTaskHandle);
	lstop = false;
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
				if(lstop)
				{
					lstop = false;
					iif_bind_filerunning_stop(lstop);
					strcpy(gStrAuto[0],DEFAULT_DESCOLA_TITLE);
					strcpy(gStrAuto[1],DEFAULT_AVISO_DESLOCA);
					cm_request_cycle_start();
				}
				break;
		case KEY_ESC:
//			xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
//			iif_bind_idle();
//			intepreterRunning = false;
//			return (ut_state)pContext->value[0];
			if (cm.machine_state == MACHINE_PROGRAM_END || lstop){
				uint32_t *value = configsVar->value;
				xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
				if(cm.machine_state != MACHINE_PROGRAM_END){
					cm_request_feedhold();
					configsVar->currentState = STATE_AUTO_MODE;
					configsVar->type = UT_CONFIG_BOOL;
					configsVar->name = "DESEJA SAIR?";
					ut_state_config_var(pContext);
				}
				if(*value || cm.machine_state == MACHINE_PROGRAM_END){
					iif_bind_idle();
					macro_func_ptr = command_idle;
					intepreterRunning = false;
					return (ut_state)pContext->value[0];
				}
			}
			if(!lstop){
				lstop = true;
				warm_stop();
			}
			break;

		case KEY_RELEASED:
			iif_func_released();
			break;

		case EMERGENCIA_SIGNAL:
			xTimerStart( swTimers[AUTO_MENU_TIMER], 0 );
			break;

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

void warm_stop(void)
{

	//iif_bind_filerunning_stop(lstop);
	cm_request_feedhold();
	if(gTitle == AUTO){
		strcpy(gStrAuto[0],STOP_AUTO_TITLE);
		strcpy(gStrAuto[1],STOP_AVISO_AUTO);
	}else if(gTitle == SIM){
		strcpy(gStrSim[0],STOP_SIM_TITLE);
		strcpy(gStrSim[1],STOP_AVISO_SIM);
	}
	pl_arcook_stop();
	if(isDwell == true)
	{
		st_command_dwell(DWELL_PAUSE);
	}
	while(cm.feedhold_requested == true)
	{
		WDT_FEED
	}
	ltorchBuffer = TORCH;
	TORCH = FALSE;
}
