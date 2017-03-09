/*
 * state_functions.c
 *
 *  Created on: 30/04/2016
 *      Author: leocafonso
 */
#include "tinyg.h"			// #1
#include "config.h"			// #2
#include "gcode_parser.h"
#include "macros.h"
#include "planner.h"

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "interpreter_if.h"
#include "state_functions.h"
#include "eeprom.h"
#include "spiffs_hw.h"

#include "keyboard.h"

#include "lcd_menu.h"
#include "lcd.h"
#include "util.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t actualLine;
extern uint32_t previousLine;
extern uint32_t choosedLine;
extern uint32_t choosedLinePosition;
extern TaskHandle_t xCncTaskHandle;

uint32_t lineEntries[2];
uint32_t LinePositionEntries[2];
float selecionarLinhas = 0;
uint32_t LineM5 = 0;
uint32_t currentLineSel = 0;
static char strLinhas[2][20];
char** pstrLinhas;

float zeroPiecebuffer[3] = {0,0,0};

uint32_t selecionarlinhasMax(void)
{
	stat_t status;
	s32_t res;
	s32_t i;
	char s;
	ut_lcd_output_warning("LENDO ARQUIVO\n");
//	iif_bind_line_selection();
	choosedLinePosition = 0;
	choosedLine = 0;
	currentLineSel = 0;
	LineM5 = 0;
	parse_gcode_func_selection(LINE_PARSER);
	macro_func_ptr = command_idle;
	selecionarLinhas = 1000000;
	xio_close(cs.primary_src);
	xio_open(cs.primary_src,0,0);
	i = 0;
	while (true) {
		i++;
		res = SPIFFS_lseek(&uspiffs[0].gSPIFFS, uspiffs[0].f, -i, SPIFFS_SEEK_END);
		res = SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, &s, 1);
		if (s == 'N')
		{	SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, &s, 1);
			if(isdigit(s))
				break;
		}

	}
	i+=2;
	res = SPIFFS_lseek(&uspiffs[0].gSPIFFS, uspiffs[0].f, -i, SPIFFS_SEEK_END);
	while(true)
	{
		if ((status = xio_gets(cs.primary_src, cs.in_buf, sizeof(cs.in_buf))) == STAT_OK) {
			cs.bufp = cs.in_buf;
		}

		if (status == STAT_EOF) {						// EOF can come from file devices only
			xio_close(cs.primary_src);
			break;
		}

		gc_gcode_parser(cs.bufp);
	}

	selecionarLinhas = 0;
	return currentLineSel;
}

void selecionarlinhas(void)
{
	stat_t status;
	ut_lcd_output_warning("LENDO ARQUIVO\n");
//	iif_bind_line_selection();
	choosedLinePosition = 0;
	choosedLine = 0;
	LineM5 = 0;
	parse_gcode_func_selection(LINE_PARSER);
	macro_func_ptr = command_idle;
	xio_close(cs.primary_src);
	xio_open(cs.primary_src,0,0);
	while (true) {
		if ((status = xio_gets(cs.primary_src, cs.in_buf, sizeof(cs.in_buf))) == STAT_OK) {
			cs.bufp = cs.in_buf;

		}
		// handle end-of-file from file devices
		if (status == STAT_EOF) {						// EOF can come from file devices only
			xio_close(cs.primary_src);
			break;
		}
		if (gc_gcode_parser(cs.bufp) == STAT_COMPLETE)
		{
			lineEntries[1] = LineM5;
			LinePositionEntries[1] = actualLine - (actualLine - previousLine);
			configsVar->type = UT_CONFIG_BOOL;
			xio_close(cs.primary_src);
			break;
		}
		if (gc_gcode_parser(cs.bufp) == STAT_OK)
		{
			lineEntries[0] = LineM5;
			LinePositionEntries[0] = actualLine - (actualLine - previousLine);
		}
	}

}

char** selecionarLinhatexto(void)
{
	sprintf(strLinhas[0], "ENTRADA LINHA %d", lineEntries[0]);
	sprintf(strLinhas[1], "ENTRADA LINHA %d", lineEntries[1]);
	pstrLinhas[0] = strLinhas[0];
	pstrLinhas[1] = strLinhas[1];
	return pstrLinhas;
}

void linhaSelecionada(uint32_t flag)
{
	if(flag == 1)
	{
		choosedLine = lineEntries[1];
		choosedLinePosition = LinePositionEntries[1];
	}
	else if(flag == 0)
	{
		choosedLine = lineEntries[0];
		choosedLinePosition = LinePositionEntries[0];
	}
}

