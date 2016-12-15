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
	" DESLOCAR - ZERO PEÇA",
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

static const char* gszConfigMenuTitle = "CORTE AUTOMÁTICO";

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
	char Str[30];
	void *temp = NULL;
	char *pstr;
	ut_menu config_menu;
	uint8_t i;
	spiffs_stat fileStat;
	uint8_t uiMsgRow = 0;

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
			xio_open(cs.primary_src,0,0);
			if(uspiffs[0].f < 0)
			{
				xio_close(cs.primary_src);
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");

				vTaskDelay(2000 / portTICK_PERIOD_MS);
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				return STATE_CHOOSE_FILE;
			}
			else
			{

				ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");

				if(delay_esc(2000) == KEY_ESC)
				{
					xio_close(cs.primary_src);
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}

				sprintf(Str, "PONTO DE ENTRADA\nLINHA %d\n", choosedLine);
				ut_lcd_output_warning(Str);

				if(delay_esc(2000) == KEY_ESC)
				{
					xio_close(cs.primary_src);
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}

				SPIFFS_fstat(&uspiffs[0].gSPIFFS, uspiffs[0].f, &fileStat);

				ut_lcd_clear();

				temp = pvPortMalloc( 30*MAX_ROW);
				memset(temp,NULL,30*MAX_ROW);
				pstr = temp;
				snprintf(&pstr[30*1],30, "Nome:%s",fileStat.name);
				if(configFlags[MODOMAQUINA] == MODO_PLASMA)
				{
					eepromReadConfig(CONFIGVAR_PL);
					snprintf(&pstr[0],30, "    MODO PLASMA     ");
					snprintf(&pstr[30*2],30, "Vel. corte: %.0f mm/min",configVarPl[PL_CONFIG_VELOC_CORTE]);
					snprintf(&pstr[30*3],30, "Tensão THC: %.0f V",configVarPl[PL_CONFIG_TENSAO_THC]);

					if (configFlags[KERF] == HABILITADO)
						snprintf(&pstr[30*4],30, "Kerf: habilitado");
					else
						snprintf(&pstr[30*4],30, "Kerf: desabilitado");

					if (configFlags[MERGULHO] == HABILITADO)
						snprintf(&pstr[30*5],30, "Anti mergulho:habilitado");
					else
						snprintf(&pstr[30*5],30, "Anti mergulho:desabilitado");
				}
				else
				{
					snprintf(&pstr[0],30, "      MODO OXI      ");
					eepromReadConfig(CONFIGVAR_OX);
					snprintf(&pstr[30*2],30, "Vel. corte: %.0f mm/min",configVarOx[OX_CONFIG_VELOC_CORTE]);
				}
				ut_lcd_drawStr(0, 0, &pstr[0], BACKGROUND_FRAMED,ITEM_NO_MARKED,u8g_font_helvB08);
				for(uiMsgRow = 1; uiMsgRow < MAX_ROW; uiMsgRow++)
				{
					ut_lcd_drawStr(uiMsgRow, 0, &pstr[uiMsgRow*30],ITEM_NO_MARKED, false,u8g_font_5x8);
				}
				/* Output */
				ut_lcd_output_str();

				vPortFree(temp);

				if(delay_esc_enter(5000) == KEY_ESC)
				{
					xio_close(cs.primary_src);
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}

				xio_close(cs.primary_src);
				configsVar->name = "DESEJA CONTINUAR?";
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_AUTO_MODE;
			}
			break;
		case CONFIG_AUTO_DESLOCAR_ZERO:
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
			pContext->value[0] = STATE_CONFIG_AUTO_MODE;
			pContext->value[1] = STATE_DESLOCAZERO_MODE;
			break;
		case CONFIG_AUTO_SELECIONAR_LINHA:
			xio_open(cs.primary_src,0,0);
			if(uspiffs[0].f < 0)
			{
				xio_close(cs.primary_src);
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");
				if(delay_esc(2000) == KEY_ESC)
				{
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}
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
					ut_lcd_output_warning("FORMATO NÃO\nRECONHECIDO\nSEM NÚMERO LINHA\n");
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}
			}
			break;
		case CONFIG_AUTO_TESTAR_TAMANHO_PECA:

			xio_open(cs.primary_src,0,0);
			if(uspiffs[0].f < 0)
			{
				xio_close(cs.primary_src);
				ut_lcd_output_warning("NENHUM ARQUIVO\n\
									   CARREGADO\n");
				if(delay_esc(2000) == KEY_ESC)
				{
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_CONFIG_AUTO_MODE;
				return STATE_CONFIG_AUTO_MODE;
			}
			else
			{
				ut_lcd_output_warning("CUIDADO!!!\nMOVIMENTO\nAUTOMÁTICO\n");
				if(delay_esc(2000) == KEY_ESC)
				{
					xio_close(cs.primary_src);
					ut_lcd_output_warning("COMANDO\nCANCELADO\n");
					/* Delay */
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					return STATE_CONFIG_AUTO_MODE;
				}
				configsVar->name = "DESEJA CONTINUAR?";
				pContext->value[0] = STATE_CONFIG_AUTO_MODE;
				pContext->value[1] = STATE_DESLOCAZERO_MODE;
			}
			break;
		default:
			pContext->value[0] = STATE_CONFIG_AUTO_MODE;
			pContext->value[1] = STATE_CONFIG_AUTO_MODE;
	}

	return geNextStateAuto[config_menu.selectedItem];
}
