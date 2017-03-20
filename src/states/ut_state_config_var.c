/*
 * ut_state_config.c
 *
 *  Created on: Jan 5, 2016
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "config_par_maquina.h"
#include "macros.h"
#include "eeprom.h"


#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "config_SwTimers.h"
#include "state_functions.h"

#include "lcd.h"
#include "lcd_menu.h"
#include "keyboard.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define DEFAULT_CONFIG_VAR_TOUT	portMAX_DELAY

extern bool sim;


static void vTimerUpdateCallback( TimerHandle_t pxTimer );
static uint8_t count = 0;
static uint32_t mult = 1;
uint8_t func_back = 0;
static bool blink = 0;

/**
 * Function pointer that can change a configuration
 * variable.
 *
 * @param Configuration variable object pointer.
 */
typedef void (*ut_config_change_ptr)(ut_config_var*);
ut_config_var* configsVar;

static char* boolOptions[2] =
{
	"NÃƒO",
	"SIM"
};

static char* boolJogMaq[2] =
{
	"PLASMA",
	"OXICORTE"
};

static char* boolSim[2] =
{
	"SIMULAÇÃO",
	"DISPARAR E CORTAR"
};

static char* boolEn[2] =
{
	"DESABILITADO",
	"HABILITADO"
};

static char* boolCrem[2] =
{
	"RETA",
	"HELICOIDAL"
};


/**
 * Config boolean variable
 *
 * @param var Boolean variable
 */
void config_bool(ut_config_var* var)
{
	char** boolStr;
	uint32_t *value;
	ut_menu menu;
	uint8_t i;
	bool Recordflag = false;
	func_back = 0;
	/* Initialize */
	ut_menu_init(&menu);
	boolStr = boolOptions;
	menu.itemMarked = 0;
	switch(configsVar->currentState)
	{
		case STATE_AUTO_MODE:
			switch(configsVar->currentItem)
			{
				case CONFIG_AUTO_MODO_SIM_RUN:
					value = var->value;
					boolStr = boolSim;
				break;
				default: break;
			}
			break;

		case STATE_CONFIG_MAQUINA:
			switch(configsVar->currentItem)
			{
				case CFG_MAQUINA_MODOMAQUINA:
					Recordflag =true;
					value = var->value;
					menu.itemMarked = *value + 1;
					boolStr = boolJogMaq;
					break;
				default: break;
			}
			break;
		case STATE_CONFIG_AUTO_MODE:
			switch(configsVar->currentItem)
			{
				case CONFIG_AUTO_SELECIONAR_LINHA:
					boolStr = selecionarLinhatexto();
				break;
				case CONFIG_AUTO_MODO_SIM_RUN:
					value = var->value;
					boolStr = boolSim;
				break;
				default: break;
			}
			break;
		case STATE_CONFIG_MAQUINA_THC:
			Recordflag =true;
			value = var->value;
			menu.itemMarked = *value + 1;
			boolStr = boolEn;
			break;
		case STATE_MAQ_MODEL_SELECTION:
			value = var->value;
			boolStr = boolCrem;
			break;
		default: break;
	}
	 menu.title = var->name;
	 menu.currentState = STATE_CONFIG_VAR;
	for(i = 0; i < 2; i++)
	{
		menu.items[menu.numItems++].text = boolStr[i];
	}
	value = var->value;
	menu.selectedItem = *value % 2; // Just to be sure - it is really not necessary
	/* Check if user selected a valid entry */
	if(ut_menu_browse(&menu, DEFAULT_CONFIG_VAR_TOUT) < 0){
		func_back = 0xFF;
		return;
	}
	func_back = menu.selectedItem;
	*value = menu.selectedItem;
	/* save it - TODO: ask for confirmation, maybe? */
	if(Recordflag)
	{
		eepromWriteConfig(CONFIGFLAG);
	}
}

/**
 * Configure int variables
 *
 * @param var Int variable
 */
