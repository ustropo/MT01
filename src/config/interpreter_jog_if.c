/*
 * interpreter_jog_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "platform.h"
#include "interpreter_if.h"

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

extern uint32_t *velocidadeJog;
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
	iif_bind_idle();
}

void iif_down_jog(void) {
	sprintf(text, "G21 G91\nG01 Y-390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}
void iif_up_jog(void) {
	sprintf(text, "G21 G91\nG01 Y390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}
void iif_left_jog(void){
	sprintf(text, "G21 G91\nG01 X-390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}

void iif_right_jog(void) {
	sprintf(text, "G21 G91\nG01 X390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}

void iif_zdown_jog(void){
	sprintf(text, "G21 G91\nG01 Z-390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}

void iif_zup_jog(void) {
	sprintf(text, "G21 G91\nG01 Z390.0 F%d\nm30",*velocidadeJog);
	xio_open(cs.primary_src,text,0);
}

void iif_released_jog(void) { xio_open(cs.primary_src,jog_stopflush,0);}

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
