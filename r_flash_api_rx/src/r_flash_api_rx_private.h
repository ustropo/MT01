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
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_api_rx_private.h
* Description  : Flash programming for the RX600 & RX200 Series
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 21.12.2009 1.00    First Release
*         : 13.01.2010 1.10    Made function names and variables RAPI compliant
*         : 11.02.2010 1.20    Fixed other RAPI issues and fixed I flag issue
*         : 29.04.2010 1.30    Added support for RX621/N Group. Moved most
*                              device specific data to header file.
*         : 26.05.2010 1.40    Added support for RX62T Group
*         : 28.07.2010 1.41    Fixed bug when performing a blank check on an
*                              entire data flash block.  Also declared 
*                              functions not in the API as 'static'.
*         : 23.08.2010 1.42    Updated source to raise the coding standard, to
*                              meet GSCE & RSK standards.
*         : 15.02.2011 1.43    Fixed bug in blank check routine when handling
*                              input arguments and moved _Flash_Init() to
*                              _Enter_PE_Mode() function.
*         : 21.04.2011 2.00    Added BGO capabilities for data flash. Made 
*                              some more changes to fit coding standard. Added
*                              ability to do ROM to ROM or DF to DF transfers.
*                              Added the ability to use the lock bit feature
*                              on ROM blocks.  Added BGO capabilities for
*                              ROM operations.
*         : 06.07.2011 2.10    Added support for RX630, RX631, and RX63N.
*                              Also added R_FlashEraseRange() for devices like
*                              RX63x that have finer granularity data flash.
*                              Various bug fixes as well. Example bug fix was
*                              removing DATA_FLASH_OPERATION_PIPL and 
*                              ROM_OPERATION_PIPL #defines since the IPL was
*                              not restored when leaving flash ready interrupt.
*         : 29.11.2011 2.20    Renamed private functions according to new 
*                              Coding Standard. Removed unused 'bytes' argument 
*                              from enter_pe_mode() function. Removed 'far' 
*                              keyword since it is not needed. Fixed where some 
*                              functions were being placed in RAM when this was
*                              not needed. Uses platform.h now instead of 
*                              having multiple iodefine_rxXXX.h's. Added 
*                              __evenaccess directive to FCU accesses. This 
*                              ensures proper bus width accesses. Added
*                              R_FlashCodeCopy() function. When clearing the
*                              FENTRYR register, the register is read back to
*                              ensure its value is 0x0000. Added call to 
*                              exit_pe_mode() when enter_pe_mode() function 
*                              fails to protect against reading ROM in P/E
*                              mode. Added option to use r_bsp package.
*         : 11.09.2012 2.30    Updated to meet FIT v0.7 spec. Now uses r_bsp
*                              locking. Removed FLASH_API_USE_R_BSP macro. Code 
*                              now automatically detects r_bsp. Moved MCU 
*                              specific information into 'ports' folder instead 
*                              of keep on expanding r_flash_api_rx_if.h.
*                              Added R_FlashGetVersion() function. Added 
*                              timeout for while() loop reading back FENTRYR 
*                              value.
*         : 29.11.2012 2.40    Updated to meet FIT v1.0 spec. Added support
*                              for RX62G and RX63T MCU groups. Cleaned up 
*                              code some by removing almost all preprocessor
*                              statements that tested against MCU groups. Now
*                              have 'targets' directory for easier addition
*                              of new MCUs. Updated to use r_bsp v2.00.
******************************************************************************/

#ifndef _FLASH_API_RX_PRIVATE_H
#define _FLASH_API_RX_PRIVATE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Get r_bsp info. */
#include "platform.h"

/******************************************************************************
Error checking
******************************************************************************/
/* Make sure that a valid version of the r_bsp package is present. */
#if (R_BSP_VERSION_MAJOR < 2)
    #error "ERROR !!! Flash API requires r_bsp >= v2.00"
#endif

/* This catches to make sure the user specified a CPU clock */
#if !defined(BSP_ICLK_HZ)
    #error "ERROR !!! You must specify the System Clock Frequency (BSP_ICLK_HZ)!"
#endif

/* This checks to make sure an IPL for the flash ready interrupt was defined */
#if (defined(FLASH_API_RX_CFG_ROM_BGO) || defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)) && \
    !defined(FLASH_API_RX_CFG_FLASH_READY_IPL)
    #error "ERROR !!! You must specify an IPL for the flash ready interrupt \
            in r_flash_api_rx_config.h when using BGO."
#endif

/* If ROM BGO is enabled then ROM programming must be enabled too. */
#if (defined(FLASH_API_RX_CFG_ROM_BGO) && !defined(FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING))
    #error "ERROR !!! In order to enable ROM BGO, you must first enable ROM programming \
            by using the FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING macro in \
            r_flash_api_rx_config.h."
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
/*  Bottom of User Flash Area */
#define ROM_PE_ADDR     ((0x100000000-BSP_ROM_SIZE_BYTES)&(0x00FFFFFF))

/* FCU-RAM address define */
/* FCU F/W Store Address */
#define FCU_PRG_TOP     0xFEFFE000
/* FCU RAM Address */
#define FCU_RAM_TOP     0x007F8000
/* FCU RAM Size */
#define FCU_RAM_SIZE    0x2000

#endif /* _FLASH_API_RX_PRIVATE_H */