void config_int(ut_config_var* var)
{
	float Buffervalue;
	float *value = configsVar->value;
	char szText[MAX_COLUMN];
	uint32_t keyEntry;
	uint32_t decNum = 1;
	uint16_t decCount = 0;
	uint16_t decimalCount = 0;
	uint16_t digits;
	Buffervalue = *value;

	decCount = get_dec_digits(configsVar->valueMax);
	decimalCount = get_decimal_digits(configsVar->step);

	if(decimalCount > 0)
		digits = decCount + decimalCount + 1;
	else
		digits = decCount;

	for (uint8_t i = 1; i<decCount; i++)
	{
		decNum = decNum*10;
	}

	sprintf(szText, "%0*.*f %s", digits , decimalCount,*value,var->unit);
	ut_lcd_output_int_var(var->name,szText,1,false);

	if (swTimers[DOWN_CONFIGVAR_TIMER] == NULL)
	{
	swTimers[DOWN_CONFIGVAR_TIMER]= xTimerCreate
				   (  /* Just a text name, not used by the RTOS kernel. */
					 "Timer Update",
					 /* The timer period in ticks, must be greater than 0. */
					 ( 250 ),
					 /* The timers will auto-reload themselves when they
					 expire. */
					 pdTRUE,
					 /* Assign each timer a unique id equal to its array
					 index. */
					 ( void * ) DOWN_CONFIGVAR_TIMER,
					 /* Each timer calls the same callback when it expires. */
					 vTimerUpdateCallback
				   );
	}

	xTimerStart( swTimers[DOWN_CONFIGVAR_TIMER], 0 );
	/* Loop to increment / decrement value */
	/* Wait for keyboard */
	while(xQueueReceive( qKeyboard, &keyEntry, DEFAULT_CONFIG_VAR_TOUT ))
	{
		/* Check which key */
		switch (keyEntry)
		{
		case KEY_DOWN:
				if(*value > configsVar->valueMin){
					*value = *value - configsVar->step*mult;
				}
				if(*value < configsVar->valueMin){
					*value = configsVar->valueMin;
				}
				blink = 0;
				//ut_lcd_output_int_var(configsVar->name,szText, count + 1,false);
			break;

		case KEY_UP:
				if(*value < configsVar->valueMax){
					*value = *value + configsVar->step*mult;
				}
				if(*value > configsVar->valueMax){
					*value = configsVar->valueMax;
				}
				blink = 0;
				//ut_lcd_output_int_var(configsVar->name,szText, count + 1,false);
			break;

		case KEY_LEFT:
				mult = mult*10;
				if (mult*configsVar->step > decNum)
				{
					mult = mult/10;
				}
				else
				{

					count++;
				}

			break;

		case KEY_RIGHT:
				mult = mult/10;
				if (mult < 1)
				{
					mult = 1;
				}
				else
				{
					count--;
				}
			break;

		case KEY_ENTER:
			xTimerStop( swTimers[DOWN_CONFIGVAR_TIMER], 0 );
			switch(configsVar->currentState)
			{
				case STATE_CONFIG_JOG:
					eepromWriteConfig(CONFIGVAR_JOG);
					if (configsVar->currentItem == CONFIG_JOG_RAPIDO)
					{
						velocidadeJog = &configVarJog[JOG_RAPIDO];
					}
					else
					{
						velocidadeJog = &configVarJog[JOG_LENTO];
					}
					ut_lcd_output_warning("     VALOR     \n     SALVO     \n");
							/* Delay */
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					break;
				case STATE_CONFIG_MENU_OX:
					eepromWriteConfig(CONFIGVAR_OX);
					ut_lcd_output_warning("     VALOR     \n     SALVO     \n");
							/* Delay */
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					break;
				case STATE_CONFIG_MENU_PL:
					eepromWriteConfig(CONFIGVAR_PL);
					ut_lcd_output_warning("     VALOR     \n     SALVO     \n");
							/* Delay */
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					break;
				case STATE_CONFIG_MAQUINA:
					eepromWriteConfig(CONFIGVAR_MAQ);
					ut_lcd_output_warning("     VALOR     \n     SALVO     \n");
							/* Delay */
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					break;
				case STATE_CONFIG_PARAMETROS_MAQ:
					eepromWriteConfig(CONFIGVAR_PAR_MAQ);
					ut_lcd_output_warning("     VALOR     \n     SALVO     \n");
							/* Delay */
					vTaskDelay(2000 / portTICK_PERIOD_MS);
					reset_flag = true;
					break;
				case STATE_CONFIG_AUTO_MODE:
					selecionarlinhas();
					break;

			}

			mult = 1;
			count = 0;
			return;

		case KEY_ESC:
			*value = Buffervalue;
			mult = 1;
			count = 0;
			if(configsVar->currentState == STATE_CONFIG_AUTO_MODE)
			{
				if(configsVar->currentItem == CONFIG_AUTO_SELECIONAR_LINHA)
				{
					configsVar->type = UT_CONFIG_INT;
				}
			}
			func_back = 0xFF;
			xTimerStop( swTimers[DOWN_CONFIGVAR_TIMER], 0 );
			return;

		case KEY_RELEASED:
		//	mult = 1;
		//	count = 0;
			break;

		default:
			break;
		}


	}

}

