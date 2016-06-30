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
* File Name	   : r_spi_flash_p5q.h
* Description  : This header file has specifics for Numonyx P5Q PCM SPI flashes.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 29.02.2012 1.00    First Release            
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Write protect bit mask. If WP is bit 0 then it would be 0x01. If it is bit 7 it would be 0x80. */
#define SF_WP_BIT_MASK                  (0x80)

/* 'Write in progress' bit mask. */
#define SF_WIP_BIT_MASK                 (0x01)

/****************SPI FLASH COMMANDS****************/
/* Write Enable command */
#define SF_CMD_WRITE_ENABLE             (0x06)

/* Write SPI flash status register command. */
#define SF_CMD_WRITE_STATUS_REG         (0x01)

/* Read status register command. */
#define SF_CMD_READ_STATUS_REG          (0x05)

/* Read ID command. */
#define SF_CMD_READ_ID                  (0x9F)

/* Erase size options. */
/* Sector erase command. */
#define SF_CMD_ERASE_SECTOR             (0xD8)
/* Erase all of memory. */
#define SF_CMD_ERASE_BULK               (0xC7)

/* Page program command. */
#define SF_CMD_PAGE_PROGRAM             (0x02)

/* Read command. */
#define SF_CMD_READ                     (0x03)

/****************MEMORY SPECIFICS****************/
/* Minimum erase size. */
#define SF_MEM_MIN_ERASE_BYTES          (0x20000)    //P5Q has 128KB erase sectors

/* Maximum bytes to program with one program command. */
#define SF_MEM_MAX_PROGRAM_BYTES        (64)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* This typedef lists the available erase options. Most SPI flashes have multiple options but the minimum options are 
   usually a sector and bulk erase. */
typedef enum
{
    SF_ERASE_SECTOR = 0,
    SF_ERASE_BULK
} sf_erase_sizes_t;


