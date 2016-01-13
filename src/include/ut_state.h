/*
 * ut_state.h
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */

#ifndef STATES_UT_STATE_H_
#define STATES_UT_STATE_H_

#include "ut_context.h"

/**
 * Enum to indicate available states
 */
typedef enum
{
	STATE_SPLASH = 0,
	STATE_WARNING,
	STATE_MAIN_MENU,
	STATE_CHOOSE_FILE,
	STATE_CONFIG_MENU,
	STATE_CONFIG_MANUAL_MODE,
	STATE_AUTO_MODE,
	STATE_CONFIG_VAR,
	STATE_MANUAL_MODE,
	STATE_DESLOCAZERO_MODE,
	/* This should be always the last one! */
	STATE_NUMBER
} ut_state;

/**
 * Function pointer to a state execution.
 * Any state must have exactly this signature
 *
 * @param ut_context_ptr pointer to context structure
 * @return next state
 */
typedef ut_state (*state_func_ptr)(ut_context*);

/**
 * States declaration.
 *
 * Any additional state goes here!
 */
extern ut_state ut_state_splash(ut_context* pContext);
extern ut_state ut_state_warning(ut_context* pContext);
extern ut_state ut_state_main_menu(ut_context* pContext);
extern ut_state ut_state_choose_file(ut_context* pContext);
extern ut_state ut_state_config_menu(ut_context* pContext);
extern ut_state ut_state_config_manual_menu(ut_context* pContext);
extern ut_state ut_state_config_var(ut_context* pContext);
extern ut_state ut_state_manual_mode(ut_context* pContext);
extern ut_state ut_state_auto_mode(ut_context* pContext);
extern ut_state ut_state_deslocaZero_mode(ut_context* pContext);

/**
 * State map definition
 * Any state relationship must goes here:
 * 	enum -> function
 * In exactly same order as the enum declaration
 */
extern const state_func_ptr states_table[STATE_NUMBER];

// ***********************************************************************
// Defines
// ***********************************************************************
#define MAX_FILE_PATH_SIZE 256
#define USB_ROOT	""
#define DEFAULT_FILE_EXTENSION	".txt"

// ***********************************************************************
// Global variables
// ***********************************************************************
extern char gszCurFile[MAX_FILE_PATH_SIZE];

#endif /* STATES_UT_STATE_H_ */
