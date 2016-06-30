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
* File Name    : r_fl_memory_spi_flash.c
* Version      : 3.00
* Description  : Low level memory operations are implemented here.  This file
*                will change depending on what storage is used for holding
*                Load Images (or other data that is downloaded). This 
*                code is setup to work with a Numonyx P5Q PCM. This part
*                uses the same interface as most SPI flashes so you should be
*                able to easily modify this one to work with your own.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 23.02.2012 3.00    Made compliant with CS v4.0. Moved over from 
*                              straight driver code to using r_rspi_rx600 
*                              package. Got rid of FL_Ext_Addresses array and
*                              replaced with g_fl_li_mem_info structure. This
*                              was done to make the Memory portion of the FL
*                              project more modular.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Info on which board is being used. */
#include "platform.h"
/* Flash Loader project includes. */
#include "r_fl_includes.h"
/* Uses r_rspi_rx package. */
#include "r_rspi_rx_if.h"
/* SPI Flash package. */
#include "r_spi_flash_if.h"

uint8_t memID[3];

/******************************************************************************
Macro definitions
******************************************************************************/
/* Choose which channel to use based on selected board. */
#if defined(BSP_BOARD_RDKRX62N) || defined(BSP_BOARD_RDKRX63N) || defined(BSP_BOARD_RSKRX210)
    #define FL_RSPI_CHANNEL     (1)
#elif defined(BSP_BOARD_RSKRX62N) || defined(BSP_BOARD_RSKRX63N)
    #define FL_RSPI_CHANNEL     (1)
#elif defined(BSP_BOARD_MT01)
    #define FL_RSPI_CHANNEL     (1)
#else
    #error "No RSPI channel chosen for SPI flash communications. Please choose channel in r_fl_memory_p5q.c"
#endif

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/
/* This structure defines the memory that load images will be stored in. */
const fl_li_storage_t g_fl_li_mem_info = 
{
    /* The minimum erase size in bytes. */
    (uint32_t)SF_MEM_MIN_ERASE_BYTES,
    /* The maximum bytes that can be programmed at once. Starting with v3.0 of the FL this is no longer a SPI flash
       specific number. The r_spi_flash package now handles programming of as many bytes as you want at once. This value
       is still kept in the event that you do want to split up SPI flash programs. Another reason I am leaving this in 
       here is because other memories may be used where this is more of a requirement.  */
    (0x400),
    /* Addresses of FL Load Images. '+1' is used because the last entry in the
       array is the max address for load image data. */
#if   FL_CFG_MEM_NUM_LOAD_IMAGES == 1
    { FL_CFG_MEM_BASE_ADDR, 
      FL_CFG_MEM_BASE_ADDR + FL_CFG_MEM_MAX_LI_SIZE_BYTES }
#elif   FL_CFG_MEM_NUM_LOAD_IMAGES == 2
    { FL_CFG_MEM_BASE_ADDR, 
      FL_CFG_MEM_BASE_ADDR + FL_CFG_MEM_MAX_LI_SIZE_BYTES,
      FL_CFG_MEM_BASE_ADDR + (FL_CFG_MEM_MAX_LI_SIZE_BYTES*2) }
#elif FL_CFG_MEM_NUM_LOAD_IMAGES == 3
    { FL_CFG_MEM_BASE_ADDR, 
      FL_CFG_MEM_BASE_ADDR + FL_CFG_MEM_MAX_LI_SIZE_BYTES,
      FL_CFG_MEM_BASE_ADDR + (FL_CFG_MEM_MAX_LI_SIZE_BYTES*2),
      FL_CFG_MEM_BASE_ADDR + (FL_CFG_MEM_MAX_LI_SIZE_BYTES*3) }
#else
    #error "Addresses are not specified for this many Load Images. Please add details for this setup in r_fl_memory**.c"
#endif
};

/******************************************************************************
* Function Name: fl_mem_read
* Description  : Reads data from memory where load images are stored
* Arguments    : rx_address - 
*                    Where to read from in memory
*                rx_buffer - 
*                    Where to place read data
*                rx_bytes - 
*                    How many bytes to read
* Return value : none
******************************************************************************/
void fl_mem_read(uint32_t rx_address, uint8_t * rx_buffer, uint32_t rx_bytes)
{      
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1) 
    {
        /* Make sure SPI flash is not busy */
    }
    
    /* Read data from external SPI flash */
    R_SF_ReadData( FL_RSPI_CHANNEL,
                   rx_address,
                   rx_buffer,
                   rx_bytes);
}
/******************************************************************************
End of function fl_mem_read
******************************************************************************/

/******************************************************************************
* Function Name: fl_mem_write
* Description  : Writes data to memory where load images are stored
* Arguments    : tx_address - 
*                    Where to write in memory
*                tx_buffer - 
*                    What data to write                 
*                tx_bytes - 
*                    How many bytes to write
* Return value : none
******************************************************************************/
void fl_mem_write(uint32_t tx_address, uint8_t *tx_buffer, uint32_t tx_bytes)
{
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1) 
    {
        /* Make sure SPI flash is not busy */
    }
    
    /* Write data to external SPI flash */
    R_SF_WriteData( FL_RSPI_CHANNEL,
                    tx_address,
                    tx_buffer,
                    tx_bytes);
}
/******************************************************************************
End of function fl_mem_write
******************************************************************************/

/******************************************************************************
* Function Name: fl_mem_get_busy
* Description  : Returns whether the memory is currently busy
* Arguments    : none
* Return value : true - 
*                    The memory is busy
*                false - 
*                    The memory is not busy
******************************************************************************/
bool fl_mem_get_busy(void)
{
    if( (R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************
End of function fl_mem_get_busy
******************************************************************************/

/******************************************************************************
* Function Name: fl_mem_init
* Description  : Initializes resources needed for talking to memory holding
*                FL load images
* Arguments    : none
* Return value : none
******************************************************************************/
void fl_mem_init(void)
{
	R_SF_Init(FL_RSPI_CHANNEL);
}
/******************************************************************************
End of function fl_mem_init
******************************************************************************/

/******************************************************************************
* Function Name: fl_mem_erase
* Description  : Erases parts, or whole, memory used for FL load images
* Arguments    : address - 
*                    Where you want to erase
*                size - 
*                    How many bytes to erase
* Return value : true - 
*                    Sucessfull
*                false - 
*                    Not successfull, invalid argument
******************************************************************************/
bool fl_mem_erase(const uint32_t address, const uint8_t size)
{
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1) 
    {
        /* Make sure SPI flash is not busy */
    }
    
    /* Erase requested part of memory */
    if(size == FL_MEM_ERASE_SECTOR)
    {
        /* Erase sector */
        R_SF_Erase(FL_RSPI_CHANNEL, address, SF_ERASE_SECTOR);
    } 
    else if(size == FL_MEM_ERASE_CHIP)
    {
        /* Bulk erase */
        R_SF_Erase(FL_RSPI_CHANNEL, address, SF_ERASE_BULK);
    } 
    else if(size == FL_MEM_ERASE_BLOCK)
    {
        /* Bulk erase */
        R_SF_Erase(FL_RSPI_CHANNEL, address, SF_ERASE_BLOCK);
    }
    else 
    {
        /* Unknown option */
        return false;
    }
    
    return true;
}
/******************************************************************************
End of function fl_mem_erase
******************************************************************************/

