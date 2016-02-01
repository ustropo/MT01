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

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	30000

typedef enum
{
	CONFIG_ALTURA_DESLOCAMENTO = 0,    //!< CONFIG_JOG_LENTO
	CONFIG_ALTURA_PERFURACAO,   //!< CONFIG_JOG_RAPIDO
	CONFIG_TEMPO_PERFURACAO,//!< CONFIG_TESTE_DISPARO
	CONFIG_ALTURA_CORTE, // ALTURA DE CORTE
	CONFIG_VELOC_CORTE, // VELOC. DE CORTE
	CONFIG_VELOC_JOG_LENTO, // VELOC. DE JOG LENTO
	CONFIG_VELOC_JOG_RAPIDO, // VELOC. DE JOG RÁPIDO
	CONFIG_CANCELAR_IHS, // CANCELAR IHS
	CONFIG_TESTE_DISPARO, //" TESTE DE DISPARO",
	CONFIG_TESTE_SENSOR_CHAPA, //" TESTE SENSOR DE CHAPA",
	CONFIG_TESTE_SENSOR_ARCOOK, //" TESTE SENSOR ARCO OK",
	CONFIG_TESTE_EMERGENCIA, //" TESTE BOTÃO DE EMERGÊNCIA"
	CONFIG_MAX           //!< CONFIG_MAX
} ut_config_name;

/* Array with all config variables */
ut_config_var configs[CONFIG_MAX];
static bool initialized = false;
extern ut_config_var* configsVar;

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_MAX] =
{
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL
};

static uint32_t init_values[CONFIG_MAX] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

static char* init_names[CONFIG_MAX] =
{
	" ALTURA DESLOCAMENTO",
	" ALTURA DE PERFURAÇÃO",
	" TEMPO DE PERFURAÇÃO",
	" ALTURA DE CORTE",
	" VELOC. DE CORTE",
	" VELOC. DE JOG LENTO",
	" VELOC. DE JOG RÁPIDO",
	" CANCELAR IHS",
	" TESTE DE DISPARO",
	" TESTE SENSOR DE CHAPA",
	" TESTE SENSOR ARCO OK",
	" TESTE BOTÃO DE EMERGÊNCIA"
};

static const char* gszConfigMenuTitle = "CONFIG. DE CORTE";

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
		configs[i].value = &configVar[i];
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
//	config_menu.offset = 1;
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
	pContext->value[0] = STATE_CONFIG_MENU;
	configsVar = &configs[config_menu.selectedItem];
	return STATE_CONFIG_VAR;
}
