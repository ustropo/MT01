/*
 * ut_states_map.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */


#include "ut_context.h"
#include "ut_state.h"

/**
 * State table
 */
const state_func_ptr states_table[STATE_NUMBER] =
{
		&ut_state_splash,
		&ut_state_warning,
		&ut_state_main_menu,
		&ut_state_choose_file,
		&ut_state_config_menu,
		&ut_state_config_manual_menu,
		&ut_state_config_auto_menu,
		&ut_state_config_var,
		&ut_state_manual_mode, //Manual menu init
		&ut_state_deslocaZero_mode, //
		&ut_state_auto_mode
};
