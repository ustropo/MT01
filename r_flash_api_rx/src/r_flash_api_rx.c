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
* File Name    : r_flash_api_rx.c
* Device       : RX600 Series, RX200 Series
* Tool-Chain   : RX Family C Compiler
* H/W Platform : RSKRX62N, RSKRX610, YRDKRX62N, RSKRX630, RSKRX63N, RSKRX62T,
*                YRDKRX63N, RSKRX63T, RSKRX62G, RSKRX210
* Description  : Simple Flash API for RX
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
*                              input arguments and moved flash_init() to
*                              enter_pe_mode() function.
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

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Intrinsic functions of MCU */
#include <machine.h>

/* Allocate flash block array here. This is required before including
   r_flash_api_rx.h */
#define FLASH_BLOCKS_DECLARE

/* Configuration options for the Flash API. This is also included in 
   r_flash_api_rx.h and would normally not need to be included separately.
   It is included separately here so that the decision can be made to use
   the r_bsp package or not. */
#include "r_flash_api_rx_config.h"
/* Get board and MCU definitions. */
#include "platform.h"

#if !defined(R_BSP_VERSION_MAJOR)
#include "iodefine.h"
#include "mcu_info.h"
#endif

/* Function prototypes and device specific info needed for Flash API */
#include "r_flash_api_rx_if.h"
/* Information needed for Flash API. */
#include "r_flash_api_rx_private.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Define read mode macro */
#define READ_MODE               0
/* Define ROM PE mode macro */
#define ROM_PE_MODE             1
/* Define data flash PE mode macro */
#define FLD_PE_MODE             2
/* The number of ICLK ticks needed for 35us delay are calculated below */
#define WAIT_TRESW              (35*(BSP_ICLK_HZ/1000000))                  
/* The number of ICLK ticks needed for 10us delay are calculated below */
#define WAIT_T10USEC            (10*(BSP_ICLK_HZ/1000000))      
/* The number of loops to wait for FENTRYR timeout. */
#define FLASH_FENTRYR_TIMEOUT   (4)

/******************************************************************************
Typedef definitions
******************************************************************************/
/* These typedefs are used for guaranteeing correct accesses to memory. When 
   working with the FCU sometimes byte or word accesses are required. */
typedef __evenaccess volatile uint8_t  * FCU_BYTE_PTR;
typedef __evenaccess volatile uint16_t * FCU_WORD_PTR;
typedef __evenaccess volatile uint32_t * FCU_LONG_PTR;

/* These flash states are used internal for locking purposes. */
typedef enum
{ 
    FLASH_READY, 
    FLASH_ERASING, 
    FLASH_WRITING, 
    FLASH_BLANKCHECK, 
    FLASH_LOCK_BIT  
} flash_states_t;

/******************************************************************************
Exported global variables 
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Signals whether FCU firmware has been transferred to the FCU RAM 
   0 : No, 1 : Yes */
static uint8_t        g_fcu_transfer_complete = 0;
/* Valid values are 'READ_MODE','ROM_PE_MODE' or 'FLD_PE_MODE' */
static uint8_t        g_current_mode;        
/* Stores whether the peripheral clock notification command has 
   been executed */
static uint8_t        g_fcu_pclk_command = 0;   
/* States for flash operations */
static flash_states_t g_flash_state;

#ifndef FLASH_API_RX_CFG_IGNORE_LOCK_BITS                          
/* Determines whether lock bit protection is used when programming/erasing */
static uint8_t g_lock_bit_protection = true;                          
#endif

/* Only allocate these arrays if needed. */
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH   
/* Used for holding data when DF to DF write is peformed */
    #if defined(ROM_PROGRAM_SIZE_LARGE)
static uint8_t g_temp_array[ROM_PROGRAM_SIZE_LARGE];
    #else
static uint8_t g_temp_array[ROM_PROGRAM_SIZE];
    #endif
/* Used for telling whether a DF to DF or ROM to ROM operation is on-going */
static uint8_t g_flash_to_flash_op;
#endif

#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO) || defined(FLASH_API_RX_CFG_ROM_BGO)
/* How many bytes are left to program */
static uint32_t  g_bgo_bytes;
/* Where we are programming or erasing */
static uint32_t  g_bgo_flash_addr;
/* Where the data is that we are programming */
static uint32_t  g_bgo_buffer_addr;
#endif

/* Flash intialisation function prototype */
static uint8_t  flash_init(void);
/* Enter PE mode function prototype */
static uint8_t  enter_pe_mode(uint32_t flash_addr);
/* Exit PE mode function prototype */
static void     exit_pe_mode(uint32_t flash_addr);
/* ROM write function prototype */
static uint8_t  rom_write(uint32_t address, uint32_t data, uint32_t size);
/* Data flash write function prototype */
static uint8_t  data_flash_write(uint32_t address, uint32_t data, uint32_t size);
/* Data flash status clear function prototype */
static void     data_flash_status_clear(void);
/* Notify peripheral clock function prototype */
static uint8_t  notify_peripheral_clock(FCU_BYTE_PTR flash_addr);
/* FCU reset function prototype */
static void     flash_reset(void);
/* Used to grab flash state */
static uint8_t  flash_grab_state(flash_states_t new_state);
/* Used to release flash state */
static void     flash_release_state(void);
/* Used to issue an erase command to the FCU */
static uint8_t  flash_erase_command(FCU_BYTE_PTR const erase_addr);
/* Used to get largest programming size that can be used. */
static uint32_t flash_get_program_size(uint32_t bytes, uint32_t flash_addr);

/******************************************************************************
* Function Name: flash_init
* Description  : Initializes the FCU peripheral block.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t flash_init (void)
{
#if defined(FCU_RAM_INIT_REQUIRED)    
    /* Declare source and destination pointers */
    uint32_t * src, * dst;  
    /* Declare iteration counter variable */
    uint16_t i;
#endif    
    /* Timeout counter. */
    volatile int32_t  wait_cnt;
        
    /* Disable FCU interrupts in FCU block */
    FLASH.FAEINT.BIT.ROMAEIE = 0;
    FLASH.FAEINT.BIT.CMDLKIE = 0;
    FLASH.FAEINT.BIT.DFLAEIE = 0;
    FLASH.FAEINT.BIT.DFLRPEIE = 0;
    FLASH.FAEINT.BIT.DFLWPEIE = 0;
    
    /* Disable FCU interrupts in ICU */
    
    /* Disable flash interface error (FIFERR) */
    IPR(FCU, FIFERR) = 0;
    IEN(FCU, FIFERR) = 0;
        
#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO) || defined(FLASH_API_RX_CFG_ROM_BGO)
    /* Enable flash ready interrupt (FRDYI) */    
    /* Make sure IPL is high enough that interrupt will trigger */
    IPR(FCU, FRDYI) = FLASH_API_RX_CFG_FLASH_READY_IPL;
    IEN(FCU, FRDYI) = 1;
#else        
    /* Disable flash ready interrupt (FRDYI) */
    IPR(FCU, FRDYI) = 0;
    IEN(FCU, FRDYI) = 0;
#endif    

    /* Transfer Firmware to the FCU RAM. To use FCU commands, the FCU firmware
        must be stored in the FCU RAM. */
    
    /* Before writing data to the FCU RAM, clear FENTRYR to stop the FCU. */
    if(FLASH.FENTRYR.WORD != 0x0000)
    {
        /* Disable the FCU from accepting commands - Clear both the
           FENTRY0(ROM) and FENTRYD(Data Flash) bits to 0 */
        FLASH.FENTRYR.WORD = 0xAA00;

        /* Initialize timeout for FENTRYR being written. */
        wait_cnt = FLASH_FENTRYR_TIMEOUT;

        /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
           of the FENTRYR register is not retained and is read as 0x00. */
        while(0x0000 != FLASH.FENTRYR.WORD)
        {
            /* Wait until FENTRYR is 0 unless timeout occurs. */
            if (wait_cnt-- <= 0)
            {
                /* This should not happen. FENTRYR getting written to 0 should
                   only take 2-4 PCLK cycles. */
                return FLASH_FAILURE;
            }
        }
    }

#if defined(FCU_RAM_INIT_REQUIRED)
    /* Enable the FCU RAM */
    FLASH.FCURAME.WORD = 0xC401;
    
    /* Copies the FCU firmware to the FCU RAM.
       Source: H'FEFFE000 to H'FF00000 (FCU firmware area)
       Destination: H'007F8000 to H'007FA000 (FCU RAM area) */
    /* Set source pointer */
    src = (uint32_t *)FCU_PRG_TOP;
    
    /* Set destination pointer */
    dst = (uint32_t *)FCU_RAM_TOP;
    
    /* Iterate for loop to copy the FCU firmware */
    for( i=0; i<(FCU_RAM_SIZE/4); i++)
    {
        /* Copy data from the source to the destination pointer */
        *dst = *src;
        
        /* Increment the source and destination pointers */
        src++;
        dst++;
    }
#endif

    /* FCU firmware transfer complete, set the flag to 1 */
    g_fcu_transfer_complete = 1;

    /* Return no errors */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  flash_init
******************************************************************************/

