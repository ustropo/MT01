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
#include "config_thc_maquina.h"


#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

static bool initialized = false;
static const char* gszConfigMenuTitle = "PAR. DO THC";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CFG_THC_MAX; i++)
		{
			configsTh[i].name = th_init_names[i];
		}
		return;
	}

	/* Zero all values */
	memset(configsTh, 0, sizeof(configsTh));

	/* Initialize all variables */
	for(i = 0; i < CFG_THC_MAX; i++)
	{
		configsTh[i].type = th_init_types[i];
		configsTh[i].valueMax = th_init_max[i];
		configsTh[i].valueMin = th_init_min[i];
		configsTh[i].name = th_init_names[i];
		configsTh[i].unit = th_init_unit[i];
		configsTh[i].step = th_init_step[i];
		configsTh[i].point = th_init_point[i];
		configsTh[i].currentState = STATE_CONFIG_MAQUINA_THC;
		configsTh[i].currentItem = i;
	}
	configsTh[0].value = &configFlags[KERF];
	configsTh[1].value = &configFlags[MERGULHO];
	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_maq_thc(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);

	/* Options */
	config_menu.title = gszConfigMenuTitle;
//	config_menu.offset = 1;
	/* Items */
	for(i = 0; i < CFG_THC_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configsTh[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}

	/* Set selected item */
	pContext->value[0] = STATE_CONFIG_MAQUINA_THC;
	configsVar = &configsTh[config_menu.selectedItem];
	return geNextStateTh[config_menu.selectedItem];
}
