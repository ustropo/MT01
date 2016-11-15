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
#include "state_functions.h"

#include "FreeRTOS.h"
#include "task.h"
#include "keyboard.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

extern TaskHandle_t xCncTaskHandle;

/* Array with all config variables */
ut_config_var configs_manual[CONFIG_MANUAL_MAX];
static bool initialized = false;
static float zeroPiecebuffer[3] = {0,0,0};

static const ut_state geNextStateManual[6] =
{
	STATE_MANUAL_MODE,
	STATE_CONFIG_JOG,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_MAIN_MENU
};

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_MANUAL_MAX] =
{
	UT_CONFIG_NULL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL
};

static uint32_t init_values[CONFIG_MANUAL_MAX] =
{
	0,
	0,
	0,
	0,
	0
};

static char* init_names[CONFIG_MANUAL_MAX] =
{
	" MODO MANUAL",
	" VELOCIDADE MANUAL",
	" ZERAR PEÇA",
	" DESLOCAR - ZERO PEÇA",
	" ZERAR MÁQUINA",
};

static var_func init_func[CONFIG_MANUAL_MAX] =
{
	0,
	0,
	zerar_peca,
	homming_eixos,
	zerar_maquina,
};

static const char* gszConfigMenuTitle = "CONFIG. MANUAL";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CONFIG_MANUAL_MAX; i++)
		{
			configs_manual[i].name = init_names[i];
		}
		return;
	}

	/* Zero all values */
	memset(configs_manual, 0, sizeof(configs_manual));

	/* Initialize all variables */
	for(i = 0; i < CONFIG_MANUAL_MAX; i++)
	{
		configs_manual[i].type = init_types[i];
		configs_manual[i].value = &init_values[i];
		configs_manual[i].name = init_names[i];
		configs_manual[i].func_var = init_func[i];
		configs_manual[i].currentState = STATE_CONFIG_MANUAL_MODE;
		configs_manual[i].currentItem = i;
	}

	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
//ut_state ut_state_config_manual_menu(ut_context* pContext)
ut_state ut_state_config_manual_menu(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);
	config_menu.currentState = STATE_CONFIG_MANUAL_MODE;
	/* Options */
	config_menu.title = gszConfigMenuTitle;

	/* Items */
	for(i = 0; i < CONFIG_MANUAL_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configs_manual[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}

	configsVar = &configs_manual[config_menu.selectedItem];
	switch(config_menu.selectedItem)
	{
		case CONFIG_MANUAL_DESLOCAR_ZERO:
//			if ((mp_get_runtime_absolute_position(0) == AXIS_X) && (mp_get_runtime_absolute_position(AXIS_Y) == 0)	)
//			{
//				ut_lcd_output_warning("MÁQUINA\nESTÁ NO\nZERO PEÇA\n");
//				/* Delay */
//				vTaskDelay(1000 / portTICK_PERIOD_MS);
//				return STATE_CONFIG_AUTO_MODE;
//			}
			ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");
			if(delay_esc(2000) == KEY_ESC)
			{
				ut_lcd_output_warning("COMANDO\nCANCELADO\n");
				/* Delay */
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				return STATE_CONFIG_AUTO_MODE;
			}
			configsVar->name = "DESEJA CONTINUAR?";
			pContext->value[0] = STATE_CONFIG_MANUAL_MODE;
			pContext->value[1] = STATE_DESLOCAZERO_MODE;
			break;
		case CONFIG_MANUAL_ZERAR_MAQUINA:
			ut_lcd_output_warning("DEVE ESTAR NOS\nLIMITES FISICOS\nX0 E Y0\n");
			if(delay_esc(2000) == KEY_ESC)
			{
				ut_lcd_output_warning("COMANDO\nCANCELADO\n");
				/* Delay */
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				return STATE_CONFIG_AUTO_MODE;
			}
			configsVar->name = "ZERAR A MÁQUINA?";
			pContext->value[0] = STATE_CONFIG_MANUAL_MODE;
			pContext->value[1] = STATE_CONFIG_MANUAL_MODE;
			break;
		default:
			pContext->value[0] = STATE_CONFIG_MANUAL_MODE;
			pContext->value[1] = STATE_CONFIG_MANUAL_MODE;
	}

	return geNextStateManual[config_menu.selectedItem];
}