#ifdef FLASH_API_RX_CFG_COPY_CODE_BY_API
/******************************************************************************
* Function Name: R_FlashCodeCopy
* Description  : Copies Flash API code from ROM to RAM. This function needs to
*                be called before any program/erase functions. This function
*                does not need to be used when 
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_FlashCodeCopy (void)
{
#ifdef FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING
    /* Pointer to beginning of RAM section where API code will be copied to. */
    uint8_t * p_ram_section;
    /* Pointer to beginning of ROM section with API code. */
    uint8_t * p_rom_section;
    /* Number of bytes copied. */
    uint32_t  bytes_copied;

    /* Initialize RAM section pointer. */
    p_ram_section = (uint8_t *)__sectop("RPFRAM");

    /* Initialize ROM section pointer. */
    p_rom_section = (uint8_t *)__sectop("PFRAM");

    /* Copy over code from ROM to RAM. */
    for (bytes_copied = 0; bytes_copied < __secsize("PFRAM"); bytes_copied++)
    {
        /* Copy over data 1 byte at a time. */
        p_ram_section[bytes_copied] = p_rom_section[bytes_copied];
    }
#endif
}
/******************************************************************************
End of function  R_FlashCodeCopy
******************************************************************************/
#endif

/******************************************************************************
* Function Name: R_FlashGetVersion
* Description  : Returns the current version of this module. The version number
*                is encoded where the top 2 bytes are the major version number 
*                and the bottom 2 bytes are the minor version number. For 
*                example, Version 4.25 would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
******************************************************************************/
#pragma inline(R_FlashGetVersion)
uint32_t R_FlashGetVersion (void)
{
    return ((((uint32_t)RX_FLASH_API_VERSION_MAJOR) << 16) | ((uint32_t)RX_FLASH_API_VERSION_MINOR));
}
/******************************************************************************
End of function  R_FlashGetVersion
******************************************************************************/

/******************************************************************************
* Function Name: data_flash_status_clear
* Description  : Clear the status of the Data Flash operation.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void data_flash_status_clear (void)
{
    /* Declare temporaty pointer */
    FCU_BYTE_PTR ptrb;

    /* Set pointer to Data Flash to issue a FCU command if needed */
    ptrb = (FCU_BYTE_PTR)(DF_ADDRESS);

    /* Check to see if an error has occurred with the FCU.  If set, then
       issue a status clear command to bring the FCU out of the 
       command-locked state */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)  
    {
        /* FASTAT must be set to 0x10 before the status clear command
           can be successfully issued  */
        if(FLASH.FASTAT.BYTE != 0x10)
        {
            /* Set the FASTAT register to 0x10 so that a status clear
               command can be issued */
            FLASH.FASTAT.BYTE = 0x10;
        }
    }

    /* Issue a status clear command to the FCU */
    *ptrb = 0x50;
}
/******************************************************************************
End of function  data_flash_status_clear
******************************************************************************/

/******************************************************************************
* Function Name: data_flash_write
* Description  : Write either bytes to Data Flash area.
*                NOTE: This function does not have to execute from in RAM.
* Arguments    : address - 
*                    The address (in the Data Flash programming area) 
*                    to write the data to   
*                data - 
*                    The address of the data to write
*                size - 
*                    The size of the data to write. Must be set to 
*                    either DF_PROGRAM_SIZE_LARGE or DF_PROGRAM_SIZE_SMALL.
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t data_flash_write (uint32_t address, uint32_t data, uint32_t size)
{
    /* Declare wait counter variable */
    volatile int32_t wait_cnt;
    
    /* Define loop iteration count variable */
    volatile uint32_t n = 0;
    
    /* Check data size is valid */   
#if defined(DF_PROGRAM_SIZE_LARGE)
    if((DF_PROGRAM_SIZE_LARGE == size) || (DF_PROGRAM_SIZE_SMALL == size))   
#else 
    if(DF_PROGRAM_SIZE_SMALL == size)    
