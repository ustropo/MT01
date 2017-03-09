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
#include "config_maquina.h"


#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"
#include "lcd_menu.h"
#include "state_functions.h"

#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

static bool initialized = false;
static const char* gszConfigMenuTitle = "CONFIG. DE MÁQUINA";

/**
 * Initialize config array
 */
static void init()
{
	uint8_t i,j;

	/* Check if already initialized */
	j = 0;
	if(initialized) {
		for(i = 0; i < CFG_MAQUINA_MAX; i++)
		{
			if (g_maq.model == COMPACTA_MAQ)
			{
				if (i == CFG_MAQUINA_MODOMAQUINA)
				{
					i++;
				}
			}
			configsMaq[j].name = mq_init_names[j];
			j++;
		}
		return;
	}

	/* Zero all values */
	memset(configsMaq, 0, sizeof(configsMaq));
	j = 0;
	/* Initialize all variables */
	for(i = 0; i < CFG_MAQUINA_MAX; i++)
	{
		if (g_maq.model == COMPACTA_MAQ)
		{
			if (i == CFG_MAQUINA_MODOMAQUINA)
			{
				i++;
			}
		}
		configsMaq[i].type = mq_init_types[i];
		configsMaq[i].valueMax = mq_init_max[i];
		configsMaq[i].valueMin = mq_init_min[i];
		configsMaq[i].name = mq_init_names[i];
		configsMaq[i].unit = mq_init_unit[i];
		configsMaq[i].step = mq_init_step[i];
		configsMaq[i].point = mq_init_point[i];
		configsMaq[i].currentState = STATE_CONFIG_MAQUINA;
		configsMaq[i].currentItem = i;
		j++;
	}
	configsMaq[0].value = &configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
	if (g_maq.model != COMPACTA_MAQ)
	{
		configsMaq[1].value = &configFlags[MODOMAQUINA];
	}
	initialized = true;
}

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_maquina(ut_context* pContext)
{
	ut_menu config_menu;
	uint8_t i,j;

	/* Initialize variables */
	init();

	/* Initialize menu */
	ut_menu_init(&config_menu);

	/* Options */
	config_menu.title = gszConfigMenuTitle;
//	config_menu.offset = 1;
	/* Items */
	j = 0;
	for(i = 0; i < CFG_MAQUINA_MAX; i++)
	{
		if (g_maq.model == COMPACTA_MAQ)
		{
			if (i == CFG_MAQUINA_MODOMAQUINA)
			{
				i++;
			}
		}
		config_menu.items[config_menu.numItems++].text = configsMaq[i].name;
		j++;
	}

	/* Show menu */
	config_menu.selectedItem = 0;
	if(ut_menu_browse(&config_menu, DEFAULT_CONFIG_TIMEOUT) < 0)
	{
		return STATE_MAIN_MENU;
	}
	eepromReadConfig(CONFIGVAR_MAQ);
	if (g_maq.model == COMPACTA_MAQ)
		{
			if (config_menu.selectedItem > CFG_MAQUINA_ALT_DESLOCAMENTO)
			{
				config_menu.selectedItem++;
			}
		}
	/* Set selected item */
	pContext->value[0] = STATE_CONFIG_MAQUINA;
	configsVar = &configsMaq[config_menu.selectedItem];
	switch(config_menu.selectedItem)
	{
		case CFG_MAQUINA_PARAMETROS:
			ut_lcd_output_warning("CUIDADO!!!\nA MÁQUINA IRÁ\nRESETAR\n");
			if(delay_esc(2000) == KEY_ESC)
			{
				xio_close(cs.primary_src);
				ut_lcd_output_warning("COMANDO\nCANCELADO\n");
				/* Delay */
				vTaskDelay(1000 / portTICK_PERIOD_MS);
				return STATE_CONFIG_MAQUINA;
			}
			configsVar->name = "DESEJA CONTINUAR?";
			pContext->value[0] = STATE_CONFIG_MAQUINA;
			pContext->value[1] = STATE_CONFIG_PARAMETROS_MAQ;
			break;
		default:
			break;

	}

	return geNextStateMaq[config_menu.selectedItem];
}
