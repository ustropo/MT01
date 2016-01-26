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
* File Name    : r_vee_rx21x.c
* Device       : RX21x
* Tool Chain   : Renesas RX Standard Toolchain
* Description  : This file implements specific MCU functions that are used along with the Virtual EEPROM Project.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 1.00    First Release
*         : 02.01.2013 1.10    Replaced use of 'magic number' for Flash API return with macro. New include name for
*                              Flash API.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Used for accessing peripheral registers */
#include "platform.h"
/* VEE interface. */
#include "r_vee_if.h"
/* Used for VEE types and MCU relevant data. */
#include "r_vee_target.h"
/* Used for accessing g_flash_BlockAddresses[] array. */
#include "r_flash_api_rx_if.h"

/* Only compile this file if a RX63x MCU is chosen. */
#if defined(BSP_MCU_RX21_ALL)    

#ifdef VEE_ENABLE_DF

/***********************************************************************************************************************
* Function Name: vee_enable_df
* Description  : Enables the data flash on the MCU
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void vee_enable_df (void)
{
#if defined(FLASH_API_RX_CFG_COPY_CODE_BY_API) && defined(FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING)
    /* Before calling any other API functions the API code needs to be copied to RAM. */
    R_FlashCodeCopy();
#endif

    /* Enable access to the data flash blocks */
    R_FlashDataAreaAccess(0xFFFF, 0xFFFF);
}
/***********************************************************************************************************************
End of function vee_enable_df
***********************************************************************************************************************/

#endif