#endif
    {
        /* Perform bit shift since 2 bytes are written at a time */
        size = (uint8_t)(size >> 1);
        
        /* Send command to data flash area */
        *(FCU_BYTE_PTR)DF_ADDRESS = 0xE8;

        /* Specify data transfer size to data flash area */
        *(FCU_BYTE_PTR)DF_ADDRESS = size;    
        
        /* Iterate through the number of data bytes */
        while(n++ < size)
        {
            /* Copy data from source address to destination area */
            *(FCU_WORD_PTR)address = *(uint16_t *)data;
        
            /* Increment data address by two bytes */
            data += 2;
        }

        /* Write the final FCU command for programming */
        *(FCU_BYTE_PTR)(DF_ADDRESS) = 0xD0;
        
#ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
        /* Program is ongoing, return */
        return FLASH_SUCCESS;
#endif

        /* Set the wait counter with timeout value */
        wait_cnt = WAIT_MAX_DF_WRITE;

        /* Check if FCU has completed its last operation */
        while(FLASH.FSTATR0.BIT.FRDY == 0)
        {   
            /* Decrement the wait counter */
            wait_cnt--;
        
            /* Check if the wait counter has reached zero */
            if(wait_cnt == 0)
            {            
                /* Maximum time for writing a block has passed, 
                   operation failed, reset FCU */
                flash_reset();
                
                /* Return FLASH_FAILURE, operation failure */
                return FLASH_FAILURE;
            }
        }

        /* Check for illegal command or programming errors */
        if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR == 1))
        {        
            /* Return FLASH_FAILURE, operation failure */
            return FLASH_FAILURE;
        }                
    }
    /* Data size is invalid */
    else
    {
        /*Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }
 
    /* Return FLASH_SUCCESS, operation success */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  data_flash_write
******************************************************************************/

/* If FLASH_API_RX_CFG_ROM_BGO is enabled then the following functions (before
   the next #pragma section) also need to be in RAM in the case that the user 
   calls them during a ROM operation. */
#ifdef FLASH_API_RX_CFG_ROM_BGO
#pragma section FRAM
#endif

/******************************************************************************
* Function Name: R_FlashDataAreaBlankCheck
* Description  : Performs a blank check on a specified data flash block
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E 
*                       operation.
*                NOTE2: RX610 & RX62x support block and 8 byte checks
*                       RX630/631/63N support block and 2 byte checks
* Arguments    : address - 
*                    The address to check if is blank.
*                    If the parameter 'size'=='BLANK_CHECK_8_BYTE', 
*                    this should be set to an 8-byte address boundary.
*                    If the parameter 'size'=='BLANK_CHECK_ENTIRE_BLOCK', 
*                    this should be set to a defined Data Block Number 
*                    ('BLOCK_DB0', 'BLOCK_DB1', etc...) or an address 
*                    in the data flash block.  Either option will work.
*                    If the parameter 'size'=='BLANK_CHECK_2_BYTE',
*                    this should be set to a 2 byte address boundary.
*                size - 
*                    This specifies if you are checking an 8-byte location,
*                    2-byte location, or an entire block. You must set this 
*                    to either 'BLANK_CHECK_8_BYTE', 'BLANK_CHECK_2_BYTE', 
*                    or 'BLANK_CHECK_ENTIRE_BLOCK'.
* Return Value : FLASH_BLANK -
*                    (2 or 8 Byte check or non-BGO) Blank
*                    (Entire Block & BGO) Blank check operation started
*                FLASH_NOT_BLANK -
*                    Not Blank
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_BUSY - 
*                    Another flash operation is in progress
*                FLASH_ERROR_ADDRESS -
*                    Invalid address
*                FLASH_ERROR_BYTES - 
*                    Incorrect 'size' was submitted
******************************************************************************/
uint8_t R_FlashDataAreaBlankCheck (uint32_t address, uint8_t size)
{
    /* Declare data flash pointer */
    FCU_BYTE_PTR ptrb;
    
    /* Declare result container variable */
    uint8_t result;
    
    /* Declare wait counter variable */
    volatile int32_t wait_cnt;

    /* Check to make sure address is valid. */
    if( ((address > (DF_NUM_BLOCKS+BLOCK_DB0)) && (address < DF_ADDRESS))
        || (address > DF_ADDRESS + BSP_DATA_FLASH_SIZE_BYTES) )
    {
        /* Address is not a valid DF address or block number */
        return FLASH_ERROR_ADDRESS;
    }

    /* Check to make sure 'size' parameter is valid */
    if( (size != BLANK_CHECK_ENTIRE_BLOCK) 
#if defined(BLANK_CHECK_8_BYTE)
        && (size != BLANK_CHECK_8_BYTE) 
#elif defined(BLANK_CHECK_2_BYTE)
        && (size != BLANK_CHECK_2_BYTE) 
#endif
      )
    {
        /* 'size' parameter is not valid. */
        return FLASH_ERROR_BYTES;
    }
        
    /* Attempt to grab state */
    if( flash_grab_state(FLASH_BLANKCHECK) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Set current FCU mode to Data Flash PE Mode */
    g_current_mode = FLD_PE_MODE;

    /* Enter Data Flash PE mode in the FCU */
    if( enter_pe_mode(address) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(address);

        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }

    /* Set  bit FRDMD (bit 4) in FMODR to 1 */
    FLASH.FMODR.BIT.FRDMD = 1;

#if   defined(BLANK_CHECK_8_BYTE)
    /* Check if 8 byte size has been passed */
    if(size == BLANK_CHECK_8_BYTE)
    {
        #ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
        /* Disable FCU ready interrupt for 8 byte checks */
        FLASH.FRDYIE.BIT.FRDYIE = 0;
        #endif        
        
        /* Set data flash pointer to beginning of the memory block */
        ptrb = (FCU_BYTE_PTR)(address & DF_MASK);   
        
        /* Check if the next 8 bytes are blank
           Bits BCADR to the address of the 8-byte location to check.         
           Set bit BCSIZE in EEPBCCNT to 0. */
        FLASH.DFLBCCNT.WORD = (uint16_t)(address & (DF_ERASE_BLOCK_SIZE-8));
    }
#elif defined(BLANK_CHECK_2_BYTE)
    /* Check if 2 byte size has been passed */
    if(size == BLANK_CHECK_2_BYTE)
    {
        #ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
        /* Disable FCU ready interrupt for 2 byte checks */
        FLASH.FRDYIE.BIT.FRDYIE = 0;
        #endif        
        
        /* Set data flash pointer to beginning of the memory block */
        ptrb = (FCU_BYTE_PTR)(address & DF_MASK);   
        
        /* Check if the next 2 bytes are blank
           Bits BCADR to the address of the 2-byte location to check. 
           Set bit BCSIZE in EEPBCCNT to 0. */
        FLASH.DFLBCCNT.WORD = (uint16_t)(address & (DF_BLOCK_SIZE_LARGE-2));
    }
#endif 
    /* Check entire data block */
    else
    {
        /* Check to see if user sent in a data block number or the 
           address.  The function description tells the user to send
           in the block number but in some early examples the address
           was used. To make this work with both versions we detect which
           is sent in below and make it work either way. */
        if( address < (DF_NUM_BLOCKS+BLOCK_DB0) )
        {
            /* A data block number was sent in */
            ptrb = (FCU_BYTE_PTR)g_flash_BlockAddresses[address];
        }           
        else
        {
            /* Any address in the erasure block */
            ptrb = (FCU_BYTE_PTR)address;
        }
        
        /* Check if the entire block is blank
           Set bit BCSIZE in EEPBCCNT to 1. */
        FLASH.DFLBCCNT.BIT.BCSIZE = 1;
    }

    /* Send commands to FCU */
    *ptrb = 0x71;
    *ptrb = 0xD0;
    
#ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
    /* Only using BGO on block blank checks */
    if( size == BLANK_CHECK_ENTIRE_BLOCK )
    {
        /* Set global address in case we have error and need to reset FCU. */
        g_bgo_flash_addr = address;

        /* Return, check result later in ISR */
        return FLASH_BLANK;
    }
#endif    

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_BLANK_CHECK;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {    
            /* Maximum timeout duration for writing to ROM has elapsed - 
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_FAILURE;
        }
    }

    /* Reset the FRDMD bit back to 0 */
    FLASH.FMODR.BIT.FRDMD = 0x00;

    /* Check if the 'ILGERR' was set during the command */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)  /* Check 'ILGERR' bit */
    {
        /* Take the FCU out of PE mode */
        exit_pe_mode(address);
        
        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }

    /* (Read the 'BCST' bit (bit 0) in the 'DFLBCSTAT' register
       0=blank, 1=not blank */
    result = (uint8_t)FLASH.DFLBCSTAT.BIT.BCST; 

    /* Take the FCU out of PE mode */
    exit_pe_mode(address);
    
    /* Release state */
    flash_release_state();
    
    /* Return (Not Blank/Blank), operation successful */
    if( result == 0 ) {
        /* Block was blank */
        return FLASH_BLANK;
    }
    else
    {
        /* Block was not blank */
        return FLASH_NOT_BLANK;
    }    
}
/******************************************************************************
End of function  R_FlashDataAreaBlankCheck
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashDataAreaAccess
* Description  : This function is used to allow read and program permissions 
*                to the Data Flash areas. 
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E 
*                       operation.
* Arguments    : read_en_mask - 
*                    Bitmasked value. Bits 0-3 represents each Data 
*                    Blocks 0-3 (respectively).
*                    '0'=no Read access. 
*                    '1'=Allows Read by CPU
*                write_en_mask - 
*                    Bitmasked value. Bits 0-3 represents each Data 
*                    Blocks 0-3 (respectively).
*                    '0'=no Erase/Write access. 
*                    '1'=Allows Erase/Write by FCU
* Return Value : none
******************************************************************************/
void R_FlashDataAreaAccess (uint16_t read_en_mask, uint16_t write_en_mask)
{
    /* Used for making sure select bits are not set. */
    uint16_t temp_mask = 0x00FF;

#if   defined(BSP_MCU_RX610)
    
    /* Set Read access for the Data Flash blocks */
    FLASH.DFLRE.WORD = (uint16_t)(0x2D00 | (read_en_mask & 0x00FF));

    /* Set Erase/Program access for the Data Flash blocks */
    FLASH.DFLWE.WORD = (uint16_t)(0x1E00 | (write_en_mask & 0x00FF));
        
#else

    #if BSP_DATA_FLASH_SIZE_BYTES == 8192
    /* Mask off bits 4-7 to make sure they are 0. */
    temp_mask = 0x000F;
    #endif

    /* Set Read access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLRE0.WORD = (uint16_t)(0x2D00 | (read_en_mask & temp_mask));

    /* Set Erase/Program access for the Data Flash blocks DB0-DB7 */
    FLASH.DFLWE0.WORD = (uint16_t)(0x1E00 |  (write_en_mask & temp_mask));

    #if BSP_DATA_FLASH_SIZE_BYTES > 16384
    /* Set Read access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLRE1.WORD = (uint16_t)(0xD200 | ((read_en_mask >> 8) & 0x00FF));

    /* Set Erase/Program access for the Data Flash blocks DB8-DB15 */
    FLASH.DFLWE1.WORD = (uint16_t)(0xE100 |  ((write_en_mask >> 8) & 0x00FF));
    #endif
    
#endif
}
/******************************************************************************
End of function  R_FlashDataAreaAccess
******************************************************************************/


/******************************************************************************
* Function Name: R_FlashGetStatus
* Description  : Returns the current state of the flash
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E
*                       operation.
* Arguments    : none
* Return Value : FLASH_SUCCESS -
*                    Flash is ready to use
*                FLASH_BUSY - 
*                    Flash is busy with another operation
******************************************************************************/
uint8_t R_FlashGetStatus (void)
{
    /* Return flash status */
    if( g_flash_state == FLASH_READY )
    {
        return FLASH_SUCCESS;
    }
    else
    {
        return FLASH_BUSY;
    }
}
/******************************************************************************
End of function  R_FlashGetStatus
******************************************************************************/


/******************************************************************************
* Function Name: flash_grab_state
* Description  : Attempt to grab the flash state to perform an operation
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E 
*                       operation.
* Arguments    : new_state -
*                    Which state to attempt to transition to
* Return Value : FLASH_SUCCESS - 
*                    State was grabbed
*                FLASH_BUSY - 
*                    Flash is busy with another operation
******************************************************************************/
static uint8_t flash_grab_state (flash_states_t new_state)
{   
    /* Check to see if lock was successfully taken */
    if (R_BSP_HardwareLock(BSP_LOCK_FLASH) == true)
    {
        /* Lock taken, we can change state */
        g_flash_state = new_state;
        
        /* Return success */
        return FLASH_SUCCESS;
    }
    else
    {
        /* Another operation is on-going */
        return FLASH_BUSY;
    }
}
/******************************************************************************
End of function  flash_grab_state
******************************************************************************/

/******************************************************************************
* Function Name: flash_release_state
* Description  : Release state so another flash operation can take place
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E 
*                       operation.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void flash_release_state (void)
{
    /* Set current FCU mode to READ */
    g_current_mode = READ_MODE;            
    
    /* Done with programming */
    g_flash_state = FLASH_READY; 

    /* Release hold on lock */
    R_BSP_HardwareUnlock(BSP_LOCK_FLASH);
}
/******************************************************************************
End of function  flash_release_state
******************************************************************************/

#ifndef  FLASH_API_RX_CFG_IGNORE_LOCK_BITS
/******************************************************************************
* Function Name: R_FlashSetLockBitProtection
* Description  : Enables or disables lock bit protection.
*                NOTE1: This function does not have to execute from in RAM. It
*                       must be in RAM though if FLASH_API_RX_CFG_ROM_BGO is 
*                       enabled and this function is called during a ROM P/E 
*                       operation.
* Arguments    : lock_bit - 
*                    Boolean, whether to enable or disable lock bit 
*                    protection (true = enabled, false = disabled)
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_BUSY - 
*                    Another flash operation is in progress
******************************************************************************/
uint8_t R_FlashSetLockBitProtection (uint32_t lock_bit)
{        
    /* Attempt to grab state */;
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }    
    
    /* Enable or disable? */
    g_lock_bit_protection = lock_bit;
        
    /* Release state */
    flash_release_state();

    /* Return success */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  R_FlashSetLockBitProtection
******************************************************************************/
#endif

#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO) || defined(FLASH_API_RX_CFG_ROM_BGO)
/******************************************************************************
* Function Name: flash_ready_isr
* Description  : ISR that is called when FCU is done with flash operation
*                NOTE: This function MUST execute from RAM only when 
*                      FLASH_API_RX_CFG_ROM_BGO is enabled.
* Arguments    : none
* Return Value : none
******************************************************************************/
#pragma interrupt flash_ready_isr(vect=VECT(FCU, FRDYI))
void flash_ready_isr (void)
{
    /* Local variables */    
    uint32_t num_byte_to_write;
    uint8_t  ret;
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH
    uint32_t i;
#endif       
    
    /* Check for any errors */
    if( (FLASH.FSTATR0.BIT.ILGLERR == 1) || 
        (FLASH.FSTATR0.BIT.ERSERR  == 1) ||
        (FLASH.FSTATR0.BIT.PRGERR == 1) )
    {
        /* Leave Program/Erase Mode and clear any error flags */
        exit_pe_mode(g_bgo_flash_addr);         
        
        /* Release flash state */
        flash_release_state();
    
        /* Operation failure, use callback function to alert user */
        FlashError();
        
        /* Exit ISR */
        return;
    }
    
    /* Check state and see if anything else needs to be done */
    if( g_flash_state == FLASH_ERASING )
    {
        /* Erase is done */ 
#if defined(DF_GROUPED_BLOCKS)
        /* If we are erasing data flash then we need to see if all requested
           blocks are erased. */
        if( FLD_PE_MODE == g_current_mode )
        {
            /* Check to see if there are more bytes to erase. */
            if( 0 < g_bgo_bytes ) {
                
                /* Send FCU command to erase block */
                ret = flash_erase_command((FCU_BYTE_PTR)g_bgo_flash_addr);

                /* Advance pointer to next block */
                g_bgo_flash_addr += DF_ERASE_BLOCK_SIZE;

                /* Subtract off bytes erased */
                g_bgo_bytes -= DF_ERASE_BLOCK_SIZE;

                /* Only continue if last command was successful */                
                if( ret == FLASH_SUCCESS )
                {
                    /* Exit ISR */
                    return;
                }
            }
        }
#endif

        /* Leave Program/Erase Mode */
        exit_pe_mode(g_bgo_flash_addr);

        /* Release flash state */
        flash_release_state();
        
        /* Flash operation finished callback function */
        FlashEraseDone(); 
    }
#ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
    else if( g_flash_state == FLASH_BLANKCHECK )
    {
        /* Blank check finished, check result */
        
        /* Reset the FRDMD bit back to 0 */
        FLASH.FMODR.BIT.FRDMD = 0x00;

        /* (Read the 'BCST' bit (bit 0) in the 'DFLBCSTAT' register
           0=blank, 1=not blank */
        ret = FLASH.DFLBCSTAT.BIT.BCST; 

        /* Take the FCU out of PE mode */
        exit_pe_mode(g_bgo_flash_addr);
    
        /* Release flash state */
        flash_release_state();
        
        /* Return result, 0 means blank, 1 means not blank */
        if(ret == 0) {
            FlashBlankCheckDone(FLASH_BLANK);        
        }
        else
        {
            FlashBlankCheckDone(FLASH_NOT_BLANK);        
        }
    }
#endif
    else if( g_flash_state == FLASH_WRITING )
    {        
        /* Iterate while there are still bytes remaining to write */
        if( g_bgo_bytes > 0 )
        {
            /* Get maximum programming size that can currently be used. */
            num_byte_to_write = flash_get_program_size(g_bgo_bytes, g_bgo_flash_addr);

#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH
            /* Check to see if we need to buffer more data */
            if( g_flash_to_flash_op == 1 )
            {
                /* We must leave PE mode to transfer next buffer to RAM */
                exit_pe_mode(g_bgo_flash_addr);                
                
                /* Copy over next bytes to write */
                for(i = 0; i < num_byte_to_write; i++)
                {
                    /* Copy over each byte */
                    g_temp_array[i] = *((uint8_t *)(g_bgo_buffer_addr+i));             
                }

                /* Re-enter PE mode, check if operation is successful */
                if( enter_pe_mode(g_bgo_flash_addr) != FLASH_SUCCESS)
                {
                    /* Leave Program/Erase Mode and clear any error flags */
                    exit_pe_mode(g_bgo_flash_addr);         
        
                    /* Release flash state */
                    flash_release_state();
    
                    /* Operation failure, use callback function to alert user */
                    FlashError();
        
                    /* Exit ISR */
                    return;
                }                
            }
#endif                          

            /* Are we writing DF or ROM? */
            if( g_current_mode == FLD_PE_MODE )
            {
                /* Writing DF */
                /* Call the Programming function again for next bytes */
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH           
                if( g_flash_to_flash_op == 1 )
                {
                    ret = data_flash_write( g_bgo_flash_addr,
                                            (uint32_t)&g_temp_array[0],
                                            num_byte_to_write);                
                }
                else
                {
                    ret = data_flash_write( g_bgo_flash_addr, 
                                            g_bgo_buffer_addr,
                                            num_byte_to_write);                
                }
#else                           
                ret = data_flash_write( g_bgo_flash_addr, 
                                        g_bgo_buffer_addr,
                                        num_byte_to_write);
#endif     
            }                 
            else
            {
                /* Writing ROM */
                /* Call the Programming function */
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH           
                if( g_flash_to_flash_op == 1 )
                {
                    /* Use RAM array */
                    ret = rom_write(g_bgo_flash_addr, 
                                    (uint32_t)&g_temp_array[0],
                                    num_byte_to_write);
                }
                else
                {          
                    /* Go as usual */
                    ret = rom_write(g_bgo_flash_addr, 
                                    g_bgo_buffer_addr,
                                    num_byte_to_write);
                }
#else            
                ret = rom_write(g_bgo_flash_addr, 
                                g_bgo_buffer_addr,
                                num_byte_to_write);
#endif                
            }                 

            /* Check the result for errors */
            if( ret != FLASH_SUCCESS )
            {
                /* Error detected during programming, stop and return */
                /* Leave Program/Erase Mode and clear any error flags */
                exit_pe_mode(g_bgo_flash_addr);         
        
                /* Release flash state */
                flash_release_state();
    
                /* Operation failure, use callback function to alert user */
                FlashError();
        
                /* Exit ISR */
                return;
            }

            /* Increment the flash address and the buffer address by the size
               of the transfer thats just completed */
            g_bgo_flash_addr += num_byte_to_write;
            g_bgo_buffer_addr += num_byte_to_write;
            
            /* Decrement the number of bytes remaining by the size of the last
               flash write */
            g_bgo_bytes -= num_byte_to_write;  
                                  
        }
        else
        {
            /* Leave Program/Erase Mode */
            exit_pe_mode(g_bgo_flash_addr);

            /* Release flash state */
            flash_release_state();
            
            /* Flash operation finished callback function */
            FlashWriteDone(); 
        }
        
    }
    else
    {
        /* Error, should never get here */
        while(1);
    }
}
/******************************************************************************
End of function  flash_ready_isr
******************************************************************************/

#endif

#ifdef FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING
#pragma section FRAM
#endif

/******************************************************************************
* Function Name: rom_write
* Description  : Write bytes to ROM Area Flash.
*                NOTE: This function MUST execute from in RAM.
* Arguments    : address - 
*                    ROM address of where to write to
*                data - 
*                    Pointer to the data to write
*                size - 
*                    The size of the data to write. Must be set to 
*                    either a supported write size of the MCU.
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
******************************************************************************/
static uint8_t rom_write (uint32_t address, uint32_t data, uint32_t size)
{
/* If ROM programming is not enabled then do not compile this code as it is
   not needed. */
#ifdef FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING

    /* Declare iteration loop count variable */
    volatile uint32_t i;  
    /* Declare wait counter variable */
    volatile int32_t  wait_cnt;

    /* Writes are done 16-bit at a time, scale 'size' argument */
    size = size >> 1;

    /* Write the FCU Program command */
    *(FCU_BYTE_PTR)address = 0xE8;  
    *(FCU_BYTE_PTR)address = size;    

    /* Write 'size' bytes into flash, 16-bits at a time */
    for(i = 0; i < size; i++)
    {
        /* Copy data from source address to destination ROM */
        *(FCU_WORD_PTR)address = *(uint16_t *)data;
        
        /* Increment destination address by 2 bytes */
        data += 2;
    }

    /* Write the final FCU command for programming */
    *(FCU_BYTE_PTR)address = 0xD0;

    #ifdef FLASH_API_RX_CFG_ROM_BGO            
    /* Return, rest of programming will be done in interrupt */
    return FLASH_SUCCESS;
    #endif

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_ROM_WRITE;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {    
            /* Maximum timeout duration for writing to ROM has elapsed - 
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_FAILURE;
        }
    }

    /* Check for illegal command or programming errors */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR  == 1)) 
    {        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }                

#endif /* FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING */

    /* If ROM programming is not enabled then this function will always just
       return FLASH_SUCCESS. This is okay because this function will never
       actually be called. The API write function will return an error when
       a ROM address is entered for programming. */
    
    /* Return FLASH_SUCCESS, operation successful */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function  rom_write
******************************************************************************/

/******************************************************************************
* Function Name: enter_pe_mode
* Description  : Puts the FCU into program/erase mode.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : flash_addr - 
*                    The programming/erasure address
*                bytes - 
*                    The number of bytes you are writing (if you are writing).  
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
******************************************************************************/
static uint8_t enter_pe_mode (uint32_t flash_addr)
{
    /* Used for timeout on FENTRYR write/read. */
    volatile int32_t wait_cnt;

    /* If FCU firmware has already been transferred to FCU RAM,
       no need to do it again */
    if( !g_fcu_transfer_complete )
    {
        /* Initialise the FCU, and store operation resilt in result variable */             
        /* Check if FCU initialisation was successful */
        if( flash_init() != FLASH_SUCCESS )
        {
            /* FCU initialisiation failed - return operation failure */
            return FLASH_FAILURE;
        }
    }
    
    /* FENTRYR must be 0x0000 before bit FENTRY0 or FENTRYD can be set to 1 */
    FLASH.FENTRYR.WORD = 0xAA00;

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. */
            return FLASH_FAILURE;
        }
    }

    /* Check if FCU mode is set to ROM PE */
    if( g_current_mode == ROM_PE_MODE) 
    {
#if defined(FLASH_API_RX_CFG_ROM_BGO)
        /* Don't enable flash interrupts if programming lock bits */
        if( g_flash_state != FLASH_LOCK_BIT )
        {       
            /* Re-enable the FRDYI interrupt */
            FLASH.FRDYIE.BIT.FRDYIE = 1;     
        }
#else
        /* Disable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 0;            
#endif                       
                                     
        /* Check which area of flash this address is in */
        if(flash_addr >= ROM_AREA_0)
        {
            /* Area 0 */
            /* Enter ROM PE mode for ROM Area 0 */
            FLASH.FENTRYR.WORD = 0xAA01;
        } 
#if defined(ROM_AREA_1)
        else if((flash_addr < ROM_AREA_0) && (flash_addr >= ROM_AREA_1)) 
        {            
            /* Area 1 */
            /* Enter ROM PE mode for ROM Area 1 */
            FLASH.FENTRYR.WORD = 0xAA02;
        }
#endif
#if defined(ROM_AREA_2)
        else if((flash_addr < ROM_AREA_1) && (flash_addr >= ROM_AREA_2)) 
        {            
            /* Area 2 */
            /* Enter ROM PE mode for ROM Area 2 */
            FLASH.FENTRYR.WORD = 0xAA04;
        }
#endif
#if defined(ROM_AREA_3)
        else
        {
            /* Area 3 */
            /* Enter ROM PE mode for ROM Area 3 */
            FLASH.FENTRYR.WORD = 0xAA08;
        }
#endif
                
    }
    /* Check if FCU mode is set to data flash PE */
    else if( g_current_mode == FLD_PE_MODE )
    {        
#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
        /* Re-enable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 1; 
#else
        /* Disable the FRDYI interrupt */
        FLASH.FRDYIE.BIT.FRDYIE = 0; 
#endif                               
                        
        /* Set FENTRYD bit(Bit 7) and FKEY (B8-15 = 0xAA) */
        FLASH.FENTRYR.WORD = 0xAA80;    

        /*  First clear the FCU's status before doing Data Flash programming.
            This is to clear out any previous errors that may have occured.
            For example, if you attempt to read the Data Flash area 
            before you make it readable using R_FlashDataAreaAccess(). */
        data_flash_status_clear();
        
    }
    /* Catch-all for invalid FCU mode */
    else
    {
        /* Invalid value of 'g_current_mode' */
        return FLASH_FAILURE;
    }
    
    /* Enable Write/Erase of ROM/Data Flash */
    FLASH.FWEPROR.BYTE = 0x01;

    /* Check for FCU error */
    if(     (FLASH.FSTATR0.BIT.ILGLERR == 1)
        ||  (FLASH.FSTATR0.BIT.ERSERR  == 1)
        ||  (FLASH.FSTATR0.BIT.PRGERR  == 1)
        ||  (FLASH.FSTATR1.BIT.FCUERR  == 1)) 
    {
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;    
    }
    
    /* Check to see if peripheral clock notification command is needed */
    if( g_fcu_pclk_command == 0 )
    {
        /* Disable FCU interrupts, so interrupt will not trigger after
           peripheral clock notification command */
        FLASH.FRDYIE.BIT.FRDYIE = 0;
        
        /* Inform FCU of flash clock speed, check if operation is succesful */
        if(notify_peripheral_clock((FCU_BYTE_PTR)flash_addr)!=0)
        {                   
            /* Return FLASH_FAILURE, operation failure */
            return FLASH_FAILURE;
        }
        
        /* Only re-enable flash ready interrupt if using BGO */
#ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
        if( g_current_mode == FLD_PE_MODE )
        {
            /* Re-enable the FRDYI interrupt */
            FLASH.FRDYIE.BIT.FRDYIE = 1;        
        }       
#endif
#ifdef FLASH_API_RX_CFG_ROM_BGO
        if( g_current_mode == ROM_PE_MODE )
        {
            /* Don't enable flash interrupts if programming lock bits */
            if( g_flash_state != FLASH_LOCK_BIT )
            {       
                /* Re-enable the FRDYI interrupt */
                FLASH.FRDYIE.BIT.FRDYIE = 1;     
            }                        
        }
#endif
        
        /* No need to notify FCU of clock supplied to flash again */
        g_fcu_pclk_command = 1;
    }    

    /* Return FLASH_SUCCESS, operation successful */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function enter_pe_mode
