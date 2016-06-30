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
* File Name	   : r_fl_globals.h
* Version      : 3.0 
* Description  : Global variables shared between Flash Loader files.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 22.02.2012 3.00     First Release (introduced in v3.0)
******************************************************************************/

#ifndef FL_GLOBALS
#define FL_GLOBALS

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>
/* Flash Loader types. */
#include "r_fl_types.h"

/******************************************************************************
Shared Global Variables
******************************************************************************/
/* Data structure to hold load image headers */
extern fl_image_header_t   g_fl_load_image_headers[];
/* Declares transmit and receive buffers.  The receive buffer
   is for commuincations to the host as well as when reading
   the external memory.  This is done to save space. */
extern uint8_t g_fl_tx_buffer[];
extern uint8_t g_fl_rx_buffer[FL_CFG_DATA_BLOCK_MAX_BYTES];
/* Defines the memory where load images will be stored. */
extern const fl_li_storage_t g_fl_li_mem_info;
extern const fl_image_header_t *g_app_header;


#endif /* FL_GLOBALS */
