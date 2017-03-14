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
* File Name    : r_fl_app_header.c
* Version      : 3.00
* Description  : Defines Application Header to be used with FlashLoader. The
*                2nd and 3rd entries of the g_fl_cur_app_header structure will
*                change with each application.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 02.03.2012 3.00    Compliant with CS v4.0. Added description for
*                              setting address of g_fl_cur_app_header.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Flash Loader project includes. */
#include "r_fl_includes.h"

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
/* This directive sets the address of g_fl_cur_app_header. The address at the
   end of this directive (0xFFFFFE00 by default) should be the same value as
   the definition of FL_CUR_APP_HEADER_ADDR in r_fl_app_header.h. If you change
   one you must change the other too. */
#pragma section C APPHEADER

const fl_image_header_t g_fl_cur_app_header = {
    /* To confirm valid header */
	FL_LI_VALID_MASK,
    /* Version major  */
    1,
    /* Version middle */
    5,
    /* Version minor */
    0,
    /* Version compilation */
    0 ,
    /* CRC-16 CCITT of image as in MCU flash */
    0xFFFF
};