******************************************************************************/

/******************************************************************************
* Function Name: exit_pe_mode
* Description  : Takes the FCU out of program/erase mode.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash 
*                programming, this function can reside and execute in Flash.
* Arguments    : flash_addr - 
*                    The programming/erasure address
* Return Value : none
******************************************************************************/
static void exit_pe_mode (uint32_t flash_addr)
{
    /* Declare wait timer count variable */
    volatile int32_t wait_cnt;
    
    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;

    /* Cast flash address so that it can be used as pointer and will be
       accessed correctly. */
    p_addr = (FCU_BYTE_PTR)flash_addr;

    /* Set wait timer count duration */  
    wait_cnt = WAIT_MAX_ERASE;

    /* Iterate while loop whilst FCU operation is in progress */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait timer count variable */
        wait_cnt--;                         

        /* Check if wait timer count value has reached zero */
        if(wait_cnt == 0)
        {
            /* Timeout duration has elapsed, assuming operation failure and
               resetting the FCU */
            flash_reset();          
            
            /* Break from the while loop prematurely */
            break;
        }
    }

    /* Check FSTATR0 and execute a status register clear command if needed */
    if(     (FLASH.FSTATR0.BIT.ILGLERR == 1) 
        ||  (FLASH.FSTATR0.BIT.ERSERR  == 1) 
        ||  (FLASH.FSTATR0.BIT.PRGERR  == 1)) 
    {

        /* Clear ILGLERR */
        if(FLASH.FSTATR0.BIT.ILGLERR == 1)
        {
            /* FASTAT must be set to 0x10 before the status clear command
               can be successfully issued  */
            if(FLASH.FASTAT.BYTE != 0x10)
            {
                /* Set the FASTAT register to 0x10 so that a status clear
                    command can be issued */
                FLASH.FASTAT.BYTE = 0x10;
            }
        }
        
        /* Send status clear command to FCU */
        *p_addr = 0x50;
    }

    /* Enter ROM Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;        

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. Could add a call to FlashError()
               here if user wants to process this. */
            break;
        }
    }

    /* Disable the FRDYI interrupt */
    FLASH.FRDYIE.BIT.FRDYIE = 0;            
    
    /* Flash write/erase disabled */
    FLASH.FWEPROR.BYTE = 0x02;

}
/******************************************************************************
End of function  exit_pe_mode
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashErase
* Description  : Erases an entire flash block.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : block - 
*                    The block number to erase (BLOCK_0, BLOCK_1, etc...)
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
*                FLASH_BUSY - 
*                    Another flash operation is in progress
******************************************************************************/
uint8_t R_FlashErase (uint32_t block)
{
    /* Declare address pointer */
    uint32_t p_addr;
    /* Declare erase operation result container variable */
    uint8_t  result = FLASH_SUCCESS;
    
    /* Attempt to grab state */
    if( flash_grab_state(FLASH_ERASING) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Erase Command Address */
    p_addr = g_flash_BlockAddresses[block];

    /* Do we want to erase a Data Flash block or ROM block? */
    if( block >= BLOCK_DB0 )
    {
        /* Set current FCU mode to data flash PE */
        g_current_mode = FLD_PE_MODE;
    }
    else
    {
        /* Set current FCU mode to ROM PE */
        g_current_mode = ROM_PE_MODE;
    }

    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode(p_addr) != FLASH_SUCCESS)        
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(p_addr);

        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }

