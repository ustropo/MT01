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
#include "macros.h"


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

const char jog_stopflush[]= "\
!\n\
%";

void iif_enter_jog(void)
{
	static bool torchEnable = false;
	if(!torchEnable)
	{
		TORCH = TRUE;
		torchEnable = true;
	}
	else
	{
		TORCH = FALSE;
		torchEnable = false;
	}
}

void iif_esc_jog(void)
{
	TORCH = FALSE;
	iif_bind_idle();
	macro_func_ptr = _command_dispatch;
}

void iif_down_jog(void) {
//	sprintf(text, "G21 G91\nG01 Y-390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_Y;
	jogMaxDistance = -1000;
	macro_func_ptr = jog_Macro;
}
void iif_up_jog(void) {
//	sprintf(text, "G21 G91\nG01 Y390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_Y;
	jogMaxDistance = 1000;
	macro_func_ptr = jog_Macro;
}
void iif_left_jog(void){
//	sprintf(text, "G21 G91\nG01 X-390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_X;
	jogMaxDistance = -1000;
	macro_func_ptr = jog_Macro;
}

void iif_right_jog(void) {
//	sprintf(text, "G21 G91\nG01 X390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_X;
	jogMaxDistance = 1000;
	macro_func_ptr = jog_Macro;
}

void iif_zdown_jog(void){
//	sprintf(text, "G21 G91\nG01 Z-390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_Z;
	jogMaxDistance = -1000;
	macro_func_ptr = jog_Macro;
}

void iif_zup_jog(void) {
//	sprintf(text, "G21 G91\nG01 Z390.0 F%.0f\nm30",*velocidadeJog);
//	xio_open(cs.primary_src,text,0);
	jogAxis = AXIS_Z;
	jogMaxDistance = 1000;
	macro_func_ptr = jog_Macro;
}

void iif_released_jog(void) {
	cm_request_feedhold();
	cm_request_queue_flush();
	//xio_open(cs.primary_src,jog_stopflush,0);
}

void iif_bind_jog(void)
{
	iif_func_enter = &iif_enter_jog;
	iif_func_esc = &iif_esc_jog;
	iif_func_down = &iif_down_jog;
	iif_func_up = &iif_up_jog;
	iif_func_left = &iif_left_jog;
	iif_func_right = &iif_right_jog;
	iif_func_zdown = &iif_zdown_jog;
	iif_func_zup = &iif_zup_jog;
	iif_func_released = &iif_released_jog;
}
