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
#include "config_par_maquina.h"
#include "state_functions.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

static bool initialized = false;
static const char* gszConfigMenuTitle = "PAR. DE MÁQUINA";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CFG_PAR_MAQ_MAX; i++)
		{
			configsParMaq[i].name = pm_init_names[i];
		}
		return;
	}

	/* Zero all values */
	memset(configsParMaq, 0, sizeof(configsParMaq));

	/* Initialize all variables */
	for(i = 0; i < CFG_PAR_MAQ_MAX; i++)
	{
		configsParMaq[i].type = pm_init_types[i];
		configsParMaq[i].valueMax = pm_init_max[i];
		configsParMaq[i].valueMin = pm_init_min[i];
		configsParMaq[i].value = &configVarParMaq[i];
		configsParMaq[i].name = pm_init_names[i];
		configsParMaq[i].unit = pm_init_unit[i];
		configsParMaq[i].step = pm_init_step[i];
		configsParMaq[i].point = pm_init_point[i];
		configsParMaq[i].currentState = STATE_CONFIG_PARAMETROS_MAQ;
		configsParMaq[i].currentItem = i;
	}
	configsParMaq[CFG_FORMAT].func_var = &mem_format;
	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_par_maq(ut_context* pContext)
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
	for(i = 0; i < CFG_PAR_MAQ_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configsParMaq[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		if (reset_flag == true)
		{
			ut_lcd_output_warning("RESETANDO...\n");
					/* Delay */
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			RESET
		}
		return STATE_MAIN_MENU;
	}

	/* Set selected item */
	pContext->value[0] = STATE_CONFIG_PARAMETROS_MAQ;
	eepromReadConfig(CONFIGVAR_PAR_MAQ);
	configsVar = &configsParMaq[config_menu.selectedItem];
	return geNextStatePar[config_menu.selectedItem];
}
