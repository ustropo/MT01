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
* File Name	   : r_fl_includes.h
* Version      : 3.0 
* Description  : Has required includes for Flash Loader project. 
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 23.02.2012 3.00     First Release (introduced in v3.0)
******************************************************************************/

#ifndef FL_INCLUDES
#define FL_INCLUDES

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Flash Loader configuration options. */
#include "r_flash_loader_rx_config.h"
/* Flash Loader types. */
#include "r_fl_types.h"
/* Flahs Loader shared variables. */
#include "r_fl_globals.h"
/* Function prototypes and defines for FlashLoader use */
#include "r_fl_downloader.h"
/* Function prototypes for FL communications */
#include "r_fl_comm.h"
/* Function prototypes for interfacing to memory holding load images */
#include "r_fl_memory.h"
/* Function prototypes for utility fuctions (CRC & Reset) */
#include "r_fl_utilities.h"
/* Flash Loader interface file. */
#include "r_flash_loader_rx_if.h"

#endif /* FL_INCLUDES */
