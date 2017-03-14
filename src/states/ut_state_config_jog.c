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
ut_config_var configs_jog[CONFIG_JOG_MAX];
static bool initialized = false;

static char* jog_unit = "mm/min";

static const ut_state geNextStateManual[CONFIG_JOG_MAX] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR
};

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_JOG_MAX] =
{
	UT_CONFIG_INT,
	UT_CONFIG_INT
};

static char* init_names[CONFIG_JOG_MAX] =
{
	" VELOCIDADE RÁPIDA",
	" VELOCIDADE LENTA"
};

static var_func init_func[CONFIG_JOG_MAX] =
{
	NULL,
	NULL,
};

static const char* gszConfigMenuTitle = "VELOCIDADE MANUAL";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CONFIG_JOG_MAX; i++)
		{
			configs_jog[i].name = init_names[i];
		}
		return;
	}

	/* Zero all values */
	memset(configs_jog, 0, sizeof(configs_jog));

	/* Initialize all variables */
	for(i = 0; i < CONFIG_JOG_MAX; i++)
	{
		configs_jog[i].type = init_types[i];
		configs_jog[i].name = init_names[i];
		configs_jog[i].func_var = init_func[i];
		configs_jog[i].valueMax = fmin(configVarParMaq[CFG_PAR_MAQ_VEL_X],configVarParMaq[CFG_PAR_MAQ_VEL_Y]);
		configs_jog[i].valueMin = MOTOR_VMIN;
		configs_jog[i].step = 1;
		configs_jog[i].unit = jog_unit;
		configs_jog[i].currentState = STATE_CONFIG_JOG;
		configs_jog[i].currentItem = i;
	}
	configs_jog[CONFIG_JOG_RAPIDO].value = &configVarJog[JOG_RAPIDO];
	configs_jog[CONFIG_JOG_LENTO].value = &configVarJog[JOG_LENTO];

	initialized = true;
}

/**
 * Shows a configuration menu for the jog.
 *
 * @param pContext Context object
 * @return Main menu state
 */

ut_state ut_state_config_jog(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);
	config_menu.currentState = STATE_CONFIG_JOG;
	/* Options */
	config_menu.title = gszConfigMenuTitle;

	/* Items */
	for(i = 0; i < CONFIG_JOG_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configs_jog[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_CONFIG_MANUAL_MODE;
	}

	pContext->value[0] = STATE_MANUAL_MODE;
	pContext->value[1] = STATE_CONFIG_JOG;
	configsVar = &configs_jog[config_menu.selectedItem];
	return geNextStateManual[config_menu.selectedItem];
}
