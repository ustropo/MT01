/*
 * ut_states_task.c
 *
 *  Created on: Nov 1, 2015
 *      Author: Fernando
 */


#include "ut_context.h"
#include "ut_state.h"
#include "lcd.h"
#include "interpreter_if.h"

#include <string.h>
#include <stdio.h>

ut_state currentState = STATE_SPLASH;

/**
 * Execute state machine.
 */
void states_task(void)
{
	ut_context pContext;
	iif_bind_idle();
	/* Initialize context */
	memset(&pContext, 0, sizeof(ut_context));

	/* Initialize lcd */
	ut_lcd_init();

	/* Run machine */
	while(currentState < STATE_NUMBER)
	{
		/* Execute state function and get next state */
		currentState = states_table[currentState](&pContext);
	}

	/* Error! */
	while(true)
	{

	}
}

