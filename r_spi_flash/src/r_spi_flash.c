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

#include "r_spi_flash_if.h"

volatile bool g_transfer_complete = false;

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

static bool RSPI1_SendReceive( uint8_t const *pSrc, uint8_t *pDest, uint16_t usBytes);
static bool RSPI1_Read( uint8_t *pDest, uint16_t usBytes);
static bool RSPI1_Write( const uint8_t *pSrc, uint16_t usBytes);
static bool RSPI1_rx_buffer_full (void);

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
    
    RSPI1_Write(&val,sizeof(val));
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
    RSPI1_Write(val,sizeof(val));

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
    RSPI1_Write(val,sizeof(val));

    /* Close peripheral for SPI */
    sf_close(channel);
  
}

void R_SF_Init(uint8_t channel)
{
	bool lock;
    /* Initialize peripherals used for talking to SPI flash */
	//R_RSPI_Open(channel, &spiflash_config, spiflash_callback, &spiflash_handle);
	/* Get the looking */
    if(R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_RSPI1)))
    {
    	sf_close(1);
		R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
		MSTP(RSPI1) = 0; /* Init peripheral */
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
        IPR(RSPI1, SPRI1) = 3;
        IEN(RSPI1, SPRI1) = 0;
        IEN(RSPI1, SPTI1) = 0;
        IR(RSPI1, SPRI1) = 0 ;

        RSPI1.SPCR.BYTE = 0x00; /*Clock synchronous operation; Master mode*/
        /* Set RSPI bit rate (SPBR) */
        /* -Set baud rate to 24Mbps (48MHz / (2 * (0 + 1) * 2^0) ) = 24Mbps */
        RSPI1.SPBR = 0;

        /* Set RSPI data control register (SPDCR) */
        /* -SPDR is accessed in longwords (32 bits)
           -Transfer 1 frame at a time */
        RSPI1.SPDCR.BYTE = 0x20;

        /* Set RSPI control register 2 (SPCR2) */
        /* -Disable Idle interrupt */
        RSPI1.SPCR2.BYTE = 0x00;

        /* Set RSPI command register 0 (SPCMD0) */
        /* -MSB first
           -8 bits data length
           -SSL0 (handled manually)
           -Use bit rate % 1
            */
        RSPI1.SPCMD0.WORD = 0x0400;

        /* Set RSPI control register (SPCR) */
        /* -Clock synchronous operation (3-wire)
           -Full duplex operation
           -Master mode
           -SPTI and SPRI enabled in RSPI (have to check ICU also)
           -Enable RSPI function */
        RSPI1.SPCR.BYTE = 0xE9;


//        DMAC0.DMCNT.BIT.DTE = 0;
//        ICU.DMRSR0 = 43; /*RSPI1 RX*/
//        ICU.DMRSR1 = 44; /*RSPI1 TX*/
//
//        DMAC0.DMTMD.WORD = 0x0001;
//        DMAC0.DMAMD.WORD =0x4040;
//
//        DMAC1.DMTMD.WORD = 0x0001;
//        DMAC1.DMAMD.WORD =0x8040;
    }
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

        RSPI1_Write(val,sizeof(val));
    }
    else if(size == SF_ERASE_BULK)
    {
        /* Assign bulk erase command. */
        val[0] = SF_CMD_ERASE_BULK;

        /* bulk erase is one byte command */
        RSPI1_Write(val,1);
    }
    else if (size == SF_ERASE_BLOCK)
    {
        /* Assign sector erase command. */
        val[0] = SF_CMD_ERASE_BLOCK;

        /* Sector erase is one byte command and 24bit address */
        val[1] = (uint8_t)(address >> 16);
        val[2] = (uint8_t)(address >>  8);
        val[3] = (uint8_t)(address >>  0);

        RSPI1_Write(val,sizeof(val));
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
        RSPI1_Write(&val,sizeof(val));

        /* Write data buffer to the flash */
        RSPI1_Write((uint8_t *)data,bytes_to_write);

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
    RSPI1_Write(val,sizeof(val));

    /* Read data. */
	RSPI1_Read(data,size);

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
    RSPI1_Write(&val,sizeof(val));

    /* Read data. */
	RSPI1_Read(data,size);

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
    RSPI1_Write(&val,sizeof(val));

    /* Read register. */
	RSPI1_Read(&val,sizeof(val));

    /* close peripheral for SPI */
    sf_close(channel);

    return val;
}

