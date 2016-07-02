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
* File Name	   : r_spi_flash.c
* Description  : This module implements the protocol used by many SPI flashes. This code does not support some of the 
*                more advanced commands that are specific to some SPI flash chips.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 29.02.2012 1.00    First Release            
*         : 20.04.2012 1.10    Added support for Numonyx M25P16 SPI flash.
*         : 10.05.2012 1.20    Updated to be compliant with FIT Module Spec v0.7. Improved locking mechanics to be more
*                              efficient.
*         : 13.02.2013 1.30    Updated to be compliant with FIT Module Spec v1.02
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed-size integer typedefs. */
#include <stdint.h>
/* bool support. */
#include <stdbool.h>
/* min() support. */
#include <machine.h>
/* Board support. */
#include "platform.h"
/* This code relies on the r_rspi_rx package. */
#include "r_rspi_rx_if.h"
/* Configuration for this package. */
#include "r_spi_flash_config.h"
/* Header file for this package. */
#include "r_spi_flash_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define RSPI_POOLING		while(!g_transfer_complete); \
							g_transfer_complete = false;
/* Processor ID for locking RSPI channel. */
#define SF_PID          (0x11111111)
static void spiflash_callback(void * pdata);
volatile bool g_transfer_complete = false;
static rspi_handle_t spiflash_handle;
rspi_command_word_t spiflash_command =
{
	.cpha = RSPI_SPCMD_CPHA_SAMPLE_ODD,
	.cpol = RSPI_SPCMD_CPOL_IDLE_LO,
	.bit_order = RSPI_SPCMD_ORDER_MSB_FIRST,
	.bit_length = RSPI_SPCMD_BIT_LENGTH_8,
	.br_div = RSPI_SPCMD_BR_DIV_1,
};

rspi_chnl_settings_t spiflash_config =
{
	.gpio_ssl = RSPI_IF_MODE_3WIRE,
	.master_slave_mode = RSPI_MS_MODE_MASTER,
	.bps_target = 24000000,
};

rspi_cmd_baud_t spiflash_setbaud;
rspi_err_t spiflash_result;

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
static void    sf_open(uint8_t channel);
static void    sf_close(uint8_t channel);
static void    sf_write_enable(uint8_t channel);
static void    sf_write_protect(uint8_t channel);
static void    sf_write_unprotect(uint8_t channel);
static bool    sf_lock_channel(uint8_t channel);
static bool    sf_unlock_channel(uint8_t channel);
static uint8_t sf_read_status (uint8_t channel);

/***********************************************************************************************************************
* Function Name: sf_write_enable
* Description  : Sets Write Enable Latch bit
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : none
***********************************************************************************************************************/
static void sf_write_enable (uint8_t channel)
{
    /* Send write enable command */
    uint8_t val = SF_CMD_WRITE_ENABLE;
    
    /* Initialize peripheral for SPI */
    sf_open(channel);
    
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Close peripheral for SPI */
    sf_close(channel);  
}


/***********************************************************************************************************************
* Function Name: sf_write_protect
* Description  : Sets the Status Register Write Disable (SRWD) bit. Setting this bit locks the status register from 
*                being rewritten if the Write Protect pin is high.  If the pin is low, then the status register can be 
*                changed. You must set/clear the Block Protect Bits at this time too.
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : none
***********************************************************************************************************************/
static void sf_write_protect (uint8_t channel)
{
    uint8_t val[2];
    
    /* Send write enable command */
    sf_write_enable(channel);
    
    /* Initialize peripheral for SPI */
    sf_open(channel);
    
    /* This section writes data.  The first character is the write command */
    val[0] = SF_CMD_WRITE_STATUS_REG;
    val[1] = SF_WP_BIT_MASK;
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Close peripheral for SPI */
    sf_close(channel);  
}

/***********************************************************************************************************************
* Function Name: sf_write_unprotect
* Description  : Clears the Status Register Write Disable (SRWD) bit, if the Write Protect pin is high. Clearing this 
*                bit unlocks the status register for rewriting. The Write Protect pin must be high to clear the SRWD 
*                bit. You can set/clear the Block Protect Bits at this time too.
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : none
***********************************************************************************************************************/
static void sf_write_unprotect (uint8_t channel)
{
    uint8_t val[2];
    
    /* Send write enable command */
    sf_write_enable(channel);
    
    /* Initialize peripheral for SPI */
    sf_open(channel);
    
    /* This section writes data.  The first character is the write command */
    val[0] = SF_CMD_WRITE_STATUS_REG;
    val[1] = 0x0;
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Close peripheral for SPI */
    sf_close(channel);
  
}

