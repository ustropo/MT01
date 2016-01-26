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
* File Name    : r_flash_api_rx_if.h
* Device       : RX600 Series, RX200 Series
* Tool-Chain   : RX Family C Compiler
* H/W Platform : RSKRX62N, RSKRX610, YRDKRX62N, RSKRX630, RSKRX63N, RSKRX62T,
*                YRDKRX63N, RSKRX63T, RSKRX62G, RSKRX210
* Description  : Interface file for Simple Flash API for RX
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

#ifndef _FLASH_API_RX_H
#define _FLASH_API_RX_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>
/* Used for boolean definitions */
#include <stdbool.h>
/* Used to get which MCU is currently being used. */
#include "platform.h"
/* User specific options for Flash API */
#include "r_flash_api_rx_config.h"

/* Memory specifics for the each MCU group */
#if   defined(BSP_MCU_RX610) 
    #include "./src/targets/rx610/r_flash_api_rx610.h"
#elif defined(BSP_MCU_RX621) || defined(BSP_MCU_RX62N) 
    #include "./src/targets/rx62n/r_flash_api_rx62n.h"
#elif defined(BSP_MCU_RX62T)
    #include "./src/targets/rx62t/r_flash_api_rx62t.h"
#elif defined(BSP_MCU_RX62G)
    #include "./src/targets/rx62g/r_flash_api_rx62g.h"
#elif defined(BSP_MCU_RX630)
    #include "./src/targets/rx630/r_flash_api_rx630.h"
#elif defined(BSP_MCU_RX631) || defined(BSP_MCU_RX63N)
    #include "./src/targets/rx63n/r_flash_api_rx63n.h"
#elif defined(BSP_MCU_RX63T)
    #include "./src/targets/rx63t/r_flash_api_rx63t.h"
#elif defined(BSP_MCU_RX210)
    #include "./src/targets/rx210/r_flash_api_rx210.h"
#else
    #error "!!! No 'targets' folder for this MCU Group !!!"
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
/* Version Number of API. */
#define RX_FLASH_API_VERSION_MAJOR           (2)
#define RX_FLASH_API_VERSION_MINOR           (40)

/* Pointer definitions for what should be sent in to R_FlashWrite */
#define FLASH_PTR_TYPE uint32_t
#define   BUF_PTR_TYPE uint32_t

/**** Function Return Values ****/
/* Operation was successful */
#define FLASH_SUCCESS           (0x00)
/* Flash area checked was blank, making this 0x00 as well to keep existing
   code checking compatibility */
#define FLASH_BLANK             (0x00)
/* The address that was supplied was not on aligned correctly for ROM or DF */
#define FLASH_ERROR_ALIGNED     (0x01)
/* Flash area checked was not blank, making this 0x01 as well to keep existing
   code checking compatibility */
#define FLASH_NOT_BLANK         (0x01)
/* The number of bytes supplied to write was incorrect */
#define FLASH_ERROR_BYTES       (0x02)
/* The address provided is not a valid ROM or DF address */
#define FLASH_ERROR_ADDRESS     (0x03)
/* Writes cannot cross the 1MB boundary on some parts */
#define FLASH_ERROR_BOUNDARY    (0x04)
/* Flash is busy with another operation */
#define FLASH_BUSY              (0x05)
/* Operation failed */
#define FLASH_FAILURE           (0x06)
/* Lock bit was set for the block in question */
#define FLASH_LOCK_BIT_SET      (0x07)
/* Lock bit was not set for the block in question */
#define FLASH_LOCK_BIT_NOT_SET  (0x08)

#if   (DF_PROGRAM_SIZE_SMALL == 8)
    /* 'size' parameter for R_FlashDataAreaBlankCheck */
    #define BLANK_CHECK_8_BYTE       0  
#elif (DF_PROGRAM_SIZE_SMALL == 2)
    /* 'size' parameter for R_FlashDataAreaBlankCheck */
    #define BLANK_CHECK_2_BYTE       0
#endif
/* 'size' parameter for R_FlashDataAreaBlankCheck */
#define BLANK_CHECK_ENTIRE_BLOCK     1  

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
uint8_t  R_FlashErase(uint32_t block);
#if defined(DF_GROUPED_BLOCKS)
uint8_t  R_FlashEraseRange(uint32_t start_addr, uint32_t bytes);
#endif
uint8_t  R_FlashWrite(uint32_t flash_addr, uint32_t buffer_addr, uint16_t bytes);
uint8_t  R_FlashProgramLockBit(uint32_t block);
uint8_t  R_FlashReadLockBit(uint32_t block);
uint8_t  R_FlashSetLockBitProtection(uint32_t lock_bit);                       
uint8_t  R_FlashGetStatus(void);       
uint32_t R_FlashGetVersion(void);
/* Data Flash Only Functions */
void     R_FlashDataAreaAccess(uint16_t read_en_mask, uint16_t write_en_mask);
uint8_t  R_FlashDataAreaBlankCheck(uint32_t address, uint8_t size);

/* These functions are only used when BGO (non-blocking) mode is enabled */
#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO) || defined(FLASH_API_RX_CFG_ROM_BGO)
/* Callback function to call when flash erase is finished */
void FlashEraseDone(void);
/* Callback function to call when flash write is finished */
void FlashWriteDone(void);
/* Function to take care of flash errors */
void FlashError(void);
/* Callback function to call when flash blank check is finished. 'result' 
   argument is 0 if block was blank and 1 if it was not */ 
void FlashBlankCheckDone(uint8_t result);
#endif

#ifdef FLASH_API_RX_CFG_COPY_CODE_BY_API
void R_FlashCodeCopy(void);
#endif

#endif /* _FLASH_API_RX_H */