/***********************************************************************************************************************
* Function Name: RSPI1_SendReceive
* Description  : Performs SPI transfers. Can read and write at the same time.
* Arguments    :
*                pSrc -
*                    pointer to data buffer with data to be transmitted.
*                    If NULL, const 0xFF as source.
*                pDest -
*                    pointer to location to put the received data (can be same as pSrc if desired).
*                    If NULL, receive data discarded.
*                usBytes -
*                    number of bytes to be sent/received
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
static bool RSPI1_SendReceive( uint8_t const *pSrc,
                        uint8_t *pDest,
                        uint16_t usBytes)
{
    uint16_t byte_count;
    volatile uint32_t temp;

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while (RSPI1.SPSR.BIT.IDLNF) ;

        /* If just reading then transmit 0xFF */
        RSPI1.SPDR.LONG = (pSrc == NULL) ? 0xFF : pSrc[byte_count];

        while (false == RSPI1_rx_buffer_full())
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }

        /* Read received data.  If transmit only, then ignore it */
        if (pDest == NULL)
        {
            temp = RSPI1.SPDR.LONG;
        }
        else
        {
            pDest[byte_count] = (uint8_t) (RSPI1.SPDR.LONG & 0xFF);
        }

    }

    return true;
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Read
* Description  : Reads data using RSPI
* Arguments    : channel -
*                    Which channel to use
*                pDest -
*                    Pointer to location to put the received data.
*                    Returned value will be incremented by number of bytes received.
*                usBytes -
*                    number of bytes to be received
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
static bool RSPI1_Read( uint8_t *pDest,
                 uint16_t usBytes)
{
    uint16_t byte_count;
    volatile uint32_t temp;

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while (RSPI1.SPSR.BIT.IDLNF) ;

        /* If just reading then transmit 0xFF */
        RSPI1.SPDR.LONG = 0xFFFFFFFF ;

        while (false == RSPI1_rx_buffer_full())
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }

        /* Read received data.  If transmit only, then ignore it */
        pDest[byte_count] = (uint8_t) (RSPI1.SPDR.LONG & 0xFF);
    }

    return true;
}


/***********************************************************************************************************************
* Function Name: R_RSPI_Write
* Description  : Write to a SPI device
* Arguments    : channel -
*                    Which channel to use
*                pSrc -
*                    Pointer to data buffer with data to be transmitted.
*                    Returned value will be incremented by number of attempted writes.
*                usBytes -
*                    Number of bytes to be sent
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
static bool RSPI1_Write( const uint8_t *pSrc, uint16_t usBytes)
{
    uint16_t byte_count;
    volatile uint32_t temp;

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while (RSPI1.SPSR.BIT.IDLNF) ;

        /* If just reading then transmit 0xFF */
        RSPI1.SPDR.LONG = pSrc[byte_count];

        while (false == RSPI1_rx_buffer_full())
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }

        /* Read received data.  If transmit only, then ignore it */
        temp = RSPI1.SPDR.LONG;
    }
//    DMAC0.DMSAR = (uint32_t *)&RSPI1.SPDR.LONG;
//    DMAC0.DMDAR = (uint32_t *)pSrc;
//    DMAC0.DMCRA = usBytes;
//
//    DMAC1.DMSAR = (uint32_t *)pSrc;
//    DMAC1.DMDAR = (uint32_t *)&RSPI1.SPDR.LONG;
//    DMAC1.DMCRA = usBytes;
//    temp1 = RSPI1.SPDR.LONG;
    return true;
}

/***********************************************************************************************************************
* Function Name: rspi_rx_buffer_full
* Description  : Returns whether the receive buffer full flag is set for a RSPI channel. Clear flag after read.
* Arguments    : channel -
*                    Which channel to use.
* Return Value : true -
*                    Flag is set.
*                false -
*                    Flag is not set.
***********************************************************************************************************************/
static bool RSPI1_rx_buffer_full (void)
{
    bool flag_set = false;

        if (1 == IR(RSPI1, SPRI1))
        {
            /* Clear bit. */
            IR(RSPI1, SPRI1) = 0;

            flag_set = true;
        }
    return flag_set;
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


