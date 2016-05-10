/*
 * ut_state_splash.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */

#include <stdio.h>
#include <string.h>
#include "ut_context.h"
#include "ut_state.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lcd.h"
#include "keyboard.h"
#include "plasma.h"

static char Str[50];

#define metalique128_width 128
#define metalique128_height 42
static unsigned char metalique128_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc0, 0x7c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x60, 0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x7c, 0x7c, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x30, 0x3c, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x10, 0x3e, 0x7f, 0x30, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x3e, 0x7f, 0x78,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x18, 0xbe, 0x7f, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x08, 0xdf, 0x3f, 0x7e, 0x00, 0xe0, 0x00, 0x00,
   0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x5f, 0x3e, 0x7f,
   0x00, 0xe0, 0x00, 0x00, 0x80, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x0c, 0x7f, 0xbe, 0x7f, 0x00, 0xe0, 0x00, 0x00, 0x80, 0x7b, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x8c, 0x3f, 0xfe, 0x7f, 0x00, 0xe0, 0x00, 0x00,
   0x80, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0x3f, 0xff, 0x3e,
   0x00, 0xe0, 0x00, 0x00, 0x80, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x8c, 0x1f, 0x7f, 0x3e, 0x00, 0xe0, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xc4, 0x0f, 0x3f, 0x3f, 0xf8, 0xe0, 0x00, 0xf0,
   0x83, 0x73, 0xe0, 0x03, 0x07, 0x70, 0xe0, 0x07, 0xc4, 0x8f, 0x1f, 0x1f,
   0xfe, 0xe3, 0x00, 0xfc, 0x87, 0x73, 0xf8, 0x0f, 0x07, 0x70, 0xf8, 0x1f,
   0xe4, 0x87, 0x1f, 0x9f, 0xff, 0xe7, 0x00, 0xfe, 0x9f, 0x73, 0xfe, 0x1f,
   0x07, 0x70, 0xfc, 0x3f, 0xec, 0xc7, 0x8f, 0x8f, 0x87, 0xe7, 0x3f, 0x0f,
   0x9e, 0x73, 0x1e, 0x3c, 0x07, 0x70, 0x1e, 0x3c, 0xec, 0xc3, 0x8f, 0xcf,
   0xc3, 0xe3, 0x3f, 0x07, 0xbc, 0x73, 0x0f, 0x38, 0x07, 0x70, 0x0e, 0x1e,
   0xf8, 0xc3, 0x87, 0xcf, 0xe1, 0xe1, 0xbf, 0x03, 0xb8, 0x73, 0x07, 0x78,
   0x07, 0x70, 0x0f, 0x0f, 0xf8, 0xe1, 0x87, 0xc7, 0xf1, 0xe0, 0x80, 0x03,
   0xb8, 0x73, 0x07, 0x70, 0x07, 0x70, 0x87, 0x07, 0xf0, 0xe1, 0xc3, 0xc7,
   0x78, 0xe0, 0x80, 0x03, 0xb8, 0x73, 0x07, 0x70, 0x07, 0x70, 0xc7, 0x03,
   0xf8, 0xc1, 0xc3, 0xc7, 0x3d, 0xe0, 0x80, 0x03, 0xb8, 0x73, 0x07, 0x70,
   0x07, 0x70, 0xe7, 0x01, 0xf8, 0xf1, 0xc3, 0xc3, 0x1f, 0xe0, 0x80, 0x07,
   0xb8, 0x73, 0x07, 0x70, 0x0f, 0x70, 0xff, 0x00, 0xf8, 0xf1, 0xe1, 0xc3,
   0x0f, 0xe0, 0x01, 0x07, 0xb8, 0x73, 0x0f, 0x70, 0x0e, 0x78, 0x7e, 0x00,
   0xfc, 0xf0, 0xe1, 0x9f, 0x07, 0xc7, 0x03, 0x0f, 0xb8, 0x73, 0x1e, 0x70,
   0x1e, 0x3c, 0x3e, 0x3c, 0x7c, 0xf8, 0xe0, 0x8f, 0xff, 0x87, 0x3f, 0xfe,
   0xbf, 0x73, 0xfc, 0x7f, 0xfc, 0x1f, 0xfc, 0x3f, 0x7c, 0xf8, 0xe0, 0x07,
   0xff, 0x03, 0x3f, 0xfc, 0xbf, 0x73, 0xf8, 0x7f, 0xf8, 0x0f, 0xf8, 0x1f,
   0x3e, 0x78, 0xe0, 0x03, 0xfc, 0x01, 0x3e, 0xf0, 0xbf, 0x73, 0xf0, 0x7f,
   0xe0, 0x07, 0xe0, 0x07, 0x1c, 0x38, 0xc0, 0x01, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define easymax_width 128
