/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 6, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	30000

/* Array with all config variables */
ut_config_var configs[CONFIG_MAX];
static bool initialized = false;

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_MAX] =
{
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_BOOL
};

static uint32_t init_values[CONFIG_MAX] =
{
	700,
	1400,
	0
};

static char* init_names[CONFIG_MAX] =
{
	"Jog Lento",
	"Jog Rapido",
	"Teste disparo"
};

static const char* gszConfigMenuTitle = "ESCOLHA UMA OPCAO:";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) return;

	/* Zero all values */
	memset(configs, 0, sizeof(configs));

	/* Initialize all variables */
	for(i = 0; i < CONFIG_MAX; i++)
	{
		configs[i].type = init_types[i];
		configs[i].value = init_values[i];
		configs[i].name = init_names[i];
	}

	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_menu(ut_context* pContext)
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
	for(i = 0; i < CONFIG_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configs[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}

	/* Set selected item */
	pContext->tag = config_menu.selectedItem;
	return STATE_CONFIG_VAR;
}
