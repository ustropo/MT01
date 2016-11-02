/*
 * interpreter_jog_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "tinyg.h"
#include "platform.h"
#include "interpreter_if.h"
#include "controller.h"
#include "spindle.h"
#include "macros.h"
#include "plasma.h"
#include "keyboard.h"
#include "settings.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void iif_enter_jog(void);
static void iif_esc_jog(void);
static void iif_down_jog(void);
static void iif_up_jog(void);
static void iif_left_jog(void);
static void iif_right_jog(void);
static void iif_zdown_jog(void);
static void iif_zup_jog(void);
static void iif_released_jog(void);

extern float *velocidadeJog;
char text[40];
uint32_t JogkeyPressed;
const char jog_stopflush[]= "\
!\n\
%";

void timerJogCallback (void *p_arg);
void timerJogInitCallback (void *p_arg);
void timerJogWalkCallback (void *p_arg);



void iif_enter_jog(void)
{
	static bool torchEnable = false;
	if(!torchEnable)
	{
	//	cm_spindle_control(SPINDLE_CW);
		isCuttingSet(true);
		TORCH = TRUE;
		torchEnable = true;
	}
	else
	{
	//	cm_spindle_control(SPINDLE_OFF);
		isCuttingSet(false);
		TORCH = FALSE;
		torchEnable = false;
	}
}

void iif_esc_jog(void)
{
	if (timerIif == 3)
	{
		R_CMT_Stop(timerIif);
	}
	TORCH = FALSE;
	cm_request_feedhold();
	cm_request_queue_flush();
	while(cm.queue_flush_requested == true)
	{

	}
	iif_bind_idle();
	JogkeyPressed = 0;
	macro_func_ptr = command_idle;
}

void iif_down_jog(void) {
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}
void iif_up_jog(void) {
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}
void iif_left_jog(void){
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_right_jog(void) {
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_zdown_jog(void){
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_zup_jog(void) {
	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_released_jog(void) {

//	cm_request_feedhold();
//	cm_request_queue_flush();
	//macro_func_ptr = _command_dispatch;
	if (timerIif == 3)
	{
		R_CMT_Stop(timerIif);
	}
}

void iif_bind_jog(void)
{
	JogkeyPressed = 0;

	jogMaxDistance[AXIS_X] = 0;
	jogMaxDistance[AXIS_Y] = 0;
	jogMaxDistance[AXIS_Z] = 0;
	macro_func_ptr = jog_Macro;
	iif_func_enter = &iif_enter_jog;
	iif_func_esc = &iif_esc_jog;
	iif_func_down = &iif_down_jog;
	iif_func_up = &iif_up_jog;
	iif_func_left = &iif_left_jog;
	iif_func_right = &iif_right_jog;
	iif_func_zdown = &iif_zdown_jog;
	iif_func_zup = &iif_zup_jog;
	iif_func_released = &iif_released_jog;
	iif_func_cycleStop = &iif_idle;
}

#define MULTI_A 0.000076923
#define NUM_B 0.4230775
#define DESLOCAMENTOXY 0.5
void timerJogCallback (void *p_arg)
{
	if ((JogkeyPressed & KEY_DOWN) == KEY_DOWN)
	{
		jogMaxDistance[AXIS_Y] = -(*velocidadeJog*MULTI_A + NUM_B);
		//jogMaxDistance[AXIS_Y] = -DESLOCAMENTOXY;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_UP) == KEY_UP)
	{
		jogMaxDistance[AXIS_Y] = (*velocidadeJog*MULTI_A + NUM_B);
		//jogMaxDistance[AXIS_Y] = DESLOCAMENTOXY;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_LEFT) == KEY_LEFT)
	{
		jogMaxDistance[AXIS_X] = -(*velocidadeJog*MULTI_A + NUM_B);
		//jogMaxDistance[AXIS_X] = -DESLOCAMENTOXY;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_RIGHT) == KEY_RIGHT)
	{
		jogMaxDistance[AXIS_X] = (*velocidadeJog*MULTI_A + NUM_B);
		//jogMaxDistance[AXIS_X] = DESLOCAMENTOXY;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_Z_DOWN) == KEY_Z_DOWN)
	{
		//jogMaxDistance[AXIS_Z] = -(*velocidadeJog*MULTI_A + NUM_B)*1;
		jogMaxDistance[AXIS_Z] = -0.3;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_Z_UP) == KEY_Z_UP)
	{
		//jogMaxDistance[AXIS_Z] = (*velocidadeJog*MULTI_A + NUM_B)*1;
		jogMaxDistance[AXIS_Z] = 0.3;
		macro_func_ptr = jog_Macro;
	}
//	R_CMT_Stop(timerIif);
}

void timerJogInitCallback (void *p_arg)
{
	if ((JogkeyPressed & KEY_DOWN) == KEY_DOWN)
	{
		//jogMaxDistance[AXIS_Y] = -(*velocidadeJog*MULTI_A + NUM_B);
		jogMaxDistance[AXIS_Y] = -1;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_UP) == KEY_UP)
	{
		//jogMaxDistance[AXIS_Y] = (*velocidadeJog*MULTI_A + NUM_B);
		jogMaxDistance[AXIS_Y] = 1;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_LEFT) == KEY_LEFT)
	{
		//jogMaxDistance[AXIS_X] = -(*velocidadeJog*MULTI_A + NUM_B);
		jogMaxDistance[AXIS_X] = -1;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_RIGHT) == KEY_RIGHT)
	{
		//jogMaxDistance[AXIS_X] = (*velocidadeJog*MULTI_A + NUM_B);
		jogMaxDistance[AXIS_X] = 1;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_Z_DOWN) == KEY_Z_DOWN)
	{
		//jogMaxDistance[AXIS_Z] = -(*velocidadeJog*MULTI_A + NUM_B)*0.1;
		jogMaxDistance[AXIS_Z] = -0.1;
		macro_func_ptr = jog_Macro;
	}
	if ((JogkeyPressed & KEY_Z_UP) == KEY_Z_UP)
	{
		//jogMaxDistance[AXIS_Z] = (*velocidadeJog*MULTI_A + NUM_B)*0.1;
		jogMaxDistance[AXIS_Z] = 0.1;
		macro_func_ptr = jog_Macro;
	}
	R_CMT_Stop(timerIif);
	R_CMT_CreatePeriodic(5,timerJogWalkCallback,&timerIif);
}

void timerJogWalkCallback (void *p_arg)
{
	if (JogkeyPressed != 0)
	{
		R_CMT_Stop(timerIif);
		R_CMT_CreatePeriodic(*velocidadeJog/10,timerJogCallback,&timerIif);
	}
	else
	{
		R_CMT_Stop(timerIif);
	}
}