void R_SF_Init(uint8_t channel)
{
    /* Initialize peripherals used for talking to SPI flash */
	R_RSPI_Open(channel, &spiflash_config, spiflash_callback, &spiflash_handle);
}

/***********************************************************************************************************************
* Function Name: R_SF_Erase
* Description  : Performs either a partial or whole chip erase. Erase size options are defined in sf_erase_sizes type
*                in the header file for this particular SPI flash.
* Arguments    : channel -
*                    Which SPI channel to use.
*                address - 
*                    Address of sector to erase
*                size -
*                    Erase part of memory or whole memory. Options are in sf_erase_sizes type.
* Return Value : true -
*                    Erase was started.
*                false -
*                    Could not start erase.
***********************************************************************************************************************/
bool R_SF_Erase (uint8_t channel, const uint32_t address, const sf_erase_sizes_t size)
{
    bool    ret = true;
    uint8_t val[4];

    /* Attempt to obtain lock for SPI channel. */
    if (false == sf_lock_channel(channel))
    {
        /* This channel is already being used. Try again later. */
        return false;
    }

    /* Allow memory to be modified */
    sf_write_unprotect(channel);

    /* Wait for WIP bit to clear */
    while ((sf_read_status(channel) & SF_WIP_BIT_MASK) == 1);

    /* Send write enable command */
    sf_write_enable(channel);

    /* Initialize peripheral for SPI */
    sf_open(channel);

    /* Erase command */
    if (size == SF_ERASE_SECTOR)
    {
        /* Assign sector erase command. */
        val[0] = SF_CMD_ERASE_SECTOR;

        /* Sector erase is one byte command and 24bit address */
        val[1] = (uint8_t)(address >> 16);
        val[2] = (uint8_t)(address >>  8);
        val[3] = (uint8_t)(address >>  0);

    	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
    	RSPI_POOLING
    }
    else if(size == SF_ERASE_BULK)
    {
        /* Assign bulk erase command. */
        val[0] = SF_CMD_ERASE_BULK;

        /* bulk erase is one byte command */
    	R_RSPI_Write(spiflash_handle,spiflash_command,&val,1);
    	RSPI_POOLING
    }
    else if (size == SF_ERASE_BLOCK)
    {
        /* Assign sector erase command. */
        val[0] = SF_CMD_ERASE_BLOCK;

        /* Sector erase is one byte command and 24bit address */
        val[1] = (uint8_t)(address >> 16);
        val[2] = (uint8_t)(address >>  8);
        val[3] = (uint8_t)(address >>  0);

    	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
    	RSPI_POOLING
    }
    else
    {
        /* Bad command. */
        ret = false;
    }

    /* Close peripheral for SPI */
    sf_close(channel);

    /* Protect memory from modification */
    sf_write_protect(channel);

    /* Release lock on channel. */
    sf_unlock_channel(channel);

    return ret;
}

