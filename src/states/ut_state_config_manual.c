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

typedef enum
{
	CONFIG_MODO_MANUAL  = 0,    //!<
	CONFIG_ZERAR_MAQUINA,   //!<
	CONFIG_ZERAR_PECA,   //!<
	CONFIG_DESLOCAR_ZERO,//!<
	CONFIG_JOG_RAP_LENTO,//!<
	CONFIG_MANUAL_MAX           //!< CONFIG_MAX
} ut_config_name;

static void zerar_maquina(void *var);
static void zerar_peca(void *var);
static void homming_eixos(void *var);
static void veljog(void *var);

/* Array with all config variables */
ut_config_var configs_manual[CONFIG_MANUAL_MAX];
static bool initialized = false;
extern ut_config_var* configsVar;
float *velocidadeJog;
float zeroPiecebuffer[3] = {0,0,0};

static const ut_state geNextStateManual[6] =
{
	STATE_MANUAL_MODE,
	STATE_CONFIG_VAR,
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
	" ZERAR MÁQUINA",
	" ZERAR PEÇA",
	" DESLOCAR PARA ZERO",
	" JOG RÁPIDO E LENTO"
};

static var_func init_func[CONFIG_MANUAL_MAX] =
{
	0,
	zerar_maquina,
	zerar_peca,
	homming_eixos,
	veljog,
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
		case CONFIG_DESLOCAR_ZERO:
			ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");
			/* Delay */
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			configsVar->name = "DESEJA CONTINUAR?";
			pContext->value[0] = STATE_CONFIG_MANUAL_MODE;
			pContext->value[1] = STATE_DESLOCAZERO_MODE;
			break;
		default:
			pContext->value[0] = STATE_CONFIG_MANUAL_MODE;
			pContext->value[1] = STATE_CONFIG_MANUAL_MODE;
	}

	return geNextStateManual[config_menu.selectedItem];
}

static void zerar_maquina(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		zeroPiecebuffer[AXIS_X] = 0;
		zeroPiecebuffer[AXIS_Y] = 0;
		zeroPiecebuffer[AXIS_Z] = 0;
		eepromReadConfig(ZEROPIECE);
		macro_func_ptr = ZerarMaquina_Macro;
	}
}

static void zerar_peca(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		zeroPiecebuffer[AXIS_X] += mp_get_runtime_absolute_position(AXIS_X);
		zeroPiecebuffer[AXIS_Y] += mp_get_runtime_absolute_position(AXIS_Y);
		zeroPiecebuffer[AXIS_Z] = 0;

		zeroPiece[AXIS_X] = zeroPiecebuffer[AXIS_X];
		zeroPiece[AXIS_Y] = zeroPiecebuffer[AXIS_Y];
		zeroPiece[AXIS_Z] = 0;
		eepromWriteConfig(ZEROPIECE);
		macro_func_ptr = ZerarMaquina_Macro;
		zeroPiece[AXIS_X] = 0;
		zeroPiece[AXIS_Y] = 0;
		zeroPiece[AXIS_Z] = 0;
	}
}

static void homming_eixos(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		macro_func_ptr = homming_Macro;
	}
}

static void veljog(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		velocidadeJog = &configVar[VELOC_JOG_RAPIDO];
	}
	else
	{
		velocidadeJog = &configVar[VELOC_JOG_LENTO];
	}
}
