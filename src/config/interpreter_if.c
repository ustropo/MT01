/*
 * interpreter_if.c
 *
 *  Created on: 21/12/2015
 *      Author: leocafonso
 */
#include "platform.h"
#include "interpreter_if.h"

const char zero_axis[]= "\
G28.3 X0 Y0 Z0\n\
m30";

const char jog_stop[]= "\
!";

const char jog_restart[]= "\
~";

iif_func_ptr iif_func_enter;
iif_func_ptr iif_func_esc;
iif_func_ptr iif_func_down;
iif_func_ptr iif_func_up;
iif_func_ptr iif_func_left;
iif_func_ptr iif_func_right;
iif_func_ptr iif_func_zdown;
iif_func_ptr iif_func_zup;
iif_func_ptr iif_func_released;
iif_func_ptr iif_func_cycleStop;
uint32_t timerIif;

void iif_idle(void) {}

void iif_bind_idle(void)
{
	iif_func_enter = &iif_idle;
	iif_func_esc = &iif_idle;
	iif_func_down = &iif_idle;
	iif_func_up = &iif_idle;
	iif_func_left = &iif_idle;
	iif_func_right = &iif_idle;
	iif_func_zdown = &iif_idle;
	iif_func_zup = &iif_idle;
	iif_func_released = &iif_idle;
	iif_func_cycleStop = &iif_idle;
}


