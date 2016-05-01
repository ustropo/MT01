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

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_state_config_var.h"
#include "interpreter_if.h"
#include "state_functions.h"

#include "lcd_menu.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t actualLine;
extern uint32_t previousLine;
extern uint32_t choosedLine;
extern uint32_t choosedLinePosition;
uint32_t lineEntries[2];
uint32_t LinePositionEntries[2];
float selecionarLinhas = 0;
uint32_t LineM5 = 0;
uint32_t currentLineSel = 0;
static char strLinhas[2][20];
char** pstrLinhas;

uint32_t selecionarlinhasMax(void)
{
	stat_t status;
	ut_lcd_output_warning("LENDO ARQUIVO\n");
//	iif_bind_line_selection();
	choosedLinePosition = 0;
	choosedLine = 0;
	currentLineSel = 0;
	LineM5 = 0;
	parse_gcode_func_selection(LINE_PARSER);
	macro_func_ptr = command_idle;
	selecionarLinhas = 100000;
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
	if(flag)
	{
		choosedLine = lineEntries[1];
		choosedLinePosition = LinePositionEntries[1];
	}
	else
	{
		choosedLine = lineEntries[0];
		choosedLinePosition = LinePositionEntries[0];
	}
}
