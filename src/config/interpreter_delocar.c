/*
 * interpreter_jog_if.c
 *
 *  Created on: 22/12/2015
 *      Author: leocafonso
 */
#include "tinyg.h"
#include "platform.h"
#include "planner.h"
#include "interpreter_if.h"
#include "controller.h"
#include "macros.h"
#include "keyboard.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void iif_cycleStop_deslocar(void);

void iif_bind_deslocar(void)
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
	iif_func_cycleStop = &iif_cycleStop_deslocar;
}

void iif_cycleStop_deslocar(void)
{
	static uint8_t count = 0;
	if(mp_get_runtime_absolute_position(0) == 0)
	{
		uint32_t qSend = KEY_ESC;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		xQueueSend( qKeyboard, &qSend, 0 );
	}
//	if(count == 1){
//		uint32_t qSend = KEY_ESC;
//		vTaskDelay(10000 / portTICK_PERIOD_MS);
//		xQueueSend( qKeyboard, &qSend, 0 );
//		count = 0;
//	}
//	else
//	{
//		count++;
//	}
}
