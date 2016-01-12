/*
 * ut_state_config.c
 *
 *  Created on: Jan 5, 2016
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"
#include "lcd_menu.h"
#include "keyboard.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ut_state_config.h"

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
	"NÃO",
	"SIM"
};


/**
 * Config boolean variable
 *
 * @param var Boolean variable
 */
void config_bool(ut_config_var* var)
{
	ut_menu menu;
	uint8_t i;

	/* Initialize */
	ut_menu_init(&menu);
	for(i = 0; i < 2; i++)
	{
		menu.items[menu.numItems++].text = boolOptions[i];
	}
	menu.title = var->name;
	menu.selectedItem = var->value % 2; // Just to be sure - it is really not necessary

	/* Check if user selected a valid entry */
	if(ut_menu_browse(&menu, DEFAULT_CONFIG_VAR_TOUT) < 0) return;

	/* save it - TODO: ask for confirmation, maybe? */
	var->value = menu.selectedItem;
}

/**
 * Configure int variables
 *
 * @param var Int variable
 */
void config_int(ut_config_var* var)
{
	uint32_t tmp = var->value;
	char szText[MAX_COLUMN];
	uint32_t keyEntry;

	/* Clear previous screen */
	ut_lcd_clear();
	/* Set title */
	ut_lcd_drawString(0, 0, var->name, true);
	/* Set value */
	sprintf(szText, "%10d", tmp);
	ut_lcd_drawString(3, 0, szText, false);
	ut_lcd_output();

	/* Loop to increment / decrement value */
	/* Wait for keyboard */
	while(xQueueReceive( qKeyboard, &keyEntry, DEFAULT_CONFIG_VAR_TOUT ))
	{
		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
			/* TODO: define a min value */
			if(tmp > 0) tmp--;
			break;

		case KEY_UP:
			/* TODO: define a max value */
			if(tmp < 100000) tmp++;
			break;

		case KEY_ENTER:
			/* Save value and exit */
			var->value = tmp;
			return;

		case KEY_ESC:
			return;

		default:
			break;
		}

		/* Show again */
		sprintf(szText, "%10d", tmp);
		ut_lcd_drawString(3, 0, szText, false);
		ut_lcd_output();
	}

}

/* Default handlers for variables */
static ut_config_change_ptr var_handlers[UT_CONFIG_MAX] =
{
	&config_int,
	&config_bool,
};

/**
 * This state configurates a single variable that is
 * shared among multiple states.
 *
 * @param pContext Context object
 * @return Config menu
 */
ut_state ut_state_config_var(ut_context* pContext)
{
	ut_state stateBack = (ut_state)pContext->tag;

	var_handlers[configsVar->type](configsVar);

	configsVar->func_var(configsVar);

	return stateBack;
}
