/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 6, 2015
 *      Author: Fernando
 */

#include "tinyg.h"		// #1
#include "hardware.h"
#include "planner.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "interpreter_if.h"
#include "eeprom.h"
#include "macros.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

/* Array with all config variables */
ut_config_var configs_model[CFG_MODEL_MAX];
static bool initialized = false;

static char* init_names[CFG_MODEL_MAX] =
{
	" EASYMAK",
	" COMPACTAXP",
	" MOBILE"
};

static const char* gszConfigMenuTitle = "MODELO MÁQUINA";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CFG_MODEL_MAX; i++)
		{
			configs_model[i].name = init_names[i];
		}
		return;
	}
	/* Zero all values */
	memset(configs_model, 0, sizeof(configs_model));
	for(i = 0; i < CFG_MODEL_MAX; i++)
	{
		configs_model[i].name = init_names[i];
	}

	initialized = true;
}

/**
 * Shows a configuration menu for the jog.
 *
 * @param pContext Context object
 * @return Main menu state
 */

ut_state ut_state_config_maq_model(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);
	/* Options */
	config_menu.title = gszConfigMenuTitle;

	/* Items */
	for(i = 0; i < CFG_MODEL_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configs_model[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_SPLASH;
	}
	switch (config_menu.selectedItem)
	{
		case CFG_MODEL_EASYMAK:
			g_maq.model = EASYMAK_MAQ;
			break;
		case CFG_MODEL_COMPACTA:
			g_maq.model = COMPACTA_MAQ;
			break;
		case CFG_MODEL_MOBILE:
			g_maq.model = MOBILE_MAQ;
			break;
	}

	configsVar->currentState = STATE_MAQ_MODEL_SELECTION;
	configsVar->type = UT_CONFIG_BOOL;
	configsVar->name = "TIPO DE CREMALHEIRA?";
	configsVar->func_var = NULL;
	configsVar->value = &g_maq.crem;
	ut_state_config_var(pContext);

	switch (g_maq.model)
	{
		case EASYMAK_MAQ:
			eepromFormat();
			if (g_maq.crem == MODEL_RETA)
				machine_type_write("EASYMAK","RETA");
			else
				machine_type_write("EASYMAK","HELI");
			g_maq = check_machine_type();
			break;
		case COMPACTA_MAQ:
			eepromFormat();
			if (g_maq.crem == MODEL_RETA)
				machine_type_write("COMPACTA","RETA");
			else
				machine_type_write("COMPACTA","HELI");
			g_maq = check_machine_type();
			break;
		case MOBILE_MAQ:
			eepromFormat();
			if (g_maq.crem == MODEL_RETA)
				machine_type_write("MOBILE","RETA");
			else
				machine_type_write("MOBILE","HELI");
			g_maq = check_machine_type();
			break;
	}
	RESET
	return STATE_SPLASH;
}