void zerar_maquina(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		zeroPiecebuffer[AXIS_X] = 0;
		zeroPiecebuffer[AXIS_Y] = 0;
		zeroPiecebuffer[AXIS_Z] = 0;
		eepromReadConfig(ZEROPIECE);
		xTaskNotifyGive(xCncTaskHandle);
		macro_func_ptr = ZerarMaquina_Macro;
	}
}

void zerar_peca(void *var)
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
		xTaskNotifyGive(xCncTaskHandle);
		macro_func_ptr = ZerarMaquina_Macro;
		zeroPiece[AXIS_X] = 0;
		zeroPiece[AXIS_Y] = 0;
		zeroPiece[AXIS_Z] = 0;
	}
}

void homming_eixos(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	if(*value)
	{
		macro_func_ptr = homming_Macro;
	}
}

void testar_peca(void *var)
{
	uint16_t i = 0;
	char s;
	char *str;
	char num[50];
	float numf;

	ut_config_var *lvar = var;
	bool *value = lvar->value;
	if(*value)
	{
		macro_func_ptr = command_idle;
		xio_close(cs.primary_src);
		xio_open(cs.primary_src,0,0);
		while (true) {
			SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, &s, 1);
			i++;
			if (i > 1000)
			{
				xio_close(cs.primary_src);
				ut_lcd_output_warning("ARQUIVO\nSEM INFO\nDE LIMITES\n");
				vTaskDelay(2000 / portTICK_PERIOD_MS);
				*value = false;
				return;
			}
			if (s == 'M')
			{
				SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, num, 2);
				numf = strtof(num,&str);
				if (numf == 98.0f)
				{
					break;
				}
			}
		}
		while (true) {
			SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, &s, 1);
			if (s == '(')
			{
				SPIFFS_read(&uspiffs[0].gSPIFFS, uspiffs[0].f, num, 50);
				Xcord = strtof(num,&str);
				Ycord = strtof(++str,NULL);
				xio_close(cs.primary_src);
				//xTaskNotifyGive(xCncTaskHandle);
				macro_func_ptr = limit_test;
				break;
			}
		}
	}
}

uint32_t delay_esc(uint32_t timems)
{
	uint32_t lret;
	uint32_t i;
	uint32_t keyEntry = 0xFFFFFFFF;
	i = 0;
	xQueueReset(qKeyboard);
	do{

		lret = xQueueReceive( qKeyboard, &keyEntry, 1 / portTICK_PERIOD_MS );
		if(lret == pdFAIL)
		{
			i++;
		}
	}while(i < timems && keyEntry != KEY_ESC);
	return keyEntry;
}

uint32_t delay_esc_enter(uint32_t timems)
{
	uint32_t lret;
	uint32_t i;
	uint32_t keyEntry = 0xFFFFFFFF;
	i = 0;
	xQueueReset(qKeyboard);
	do{

		lret = xQueueReceive( qKeyboard, &keyEntry, 1 / portTICK_PERIOD_MS );
		if(lret == pdFAIL)
		{
			i++;
		}
	}while(i < timems && keyEntry != KEY_ESC && keyEntry != KEY_ENTER );
	return keyEntry;
}

void mem_format(void *var)
{
	ut_config_var *lvar = var;
	uint32_t *value = lvar->value;
	mem_check mem_ret;
	s32_t res_ext = 0;
	if(*value)
	{
		eepromFormat();
		mem_ret = eepromIntegrityCheck();
		if(mem_ret == MEM_FAIL)
		{
			ut_lcd_output_warning("ERRO 001\n");
			vTaskDelay(4000 / portTICK_PERIOD_MS);
		}
		else
		{
			ut_lcd_output_warning("MEMORIA INTERNA\nINTEGRA\n");
			vTaskDelay(4000 / portTICK_PERIOD_MS);
		}
		ut_lcd_output_warning("FORMATANDO\nMEMÓRIA EXTERNA...\n");
		res_ext = spiffs_format();
		if (res_ext != SPIFFS_OK)
		{
			ut_lcd_output_warning("ERRO 002\n");
			vTaskDelay(4000 / portTICK_PERIOD_MS);
		}
		else
		{
			ut_lcd_output_warning("MEMORIA EXTERNA\nINTEGRA\n");
			vTaskDelay(4000 / portTICK_PERIOD_MS);
		}
		RESET
	}
}

uint8_t get_dec_digits(float fnum)
{
	uint8_t decDigits = 0;
	while (fnum > 1)
	{
		fnum = fnum/10;
		if (fp_EQ(fnum,1))
			fnum = 1;
		decDigits++;
	}
	return decDigits;
}

uint8_t get_decimal_digits(float fnum)
{
	uint8_t decimalDigits = 0;
	while (fnum < 1)
	{
		fnum = fnum*10;
		if (fp_EQ(fnum,1))
			fnum = 1;
		decimalDigits++;
	}
	return decimalDigits;
}

void idle(void *var)
{

}

