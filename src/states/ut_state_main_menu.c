/*
 * ut_states_main_menu.c
 *
 *  Created on: Nov 1, 2015
 *      Author: Fernando
 */


#include "ut_context.h"
#include "ut_state.h"

#include "lcd_menu.h"

#include "FreeRTOS.h"
#include "task.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "interpreter_if.h"
#include "eeprom.h"
#include "macros.h"

/**
 * Menu options
 */
static const char* gszMainMenuLabels[MAIN_MENU_NUMBER] =
{
	" CARREGAR ARQUIVO",
	" MODO MANUAL",
	" CORTE AUTOMÁTICO",
	" CONFIG. DE CORTE",
	" CONFIG. DE MÁQUINA"
};
static const char* gszMainMenuTitle = "PRINCIPAL";

/**
 * Next states
 */
static const ut_state geNextState[MAIN_MENU_NUMBER] =
{
	STATE_CHOOSE_FILE,
	STATE_CONFIG_MANUAL_MODE,
	STATE_CONFIG_AUTO_MODE,
	STATE_CONFIG_MENU_PL,
	STATE_CONFIG_MAQUINA,
};

ut_config_var configs_main;

/**
 * Main menu options.
 *
 * This should be the main option and the idle state.
 *
 * @param pContext
 * @return Next state
 */
ut_state ut_state_main_menu(ut_context* pContext)
{
	ut_state lreturn;
	ut_menu main_menu;
	uint8_t i = 0;

	/* Init menu */
	ut_menu_init(&main_menu);
	main_menu.maxItemsPerPage = 4;

	configs_main.type = UT_CONFIG_BOOL;
	configs_main.value = &configFlags[MODOMAQUINA];
	configs_main.name = gszMainMenuLabels[MAIN_MENU_MODOMAQUINA];
	configs_main.func_var = NULL;
	configs_main.currentState = STATE_MAIN_MENU;
	configs_main.currentItem = MAIN_MENU_MODOMAQUINA;

	/* Options */
	main_menu.title = gszMainMenuTitle;
	main_menu.currentState = STATE_MAIN_MENU;
	/* Items */
	for(i = 0; i < MAIN_MENU_NUMBER; i++)
	{
		main_menu.items[main_menu.numItems++].text = gszMainMenuLabels[i];
	}

	/* Show menu - it waits here until a valid
	 * option is selected */
	main_menu.selectedItem = 0;
	while(ut_menu_browse(&main_menu, portMAX_DELAY) < 0)
	{
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	/* Set selected item */
	pContext->value[0] = STATE_MAIN_MENU;
	if(main_menu.selectedItem == MAIN_MENU_MODOMAQUINA)
		configsVar = &configs_main;
	if(main_menu.selectedItem == MAIN_MENU_CONFIG)
	{
		if(configFlags[MODOMAQUINA] == MODO_PLASMA)
			lreturn = STATE_CONFIG_MENU_PL;
		else
			lreturn = STATE_CONFIG_MENU_OX;
	}
	else
	{
		lreturn = geNextState[main_menu.selectedItem];
	}
	return lreturn;
}
