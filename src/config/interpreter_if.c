/*
 * interpreter_if.c
 *
 *  Created on: 21/12/2015
 *      Author: leocafonso
 */
#include "platform.h"
#include "keyboard.h"
#include "interpreter_if.h"

const char zero_axis[]= "\
G28.3 X0 Y0 Z0\n\
m30";

const char jog_startxp[]= "\
G21 G91\n\
G01 X390.0 F7000\n\
m30";

const char jog_startxn[]= "\
G21 G91\n\
G01 X-390.0 F1000\n\
m30";

const char jog_startyp[]= "\
G21 G91\n\
G01 Y390.0 F4000\n\
m30";

const char jog_startyn[]= "\
G21 G91\n\
G01 Y-390.0 F4000\n\
m30";

const char jog_stopflush[]= "\
!\n\
%";

const char jog_stop[]= "\
!";

const char jog_restart[]= "\
~";

iif_func_ptr iif_func_enter = &iif_idle;
iif_func_ptr iif_func_esc = &iif_idle;
iif_func_ptr iif_func_down = &iif_idle;
iif_func_ptr iif_func_up = &iif_idle;
iif_func_ptr iif_func_left = &iif_left_filerunning;
iif_func_ptr iif_func_right = &iif_idle;
iif_func_ptr iif_func_released = &iif_idle;

void iif_enter_filerunning(void)
{
	cm_request_cycle_start();
}

void iif_esc_filerunning(void)
{
	cm_request_feedhold();
}

void iif_down_filerunning(void) {}
void iif_up_filerunning(void) {}
void iif_left_filerunning(void)
{
	tg_set_primary_source(XIO_DEV_COMMAND);
	xio_open(XIO_DEV_COMMAND,zero_axis,0);
}
void iif_right_filerunning(void) {}
void iif_released_filerunning(void) {}
void iif_idle(void) {}


