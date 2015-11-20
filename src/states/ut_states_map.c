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
		&ut_state_choose_file
};