#ifdef  FLASH_API_RX_CFG_IGNORE_LOCK_BITS
    /* Cancel the ROM Protect feature 
       NOTE: If you are using block locking feature to protect your data,
       then comment out 'FLASH_API_RX_CFG_IGNORE_LOCK_BITS' in r_flash_api_rx_config.h */
    FLASH.FPROTR.WORD = 0x5501;
#else
    /* Only disable lock bit protection if user has specified to 
       do so earlier */
    if( g_lock_bit_protection == false )
    {
        /* Cancel the ROM Protect feature */
        FLASH.FPROTR.WORD = 0x5501;    
    }       
#endif


#if defined(DF_GROUPED_BLOCKS)
    /* NOTE:
       Some RX MCUs (e.g. RX63N) actually has 1024 x 32 byte blocks instead of 
       the 16 x 2Kbyte blocks defined in port file. These are grouped 
       into 16 blocks to make it easier for the user to delete larger sections 
       of the data flash at once. The user can still delete individual blocks 
       but they will need to use the new flash erase function that takes 
       addresses instead of blocks. For reference, the 2KB blocks defined
       are called 'fake' blocks. */
    if( FLD_PE_MODE == g_current_mode ) {

        /* Calculate how many bytes to erase */
        int32_t bytes_to_erase = (int32_t)
                                 (g_flash_BlockAddresses[BLOCK_DB1] - 
                                  g_flash_BlockAddresses[BLOCK_DB0]);

        /* Erase real data flash blocks until the 'fake' block is erased .*/
        while(0 < bytes_to_erase) {

            /* Send FCU command to erase block */
            result = flash_erase_command((FCU_BYTE_PTR)p_addr);

            /* Advance pointer to next block */
            p_addr += DF_ERASE_BLOCK_SIZE;

            /* Subtract off bytes erased */
            bytes_to_erase -= DF_ERASE_BLOCK_SIZE;

    #if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
            /* Set global variables so that erase can continue in ISR. */
            g_bgo_flash_addr = p_addr;
            g_bgo_bytes = bytes_to_erase;

            /* Return, check result and continue erasure later in ISR */
            return FLASH_SUCCESS;
    #endif    
        }
    }
    else
    {
        /* This is a ROM block erase */
        /* Send FCU command to erase block */
        result = flash_erase_command((FCU_BYTE_PTR)p_addr);
    }
