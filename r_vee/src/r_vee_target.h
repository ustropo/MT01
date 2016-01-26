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
* File Name    : r_vee_target.h
* Description  : Includes the appropriate header file for the currently chosen MCU.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 03.01.2013 1.70    Added R_VEE_Open() function to initialize or reset VEE. Created r_vee_target.h to replace
*                              multiple r_vee_<mcu>.h files that had duplicate information. Updated to be compliant with
*                              FIT v1.00 specification. This means that config file is now in 'ref' folder. Tested with
*                              RX62G, RX210, and RX63T. Added R_VEE_Control() function. First release for this file.
***********************************************************************************************************************/

#ifndef VEE_TARGET_H
#define VEE_TARGET_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* From r_bsp. Gives us MCU information used to configure VEE project. */
#include "platform.h"
/* VEE types. */
#include "r_vee_types.h"

/* Depending on which MCU was chosen in mcu_info.h we can bring in the appropriate VEE port header files. The header
   files needed are:
   -r_vee_*MCU Group*.h: This file has info about MCU specific part of VEE (size of VEE typedefs)
   -r_vee_config_*MCU Group*.h: This file has record and sector configurations. */
#if   defined(BSP_MCU_RX63_ALL)    
    #if   (BSP_DATA_FLASH_SIZE_BYTES == 8192)
        #include "targets\rx63x\r_vee_config_rx63x_8kb.h"
    #elif (BSP_DATA_FLASH_SIZE_BYTES == 32768)
        #include "targets\rx63x\r_vee_config_rx63x_32kb.h"
    #else
        #error "No VEE configuration file for a MCU with this size data flash. Make new config file in targets\rx63x\"
    #endif
#elif defined(BSP_MCU_RX62_ALL)
    #if   (BSP_DATA_FLASH_SIZE_BYTES == 8192)
        #include "targets\rx62x\r_vee_config_rx62x_8kb.h"
    #elif (BSP_DATA_FLASH_SIZE_BYTES == 32768)
        #include "targets\rx62x\r_vee_config_rx62x_32kb.h"
    #else
        #error "No VEE configuration file for a MCU with this size data flash. Make new config file in targets\rx62x\"
    #endif
#elif defined(BSP_MCU_RX21_ALL)
    #if   (BSP_DATA_FLASH_SIZE_BYTES == 8192)
        #include "targets\rx21x\r_vee_config_rx21x_8kb.h"
    #else
        #error "No VEE configuration file for a MCU with this size data flash. Make new config file in targets\rx21x\"
    #endif
#else
    #error "No MCU chosen for VEE. Please select platform in platform.h"
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#if   (DF_PROGRAM_SIZE_SMALL == 1)
    /* VEE Block state flags, the size of these should be the minimum write size of this MCU's data flash */
    #define VEE_BLOCK_FLAG_ERASING  (0xAA)
    #define VEE_BLOCK_FLAG_ACTIVE   (0xBB)
    #define VEE_BLOCK_FLAG_FULL     (0xCC)
    #define VEE_BLOCK_FLAG_NEXTUP   (0xDD)
    
    /* Value that is written into the 'check' field when programming a record */
    #define VEE_RECORD_WRITTEN      (0xAB)    
#elif (DF_PROGRAM_SIZE_SMALL == 2)
    /* VEE Block state flags, the size of these should be the minimum write size of this MCU's data flash */
    #define VEE_BLOCK_FLAG_ERASING  (0xAAAA)
    #define VEE_BLOCK_FLAG_ACTIVE   (0xBBBB)
    #define VEE_BLOCK_FLAG_FULL     (0xCCCC)
    #define VEE_BLOCK_FLAG_NEXTUP   (0xDDDD)
    
    /* Value that is written into the 'check' field when programming a record */
    #define VEE_RECORD_WRITTEN      (0xABCD)
#elif (DF_PROGRAM_SIZE_SMALL == 4)
    /* VEE Block state flags, the size of these should be the minimum write size of this MCU's data flash */
    #define VEE_BLOCK_FLAG_ERASING  (0xAAAAAAAA)
    #define VEE_BLOCK_FLAG_ACTIVE   (0xBBBBBBBB)
    #define VEE_BLOCK_FLAG_FULL     (0xCCCCCCCC)
    #define VEE_BLOCK_FLAG_NEXTUP   (0xDDDDDDDD)
    
    /* Value that is written into the 'check' field when programming a record */
    #define VEE_RECORD_WRITTEN      (0xABCDABCD)
#elif (DF_PROGRAM_SIZE_SMALL == 8)
    /* VEE Block state flags, the size of these should be the minimum write size of this MCU's data flash */
    #define VEE_BLOCK_FLAG_ERASING  (0xAAAAAAAAAAAAAAAA)
    #define VEE_BLOCK_FLAG_ACTIVE   (0xBBBBBBBBBBBBBBBB)
    #define VEE_BLOCK_FLAG_FULL     (0xCCCCCCCCCCCCCCCC)
    #define VEE_BLOCK_FLAG_NEXTUP   (0xDDDDDDDDDDDDDDDD)
    
    /* Value that is written into the 'check' field when programming a record */
    #define VEE_RECORD_WRITTEN      (0xABCDABCDABCDABCD)
#endif

#if defined(BSP_MCU_SERIES_RX600) || defined(BSP_MCU_SERIES_RX200)
    /* This defines whether we need to explicitly enable the data flash before trying to read it */
    #define VEE_ENABLE_DF           (1)
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
#ifdef VEE_ENABLE_DF
void vee_enable_df(void);
#endif 
uint8_t vee_get_block_info(uint8_t sector, uint32_t block, vee_block_info_t *VEE_block);
uint8_t vee_blank_check_address(uint8_t *addr);
uint32_t vee_move_to_boundary(uint32_t address);
uint8_t vee_check_record(vee_record_t *record);
uint8_t R_VEE_GenerateCheck(vee_record_t *record);
uint8_t vee_blank_check_block(uint32_t block);

#endif //VEE_TARGET_H


