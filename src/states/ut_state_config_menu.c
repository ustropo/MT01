/*
 * ut_state_config_menu.c
 *
 *  Created on: Dec 8, 2015
 *      Author: Fernando
 */

#include "FreeRTOS.h"
#include "task.h"
#include "r_tfat_lib.h"

#include "ut_context.h"
#include "ut_state.h"
#include "ut_config.h"

#include "lcd_menu.h"
#include "lcd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ***********************************************************************
// Global variables
// ***********************************************************************

/* Contains machine configuration */
ut_config_st gstBasParam;

// ***********************************************************************
// Internal variables
// ***********************************************************************

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
 * Menu for machine configuration
 * @param pContext Context object
 * @return Next state
 */
ut_state ut_state_config_menu(ut_context* pContext)
{
	ut_menu config_menu;

	/* Initialize menu */
	ut_menu_init(&config_menu);



	return STATE_MAIN_MENU;
}