#else
    /* Send FCU command to erase block */
    result = flash_erase_command((FCU_BYTE_PTR)p_addr);
#endif

#if defined(FLASH_API_RX_CFG_ROM_BGO)
    if( g_current_mode == ROM_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif    
    
#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
    if( g_current_mode == FLD_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif 

    /* Leave Program/Erase Mode */
    exit_pe_mode(p_addr);

    /* Release state */
    flash_release_state();

    /* Return erase result */
    return result;
}
/******************************************************************************
End of function  R_FlashErase
******************************************************************************/

#if defined(DF_GROUPED_BLOCKS)
/******************************************************************************
* Function Name: R_FlashEraseRange
* Description  : Erases blocks starting at an address and stopping when the
*                number of bytes to erase has been reached.
*                NOTE: This function is currently only for data flash blocks
*                on RX MCUs.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : start_addr - 
*                    The address of where to start erasing. Must be on
*                    erase boundary.
*                bytes - 
*                    The numbers of bytes to erase. Must be multiple of 
*                    erase size.
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
*                FLASH_ERROR_ALIGNED - 
*                    Flash address was not on correct boundary
*                FLASH_BUSY - 
*                    Another flash operation is in progress
*                FLASH_ERROR_BYTES - 
*                    Number of bytes did not match erase size
*                FLASH_ERROR_ADDRESS - 
*                    Invalid address, this is only for DF
******************************************************************************/
uint8_t R_FlashEraseRange (uint32_t start_addr, uint32_t bytes)
{
    /* Declare erase operation result container variable */
    uint8_t result = FLASH_SUCCESS;
    
    /* Take off upper byte since for programming/erase addresses for ROM are 
        the same as read addresses except upper byte is masked off to 0's.
        Data Flash addresses are not affected. */
    start_addr &= 0x00FFFFFF;

    /* Confirm this is for data flash */  
    if( start_addr >= DF_ADDRESS && start_addr < (DF_ADDRESS + BSP_DATA_FLASH_SIZE_BYTES) )
    {
        /* Check if the number of bytes were passed is a multiple of the 
           erase size for the data flash */
        if( bytes & (DF_ERASE_BLOCK_SIZE-1) )
        {
            /* Return number of bytes not a multiple of the programming size */
            return FLASH_ERROR_BYTES;
        }
        /* Check for an address on an erase boundary. */
        if( start_addr & (DF_ERASE_BLOCK_SIZE-1) )
        {
            /* Return address not on a programming boundary */
            return FLASH_ERROR_ALIGNED;
        }
    }
    else
    {
        /* This function is only for data flash blocks. */
        return FLASH_ERROR_ADDRESS;
    }

    /* Attempt to grab state */
    if( flash_grab_state(FLASH_ERASING) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Set current FCU mode to data flash PE */
    g_current_mode = FLD_PE_MODE;   

    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode(start_addr) != FLASH_SUCCESS)        
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(start_addr);

        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }

    /* Erase real data flash blocks until the 'fake' block is erased .*/
    while(0 < bytes) {

        /* Send FCU command to erase block */
        result = flash_erase_command((FCU_BYTE_PTR)start_addr);

        /* Advance pointer to next block */
        start_addr += DF_ERASE_BLOCK_SIZE;

        /* Subtract off bytes erased */
        bytes -= DF_ERASE_BLOCK_SIZE;

#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
        /* Set global variables so that erase can continue in ISR. */
        g_bgo_flash_addr = start_addr;
        g_bgo_bytes = bytes;

        /* Return, check result and continue erasure later in ISR */
        return FLASH_SUCCESS;
#endif    
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode(start_addr);

    /* Release state */
    flash_release_state();

    /* Return erase result */
    return result;
}
/******************************************************************************
End of function  R_FlashEraseRange
******************************************************************************/
#endif /* defined(DF_GROUPED_BLOCKS) */

/******************************************************************************
* Function Name: flash_erase_command
* Description  : Issues the FCU command to erase a flash block
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : erase_addr - 
*                    An address in the block to be erased
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
******************************************************************************/
static uint8_t flash_erase_command (FCU_BYTE_PTR const erase_addr)
{
    /* Declare timer wait count variable */
    volatile int32_t wait_cnt;
    /* Declare erase operation result container variable */
    uint8_t result = FLASH_SUCCESS;

    /* Send the FCU Command */
    *erase_addr = 0x20;
    *erase_addr = 0xD0;

#if defined(FLASH_API_RX_CFG_ROM_BGO)
    if( g_current_mode == ROM_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif    
    
#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
    if( g_current_mode == FLD_PE_MODE )
    {
        /* Return, check result later in ISR */
        return FLASH_SUCCESS;
    }
#endif    

    /* Set the wait counter timeout duration */
    wait_cnt = WAIT_MAX_ERASE;

    /* Wait while FCU operation is in progress */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;
        
        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0){

            /* Maximum timeout duration for erasing a ROM block has 
               elapsed, assuming operation failure - reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            return FLASH_FAILURE;
        }
    }

    /* Check if erase operation was successful by checking 
       bit 'ERSERR' (bit5) and 'ILGLERR' (bit 6) of register 'FSTATR0' */
    /* Check FCU error */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.ERSERR  == 1)) 
    {
        result = FLASH_FAILURE;
    }
 
    /* Return erase result */
    return result;
}
/******************************************************************************
End of function  flash_erase_command
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashWrite
* Description  : Writes bytes into flash.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : flash_addr - 
*                    Flash address location to write to. This address 
*                    must be on a program boundary (e.g. RX62N has 
*                    256-byte ROM writes and 8-byte DF writes).
*                buffer_addr - 
*                    Address location of data buffer to write into flash.
*                bytes - 
*                    The number of bytes to write. You must always pass a 
*                    multiple of the programming size (e.g. RX62N has 
*                    256-byte ROM writes and 8-byte DF writes).
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE - 
*                    Operation Failed
*                FLASH_ERROR_ALIGNED - 
*                    Flash address was not on correct boundary
*                FLASH_ERROR_BYTES - 
*                    Number of bytes did not match programming size of ROM or DF
*                FLASH_ERROR_ADDRESS - 
*                    Invalid address
*                FLASH_ERROR_BOUNDARY - 
*                    (ROM) Cannot write across flash areas.                                       
*                FLASH_BUSY - 
*                    Flash is busy with another operation
******************************************************************************/
uint8_t R_FlashWrite (uint32_t flash_addr, 
                      uint32_t buffer_addr, 
                      uint16_t bytes)
{
    /* Declare result container and number of bytes to write variables */
    uint8_t  result = FLASH_SUCCESS;
    uint32_t num_byte_to_write;    
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH 
    /* Local variable when using FLASH_API_RX_CFG_FLASH_TO_FLASH */
    uint16_t i; 
#endif    
        
    /* Take off upper byte since for programming/erase addresses for ROM are 
        the same as read addresses except upper byte is masked off to 0's.
        Data Flash addresses are not affected. */
    flash_addr &= 0x00FFFFFF;

    /* ROM area or Data Flash area - Check for DF first */  
    if( flash_addr >= DF_ADDRESS && (flash_addr < DF_ADDRESS + BSP_DATA_FLASH_SIZE_BYTES) )
    {
        /* Check if the number of bytes were passed is a multiple of the 
           programming size for the data flash */
        if( bytes & (DF_PROGRAM_SIZE_SMALL-1) )
        {
            /* Return number of bytes not a multiple of the programming size */
            return FLASH_ERROR_BYTES;
        }
        /* Check for an address on a programming boundary. */
        if( flash_addr & (DF_PROGRAM_SIZE_SMALL-1) )
        {
            /* Return address not on a programming boundary */
            return FLASH_ERROR_ALIGNED;
        }
    }
    /* Check for ROM area */
    else if(    (flash_addr >= ROM_PE_ADDR)
            &&  (flash_addr < ROM_PE_ADDR + BSP_ROM_SIZE_BYTES) )
    {
        /* First make sure that ROM operations are enabled. */
#ifndef FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING
        /* ROM operations are not enabled! Enable them in r_flash_api_rx_config.h */
        return FLASH_FAILURE;
#endif

        /* Check if the number of bytes were passed is a multiple of the 
           programming size for ROM */
        if( bytes & (ROM_PROGRAM_SIZE-1) )
        {
            /* Return number of bytes not a multiple of the programming size */
            return FLASH_ERROR_BYTES;
        }

        /* Check for an address on a programming boundary. */
        if( flash_addr & (ROM_PROGRAM_SIZE-1))
        {
            /* Return address not on a ROM programming byte boundary */
            return FLASH_ERROR_ALIGNED; 
        }

        /* Check for attempts to program over flash area boundaries. These 
           boundaries are defined in r_flash_api_rx*MCU Group*.h as 'ROM_AREA_#'. */
        if (((flash_addr < (ROM_AREA_0)) && ((flash_addr + bytes) > ROM_AREA_0))
#if defined(ROM_AREA_1)
           || ((flash_addr < (ROM_AREA_1))  && ((flash_addr + bytes) > ROM_AREA_1))
#endif
#if defined(ROM_AREA_2)
           || ((flash_addr < (ROM_AREA_2))  && ((flash_addr + bytes) > ROM_AREA_2))
#endif
#if defined(ROM_AREA_3)
           || ((flash_addr < (ROM_AREA_3))  && ((flash_addr + bytes) > ROM_AREA_3))
#endif
            )
        {
            /* Return cannot write across a flash area boundary */
            return FLASH_ERROR_BOUNDARY;
        } 

        /* Make sure write is not going over end of flash. */
        if ((flash_addr + bytes) > (ROM_PE_ADDR + BSP_ROM_SIZE_BYTES))
        {
            return FLASH_ERROR_BOUNDARY;
        }
    }
    else 
    {
        /* Return invalid flash address */
        return FLASH_ERROR_ADDRESS;
    }
    
    /* Attempt to grab state */
    if( flash_grab_state(FLASH_WRITING) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Do we want to program a DF area or ROM area? */
    if( flash_addr < g_flash_BlockAddresses[ROM_NUM_BLOCKS-1] )
    {
        /* Set current FCU mode to data flash PE */
        g_current_mode = FLD_PE_MODE;
    }
    else
    {
        /* Set FCU to ROM PE mode */
        g_current_mode = ROM_PE_MODE;
    }     
    
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH    
    /* Are we doing a ROM to ROM or DF to DF transfer? */
    if( (buffer_addr >= ROM_PE_ADDR) ||
        ((buffer_addr >= DF_ADDRESS) && (buffer_addr < (DF_ADDRESS + BSP_DATA_FLASH_SIZE_BYTES))))
    {
        /* Buffering is needed. */
        /* Get maximum programming size that can currently be used. */
        num_byte_to_write = flash_get_program_size(bytes, flash_addr);
                       
        /* Copy over first buffer */
        for(i = 0; i < num_byte_to_write; i++)
        {
            /* Copy over each byte */
            g_temp_array[i] = *((uint8_t *)(buffer_addr+i));             
        }

        /* Check size */
        if( bytes == num_byte_to_write )
        {        
            /* Since we were able to put the entire buffer in RAM we can
               just change the buffer pointer and go on as usual. */
            g_flash_to_flash_op = 0;
            
            /* Set new buffer address */
            buffer_addr = (uint32_t)&g_temp_array[0];
        }        
        else
        {
            /* Set FLASH_API_RX_CFG_FLASH_TO_FLASH flag */
            g_flash_to_flash_op = 1;   
        }        
    }
    else
    {
        /* No need to buffer */
        g_flash_to_flash_op = 0;
    }
              
#endif /* FLASH_API_RX_CFG_FLASH_TO_FLASH */

    /* Enter PE mode, check if operation is successful */
    if( enter_pe_mode(flash_addr) != FLASH_SUCCESS)
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode(flash_addr);

        /* Release state */
        flash_release_state();
                
        /* Return operation failure */
        return FLASH_FAILURE;
    }
    
    #ifdef  FLASH_API_RX_CFG_IGNORE_LOCK_BITS
    /* Cancel the ROM Protect feature 
       NOTE: If you are using block locking feature to protect your data,
       then comment out 'FLASH_API_RX_CFG_IGNORE_LOCK_BITS' in r_flash_api_rx_config.h */
    FLASH.FPROTR.WORD = 0x5501;
    #else
    /* Only disable lock bit protection if user has specified to 
       do so earlier */
    if( g_lock_bit_protection == false )
    {
        /* Cancel the ROM Protect feature */
        FLASH.FPROTR.WORD = 0x5501;    
    }       
    #endif
    
    /* Iterate while there are still bytes remaining to write */
    while( bytes )
    {
        /* Get maximum programming size that can currently be used. */
        num_byte_to_write = flash_get_program_size((uint32_t)bytes, flash_addr);

#ifdef FLASH_API_RX_CFG_DATA_FLASH_BGO
        if( g_current_mode == FLD_PE_MODE )
        {
            /* At this point we are going to set some global variables which
               will continue on with write after these bytes are written. 
               I set these values before the actual first write because I was 
               getting an error when the flash ready interrupt was being called
               before these values were set. */        
            g_bgo_flash_addr = flash_addr + num_byte_to_write;               
            g_bgo_buffer_addr = buffer_addr + num_byte_to_write;
            g_bgo_bytes = bytes - num_byte_to_write;
        }        
#endif

#ifdef FLASH_API_RX_CFG_ROM_BGO
        if( g_current_mode == ROM_PE_MODE )
        {
            /* At this point we are going to set some global variables which
               will continue on with write after these bytes are written. 
               I set these values before the actual first write because I was 
               getting an error when the flash ready interrupt was being called
               before these values were set. */        
            g_bgo_flash_addr = flash_addr + num_byte_to_write;               
            g_bgo_buffer_addr = buffer_addr + num_byte_to_write;
            g_bgo_bytes = bytes - num_byte_to_write;
        }        
#endif

        /* Call the Programming function, store the operation status in the
           container variable, result */
#ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH           
        if( g_flash_to_flash_op == 1 )
        {
            if( g_current_mode == FLD_PE_MODE )
            {
                result = data_flash_write( flash_addr, (uint32_t)&g_temp_array[0],
                                           num_byte_to_write);                
            }
            else
            {
                result = rom_write( flash_addr, (uint32_t)&g_temp_array[0],
                                    num_byte_to_write);                
            }
        }
        else
        {
            if( g_current_mode == FLD_PE_MODE )
            {
                result = data_flash_write( flash_addr, buffer_addr,
                                           num_byte_to_write);                
            }
            else
            {
                result = rom_write( flash_addr, buffer_addr, 
                                    num_byte_to_write);     
            }
        }
#else                                       
        if( g_current_mode == FLD_PE_MODE )
        {
            result = data_flash_write(flash_addr, buffer_addr, num_byte_to_write);
        }
        else
        {
            result = rom_write(flash_addr, buffer_addr, num_byte_to_write);
        }
#endif                                       

        /* Check the container variable result for errors */
        if( result != FLASH_SUCCESS )
        {
            /* Data flash write error detected, break from flash write
               while loop prematurely */
            break;
        }

#if defined(FLASH_API_RX_CFG_DATA_FLASH_BGO)
        if( g_current_mode == FLD_PE_MODE )
        {
            /* Return FLASH_SUCCESS, rest of programming will be done 
               in interrupt */
            return FLASH_SUCCESS;
        }
#endif
#if defined(FLASH_API_RX_CFG_ROM_BGO)            
        if( g_current_mode == ROM_PE_MODE )
        {
            /* Return FLASH_SUCCESS, rest of programming will be done 
               in interrupt */
            return FLASH_SUCCESS;
        }
#endif 

#if (!defined(FLASH_API_RX_CFG_ROM_BGO)) || (!defined(FLASH_API_RX_CFG_DATA_FLASH_BGO))

        /* Increment the flash address and the buffer address by the size
           of the transfer thats just completed */
        flash_addr += num_byte_to_write;
        buffer_addr += num_byte_to_write;
        
        /* Decrement the number of bytes remaining by the size of the last
           flash write */
        bytes -= num_byte_to_write;
                    
    #ifdef FLASH_API_RX_CFG_FLASH_TO_FLASH
        /* Check to see if we need to buffer more data */
        if( (bytes > 0) &&
            (g_flash_to_flash_op == 1) )
        {
            /* We must leave PE mode to transfer next buffer to RAM */
            exit_pe_mode(flash_addr);                

            /* Get maximum programming size that can currently be used. */
            num_byte_to_write = flash_get_program_size(bytes, flash_addr);

            /* Copy over up to next bytes */
            for(i = 0; i < num_byte_to_write; i++)
            {
                /* Copy over each byte */
                g_temp_array[i] = *((uint8_t *)(buffer_addr+i));             
            }

            /* Re-enter PE mode, check if operation is successful */
            if( enter_pe_mode(flash_addr) != FLASH_SUCCESS)
            {
                /* Make sure part is in ROM read mode. */
                exit_pe_mode(flash_addr);

                /* Release state */
                flash_release_state();
    
                /* Return operation failure */
                return FLASH_FAILURE;
            }                
        }
    #endif /* FLASH_API_RX_CFG_FLASH_TO_FLASH */  
#endif           
    }

    /* Leave Program/Erase Mode */
    exit_pe_mode(flash_addr);
    
    /* Release state */
    flash_release_state();

    /* Return flash programming result */
    return result;
}
/******************************************************************************
End of function  R_FlashWrite
******************************************************************************/

