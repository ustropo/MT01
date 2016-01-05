/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 6, 2015
 *      Author: Fernando
 */

#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"

#include "keyboard.h"
#include "interpreter_if.h"

#include "lcd.h"

/**
 * Shows a configuration menu for the machine.
 *
 * @param pContext Context object
 * @return Main menu state
 */
ut_state ut_state_config_menu(ut_context* pContext)
{

	return STATE_MAIN_MENU;
}
