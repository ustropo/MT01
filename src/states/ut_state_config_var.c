/*
 * ut_state_config.c
 *
 *  Created on: Jan 5, 2016
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "eeprom.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "lcd.h"
#include "lcd_menu.h"
#include "keyboard.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define DEFAULT_CONFIG_VAR_TOUT	portMAX_DELAY

extern bool sim;

static void vTimerUpdateCallback( TimerHandle_t pxTimer );
static TimerHandle_t TimerUpdate[2];
static uint8_t count = 0;
static uint16_t mult = 1;

/**
 * Function pointer that can change a configuration
 * variable.
 *
 * @param Configuration variable object pointer.
 */
typedef void (*ut_config_change_ptr)(ut_config_var*);
ut_config_var* configsVar;

static char* boolOptions[2] =
{
	"NÃƒO",
	"SIM"
};

static char* boolJogVel[2] =
{
	"LENTO",
	"RÁPIDO"
};


/**
 * Config boolean variable
 *
 * @param var Boolean variable
 */
void config_bool(ut_config_var* var)
{
	char** boolStr;
	uint32_t *value;
	ut_menu menu;
	uint8_t i;
	bool Recordflag = false;

	/* Initialize */
	ut_menu_init(&menu);
	boolStr = boolOptions;
	switch(configsVar->currentState)
	{
	case STATE_CONFIG_MANUAL_MODE:
		switch(configsVar->currentItem)
		{
			case 3:  boolStr = boolJogVel;
			break;
			default:
		}
		break;
	case STATE_CONFIG_MENU:
		switch(configsVar->currentItem)
		{
			case 7:  Recordflag =true;
					 value = var->value;
			break;
			default:
		}
		break;
	default:
	}
	 menu.title = var->name;
	for(i = 0; i < 2; i++)
	{
		menu.items[menu.numItems++].text = boolStr[i];
	}
	value = var->value;
	menu.selectedItem = *value % 2; // Just to be sure - it is really not necessary

	/* Check if user selected a valid entry */
	if(ut_menu_browse(&menu, DEFAULT_CONFIG_VAR_TOUT) < 0){
		*value = 0;
		return;
	}
	*value = menu.selectedItem;
	/* save it - TODO: ask for confirmation, maybe? */
	if(Recordflag)
	{
		eepromWriteConfig(CONFIGFLAG);
	}
}

/**
 * Configure int variables
 *
 * @param var Int variable
 */
void config_int(ut_config_var* var)
{
	float Buffervalue;
	float *value = configsVar->value;
	char szText[MAX_COLUMN];
	uint32_t keyEntry;
	Buffervalue = *value;
	switch(var->point)
	{
		case 0: sprintf(szText, "%.0f %s", *value,var->unit); break;
		case 1: sprintf(szText, "%.1f %s", *value,var->unit); break;
	}

	ut_lcd_output_int_var(var->name,szText);

	TimerUpdate[0] = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) 3,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );

	TimerUpdate[1] = xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 200 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) 4,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );

	/* Loop to increment / decrement value */
	/* Wait for keyboard */
	while(xQueueReceive( qKeyboard, &keyEntry, DEFAULT_CONFIG_VAR_TOUT ))
	{
		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			/* TODO: define a min value */
			xTimerStart( TimerUpdate[0], 0 );
			break;

		case KEY_UP:
			/* TODO: define a max value */
			xTimerStart( TimerUpdate[1], 0 );
			break;

		case KEY_ENTER:
			eepromWriteConfig(CONFIGVAR);
			ut_lcd_output_warning("     VALOR     \n\
								     SALVO     \n");
					/* Delay */
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			mult = 1;
			count = 0;
			return;

		case KEY_ESC:
			*value = Buffervalue;
			mult = 1;
			count = 0;
			return;

		case KEY_RELEASED:
			xTimerStop( TimerUpdate[0], 0 );
			xTimerStop( TimerUpdate[1], 0 );
			mult = 1;
			count = 0;
			break;

		default:
			break;
		}


	}

}

/**
 * Null handler
 * @param var
 */
void config_null(ut_config_var* var)
{

}

/* Default handlers for variables */
static ut_config_change_ptr var_handlers[UT_CONFIG_MAX] =
{
	&config_int,
	&config_bool,
	&config_null
};

/**
 * This state configures a single variable that is
 * shared among multiple states.
 *
 * @param pContext Context object
 * @return Config menu
 */
ut_state ut_state_config_var(ut_context* pContext)
{
	uint32_t *Flag = configsVar->value;
	ut_state stateBack = (ut_state)pContext->value[0];

	var_handlers[configsVar->type](configsVar);

	switch(configsVar->currentState)
	{
		case STATE_CONFIG_MANUAL_MODE:
			if(configsVar->currentItem == 2 )
			{
				uint32_t *Flag = configsVar->value;
				if(*Flag == 1)
				{
					stateBack = (ut_state)pContext->value[1];
				}
			}
			break;
		case STATE_CONFIG_AUTO_MODE:
			switch(configsVar->currentItem)
			{
				case 0:
				case 2:
					if(*Flag == 1)
					{
						stateBack = (ut_state)pContext->value[1];
						sim = false;
					}
					break;
				case 3:
					if(*Flag == 1)
					{
						sim = true;
						stateBack = (ut_state)pContext->value[1];
					}
					else
					{
						sim = false;
					}
					break;
			}
		break;
	}
	/* Avoid null handler */
	if(configsVar->func_var) { configsVar->func_var(configsVar); }
	return stateBack;
}

static void vTimerUpdateCallback( TimerHandle_t pxTimer )
{
	char szText[MAX_COLUMN];
	long lArrayIndex;
	float *value = configsVar->value;

	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );
	if(count == 10)
	{
		mult = mult*10;
		if (mult > 1000)
			mult = 1000;
		count = 0;
	}

	/* Which timer expired? */
	lArrayIndex = ( long ) pvTimerGetTimerID( pxTimer );
	switch (lArrayIndex)
	{
		case 3: if(*value > configsVar->valueMin){
			*value = *value - configsVar->step*mult;
		}
		else{
			*value = configsVar->valueMin;
		}
		break;
		case 4: if(*value < configsVar->valueMax){
			*value = *value + configsVar->step*mult;
		}else{
			*value = configsVar->valueMax;
		}
		break;
	}

	switch(configsVar->point)
	{
		case 0: sprintf(szText, "%.0f %s", *value,configsVar->unit); break;
		case 1: sprintf(szText, "%.1f %s", *value,configsVar->unit); break;
	}

	ut_lcd_output_int_var(configsVar->name,szText);
	count++;
}
