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

#include "lcd.h"
#include "lcd_menu.h"
#include "keyboard.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define DEFAULT_CONFIG_VAR_TOUT	30000

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

	/* Initialize */
	ut_menu_init(&menu);
	switch(configsVar->currentState)
	{
	case STATE_CONFIG_MANUAL_MODE:
		if(configsVar->currentItem == 3)
		{
			boolStr = boolJogVel;
		}
		else
		{
			boolStr = boolOptions;
		}
		if (configsVar->currentItem == 2)
		{
			menu.title = "CUIDADO! MOVIMENTO AUTOMÁTICO";
		}
		else
		{
			menu.title = var->name;
		}
		break;
	case STATE_CONFIG_AUTO_MODE:
		boolStr = boolOptions;
		if (configsVar->currentItem == 2)
		{
			menu.title = "CUIDADO! MOVIMENTO AUTOMÁTICO";
		}
		else
		{
			menu.title = var->name;
		}
		break;
	default: boolStr = boolOptions;
			 menu.title = var->name;
	}

	for(i = 0; i < 2; i++)
	{
		menu.items[menu.numItems++].text = boolStr[i];
	}
	value = var->value;
	menu.selectedItem = *value % 2; // Just to be sure - it is really not necessary

	/* Check if user selected a valid entry */
	if(ut_menu_browse(&menu, DEFAULT_CONFIG_VAR_TOUT) < 0) return;

	/* save it - TODO: ask for confirmation, maybe? */
	*value = menu.selectedItem;
}

/**
 * Configure int variables
 *
 * @param var Int variable
 */
void config_int(ut_config_var* var)
{
	float* tmp = var->value;
	char szText[MAX_COLUMN];
	uint32_t keyEntry;


	sprintf(szText, "%.1f %s", *tmp,var->unit);

	ut_lcd_output_int_var(var->name,szText);

	/* Loop to increment / decrement value */
	/* Wait for keyboard */
	while(xQueueReceive( qKeyboard, &keyEntry, DEFAULT_CONFIG_VAR_TOUT ))
	{
		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			/* TODO: define a min value */
			if(*tmp > 0) *tmp = *tmp - 1.0;
			break;

		case KEY_UP:
			/* TODO: define a max value */
			if(*tmp < 100000) *tmp = *tmp + 1.0;
			break;

		case KEY_ENTER:
			eepromWriteConfig();
			return;

		case KEY_ESC:
			return;

		default:
			break;
		}

		sprintf(szText, "%.1f %s", *tmp,var->unit);

		ut_lcd_output_int_var(var->name,szText);
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
	ut_state stateBack = (ut_state)pContext->value[0];

	var_handlers[configsVar->type](configsVar);

	/* Avoid null handler */
	if(configsVar->func_var) { configsVar->func_var(configsVar); }

	switch(configsVar->currentState)
	{
		case STATE_CONFIG_MANUAL_MODE:
			if(configsVar->currentItem == 2)
			{
				if(configsVar->value)
				{
					stateBack = (ut_state)pContext->value[1];
				}
			}
		break;
	}
	return stateBack;
}
