/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 6, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "eeprom.h"
#include "config_menu_ox.h"
#include "config_menu_pl.h"


#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY


static const char* gszConfigMenuTitle = "CONFIG. DE CORTE";

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_menu_ox(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	initOx();

	/* Initialize menu */
	ut_menu_init(&config_menu);

	/* Options */
	config_menu.title = gszConfigMenuTitle;
	config_menu.currentState = STATE_CONFIG_MENU_OX;
//	config_menu.offset = 1;
	/* Items */
	for(i = 0; i < OX_CONFIG_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configsOx[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}
	eepromReadConfig(CONFIGVAR_OX);
	/* Set selected item */
	pContext->value[0] = STATE_CONFIG_MENU_OX;
	configsVar = &configsOx[config_menu.selectedItem];
	return STATE_CONFIG_VAR;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_menu_pl(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	initPl();

	/* Initialize menu */
	ut_menu_init(&config_menu);

	/* Options */
	config_menu.title = gszConfigMenuTitle;
	config_menu.currentState = STATE_CONFIG_MENU_PL;
//	config_menu.offset = 1;
	/* Items */
	for(i = 0; i < PL_CONFIG_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configsPl[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}

	/* Set selected item */
	eepromReadConfig(CONFIGVAR_PL);
	pContext->value[0] = STATE_CONFIG_MENU_PL;
	configsVar = &configsPl[config_menu.selectedItem];
	return STATE_CONFIG_VAR;
}