/***********************************************************************************************************************
* Function Name: vee_get_block_info
* Description  : Gets flag information for a VEE Block. This is separated from VEE.c since different methods will need 
*                to be used for different MCUs. For instance, the R8C can read a flag and tell it has not been written 
*                by reading 0xFF. The RX21x cannot do this though and you have to use the blank check command.
* Arguments    : sector - 
*                    Sector to look up
*                block - 
*                    Which VEE Block to look up
*                vee_block - 
*                    Structure to hold flags
* Return Value : VEE_SUCCESS -  
*                    Successful, structure filled in
*                VEE_FAILURE -  
*                    Failure
***********************************************************************************************************************/
uint8_t vee_get_block_info (uint8_t sector, uint32_t block, vee_block_info_t * vee_block)
{        
    uint8_t * ptr;
    
    /* Flags are at the lower addresses of the block */
    ptr = (uint8_t *) g_vee_Sectors[sector].VEE_block_addr[block];   
    
    /* Start off with all flags as not set */     
    vee_block->erasing = VEE_BLOCK_FLAG_NOT_SET;    
    vee_block->active = VEE_BLOCK_FLAG_NOT_SET;    
    vee_block->full = VEE_BLOCK_FLAG_NOT_SET;    
    vee_block->nextup = VEE_BLOCK_FLAG_NOT_SET;    
          
    /* Test for ERASING flag */  
    if (vee_blank_check_address((uint8_t *)ptr) != VEE_SUCCESS)
    {
        /* Make sure flag is correct */
        if (*(vee_var_min_t *)ptr == VEE_BLOCK_FLAG_ERASING)
        {
            /* Flag was correct */
            vee_block->erasing = VEE_BLOCK_FLAG_SET;
        }
    }         
    
    /* Move to ACTIVE flag */
    ptr += sizeof(vee_var_min_t);
    
    /* Test for ACTIVE flag */  
    if (vee_blank_check_address((uint8_t *)ptr) != VEE_SUCCESS)
    {
        /* Make sure flag is correct */
        if (*(vee_var_min_t *)ptr == VEE_BLOCK_FLAG_ACTIVE)
        {
            /* Flag was correct */
            vee_block->active = VEE_BLOCK_FLAG_SET;
        }
    }         
    
    /* Move to FULL flag */
    ptr += sizeof(vee_var_min_t);
    
    /* Test for FULL flag */  
    if (vee_blank_check_address((uint8_t *)ptr) != VEE_SUCCESS)
    {
        /* Make sure flag is correct */
        if(*(vee_var_min_t *)ptr == VEE_BLOCK_FLAG_FULL)
        {
            /* Flag was correct */
            vee_block->full = VEE_BLOCK_FLAG_SET;
        }
    }         

    /* Move to NEXTUP flag */
    ptr += sizeof(vee_var_min_t);

    /* Test for NEXTUP flag */          
    if (vee_blank_check_address((uint8_t *)ptr) != VEE_SUCCESS)
    {
        /* Make sure flag is correct */
        if (*(vee_var_min_t *)ptr == VEE_BLOCK_FLAG_NEXTUP)
        {
            /* Flag was correct */
            vee_block->nextup = VEE_BLOCK_FLAG_SET;
        }
    }         
    
    /* Finished successfully */
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function vee_get_block_info
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_blank_check_address
* Description  : Checks to see if this data address is blank
* Arguments    : addr - 
*                    Address to check
* Return Value : VEE_SUCCESS - 
*                    Address was blank
*                VEE_FAILURE - 
*                    Address has been written
***********************************************************************************************************************/
uint8_t vee_blank_check_address(uint8_t *addr)
{
    /* Local variable */
    uint8_t ret;
    
    /* Verify that address is on 2 byte boundary */
    if( (((uint32_t)addr) & 0x00000001) != 0 )
    {
        /* This should not happen */
        FlashError();
    }
    
    /* Perform 2 byte blank check */
    ret = R_FlashDataAreaBlankCheck( (uint32_t)addr, BLANK_CHECK_2_BYTE);
    
    /* Check return value */
    if( ret == FLASH_BLANK )
    {
        /* Address was blank */
        return VEE_SUCCESS;
    }
    else if( ret == FLASH_NOT_BLANK )
    {
        /* Address was not blank */
        return VEE_FAILURE;
    }
    else
    {
        /* Operation failure in Flash API */
        FlashError();
    }
    
    /* Return */
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function vee_blank_check_address
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_blank_check_block
* Description  : Checks to see if this data flash block is empty
* Arguments    : block - 
*                    block to check
* Return Value : VEE_SUCCESS - 
*                    Block was blank
*                VEE_FAILURE - 
*                    Block has been written
***********************************************************************************************************************/
uint8_t vee_blank_check_block(uint32_t block)
{       
    /* Perform blank check */
    if( R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[block], BLANK_CHECK_ENTIRE_BLOCK) == 0 )
    {
        /* Blank check is on-going */
        return VEE_SUCCESS;
    }
    else
    {
        /* Problem with blank check in FlashAPI */
        return VEE_FAILURE;
    }    
}
/***********************************************************************************************************************
End of function vee_blank_check_block
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_move_to_boundary
* Description  : Moves and address to a data flash programming boundary
* Arguments    : address - 
*                    Address to check
* Return Value : The address aligned to the data flash boundary
***********************************************************************************************************************/
uint32_t vee_move_to_boundary(uint32_t address)
{
    /* On the RX21x we have 2 byte boundaries */
    
    /* Check to see if it is already on an 2 byte boundary */
    if( (address & 0x00000001) != 0 )
    {
        /* Address is not on a boundary. Since this MCU has 2 byte writes, we just need to move 1 byte forward. */
        return (address+1);
    }
    else
    {
        /* Address is already on boundary */
        return address;
    }
}
/***********************************************************************************************************************
End of function vee_move_to_boundary
***********************************************************************************************************************/

#ifdef VEE_USE_DEFAULT_CHECK_FUNCTIONS

/***********************************************************************************************************************
* Function Name: vee_check_record
* Description  : Validates a record.  The user could implement a CRC or checksum check here if the default flag is 
*                not enough.
* Arguments    : record - 
*                    Address of record in data flash
* Return Value : VEE_SUCCESS - 
*                    Record is valid
*                VEE_FAILURE - 
*                    Record is not valid
***********************************************************************************************************************/
uint8_t vee_check_record(vee_record_t *record)
{
    /* By default we only check the flag that is written to a record once that record's data has been written. You 
       could use any check here. */
    /* First need to make sure the record has been written */
    if( vee_blank_check_address((uint8_t *)&record->check) == VEE_SUCCESS )
    {
        /* Record was blank so it is not valid */
        return VEE_FAILURE;
    }
    
    /* Check to see if the check field has the programmed flag */
    if( record->check == VEE_RECORD_WRITTEN )
    {
        /* Record verified */
        return VEE_SUCCESS;
    }
    else
    {
        /* Error with record */
        return VEE_FAILURE;
    }
}
/***********************************************************************************************************************
End of function vee_check_record
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_GenerateCheck
* Description  : Generates the 'check' field for a record.  The user could implement a CRC or checksum check here if 
*                the default static flag is not enough.
* Arguments    : record - 
*                    Pointer to record to generate check for
* Return Value : VEE_SUCCESS - 
*                    'check' field is filled in
*                VEE_FAILURE - 
*                    Invalid record
***********************************************************************************************************************/
uint8_t R_VEE_GenerateCheck(vee_record_t *record)
{    
    /* Just fill in a predefined value */
    record->check = VEE_RECORD_WRITTEN;  
    
    /* Return success */
    return VEE_SUCCESS; 
}
/***********************************************************************************************************************
End of function R_VEE_GenerateCheck
***********************************************************************************************************************/

#endif //VEE_USE_DEFAULT_CHECK_FUNCTIONS

#endif //BSP_MCU_RX21_ALL

