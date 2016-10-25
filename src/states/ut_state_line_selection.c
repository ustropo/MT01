/*
 * ut_state_line_selection.c
 *
 *  Created on: Mar 22, 2016
 *      Author: Leonardo
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
#include "interpreter_if.h"

#include "lcd_menu.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_ENTRIES 20
#define DEFAULT_CONFIG_TIMEOUT	portMAX_DELAY

// ***********************************************************************
// Global variables
// ***********************************************************************
uint32_t lineNumEntry[NUM_ENTRIES];
uint16_t index = 0;
extern uint32_t actualLine;
extern uint32_t previousLine;
extern uint32_t choosedLine;
extern uint32_t choosedLinePosition;

// ***********************************************************************
// Internal variables
// ***********************************************************************
static const char* gszConfigMenuTitle = "SELECIONAR LINHA";

// ***********************************************************************
// Global types
// ***********************************************************************


// ***********************************************************************
// Global functions
// ***********************************************************************


// ***********************************************************************
// Public functions
// ***********************************************************************

/**
 * Select a M5 entry point G
 * @param pContext
 * @return
 */
ut_state ut_state_line_selection(ut_context* pContext)
{
	uint8_t i = 0;
	ut_menu config_line;
	stat_t status;
	char Str[20][20];
	xio_open(cs.primary_src,0,0);
	if(uspiffs[0].f < 0)
	{
		xio_close(cs.primary_src);
		ut_lcd_output_warning("NENHUM ARQUIVO\n\
							   CARREGADO\n");

		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	else
	{
		/* Initialize menu */
		ut_lcd_output_warning("LENDO ARQUIVO\n");
		iif_bind_line_selection();
		ut_menu_init(&config_line);
		choosedLinePosition = 0;
		choosedLine = 0;
		/* Options */
		config_line.title = gszConfigMenuTitle;
		config_line.currentState = STATE_LINE_SELECTION;
		index = 0;
		memset(lineNumEntry,0xFFFFFFFF,sizeof(lineNumEntry));
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
			// set up the buffers
			cs.linelen = strlen(cs.in_buf)+1;					// linelen only tracks primary input
			if (gc_gcode_parser(cs.bufp) == STAT_OK)
			{
				sprintf(Str[config_line.numItems], "ENTRADA LINHA %d", lineNumEntry[config_line.numItems]);
				config_line.items[config_line.numItems].value = actualLine - (actualLine - previousLine);
				if(++config_line.numItems == MENU_MAX_ITEMS) { break; }
			}
		}

		/* Fill menu */
		for(i = 0; i < config_line.numItems; ++i)
		{
			config_line.items[i].text = Str[i];
		}

		/* Show menu */
		config_line.selectedItem = 0;
		if(ut_menu_browse(&config_line, DEFAULT_CONFIG_TIMEOUT) < 0)
		{
			iif_bind_idle();
			return STATE_MAIN_MENU;
		}
		choosedLinePosition = config_line.items[config_line.selectedItem].value;
		choosedLine = lineNumEntry[config_line.selectedItem];
	}
	iif_bind_idle();
	/* Go back to menu */
	return STATE_CONFIG_AUTO_MODE;
}
