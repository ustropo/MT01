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
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

extern char gszCurFile[MAX_FILE_PATH_SIZE];
extern uint32_t choosedLine;

/* Array with all config variables */
ut_config_var configs_auto[CONFIG_AUTO_MAX];
static bool initialized = false;

static const ut_state geNextStateAuto[CONFIG_AUTO_MAX] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
};

/* Initial values for each config variable */
static ut_config_type init_types[CONFIG_AUTO_MAX] =
{
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_BOOL,
	UT_CONFIG_INT,
	UT_CONFIG_BOOL
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
	" RODAR SIMULADO",
	" DESLOCAR - ZERO PE�A",
	" SELECIONAR LINHA",
	" LIMITES DO DESENHO"
};

static var_func init_func[CONFIG_AUTO_MAX] =
{
	idle,
	idle,
	homming_eixos,
	idle,
	testar_peca
};

static const char* gszConfigMenuTitle = "CORTE AUTOM�TICO";

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
	configs_auto[CONFIG_AUTO_SELECIONAR_LINHA].unit = "";
	configs_auto[CONFIG_AUTO_SELECIONAR_LINHA].step = 1;
	configs_auto[CONFIG_AUTO_SELECIONAR_LINHA].valueMax = 100000;
	configs_auto[CONFIG_AUTO_SELECIONAR_LINHA].valueMin = 0;
	configs_auto[CONFIG_AUTO_SELECIONAR_LINHA].value = &selecionarLinhas;
	initialized = false;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_auto_menu(ut_context* pContext)
{
	char Str[20];
	ut_menu config_menu;
	uint8_t i;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);

	/* Options */
	config_menu.title = gszConfigMenuTitle;
	config_menu.currentState = STATE_CONFIG_AUTO_MODE;
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
		case CONFIG_AUTO_RODAR_PROG:
		case CONFIG_AUTO_MODO_SIM:
			if(0)
			//if(gszCurFile[0] == NULL)
			{
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");

				vTaskDelay(2000 / portTICK_PERIOD_MS);
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				return STATE_CHOOSE_FILE;
			}
			else
			{
				ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOM�TICO\n");
				/* Delay */
				vTaskDelay(2000 / portTICK_PERIOD_MS);

				sprintf(Str, "PONTO DE ENTRADA\nLINHA %d\n", choosedLine);
				ut_lcd_output_warning(Str);
				/* Delay */
				vTaskDelay(2000 / portTICK_PERIOD_MS);
				if(configFlags[MODOMAQUINA] == MODO_PLASMA)
				{
					eepromReadConfig(CONFIGVAR_PL);
					sprintf(Str, "VEL. CORTE: %.0f\nTENS�O THC: %.0f V\n",
							configVarPl[PL_CONFIG_VELOC_CORTE],
							configVarPl[PL_CONFIG_TENSAO_THC]);
				}
				else
				{
					eepromReadConfig(CONFIGVAR_OX);
					sprintf(Str, "VEL. CORTE: %.0f\n",configVarOx[OX_CONFIG_VELOC_CORTE]);
				}
				ut_lcd_output_warning(Str);
				/* Delay */
				vTaskDelay(2500 / portTICK_PERIOD_MS);

				configsVar->name = "DESEJA CONTINUAR?";
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_AUTO_MODE;
			}
			break;
		case CONFIG_AUTO_DESLOCAR_ZERO:
			ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOM�TICO\n");
			/* Delay */
			vTaskDelay(2000 / portTICK_PERIOD_MS);
			configsVar->name = "DESEJA CONTINUAR?";
			pContext->value[0] = STATE_CONFIG_AUTO_MODE;
			pContext->value[1] = STATE_DESLOCAZERO_MODE;
			break;
		case CONFIG_AUTO_SELECIONAR_LINHA:
			if(gszCurFile[0] == NULL)
			{
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");

				vTaskDelay(2000 / portTICK_PERIOD_MS);
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				return STATE_CONFIG_AUTO_MODE;
			}
			else
			{
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				configsVar->valueMax = (float)selecionarlinhasMax();
				if (configsVar->valueMax < 1){
					ut_lcd_output_warning("FORMATO N�O\nRECONHECIDO\nSEM N�MERO LINHA\n");
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}
			}
			break;
		case CONFIG_AUTO_TESTAR_TAMANHO_PECA:
			ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOM�TICO\n");
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
