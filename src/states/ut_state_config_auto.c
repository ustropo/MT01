/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 6, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "interpreter_if.h"
#include "macros.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	30000

extern char gszCurFile[MAX_FILE_PATH_SIZE];

typedef enum
{
	CONFIG_RODAR_PROG  = 0,    //!<
	CONFIG_ZERAR_EIXOS,   //!<
	CONFIG_DESLOCAR_ZERO,//!<
	CONFIG_MODO_SIM,//!<
	CONFIG_SELECIONAR_LINHA,//!<
	CONFIG_AUTO_MAX           //!< CONFIG_MAX
} ut_config_name;

extern const char zera_eixos[];

extern const char home_eixos[];

static void zerar_eixos(void *var);
static void homming_eixos(void *var);
static void idle(void *var);

/* Array with all config variables */
ut_config_var configs_auto[CONFIG_AUTO_MAX];
static bool initialized = false;
extern ut_config_var* configsVar;

static const ut_state geNextStateAuto[5] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_MAIN_MENU,
};

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_AUTO_MAX] =
{
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_NULL
};

static uint32_t init_values[CONFIG_AUTO_MAX] =
{
	0,
	0,
	0,
	0,
	0
};

static char* init_names[CONFIG_AUTO_MAX] =
{
	" RODAR PROGRAMA",
	" ZERAR EIXOS",
	" DESLOCAR PARA ZERO",
	" MODO SIMULADO",
	" SELECIONAR LINHA"
};

static var_func init_func[CONFIG_AUTO_MAX] =
{
	idle,
	zerar_eixos,
	homming_eixos,
	idle,
	idle
};

static const char* gszConfigMenuTitle = "CONFIG. AUTOMÁTICO";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i;

	/* Check if already initialized */
	if(initialized) {
		for(i = 0; i < CONFIG_AUTO_MAX; i++)
		{
			configs_auto[i].name = init_names[i];
		}
		return;
	}

	/* Zero all values */
	memset(configs_auto, 0, sizeof(configs_auto));

	/* Initialize all variables */
	for(i = 0; i < CONFIG_AUTO_MAX; i++)
	{
		configs_auto[i].type = init_types[i];
		configs_auto[i].value = &init_values[i];
		configs_auto[i].name = init_names[i];
		configs_auto[i].func_var = init_func[i];
		configs_auto[i].currentState = STATE_CONFIG_AUTO_MODE;
		configs_auto[i].currentItem = i;
	}

	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_auto_menu(ut_context* pContext)
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
	for(i = 0; i < CONFIG_AUTO_MAX; i++)
	{
		config_menu.items[config_menu.numItems++].text = configs_auto[i].name;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}
	configsVar = &configs_auto[config_menu.selectedItem];
	switch(config_menu.selectedItem)
	{
		case CONFIG_RODAR_PROG:
		case CONFIG_MODO_SIM:
			if(gszCurFile[0] == NULL)
			{
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");

				vTaskDelay(1000 / portTICK_PERIOD_MS);
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				return STATE_CONFIG_AUTO_MODE;
			}
			else
			{
				ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");
				/* Delay */
				vTaskDelay(2000 / portTICK_PERIOD_MS);
				configsVar->name = "DESEJA CONTINUAR?";
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_AUTO_MODE;
			}
			break;
		case CONFIG_DESLOCAR_ZERO:
			ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");
			/* Delay */
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			configsVar->name = "DESEJA CONTINUAR?";
			pContext->value[0] = STATE_CONFIG_AUTO_MODE;
			pContext->value[1] = STATE_DESLOCAZERO_MODE;
			break;
		default:
			pContext->value[0] = STATE_CONFIG_AUTO_MODE;
			pContext->value[1] = STATE_CONFIG_AUTO_MODE;
	}

	return geNextStateAuto[config_menu.selectedItem];
}

static void zerar_eixos(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		tg_set_primary_source(XIO_DEV_COMMAND);
		xio_open(cs.primary_src,zera_eixos,0);
//		macro_func_ptr = ZerarEixos_Macro();
	}
}

static void homming_eixos(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		tg_set_primary_source(XIO_DEV_COMMAND);
		xio_open(cs.primary_src,home_eixos,0);
	}
}

static void idle(void *var)
{

}
