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
* File Name    : r_fl_memory.h
* Version      : 3.00
* Description  : Low level memory operations are implemented here.  
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 23.02.2012 3.00    Removed all memory specific macros. This info is
*                              now found in the 'g_fl_li_mem_info' structure.
*                              The 'FL_CFG_MEM_NUM_LOAD_IMAGES' was moved to  
*                              r_flash_loader_rx_config.h.
******************************************************************************/

#ifndef FL_MEMORY_H
#define FL_MEMORY_H

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
/* Option to erase sector on SPI flash */
#define FL_MEM_ERASE_SECTOR     0
/* Option to erase entire SPI flash chip */
#define FL_MEM_ERASE_CHIP       1
/* Option to erase block on SPI flash */
#define FL_MEM_ERASE_BLOCK       2

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void fl_mem_init(void);
void fl_mem_read(uint32_t rx_address, uint8_t *rx_buffer, uint32_t rx_bytes);
void fl_mem_write(uint32_t tx_address, uint8_t *tx_buffer, uint32_t tx_bytes);
bool fl_mem_erase(const uint32_t address, const uint8_t size);
bool fl_mem_get_busy(void);

#endif /* FL_MEMORY_H */
