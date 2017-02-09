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
uint16_t xPress = 0;
void timerJogScan (void *p_arg);



void iif_enter_jog(void)
{
	static bool torchEnable = false;
	if(!torchEnable)
	{
	//	cm_spindle_control(SPINDLE_CW);
	//	isCuttingSet(true);
	//	pl_arcook_start();
		TORCH = TRUE;
		torchEnable = true;
	}
	else
	{
	//	cm_spindle_control(SPINDLE_OFF);
	//	isCuttingSet(false);
	//	pl_arcook_stop();
		TORCH = FALSE;
		torchEnable = false;
	}
}

void iif_esc_jog(void)
{
	pl_arcook_stop();
	delay_thcStartStop(false);
	if (timerIif == 3)
	{
		R_CMT_Stop(timerIif);
	}
	TORCH = FALSE;
	cm_request_feedhold();
	cm_request_queue_flush();
	while(cm.feedhold_requested == true)
	{

	}
	iif_bind_idle();
	JogkeyPressed = 0;
	macro_func_ptr = command_idle;
}

void iif_down_jog(void) {
	jogMaxDistance[AXIS_Y] = -100000;
	macro_func_ptr = jog_Macro;
//	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}
void iif_up_jog(void) {
	jogMaxDistance[AXIS_Y] = 100000;
	macro_func_ptr = jog_Macro;
//	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}
void iif_left_jog(void){
	jogMaxDistance[AXIS_X] = -100000;
	macro_func_ptr = jog_Macro;
//	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_right_jog(void) {
	jogMaxDistance[AXIS_X] = 100000;
	macro_func_ptr = jog_Macro;
//	R_CMT_CreatePeriodic(20,timerJogInitCallback,&timerIif);
}

void iif_zdown_jog(void){
	if (JogkeyPressed == KEY_Z_DOWN)
	{
		jogMaxDistance[AXIS_Z] = -100000;
		macro_func_ptr = jog_Macro;
	}
	else
	{
		zmove = -0.01;
	}
}

void iif_zup_jog(void) {
	if (JogkeyPressed == KEY_Z_UP)
	{
		jogMaxDistance[AXIS_Z] = 100000;
		macro_func_ptr = jog_Macro;
	}
	else
	{
		zmove = 0.01;
	}
}

void iif_released_jog(void) {

	cm_request_feedhold();
	cm_request_queue_flush();
	//macro_func_ptr = _command_dispatch;
	xPress = 0;
//	if (timerIif == 3)
//	{
//		R_CMT_Stop(timerIif);
//	}
}

void iif_bind_jog(void)
{
	JogkeyPressed = 0;
	xPress = 0;
	state = 0;
	R_CMT_CreatePeriodic(20,timerJogScan,&timerIif);
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

void timerJogScan (void *p_arg)
{
	if(configFlags[MODOMAQUINA] == MODO_PLASMA){
		pl_thc_read();
	}
}

