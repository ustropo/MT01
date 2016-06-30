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
* File Name	   : r_fl_types.h
* Version      : 3.0 
* Description  : Defines Flash Loader data structures..
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 22.02.2012 3.00     First Release. These structures were originally
*                               in downloader.h. They were moved here to make
*                               them easier to modify or add new ones. They 
*                               were also slightly changed to match CS v4.0.
*                               (introduced in v3.0)
******************************************************************************/

#ifndef FL_TYPES
#define FL_TYPES

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>
/* Flash Loader configuration options. */
#include "r_flash_loader_rx_config.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/* Holds all FlashLoader Downloader state machine states */
typedef enum  
{
    FL_STATE_START,
    FL_STATE_INIT_1,
    FL_STATE_INIT_2,
    FL_STATE_OP_NEW_IMAGE,
    FL_STATE_NEW_IMAGE,
    FL_STATE_NEW_IMAGE_ERASING,
    FL_STATE_RETRY_INIT,
    FL_STATE_RETRY_GET_BLOCK,
    FL_STATE_RETRY_FINISH,
    FL_STATE_RECEIVING_INIT,
    FL_STATE_RECEIVING_HEADER,
    FL_STATE_RECEIVING_DATA,
    FL_STATE_STORING_DATA,
    FL_STATE_RECEIVE_NEXT,
    FL_STATE_OP_ERASE_BLOCK_START,
    FL_STATE_OP_ERASE_BLOCK_RUNNING
} fl_receive_states_t;

/* The 'pack' option is used here to make sure that the toolchain does not put
   any padding in between the structure entries. Doing this causes problems
   when dealing with communication structures. */
#pragma pack

/* Structure of FlashLoader Load Image Header */
typedef struct  
{
    /* To confirm valid header */
    uint8_t     valid_mask;
    /* Version major  */
    uint8_t     version_major;
    /* Version middle */
    uint8_t     version_middle;
    /* Version minor */
    uint8_t    version_minor;
    /* Version compilation */
    uint8_t    version_comp;
    /* CRC-16 CCITT of image as in MCU flash */
    uint16_t    raw_crc;
} fl_image_header_t;

/* Structure of FlashLoader Block Header */
typedef struct  
{
    /* To confirm valid header */
    uint8_t     valid_mask;
    /* Packet number */
    uint16_t    sequence_ID;
    /* Address in MCU flash */
    uint32_t    flash_address;
    /* Number of bytes of data */
    uint32_t    data_size;
    /* CRC-16 CCITT of data */
    uint16_t    data_crc;
    /* Address in SPI of next BlockHeader */
    uint32_t    next_block_address;
    /* Then comes the actual data */
} fl_block_header_t;

/* Turn off the pack option and put back to default. */
#pragma packoption

/* Structure for defining FL Load Image storage area. */
typedef struct
{
    /* The minimum erase size in bytes. */
    uint32_t    erase_size;
    /* The maximum bytes that can be programmed at once. */
    uint32_t    max_program_size;
    /* Addresses of FL Load Images. '+1' is used because the last entry in the
       array is the max address for load image data. */
    uint32_t    addresses[FL_CFG_MEM_NUM_LOAD_IMAGES+1];
} fl_li_storage_t;

#endif /* FL_TYPES */
