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
* File Name    : r_fl_downloader.h
* Version      : 3.00
* Description  : Contains the FlashLoader state machine.  This file should
*                not be changed unless you are changing part of the protocol.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 02.03.2012 3.00    Made compliant with CS v4.0. Move structure 
*                              definitions to r_fl_types.h and moved macro
*                              configuration options to 
*                              r_flash_loader_rx_config.h.
******************************************************************************/

#ifndef FL_DOWNLOADER_H
#define FL_DOWNLOADER_H

/******************************************************************************
Macro definitions
******************************************************************************/
/* Valid Mask for Load Image Header */
#define FL_LI_VALID_MASK                (0xAA)

/* Value of 'successfully_stored' value before image has been 
   successfully downloaded.  If this is the value, then the
   image did not download correctly. */
#define FL_LI_NOT_SUCCESSFULLY_STORED   (0xFFFFFFFF)

/* Valid Mask for Block Header */
#define FL_BH_VALID_MASK                (0xBB)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void R_FL_DownloaderInit(void);
void R_FL_StateMachine(void);

#endif /* FL_DOWNLOADER_H */


