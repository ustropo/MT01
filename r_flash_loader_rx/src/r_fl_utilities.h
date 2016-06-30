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
* File Name    : r_fl_utilities.h
* Version      : 3.00
* Description  : Contains functions for FlashLoader use such as CRC and Reset
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 21.09.2011 2.01    Fixed calculation for timer register in 
*                              fl_start_timer.
*         : 23.02.2012 3.00    Removed 'LOWEST_ROM_ADDRESS' macro. Instead 
*                              getting this info from Flash API. Made code
*                              compliant with CS v4.0.
******************************************************************************/

#ifndef FL_UTIL_H
#define FL_UTIL_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>

/******************************************************************************
Macro definitions
******************************************************************************/
/* Seed used for CRCs in communications */
#define FL_CRC_SEED     (0x1D0F)
/* Seed used by RX linker.  Used for calculating raw_crc */
#define RX_LINKER_SEED  (0xFFFF)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
uint16_t fl_check_application(void);
void     fl_reset(void);
void     fl_signal(void);
bool     fl_check_bootloader_bypass(void);

#endif /* FL_UTIL_H */