/***********************************************************************************************************************
* Function Name: R_SF_WriteData
* Description  : Writes data to the external flash.
* Arguments    : channel -
*                    Which SPI channel to use.
*                address -
*                    Target address to write to.
*                data - 
*                    Location to retrieve data to write.
*                size - 
*                    Amount of data to write.
* Return Value : true -
*                    Data sent successfully.
*                false -
*                    Bad input parameters.
***********************************************************************************************************************/
bool R_SF_WriteData (uint8_t channel, uint32_t address, uint8_t * data, uint32_t size)
{
    uint8_t  val[4];
#if (SF_MEM_MAX_PROGRAM_BYTES > 1)               
    uint32_t next_page_addr;
#endif    
    uint32_t bytes_to_write = 0;

    /* Attempt to obtain lock for SPI channel. */
    if (false == sf_lock_channel(channel))
    {
        /* This channel is already being used. Try again later. */
        return false;
    }
        
    /* We only need to worry about being on a program boundary for the first write. If there are other writes
       needed after that then it will always be on a page boundary. */
       
    /* How many bytes to write this time? This will be either max program size or how many bytes are left. */
    bytes_to_write = (uint32_t)min(SF_MEM_MAX_PROGRAM_BYTES, size);   
    
#if (SF_MEM_MAX_PROGRAM_BYTES > 1)           
    /* Get address of start of next page. This is done by moving to next page and then masking off bottom bits. 
       Example: 
       SF_MEM_MAX_PROGRAM_BYTES = 0x100
       address = 0x1234
       next_page_addr = (0x1234 + 0x100) & (~(0x100-1))
       next_page_addr = (0x1334) & (~(0xFF))
       next_page_addr = (0x1334) & (0xFFFFFF00)
       next_page_addr = 0x00001300
      */
    next_page_addr = (address + SF_MEM_MAX_PROGRAM_BYTES) & (~((uint32_t)SF_MEM_MAX_PROGRAM_BYTES-1));
    
    /* If we are programming over a page boundary then we will need to split this up. */
    if ((address + bytes_to_write) > next_page_addr)
    {
        /* We are cannot write over page boundary so only write up to boundary. */
        bytes_to_write = next_page_addr - address;
    }
#endif
    
    /* Allow memory to be modified */
    sf_write_unprotect(channel);

    while (size > 0)
    {
        /* Wait for WIP bit to clear */
        while((sf_read_status(channel) & SF_WIP_BIT_MASK) == 1);
        
        /* Send write enable command */
        sf_write_enable(channel);
        
        /* Initialize peripheral for SPI */
        sf_open(channel);
        
        /* This section writes data.  The first character is the write command */
        val[0] = SF_CMD_PAGE_PROGRAM;
        val[1] = (uint8_t)(address >> 16);
        val[2] = (uint8_t)(address >>  8);
        val[3] = (uint8_t)(address >>  0);
    	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
    	RSPI_POOLING
        /* Write data buffer to the flash */
       	R_RSPI_Write(spiflash_handle,spiflash_command,(uint8_t *)data,bytes_to_write);
    	RSPI_POOLING
        /* Close peripheral for SPI */
        sf_close(channel);

        /* Decrement bytes left to write. */
        size -= bytes_to_write;
        /* Increment data pointer. */
        data += bytes_to_write;
        /* Increment write address. */
        address += bytes_to_write;
        
        /* Update bytes_to_write for next loop iteration (if needed). */
        if (size > 0)
        {
            /* How many bytes to write this time? This will be either max program size or how many bytes are left. */
            bytes_to_write = (uint32_t)min(SF_MEM_MAX_PROGRAM_BYTES, size);
        }
    }
    
    /* Protect memory from modification */
    sf_write_protect(channel);

    /* Release lock on channel. */
    sf_unlock_channel(channel);
    
    return true;
}

/***********************************************************************************************************************
* Function Name: R_SF_ReadData
* Description  : Performs a read of the external flash to specified buffer.
* Arguments    : channel -
*                    Which SPI channel to use.
*                address -
*                    Target address to read from.
*                data - 
*                    Location to place read data.
*                size - 
*                    Amount of data to read.
* Return Value : true - 
*                    Success.
*                false -
*                    Failure.
***********************************************************************************************************************/
bool R_SF_ReadData (uint8_t channel, const uint32_t address, uint8_t * data, const uint32_t size)
{
    uint8_t val[4];

    /* Attempt to obtain lock for SPI channel. */
    if (false == sf_lock_channel(channel))
    {
        /* This channel is already being used. Try again later. */
        return false;
    }
    
    /* Initialize peripheral for SPI */
    sf_open(channel);
    
    /* This section reads back data.  The first character is the read command. */
    val[0] = SF_CMD_READ;
    val[1] = (uint8_t)(address >> 16);
    val[2] = (uint8_t)(address >>  8);
    val[3] = (uint8_t)(address >>  0);
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Read data. */
    R_RSPI_Read(spiflash_handle,spiflash_command,data,size);
	RSPI_POOLING
    /* Close peripheral for SPI */
    sf_close(channel);

    /* Release lock on channel. */
    sf_unlock_channel(channel);
            
    return true;
}

