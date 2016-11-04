/*
 * keyboard.c
 *
 *  Created on: 07/08/2015
 *      Author: LAfonso01
 */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "platform.h"
#include "keyboard.h"
#include "lcd_menu.h"
//#include "hardware.h"
#include "tinyg.h"
#include "config.h"
#include "switch.h"

#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#endif
/*****************************************************************************
Macro definitions
 ******************************************************************************/


/******************************************************************************
Section    <Section Definition> , "Data Sections"
 ******************************************************************************/

/******************************************************************************
Private global variables and functions
 ******************************************************************************/


/******************************************************************************
External variables and functions
 ******************************************************************************/


/*****************************************************************************
Enumerated Types
 ******************************************************************************/

/******************************************************************************
Section    <Section Definition> , "Project Sections"
 ******************************************************************************/

/******************************************************************************
Function Name   : keyboard_task
Description     : keyboard scan Task process
Arguments       : none
Return value    : none
 ******************************************************************************/
#ifndef MODULO
void keyboard_task(void)
{
	uint8_t key_buf[4][3];
	bool keyPressed = false;
	uint8_t colPressed = 0xff;
	uint32_t key = 0;
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t col[4] = {KC1,KC2,KC3,KC4};

	while(1)
	{
		vTaskDelay(30 / portTICK_RATE_MS);
		switch_rtc_callback();					// switch debouncing
		for (i = 0; i < 4; i++)
		{
			KCOL = col[i];
			vTaskDelay(1 / portTICK_RATE_MS);
			key_buf[i][k] = KLINE;
			if (i == colPressed)
			{
				if (key_buf[colPressed][k] == 0xFF)
				{
					key = 0;
					xQueueSend( qKeyboard, &key, 0 );
					keyPressed = false;
					colPressed = 0xFF;
				}
			}
			if (key_buf[i][k] != 0xFF)
			{
				if (key_buf[i][0] == key_buf[i][1])
				{
					if (key_buf[i][0] == key_buf[i][2])
					{
						key_buf[i][0] = ~key_buf[i][0];
						key |=(uint32_t)key_buf[i][0]<<8*i;
						colPressed = i;
						if (keyPressed == false)
						{
							xQueueSend( qKeyboard, &key, 0 );
						}
						keyPressed = true;
					}
				}
			}
		}
		k++;
		if (k > 2)
		{
			k = 0;
		}
	}
}
#else
#define KEY_DEBOUNCE 10
#define KEY_DEBOUNCE_DLYMS 2

void keyboard_task(void)
{
	uint8_t key_buf[3][20];
	uint8_t colPressed = 0xff;
	uint32_t key = 0;
	uint32_t key_old = 0;
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t j = 0;
	uint8_t matchCount = 0;
	uint8_t col[3] = {KC0,KC1,KC2};

	while(1)
	{
		vTaskDelay(KEY_DEBOUNCE_DLYMS / portTICK_RATE_MS);
		WDT_FEED
		switch_rtc_callback();					// switch debouncing
		for (i = 0; i < 3; i++)
		{
			KCOL = col[i];
			vTaskDelay(1 / portTICK_RATE_MS);
			key_buf[i][k] = (~(KLINE | 0x81)>>4) & 0x0F;
			if (i == colPressed)
			{
				if (key_buf[colPressed][k] == 0x00)
				{
					key = 0;
					key_old = 0;
					xQueueSend( qKeyboard, &key, 0 );
					colPressed = 0xFF;
				}
			}
			if (key_buf[i][k] != 0x00)
			{
				for (j = 0;j < KEY_DEBOUNCE;j++)
				{
					if (key_buf[i][0] == key_buf[i][j])
					{
						matchCount++;
					}
				}
				if (matchCount == KEY_DEBOUNCE)
				{
					key = key_buf[2][1] << 8 | key_buf[1][1] << 4 | key_buf[0][1];
					colPressed = i;
					if (key != key_old)
					{
						xQueueSend( qKeyboard, &key, 0 );
					}
					key_old = key;
				}
				matchCount = 0;
			}
		}
		k++;
		if (k == KEY_DEBOUNCE)
		{
			k = 0;
		}
	}
}
#endif