/******************************************************************************
* Function Name: notify_peripheral_clock
* Description  : Notifies FCU or clock supplied to flash unit
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash programming,
*                this function can reside and execute in Flash.
* Arguments    : flash_addr - 
*                    Flash address you will be erasing or writing to
* Return Value : FLASH_SUCCESS - 
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
******************************************************************************/
static uint8_t notify_peripheral_clock (FCU_BYTE_PTR flash_addr)
{
    /* Declare wait counter variable */
    volatile int32_t wait_cnt;

    /* Notify Peripheral Clock(PCK) */
    /* Set frequency of PCK in MHz */
    FLASH.PCKAR.WORD = (FLASH_CLOCK_HZ/1000000);            

    /* Execute Peripheral Clock Notification Commands */
    *flash_addr = 0xE9;         
    *flash_addr = 0x03;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *(FCU_WORD_PTR)flash_addr = 0x0F0F;
    *flash_addr = 0xD0;

    /* Set timeout wait duration */
    wait_cnt = WAIT_MAX_NOTIFY_FCU_CLOCK;

    /* Check FRDY */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement wait counter */
        wait_cnt--;                             
        
        /* Check if wait counter has reached zero */
        if(wait_cnt == 0)
        {
            /* Timeout duration elapsed, assuming operation failure - Reset
               FCU */
            flash_reset();                      

            /* Return FLASH_FAILURE, operation failure  */
            return FLASH_FAILURE;
        }
    }

    /* Check ILGLERR */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1)
    {
        /* Return FLASH_FAILURE, operation failure*/
        return FLASH_FAILURE;
    }

    /* Return FLASH_SUCCESS, operation success */
    return FLASH_SUCCESS;
}
/******************************************************************************
End of function notify_peripheral_clock
******************************************************************************/

