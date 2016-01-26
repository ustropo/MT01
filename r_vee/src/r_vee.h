/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_vee.h
* Description  : Private header file for Virtual EEPROM.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 03.01.2013 1.70    Added R_VEE_Open() function to initialize or reset VEE. Created r_vee_target.h to replace
*                              multiple r_vee_<mcu>.h files that had duplicate information. Updated to be compliant with
*                              FIT v1.00 specification. This means that config file is now in 'ref' folder. Tested with
*                              RX62G, RX210, and RX63T. Added R_VEE_Control() function. First release for this file.
***********************************************************************************************************************/

#ifndef VEE_H
#define VEE_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* From r_bsp. Gives us MCU information used to configure VEE project. */
#include "platform.h"
/* For data flash program size. */
#include "r_flash_api_rx_if.h"
/* For detecting correct Flash API configuration. */


/***********************************************************************************************************************
Proper configuration checks
***********************************************************************************************************************/
/* Flash API checks. */
#if !defined(FLASH_API_RX_CFG_FLASH_TO_FLASH)
    #error "The FLASH_API_RX_CFG_FLASH_TO_FLASH macro in r_flash_api_rx_config.h must be enabled to use the VEE."
#endif

#if !defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
    #error "The FLASH_API_RX_CFG_DATA_FLASH_BGO macro in r_flash_api_rx_config.h must be enabled to use the VEE."
#endif

/* VEE checks. */
#if (VEE_NUM_SECTORS <= 0)
    #error "VEE_NUM_SECTORS in r_vee_config.h must be defined to a value >= 1"
#endif

#if (VEE_MAX_RECORD_ID <= 0)
    #error "VEE_MAX_RECORD_ID in r_vee_config.h must be defined to a value >= 1"
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Marks a cache entry as valid */
#define VEE_CACHE_ENTRY_VALID       (0x85)
/* Any value that is not VEE_CACHE_ENTRY_VALID will work, but define one here to get rid of magic numbers. */
#define VEE_CACHE_ENTRY_INVALID     (0)

/* Offsets from start of VEE Blocks to that blocks state flags */
#define VEE_BLOCK_STATE_ERASING     (0)
#define VEE_BLOCK_STATE_ACTIVE      (1)
#define VEE_BLOCK_STATE_FULL        (2)
#define VEE_BLOCK_STATE_NEXTUP      (3)

/* For determining if a VEE Block's state flags are set */
#define VEE_BLOCK_FLAG_SET          (1)
#define VEE_BLOCK_FLAG_NOT_SET      (0)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
#if   (DF_PROGRAM_SIZE_SMALL == 1)
/* Set size of vee_var_data_t to the minimum write size of MCU's data flash or larger. This is the size of the 
   variables in a record structure. */
typedef uint8_t vee_var_data_t;
/* Set size of vee_var_min_t to the minimum write size of MCU's data flash */
typedef uint8_t vee_var_min_t;
#elif (DF_PROGRAM_SIZE_SMALL == 2)
/* Set size of vee_var_data_t to the minimum write size of MCU's data flash or larger. This is the size of the 
   variables in a record structure. */
typedef uint16_t vee_var_data_t;
/* Set size of vee_var_min_t to the minimum write size of MCU's data flash */
typedef uint16_t vee_var_min_t;
#elif (DF_PROGRAM_SIZE_SMALL == 4)
/* Set size of vee_var_data_t to the minimum write size of MCU's data flash or larger. This is the size of the 
   variables in a record structure. */
typedef uint32_t vee_var_data_t;
/* Set size of vee_var_min_t to the minimum write size of MCU's data flash */
typedef uint32_t vee_var_min_t;
#elif (DF_PROGRAM_SIZE_SMALL == 8)
/* Set size of vee_var_data_t to the minimum write size of MCU's data flash or larger. This is the size of the 
   variables in a record structure. */
typedef uint64_t vee_var_data_t;
/* Set size of vee_var_min_t to the minimum write size of MCU's data flash */
typedef uint64_t vee_var_min_t;
#endif

#endif //VEE_H


