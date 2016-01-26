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
* File Name    : r_vee_types.h
* Description  : Virtual EEPROM structures and types
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.07.2011 1.00    First Release
*         : 03.01.2012 1.50    Updated for internal CS and for FIT. Added support for RX63x Groups.
*         : 14.09.2012 1.60    Updated for FIT v0.7 Spec. Fixed bug found when reset occurred after FULL flag was 
*                              written and before NEXTUP was written. VEE now handles this event.
*         : 03.01.2013 1.70    Added R_VEE_Open() function to initialize or reset VEE. Created r_vee_target.h to replace
*                              multiple r_vee_<mcu>.h files that had duplicate information. Updated to be compliant with
*                              FIT v1.00 specification. This means that config file is now in 'ref' folder. Tested with
*                              RX62G, RX210, and RX63T. Added R_VEE_Control() function.
***********************************************************************************************************************/

#ifndef VEE_TYPES_H
#define VEE_TYPES_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed-width types. */
#include <stdint.h>
/* VEE interface has some typedefs that are needed. */
#include "r_vee_if.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* VEE Sector Structure */
typedef struct
{
    /* Unique VEE Sector identifier */
    uint8_t             sector_ID;
    /* The number of VEE Blocks in this VEE Sector */
    uint8_t             num_VEE_blocks;
    /* The size of each VEE Block in bytes */
    uint32_t            VEE_block_size;    
    /* The start addresses of these blocks */
    uint32_t const *    VEE_block_addr;
    /* Number of data flash blocks per VEE Block */
    uint8_t             num_df_blocks;
    /* This holds the lowest and highest data flash blocks for each VEE Block */
    uint32_t const      (* df_blocks)[2];
} vee_sector_t;

/* VEE Next Address entry */
typedef struct
{
    /* Flags this entry as valid or not */
    uint8_t             valid;
    /* Which VEE Block is used */
    uint32_t            block;
    /* Next open spot in the data flash */
    uint8_t far *       address;
} vee_next_address_t;

/* VEE Cache Entry */
typedef struct
{
    /* Flags this entry as valid or not */
    uint8_t               valid;
    /* Which VEE Block is used */
    uint32_t              block;
    /* Address of entry in the data flash */
    vee_record_t far *    address;
} vee_cache_entry_t;

/* VEE Cache */
typedef struct
{
    /* Tells whether cache has been filled or not */
    uint8_t                 valid;
    /* Cache entries */
    vee_cache_entry_t       entries[VEE_MAX_RECORD_ID];
} vee_cache_t;

/* VEE Block Info Structure */
typedef struct
{
    /* VEE Block state flags */
    uint8_t erasing;
    uint8_t active;
    uint8_t full;
    uint8_t nextup;
} vee_block_info_t;

/* Defines the possible VEE write states */
typedef enum 
{ 
    VEE_WRITE_FLAG_ACTIVE,
    VEE_WRITE_FLAG_ERASING,
    VEE_WRITE_FLAG_FULL,
    VEE_WRITE_FLAG_NEXTUP,
    VEE_WRITE_START_DEFRAG,
    VEE_WRITE_ID, 
    VEE_WRITE_SIZE,
    VEE_WRITE_BLOCK,
    VEE_WRITE_CHECK,
    VEE_WRITE_DATA,
    VEE_WRITE_DONE
} vee_write_states_t;

#endif /* VEE_TYPES_H */