/**
 * Null handler
 * @param var
 */
void config_null(ut_config_var* var)
{

}

/* Default handlers for variables */
static ut_config_change_ptr var_handlers[UT_CONFIG_MAX] =
{
	&config_int,
	&config_bool,
	&config_null
};

/**
 * This state configures a single variable that is
 * shared among multiple states.
 *
 * @param pContext Context object
 * @return Config menu
 */
ut_state ut_state_config_var(ut_context* pContext)
{
	uint32_t *Flag = configsVar->value;
	ut_state stateBack = (ut_state)pContext->value[0];

	var_handlers[configsVar->type](configsVar);

	switch(configsVar->currentState)
	{
		case STATE_CONFIG_JOG:
			if(func_back == 0xFF)
			{
				stateBack = (ut_state)pContext->value[1];
				func_back = 0;
			}
			break;
		case STATE_CONFIG_MANUAL_MODE:
			if(configsVar->currentItem == CONFIG_MANUAL_DESLOCAR_ZERO )
			{
				uint32_t *Flag = configsVar->value;
				if(*Flag == 1)
				{
					stateBack = (ut_state)pContext->value[1];
				}
			}
			break;
		case STATE_CONFIG_AUTO_MODE:
			switch(configsVar->currentItem)
			{
				case CONFIG_AUTO_RODAR_PROG:
				case CONFIG_AUTO_DESLOCAR_ZERO:
				case CONFIG_AUTO_TESTAR_TAMANHO_PECA:
					if(*Flag == 1)
					{
						stateBack = (ut_state)pContext->value[1];
						sim = false;
					}
					break;
				case CONFIG_AUTO_MODO_SIM:
					if(*Flag == 1)
					{
						sim = true;
						stateBack = (ut_state)pContext->value[1];
					}
					else
					{
						sim = false;
					}
					break;
				case CONFIG_AUTO_SELECIONAR_LINHA:
					do{
						if(configsVar->type == UT_CONFIG_BOOL){
							var_handlers[configsVar->type](configsVar);
							if (func_back == 0xFF)
							{
								configsVar->type = UT_CONFIG_INT;
								var_handlers[configsVar->type](configsVar);
								func_back = 0xFF;
								if(configsVar->type == UT_CONFIG_INT)
								{
									ut_lcd_output_warning("NENHUM PONTO\nDE ENTRADA\nSELECIONADO\n");
									vTaskDelay(2000 / portTICK_PERIOD_MS);
								}
							}
						linhaSelecionada(func_back);
						}
						else
						{
							ut_lcd_output_warning("NENHUM PONTO\nDE ENTRADA\nSELECIONADO\n");
							vTaskDelay(2000 / portTICK_PERIOD_MS);
						}
					}while(func_back == 0xFF && configsVar->type == UT_CONFIG_BOOL);
					func_back = 0;
					break;
				}
			break;
		case STATE_CONFIG_MAQUINA:
			switch(configsVar->currentItem)
			{
			case CFG_MAQUINA_PARAMETROS:
				if(*Flag == 1)
				{
					stateBack = (ut_state)pContext->value[1];
				}
				break;
			}
		break;
	}
	/* Avoid null handler */
	if(configsVar->func_var) { configsVar->func_var(configsVar); }
	switch(configsVar->currentState)
	{
		case STATE_CONFIG_AUTO_MODE:
			switch(configsVar->currentItem)
			{
				case CONFIG_AUTO_TESTAR_TAMANHO_PECA:
					bool *retFile = configsVar->value;
					if (*retFile == false)
					{
						stateBack = (ut_state)pContext->value[0];
					}
				break;
			}
		break;
	}

	return stateBack;
}

static void vTimerUpdateCallback( TimerHandle_t pxTimer )
{
	char szText[MAX_COLUMN];
	long lArrayIndex;
	uint8_t blinkpos = 1;
	float *value = configsVar->value;
	uint16_t decCount = 0;
	uint16_t decimalCount = 0;
	uint16_t digits = 0;

	/* Optionally do something if the pxTimer parameter is NULL. */
	configASSERT( pxTimer );
	decCount = get_dec_digits(configsVar->valueMax);
	decimalCount = get_decimal_digits(configsVar->step);
	if(decimalCount > 0)
		digits = decCount + decimalCount + 1;
	else
		digits = decCount;

	sprintf(szText, "%0*.*f %s", digits , decimalCount,*value,configsVar->unit);
	ut_lcd_output_int_var(configsVar->name,szText, count + 1,blink);
	blink = !blink;
}