/******************************************************************************
* Function Name: flash_reset
* Description  : Reset the FCU.
*                NOTE: This function MUST execute from in RAM for 'ROM Area' 
*                programming, but if you are ONLY doing Data Flash 
*                programming, this function can reside and execute in Flash.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void flash_reset (void)
{
    /* Declare wait counter variable */
    volatile int32_t    wait_cnt;

    /* Reset the FCU */
    FLASH.FRESETR.WORD = 0xCC01;

    /* Give FCU time to reset */
    wait_cnt = WAIT_TRESW;

    /* Wait until the timer reaches zero */
    while(wait_cnt != 0)
    {
        /* Decrement the timer count each iteration */
        wait_cnt--;
    }

    /* Release state */
    flash_release_state();

    /* FCU is not reset anymore */
    FLASH.FRESETR.WORD = 0xCC00;
    
    /* Enter ROM Read mode */
    FLASH.FENTRYR.WORD = 0xAA00;                

    /* Initialize timeout for FENTRYR being written. */
    wait_cnt = FLASH_FENTRYR_TIMEOUT;

    /* Read FENTRYR to ensure it has been set to 0. Note that the top byte
       of the FENTRYR register is not retained and is read as 0x00. */
    while(0x0000 != FLASH.FENTRYR.WORD)
    {
        /* Wait until FENTRYR is 0 unless timeout occurs. */
        if (wait_cnt-- <= 0)
        {
            /* This should not happen. FENTRYR getting written to 0 should
               only take 2-4 PCLK cycles. Could add a call to FlashError()
               here if user wants to process this. */
            break;
        }
    }
    
    /* Flash write/erase disabled */
    FLASH.FWEPROR.BYTE = 0x02;    
}
/******************************************************************************
End of function  flash_reset
******************************************************************************/

/******************************************************************************
* Function Name: flash_get_program_size
* Description  : Gets the number of bytes that can be written at this time. 
*                Since the DF and ROM have different available programming 
*                sizes, we want to use the largest option to save time.
* Arguments    : bytes - 
*                    How many bytes are left to write total.
*                flash_addr - 
*                    Current address we are writing to.
* Return Value : Number of bytes to write at this time.
******************************************************************************/
static uint32_t flash_get_program_size (uint32_t bytes, uint32_t flash_addr)
{
    uint32_t num_byte_to_write = 0;
   
    if( g_current_mode == FLD_PE_MODE )
    {
        /* This is a DF write. */
#if defined(DF_PROGRAM_SIZE_LARGE)
        if( (bytes >= DF_PROGRAM_SIZE_LARGE) && 
            ((flash_addr & (DF_PROGRAM_SIZE_LARGE-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = DF_PROGRAM_SIZE_LARGE;
        }
        else
        {   
            /* Cannot use large program size, use small */
            num_byte_to_write = DF_PROGRAM_SIZE_SMALL;
        }
#else
        /* Set the next data transfer size to be DF_PROGRAM_SIZE_SMALL*/
        num_byte_to_write = DF_PROGRAM_SIZE_SMALL;
#endif
    }
    else
    {
        /* This is a ROM write. */
#if defined(ROM_PROGRAM_SIZE_LARGE)
        if( (bytes >= ROM_PROGRAM_SIZE_LARGE) && 
            ((flash_addr & (ROM_PROGRAM_SIZE_LARGE-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = ROM_PROGRAM_SIZE_LARGE;
        }
    #if defined(ROM_PROGRAM_SIZE_MEDIUM)
        else if( (bytes >= ROM_PROGRAM_SIZE_MEDIUM) && 
                 ((flash_addr & (ROM_PROGRAM_SIZE_MEDIUM-1)) == 0x00))
        {
            /* Use large write size since it's more time efficient */
            num_byte_to_write = ROM_PROGRAM_SIZE_MEDIUM;
        }
    #endif
        else
        {   
            /* Cannot use large program size, use small */
            num_byte_to_write = ROM_PROGRAM_SIZE_SMALL;
        }
#else
        /* Set the next data transfer size to be smallest supported write. */
        num_byte_to_write = ROM_PROGRAM_SIZE;
#endif
    }

    return num_byte_to_write;
}

#ifndef  FLASH_API_RX_CFG_IGNORE_LOCK_BITS
/******************************************************************************
* Function Name: R_FlashProgramLockBit
* Description  : Programs the lock bit for a specified ROM erasure block. If
*                the lock bit for a block is set and lock bit protection is 
*                enabled then that block cannot be programmed/erased.
*                NOTE: This function MUST execute from RAM
* Arguments    : block - 
*                    Which ROM erasure block to set the lock bit for
* Return Value : FLASH_SUCCESS -
*                    Operation Successful
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_BUSY -
*                    Another flash operation is in progress
******************************************************************************/
uint8_t R_FlashProgramLockBit (uint32_t block)
{
    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;
    
    /* Declare operation result container variable */
    uint8_t result = FLASH_SUCCESS;
    
    /* Declare timer wait count variable */
    volatile int32_t wait_cnt;
    
    /* Attempt to grab state */
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Flash Command Address */
    p_addr = (FCU_BYTE_PTR)(g_flash_BlockAddresses[block]);
    
    /* Set FCU to ROM PE mode */
    g_current_mode = ROM_PE_MODE;    
    
    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode((uint32_t)p_addr) != FLASH_SUCCESS)        
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode((uint32_t)p_addr);

        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }
    
    /* Send the FCU Command */
    *p_addr = 0x77;
    *p_addr = 0xD0;

    /* Set timeout wait counter value */
    wait_cnt = WAIT_MAX_ROM_WRITE;

    /* Wait until FCU operation finishes, or a timeout occurs */
    while(FLASH.FSTATR0.BIT.FRDY == 0)
    {
        /* Decrement the wait counter */
        wait_cnt--;

        /* Check if the wait counter has reached zero */
        if(wait_cnt == 0)
        {    
            /* Maximum timeout duration for writing to ROM has elapsed - 
               assume operation failure and reset the FCU */
            flash_reset();

            /* Return FLASH_FAILURE, operation failure */
            result = FLASH_FAILURE;
        }
    }

    /* Check for illegal command or programming errors */
    if((FLASH.FSTATR0.BIT.ILGLERR == 1) || (FLASH.FSTATR0.BIT.PRGERR  == 1)) 
    {        
        /* Return FLASH_FAILURE, operation failure */
        result = FLASH_FAILURE;
    }      
    
    /* Leave Program/Erase Mode */
    exit_pe_mode((uint32_t)p_addr);
    
    /* Release state */
    flash_release_state();              
    
    /* Return result of lock bit programming */
    return result;
}
/******************************************************************************
End of function  R_FlashProgramLockBit
******************************************************************************/

/******************************************************************************
* Function Name: R_FlashReadLockBit
* Description  : Reads and returns the lock bit status for a ROM block
*                NOTE: This function MUST execute from RAM
* Arguments    : block - 
*                    Which ROM erasure block to read the lock bit of
* Return Value : FLASH_LOCK_BIT_SET -
*                    Lock bit was set
*                FLASH_LOCK_BIT_NOT_SET -
*                    Lock bit was not set
*                FLASH_FAILURE -
*                    Operation Failed
*                FLASH_BUSY -
*                    Another flash operation is in progress
******************************************************************************/
uint8_t R_FlashReadLockBit (uint32_t block)
{
    /* Declare address pointer */
    FCU_BYTE_PTR p_addr;
    
    /* Declare operation result container variable */
    uint8_t result = FLASH_SUCCESS;
        
    /* Holds outcome of lock-bit read */
    uint8_t lock_bit;
    
    /* Attempt to grab state */;
    if( flash_grab_state(FLASH_LOCK_BIT) != FLASH_SUCCESS )
    {
        /* Another operation is already in progress */
        return FLASH_BUSY;
    }
    
    /* Flash Command Address */
    p_addr = (FCU_BYTE_PTR)(g_flash_BlockAddresses[ block ]);
    
    /* Set FCU to ROM PE mode */
    g_current_mode = ROM_PE_MODE;

    /* Enter ROM PE mode, check if operation successful */
    if( enter_pe_mode((uint32_t)p_addr) != FLASH_SUCCESS)        
    {
        /* Make sure part is in ROM read mode. */
        exit_pe_mode((uint32_t)p_addr);

        /* Release state */
        flash_release_state();
        
        /* Return FLASH_FAILURE, operation failure */
        return FLASH_FAILURE;
    }
    
    /* Switch to ROM Lock-Bit Read Mode */
    *p_addr = 0x71;
    
    /* Check for illegal command */
    if(FLASH.FSTATR0.BIT.ILGLERR == 1) 
    {        
        /* Return FLASH_FAILURE, operation failure */
        result = FLASH_FAILURE;        
    }      
    else
    {       
        /* Read lock bit value */
        lock_bit = *p_addr;
        
        /* Set return value */
        if(lock_bit != 0x00)
        {
            /* Lock bit is not set */
            result = FLASH_LOCK_BIT_NOT_SET;
        }
        else
        {
            /* Lock bit is set */
            result = FLASH_LOCK_BIT_SET;
        }
    }
              
    /* Leave Program/Erase Mode */
    exit_pe_mode((uint32_t)p_addr);
    
    /* Release state */
    flash_release_state();   
        
    /* Return result of lock bit programming */
    return result;
}
/******************************************************************************
End of function  R_FlashReadLockBit
******************************************************************************/
#endif /* FLASH_API_RX_CFG_IGNORE_LOCK_BITS */

