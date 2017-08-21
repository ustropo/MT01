/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_fl_downloader.c
* Version      : 3.00
* Description  : Contains the FlashLoader state machine.  This file should
*                not be changed unless you are changing part of the protocol.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 02.03.2012 3.00    Made compliant with CS v4.0. State machine is
*                              now triggered by user supplied timer. Uses
*                              r_crc_rx package.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Functions prototypes for dealing with strings (used for memcpy) */
#include <string.h>
/* Included for definitions for 'true' and 'false' */
#include <stdbool.h>
/* Flash Loader project includes. */
#include "r_fl_includes.h"
/* Uses r_crc_rx package for CRC calculations. */
#include "r_crc_rx_if.h"
/* Used for re-entrancy protection with state machine. */
#include "platform.h"
#include "keyboard.h"
#include "plasma.h"

#include "ff.h"

#include "lcd.h"
#include "lcd_menu.h"

#include "spiffs_hw.h"
#include "spiflash.h"

#include <string.h>
#include <stdio.h>

const char proj_name[] = "MT01_proj.bin";
const char proj_programmed[] = "MT01_done.bin";

#define CRC_ADDRESS (((uint32_t)__sectop("APPHEADER_1"))-0xFFF00000)
extern bool drivemountFlag;
FATFS media;
FIL file;
const fl_image_header_t *g_app_header;

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
/* Declares transmit and receive buffers.  The receive buffer
   is for commuincations to the host as well as when reading
   the external memory.  This is done to save space. */
uint8_t g_fl_tx_buffer[5]; 
uint8_t g_fl_rx_buffer[FL_CFG_DATA_BLOCK_MAX_BYTES];

/* Data structure to hold load image headers */
fl_image_header_t   g_fl_load_image_headers[FL_CFG_MEM_NUM_LOAD_IMAGES];
static const char* gszbootMsg[MAX_ROW] =
{
		/* "12345678901234567890" */
		   "    DETECTADO NOVO  ",
		   "       FIRMWARE     ",
		   "                    ",
		   "                    ",
		   " ENTER: CONTINUA    ",
		   " ESC  : PULA        ",
};

static const char* gszCarMsg[17] =
{
		/* "12345678901234567890" */
		   "CARREGANDO\n[                ]\n",
		   "CARREGANDO\n[*               ]\n",
		   "CARREGANDO\n[**              ]\n",
		   "CARREGANDO\n[***             ]\n",
		   "CARREGANDO\n[****            ]\n",
		   "CARREGANDO\n[*****           ]\n",
		   "CARREGANDO\n[******          ]\n",
		   "CARREGANDO\n[*******         ]\n",
		   "CARREGANDO\n[********        ]\n",
		   "CARREGANDO\n[*********       ]\n",
		   "CARREGANDO\n[**********      ]\n",
		   "CARREGANDO\n[***********     ]\n",
		   "CARREGANDO\n[************    ]\n",
		   "CARREGANDO\n[*************   ]\n",
		   "CARREGANDO\n[**************  ]\n",
		   "CARREGANDO\n[*************** ]\n",
		   "CARREGANDO\n[****************]\n",
};


char StrBoot[MAX_COLUMN];

/******************************************************************************
Private global variables and functions
******************************************************************************/

/* Points to info on current application */
fl_image_header_t * g_pfl_cur_app_header;
uint32_t	address = 0;

/******************************************************************************
* Function Name: R_FL_DownloaderInit
* Description  : Initializes communications for FlashLoader operations
* Arguments    : none
* Return value : none
******************************************************************************/
void R_FL_DownloaderInit(void)
{
    /* Initialize pointer to current app's load image header */
    g_pfl_cur_app_header = (fl_image_header_t *)__sectop("APPHEADER_1");
    
    /* Initialize resources needed for using external memory */
  //  fl_mem_init();

    /* Initialize CRC. */
    R_CRC_Init();
    
}
/******************************************************************************
End of function FL_Downloader_Init
******************************************************************************/
uint8_t     chargeIndex = 0;

/******************************************************************************
* Function Name: R_FL_StateMachine
* Description  : This is the state machine that runs the FL project. This 
*                function should be called on a periodic basis by the user.
* Arguments    : none
* Return value : none
******************************************************************************/
void R_FL_StateMachine(void)
{
    FIL         file;
    FRESULT     res;
    uint16_t    file_rw_cnt;
	uint8_t     uiMsgRow = 0;

	uint32_t    keyEntry = 0;


    R_FL_DownloaderInit();

	/* Open a text file */
	if(drivemountFlag){
		res = f_open(&file, proj_name, FA_READ | FA_WRITE);
		if(FR_OK != res)
		{
			nop();
		}
		else
		{
			res = f_lseek(&file,CRC_ADDRESS);
			f_read(&file, g_fl_load_image_headers, sizeof(g_fl_load_image_headers), (UINT *)&file_rw_cnt);
			sprintf(StrBoot, "   Versão:%d.%d.%d.%03d", g_fl_load_image_headers[0].version_major,
													 g_fl_load_image_headers[0].version_middle,
					                                 g_fl_load_image_headers[0].version_minor,
					                                 g_fl_load_image_headers[0].version_comp);
			gszbootMsg[2] = StrBoot;
			/* Write strings */
			for(uiMsgRow = 0; uiMsgRow < MAX_ROW; uiMsgRow++)
			{
				ut_lcd_drawStr(uiMsgRow, 0, gszbootMsg[uiMsgRow], false,ITEM_NO_MARKED,u8g_font_6x10);
			}
			/* Output */
			ut_lcd_output_str();

			while(keyEntry != KEY_ENTER && keyEntry != KEY_ESC){
				WDT_FEED
				xQueueReceive( qKeyboard, &keyEntry, portMAX_DELAY );
			}
			if(keyEntry == KEY_ESC){
				return;
			}

            res = f_lseek(&file,0);
			SPIFLASH_erase(&spif, address, SPIFFS_CFG_PHYS_ERASE_SZ(0));
		    chargeIndex = 0;
			ut_lcd_output_warning(gszCarMsg[chargeIndex]);
		   while(!f_eof(&file)){
				f_read(&file, g_fl_rx_buffer, sizeof(g_fl_rx_buffer), (UINT *)&file_rw_cnt);
				if(memcmp(g_fl_rx_buffer, 0xFF, sizeof(g_fl_rx_buffer)) != 0)
					SPIFLASH_write(&spif,address, sizeof(g_fl_rx_buffer),g_fl_rx_buffer);
				address += sizeof(g_fl_rx_buffer);
				if((address % 0x10000) == 0){
					chargeIndex++;
					ut_lcd_output_warning(gszCarMsg[chargeIndex]);
					SPIFLASH_erase(&spif, address, SPIFFS_CFG_PHYS_ERASE_SZ(0));
				}
				WDT_FEED
		   }
			res = f_close(&file);
			res = f_open(&file, proj_name, FA_WRITE);
		    f_rename(proj_name, proj_programmed);
		    RESET
		}
	}
    
}
/******************************************************************************
End of function R_FL_StateMachine
******************************************************************************/

