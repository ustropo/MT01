/*
 * interpreter_jog_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "platform.h"
#include "interpreter_if.h"

static void iif_enter_jog(void);
static void iif_esc_jog(void);
static void iif_down_jog(void);
static void iif_up_jog(void);
static void iif_left_jog(void);
static void iif_right_jog(void);
static void iif_released_jog(void);

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

void iif_enter_jog(void)
{

}

void iif_esc_jog(void)
{
	iif_bind_idle();
}

void iif_down_jog(void) { xio_open(cs.primary_src,jog_startxp,0);}
void iif_up_jog(void) { xio_open(cs.primary_src,jog_startxn,0);}
void iif_left_jog(void){ xio_open(cs.primary_src,jog_startyp,0);}
void iif_right_jog(void) { xio_open(cs.primary_src,jog_startyn,0);}
void iif_released_jog(void) { xio_open(cs.primary_src,jog_stopflush,0);}

void iif_bind_jog(void)
{
	iif_func_enter = &iif_enter_jog;
	iif_func_esc = &iif_esc_jog;
	iif_func_down = &iif_down_jog;
	iif_func_up = &iif_up_jog;
	iif_func_left = &iif_left_jog;
	iif_func_right = &iif_right_jog;
	iif_func_released = &iif_released_jog;
}
