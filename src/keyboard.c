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
void keyboard_task(void)
{
	uint8_t key_buf[4][3];
	uint8_t keyPressed = 0;
	uint32_t key = 0;
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t col[4] = {KC1,KC2,KC3,KC4};

	while(1)
	{
		vTaskDelay(30 / portTICK_RATE_MS);
		for (i = 0; i < 4; i++)
		{
			KCOL = col[i];
			vTaskDelay(1 / portTICK_RATE_MS);
			key_buf[i][k] = KLINE;
			if (key_buf[i][k] != 0xFF)
			{
				if (key_buf[i][0] == key_buf[i][1])
				{
					if (key_buf[i][0] == key_buf[i][2])
					{
						key_buf[i][0] = ~key_buf[i][0];
						key |=(uint32_t)key_buf[i][0]<<8*i;
						keyPressed = 1;
					}
				}
			}
		}
		k++;
		if (k > 2)
		{
			if (keyPressed == 1)
			{
				xQueueSend( qKeyboard, &key, 0 );
				key = 0;
				keyPressed = 0;
			}
			k = 0;
		}
	}
}