/***********************************************************************************************************************
* Function Name: R_SF_ReadStatus
* Description  : Reads flash status register and returns. This is just a wrapper function for sf_read_status(). This was
*                done because other API functions need to read the status register and they will already have locked
*                the RSPI channel. If the user wishes to read the status register themselves, then this function needs
*                to exist to obtain the lock to make sure this does not interfere with other RSPI operations.
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : Status register contents from SPI flash 
***********************************************************************************************************************/
uint8_t R_SF_ReadStatus (uint8_t channel)
{
    uint8_t status_reg;

    /* Attempt to obtain lock for SPI channel. */
    if (false == sf_lock_channel(channel))
    {
        /* This channel is already being used. Try again later. */
        return false;
    }
    
    /* Read status register. */
    status_reg = sf_read_status(channel);

    /* Release lock on channel. */
    sf_unlock_channel(channel);
    
    return status_reg;
}

/***********************************************************************************************************************
* Function Name: R_SF_ReadID
* Description  : Read identification of SPI Flash
* Arguments    : channel -
*                    Which SPI channel to use.
*                data - 
*                    Location to place read data.
*                size -
*                    Number of bytes to read.
* Return Value : true -
*                    Success.
*                false -
*                    Failure.
***********************************************************************************************************************/
bool R_SF_ReadID (uint8_t channel, uint8_t * data, uint32_t size)
{
    uint8_t val;

    /* Attempt to obtain lock for SPI channel. */
    if (false == sf_lock_channel(channel))
    {
        /* This channel is already being used. Try again later. */
        return false;
    }
    
    /* Initialize peripheral for SPI */
    sf_open(channel);
    
    val = SF_CMD_READ_ID;
    
    /* Send command. */
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Read data. */
    R_RSPI_Read(spiflash_handle,spiflash_command,data,size);
	RSPI_POOLING
    /* Close peripheral for SPI */
    sf_close(channel);

    /* Release lock on channel. */
    sf_unlock_channel(channel);
    
    return true;
}


/***********************************************************************************************************************
* Function Name: sf_open
* Description  : Performs steps to get ready for SPI flash communications (not initialization of SPI MCU peripheral)
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : true -
*                    Success.
*                false -
*                    Failure.
***********************************************************************************************************************/
static void sf_open (uint8_t channel)
{
    /* Use chip select to select SPI flash. */
	SPIFLASH_CS = 0;
}


/***********************************************************************************************************************
* Function Name: sf_close
* Description  : Performs steps to close SPI flash communications
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : none
***********************************************************************************************************************/
static void sf_close (uint8_t channel)
{
    /* Deselect SPI flash. */
	SPIFLASH_CS = 1;
}


/***********************************************************************************************************************
* Function Name: sf_lock_channel
* Description  : Make sure we have permission to use RSPI channel
* Arguments    : channel -
*                    Which SPI channel to use.
* Return Value : true -
*                    Permission granted, we can use the channel
*                false -
*                    Channel already in use, try again later
***********************************************************************************************************************/
static bool sf_lock_channel (uint8_t channel)
{
    return true;
}

/***********************************************************************************************************************
* Function Name: sf_unlock_channel
* Description  : Release lock on RSPI channel so other processes can use it.
* Arguments    : channel -
*                    Which SPI channel to unlock.
* Return Value : true -
*                    Lock released.
*                false -
*                    Lock not released because we did not have permission for lock. Error!
***********************************************************************************************************************/
static bool sf_unlock_channel (uint8_t channel)
{
    return true;
}


/***********************************************************************************************************************
* Function Name: sf_read_status
* Description  : Reads the status register on the SPI flash.
* Arguments    : channel -
*                    Which SPI channel to use..
* Return Value : Status register contents from SPI flash 
***********************************************************************************************************************/
static uint8_t sf_read_status (uint8_t channel)
{
    uint8_t val;

    /* Initialize peripheral for SPI */
    sf_open(channel);
    
    val = SF_CMD_READ_STATUS_REG;
    
    /* Send command. */
	R_RSPI_Write(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* Read register. */
    R_RSPI_Read(spiflash_handle,spiflash_command,&val,sizeof(val));
	RSPI_POOLING
    /* close peripheral for SPI */
    sf_close(channel);

    return val;
}

/***********************************************************************************************************************
* Function Name: R_SF_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
#pragma inline(R_SF_GetVersion)
uint32_t R_SF_GetVersion (void)
{
    /* These version macros are defined in r_spi_flash_if.h. */
    return ((((uint32_t)SPI_FLASH_VERSION_MAJOR) << 16) | (uint32_t)SPI_FLASH_VERSION_MINOR);
}

static void spiflash_callback(void * pdata)
{
	g_transfer_complete = true;
}


