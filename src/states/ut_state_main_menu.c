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

/**
 *
 */
typedef enum
{
	MAIN_MENU_FILE = 0,	//!< MAIN_MENU_FILE
	MAIN_MENU_MANUAL,  	//!< MAIN_MENU_MANUAL
	MAIN_MENU_AUTO,    	//!< MAIN_MENU_AUTO
	MAIN_MENU_CONFIG,   //!< MAIN_MENU_CONFIG
	/* Insert options before */
	MAIN_MENU_NUMBER	//!< MAIN_MENU_NUMBER
} main_menu_options;

/**
 * Menu options
 */
static const char* gszMainMenuLabels[MAIN_MENU_NUMBER] =
{
	"LER ARQUIVO DE CORTE",
	"MODO MANUAL",
	"CORTE AUTOMÁTICO",
	"CONFIGURAÇÃO DE CORTE",
};
static const char* gszMainMenuTitle = "  ESCOLHA UMA OPÇÃO:";

/**
 * Next states
 */
static const ut_state geNextState[MAIN_MENU_NUMBER] =
{
	STATE_CHOOSE_FILE,
	STATE_MANUAL_MODE,
	STATE_AUTO_MODE,
	STATE_CONFIG_MENU,
};

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
	ut_menu main_menu;
	uint8_t i = 0;

	/* Init menu */
	ut_menu_init(&main_menu);

	/* Options */
	main_menu.title = gszMainMenuTitle;
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

	return geNextState[main_menu.selectedItem];
}