#define easymax_height 25
static unsigned char easymax_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x07, 0xfe, 0x81, 0xff, 0xf9, 0xf3,
   0xe7, 0x1f, 0xf8, 0x0f, 0xfc, 0x83, 0xbf, 0x7f, 0xe0, 0xff, 0x07, 0xff,
   0xc1, 0xff, 0xfb, 0xf3, 0xe7, 0x1f, 0xf8, 0x0f, 0xfc, 0x83, 0xbf, 0x3f,
   0xe0, 0xff, 0x07, 0xff, 0xe1, 0xff, 0xfb, 0xf3, 0xe7, 0x1f, 0xfc, 0x07,
   0xfe, 0x83, 0xbf, 0x3f, 0xe0, 0xff, 0x07, 0xff, 0xf1, 0xff, 0xfb, 0xf3,
   0xe7, 0x3f, 0xfc, 0x07, 0xfe, 0x83, 0xbf, 0x3f, 0xe0, 0xff, 0x83, 0xff,
   0xf1, 0xf3, 0xfb, 0xf3, 0xe3, 0x3f, 0xfe, 0x07, 0xfe, 0x83, 0x9f, 0x1f,
   0xe0, 0xe7, 0x83, 0xff, 0xf1, 0xf1, 0xf1, 0xfb, 0xe1, 0x3f, 0xff, 0x03,
   0xff, 0x83, 0x9f, 0x1f, 0xe0, 0xe7, 0xc3, 0xff, 0xf9, 0xf1, 0xf1, 0xfb,
   0xe0, 0x3f, 0xff, 0x03, 0xff, 0x83, 0xcf, 0x0f, 0xf0, 0xe7, 0xc3, 0xf7,
   0xf9, 0x03, 0xe0, 0xff, 0xe0, 0x7f, 0xff, 0x03, 0xef, 0xc3, 0xef, 0x07,
   0xf0, 0x03, 0xc0, 0xf3, 0xf9, 0x0f, 0xe0, 0x7f, 0xe0, 0xff, 0xff, 0x83,
   0xef, 0xc3, 0xff, 0x03, 0xf0, 0x3f, 0xc0, 0xf3, 0xf9, 0x1f, 0xc0, 0x3f,
   0xf0, 0xff, 0xff, 0x83, 0xef, 0xc3, 0xff, 0x03, 0xf0, 0x3f, 0xe0, 0xf3,
   0xf1, 0x7f, 0xc0, 0x3f, 0xf0, 0xff, 0xf7, 0x81, 0xe7, 0xc3, 0xff, 0x01,
   0xf0, 0x3f, 0xe0, 0xf9, 0xf1, 0xff, 0x80, 0x1f, 0xf0, 0xfb, 0xf3, 0xc1,
   0xe7, 0xc3, 0xff, 0x00, 0xf0, 0x3f, 0xe0, 0xf9, 0xe1, 0xff, 0x80, 0x0f,
   0xf0, 0xf3, 0xf1, 0xc1, 0xe7, 0xe3, 0xff, 0x00, 0xf8, 0x0f, 0xf0, 0xff,
   0x81, 0xff, 0x80, 0x0f, 0xf0, 0xf3, 0xf9, 0xc1, 0xff, 0xe3, 0xff, 0x01,
   0xf8, 0x01, 0xf0, 0xff, 0x01, 0xfe, 0x80, 0x0f, 0xf0, 0xf1, 0xf8, 0xe1,
   0xff, 0xe3, 0xff, 0x01, 0xf8, 0xf1, 0xf0, 0xff, 0xf9, 0xfc, 0x80, 0x0f,
   0xf8, 0xe1, 0xf8, 0xe0, 0xff, 0xe3, 0xf7, 0x03, 0xf8, 0xf1, 0xf8, 0xff,
   0xfd, 0xfc, 0x80, 0x0f, 0xf8, 0x61, 0xf8, 0xe0, 0xff, 0xe3, 0xf7, 0x03,
   0xf8, 0xf9, 0xf8, 0xf8, 0xfd, 0xfc, 0xc0, 0x0f, 0xf8, 0x21, 0xf8, 0xf0,
   0xf3, 0xe3, 0xe7, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x07,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xfe, 0xfc, 0xfd, 0x7f, 0xe0, 0x0f,
   0xfc, 0x01, 0xfe, 0xf8, 0xf3, 0xf3, 0xf7, 0x0f, 0xfe, 0xff, 0xfe, 0xfc,
   0xfd, 0x3f, 0xe0, 0x0f, 0xfc, 0x01, 0xfe, 0xf8, 0xf3, 0xf3, 0xf7, 0x0f,
   0xfc, 0x7f, 0xfe, 0xfc, 0xf0, 0x07, 0xe0, 0x07, 0xfc, 0x00, 0x7e, 0xf8,
   0xf9, 0xf1, 0xf3, 0x07 };


/**
 * Execute a splash screen and
 * do some other initialization.
 *
 * @param pContext Context pointer
 * @return Next state.
 */
ut_state ut_state_splash(ut_context* pContext)
{
	uint32_t keyEntry = 0;
    IWDT.IWDTRR = 0x00u;
    IWDT.IWDTRR = 0xFFu;
//	if(SYSTEM.RSTSR2.BIT.IWDTRF){
//		pl_emergencia_init();
//    	if (currentLine == 0)
//    		strcpy(Str,"MODO DE EMERG�NCIA\n");
//    	else
//    		sprintf(Str,"MODO DE EMERG�NCIA\nPARADO LINHA\n%d\n",currentLine);
//    	ut_lcd_output_warning(Str);
//		while(keyEntry != KEY_ENTER){
//			IWDT.IWDTRR = 0x00u;
//			IWDT.IWDTRR = 0xFFu;
//			xQueueReceive( qKeyboard, &keyEntry, portMAX_DELAY );
//		}
//		currentLine = 0;
//		return STATE_MAIN_MENU;
//	}
	currentLine = 0;
	ut_lcd_clear();

	/* Delay para a inicialização do Display */
	vTaskDelay(10 / portTICK_PERIOD_MS);

	ut_lcd_bitmap(0,11,metalique128_width,metalique128_height,metalique128_bits,"\n");

	/* Delay */
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	ut_lcd_bitmap(0,11,easymax_width,easymax_height,easymax_bits,"Vers�o 1.0.1");

	/* Delay */
	vTaskDelay(2000 / portTICK_PERIOD_MS);

	/* Next state */
	return STATE_WARNING;
}

