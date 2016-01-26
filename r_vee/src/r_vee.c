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
* File Name    : r_vee.c
* Description  : Virtual EEPROM implementation using MCU's data flash memory
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.07.2011 1.00    First Release
*         : 03.01.2012 1.50    Updated for internal CS and for FIT. Added support for RX63x Groups. 
*         : 14.09.2012 1.60    Updated for FIT v0.7 Spec. Fixed bug found when reset occurred after FULL flag was 
*                              written and before NEXTUP was written. VEE now handles this event.
*         : 03.01.2013 1.70    Added R_VEE_Open() function to initialize or reset VEE. Created r_vee_target.h to replace
*                              multiple r_vee_<mcu>.h files that had duplicate information. Updated to be compliant with
*                              FIT v1.00 specification. This means that config file is now in 'ref' folder. Tested with
*                              RX62G, RX210, and RX63T. Added R_VEE_Control() function. Added vee_invalidate_cache().
***********************************************************************************************************************/

/* This file will declare const VEE Sector structures from r_vee_config.h */
#define VEE_MEM_DECLARE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Used for offsetof() */
#include <stddef.h>
/* Used for memcpy() */
#include <string.h>
/* Defines, prototypes, and other configuration parameters for VEE use */
#include "r_vee_if.h"
/* VEE structures. */
#include "r_vee_types.h"
/* Includes correct files for current VEE target MCU. */
#include "r_vee_target.h"
/* Used for Flash API calls. */
#include "r_flash_api_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* This macro should only be used for testing purposes. This macro will remove the 'static' keyword from VEE data 
   structures and functions. This is done so that these items can be expected during testing. */
//#define VEE_TESTING

#if defined(VEE_TESTING)
    #define VEE_STATIC  
#else
    #define VEE_STATIC      static
#endif

/***********************************************************************************************************************
Private Global Variables
***********************************************************************************************************************/
/* Holds the current state of the VEE */
VEE_STATIC vee_states_t         g_vee_state = VEE_READY;
/* Cache used by VEE to hold record addresses */
VEE_STATIC vee_cache_t          g_vee_cache;  
/* Cache used to hold the next open address in sectors */
VEE_STATIC vee_next_address_t   g_vee_next_address[VEE_NUM_SECTORS];               
/* A VEE Block can be made up of multiple MCU data flash blocks. When erasing a VEE Block we may end up erasing multiple 
   MCU data flash bocks. */
/* Current data flash block to erase */
VEE_STATIC uint32_t             g_vee_erase_block_current;
/* Highest numbered data flash block to erase */
VEE_STATIC uint32_t             g_vee_erase_block_end;
/* Holds the current write state */
VEE_STATIC vee_write_states_t   g_vee_write_state;
/* Holds the record currently being written */
VEE_STATIC vee_record_t         g_vee_current_record;
/* Holds the sector currently being operated on */
VEE_STATIC uint8_t              g_vee_current_sector;
/* Holds the ACTIVE block currently being operated on */
VEE_STATIC uint32_t             g_vee_current_block_active;
/* Holds the NEXTUP block currently being operated on */
VEE_STATIC uint32_t             g_vee_current_block_nextup;
/* Current block being blank checked */
VEE_STATIC uint32_t             g_vee_blank_check_block_current;
/* Last block to be blank checked */
VEE_STATIC uint32_t             g_vee_blank_check_block_end;
/* Holds the current flash write address */
VEE_STATIC vee_record_t far *   g_vee_write_address;
/* Used for locking state of VEE */
VEE_STATIC bsp_lock_t           g_vee_lock;
#ifdef VEE_ENABLE_DF
/* Holds whether the data flash has been enabled yet */
VEE_STATIC uint8_t              g_vee_df_enabled;
#endif

/***********************************************************************************************************************
Private Function Prototypes
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_seek(vee_record_t *vee_temp);
VEE_STATIC uint8_t vee_find_record(uint8_t sector, uint32_t block, vee_record_t *vee_temp);
#ifdef VEE_CACHE_FILL_ALL                                                               
VEE_STATIC uint8_t vee_fill_cache(void);
#endif
VEE_STATIC uint8_t vee_check_sector(uint8_t sector);
VEE_STATIC uint8_t vee_check_block(uint8_t sector, uint32_t block);
VEE_STATIC uint8_t vee_erase_block(uint8_t sector, uint32_t block);
VEE_STATIC uint8_t vee_defrag_block(uint8_t sector, uint32_t active, uint32_t nextup);
VEE_STATIC uint8_t vee_write_flag(uint8_t sector, uint32_t block, uint8_t flag, vee_var_min_t value);
VEE_STATIC uint8_t vee_erase_and_defrag(uint8_t sector, uint32_t active, uint32_t nextup);                           
VEE_STATIC uint8_t vee_start_write(vee_record_t * vee_temp);  
VEE_STATIC uint8_t vee_grab_state(vee_states_t state);                                  
VEE_STATIC uint8_t vee_check_input_record(vee_record_t * vee_temp, bool read_operation);
VEE_STATIC uint8_t vee_check_input_sector(uint8_t sector);
VEE_STATIC void    vee_reset(void);
VEE_STATIC uint8_t vee_invalidate_cache(uint8_t sector);

/***********************************************************************************************************************
* Function Name: vee_grab_state
* Description  : Tries to grab the VEE state
* Arguments    : state - 
*                    Which state to try to transfer to
* Return value : VEE_SUCCESS -     
*                    Successful, state grabbed
*                VEE_BUSY -
*                    Data flash is busy, state not grabbed
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_grab_state (vee_states_t state)
{
    /* Local return variable */
    uint8_t ret = VEE_SUCCESS;
    
    /* Check flash status */
    if (1 == g_vee_df_enabled)
    {
        if( R_FlashGetStatus() != FLASH_SUCCESS )
        {
            /* Flash is busy */
            return VEE_BUSY;
        }   
    }
    
    /* Try to lock VEE to change state. */
    /* Check to see if lock was successfully taken. */
    if(false == R_BSP_SoftwareLock(&g_vee_lock))
    {
        /* Another operation is on-going */
        return VEE_BUSY;
    }
        
    #ifdef VEE_ENABLE_DF
    
    /* Make sure data flash is enabled */
    if( g_vee_df_enabled == 0 )
    {
        /* Enable data flash */
        vee_enable_df();
        
        /* Remember that data flash has been enabled */
        g_vee_df_enabled = 1;
    }
    
    #endif

    /* If a reset is occuring then change the state no matter what. */
    if (VEE_RESET != state)
    {
        /* Check VEE status to make sure we are not stomping on another thread */
        if( state == VEE_READING )
        {
            /* If another read comes in while the state is reading then we are okay */
            if( ( g_vee_state != VEE_READY ) && ( g_vee_state != VEE_READING) ) 
            {
                /* VEE is busy */
                ret = VEE_BUSY;
            }        
        }
        else
        {
            /* If we are doing something other than reading then we must be in the VEE_READY state */
            if (g_vee_state != VEE_READY)
            {
                /* VEE is busy */
                ret = VEE_BUSY;
            }
        }
    }
    
    if( ret == VEE_SUCCESS )
    {                    
        /* Grab state */
        g_vee_state = state;
    }
    
    /* Release lock. */
    R_BSP_SoftwareUnlock(&g_vee_lock);
        
    return ret;    

}
/***********************************************************************************************************************
End of function vee_grab_state
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_ReleaseState
* Description  : Sets the VEE State to ready. User would call this function after having called R_VEE_Read().  This 
*                will release the VEE state so that other operations can occur.
* Arguments    : none
* Return Value : VEE_SUCCESS - 
*                    Successful, state released
*                VEE_FAILURE -
*                    Cannot releas state during non-read operation
***********************************************************************************************************************/
uint8_t R_VEE_ReleaseState (void)
{
    /* Initialize return to successful. */
    uint8_t ret = VEE_SUCCESS;

    /* This function can only release state if it was reading */    
    if (g_vee_state == VEE_READING)
    {
        /* Release state */
        g_vee_state = VEE_READY;        
    }
    else if ((g_vee_state != VEE_READY) && (g_vee_state != VEE_READING))
    {
        /* Cannot release state. Another operation is still on-going. */
        ret = VEE_FAILURE;
    }
    else
    {
        /* This means that g_vee_state is VEE_READY which means that g_vee_state should not be modified.
           The g_vee_state variable should not be modified because of a race condition where g_vee_state is
           just about to be written as VEE_READY and then an interrupt occurs and R_VEE_Write() is called. At this time
           the state will be set to some writing state and when execution returns to here it could set the state to 
           VEE_READY when it should not. */
    }

    return ret;
}
/***********************************************************************************************************************
End of function R_VEE_ReleaseState
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_GetState
* Description  : Returns the current state of the VEE
* Arguments    : none
* Return Value : State of VEE
***********************************************************************************************************************/
vee_states_t R_VEE_GetState (void)
{
    /* Return state */
    return g_vee_state;
}
/***********************************************************************************************************************
End of function R_VEE_GetState
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_Open
* Description  : Initializes and resets all VEE data structures
* Arguments    : none
* Return Value : VEE_SUCCESS -
*                    VEE initialized successfully.
***********************************************************************************************************************/
uint8_t R_VEE_Open (void)
{
    /* Try to grab VEE State. */
    if( vee_grab_state(VEE_OPENING) != VEE_SUCCESS )
    {
        /* Could not grab state */
        return VEE_BUSY;
    }

    vee_reset();

    g_vee_state = VEE_READY;

    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function R_VEE_Open
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_reset
* Description  : Initializes and resets all VEE data structures
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
VEE_STATIC void vee_reset (void)
{
    uint32_t i;

    /* Initialize all global variables. */
    g_vee_state = VEE_READY;
    g_vee_erase_block_current = 0;
    g_vee_erase_block_end = 0;
    g_vee_write_state = VEE_WRITE_DONE;
    g_vee_current_record.ID = 0;
    g_vee_current_record.size = 0;
    g_vee_current_record.check = 0;
    g_vee_current_record.block = 0;
    g_vee_current_record.pData = 0;
    g_vee_current_sector = 0;
    g_vee_current_block_active = 0;
    g_vee_current_block_nextup = 0;
    g_vee_blank_check_block_current = 0;
    g_vee_blank_check_block_end = 0;
    g_vee_write_address = (vee_record_t far *)0;
    #ifdef VEE_ENABLE_DF
    g_vee_df_enabled = 0;
    #endif

    /* Invalidate next address structure and cache. */
    for (i = 0; i < VEE_NUM_SECTORS; i++)
    {
        vee_invalidate_cache(i);
    }
}
/***********************************************************************************************************************
End of function vee_reset
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_Read
* Description  : Returns address for data if record is cached. If not cached then it searches the data flash.
* Arguments    : vee_temp - 
*                    Structure with record information
* Return Value : VEE_SUCCESS -
*                    Successful, structure members set accordingly
*                VEE_NOT_FOUND -  
*                    Record not found
*                VEE_BUSY - 
*                    Data flash is busy, try again later
*                VEE_INVALID_INPUT -
*                    Record sent in had invalid data.
***********************************************************************************************************************/
uint8_t R_VEE_Read (vee_record_t *vee_temp)
{
    /* Local variable */
    vee_record_t far * record;

    /* Check record. */
    if (VEE_SUCCESS != vee_check_input_record(vee_temp, true))
    {
        /* Record was invalid. E.g. record ID is out of range. */
        return VEE_INVALID_INPUT;
    }
    
    /* Try to grab VEE State */
    if( vee_grab_state(VEE_READING) != VEE_SUCCESS )
    {
        /* Could not grab state */
        return VEE_BUSY;
    }
    
    #ifdef VEE_CACHE_FILL_ALL
    
    /* Check to see if cache has been filled yet, if not then fill it */
    if( g_vee_cache.valid != VEE_CACHE_ENTRY_VALID )
    {
        /* Cache needs to be filled */
        vee_fill_cache();
    }
    
    #endif                
    
    /* Check the cache to see if entry is found */
    if( g_vee_cache.entries[vee_temp->ID].valid == VEE_CACHE_ENTRY_VALID )
    {
        /* Success, entry found in cache, fill in fields  */
        record = g_vee_cache.entries[vee_temp->ID].address;
        
        /* Fill in data */
        vee_temp->pData = (uint8_t far *)&record->pData;
                          
        /* Fill in size */
        vee_temp->size = record->size;
        
        /* Fill in check */
        vee_temp->check = record->check;
        
        /* Fill in block */
        vee_temp->block = record->block;
        
        /* Success */
        return VEE_SUCCESS;                 
    }
#ifndef VEE_CACHE_FILL_ALL
    else
    {
        /* Seek for record in data flash */
        if( vee_seek(vee_temp) == VEE_SUCCESS )
        {
            /* Record was found, update cache */            
            g_vee_cache.entries[vee_temp->ID].address = (vee_record_t far *)
                                                        (vee_temp->pData - offsetof(vee_record_t, pData));

            /* Update which VEE Block the record is in */
            g_vee_cache.entries[vee_temp->ID].block = vee_temp->block;
                    
            /* Set cache entry as valid */
            g_vee_cache.entries[vee_temp->ID].valid = VEE_CACHE_ENTRY_VALID;            
            
            /* Success */
            return VEE_SUCCESS;             
        }
    }
#endif

    /* Nothing to read so release state */
    g_vee_state = VEE_READY;

    /* Record Not Found */
    return VEE_NOT_FOUND;                 

}
/***********************************************************************************************************************
End of function R_VEE_Read
***********************************************************************************************************************/

#ifdef VEE_CACHE_FILL_ALL

/***********************************************************************************************************************
* Function Name: vee_fill_cache
* Description  : Fills in the VEE Cache with record locations            
* Arguments    : none
* Return Value : VEE_SUCCESS -  
*                    Successful, cache has been filled in
*                VEE_FAILURE -
*                    Failure
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_fill_cache (void)
{
    /* Local variables */
    uint8_t i;
    vee_record_t vee_temp;
    
    for(i = 0; i < VEE_MAX_RECORD_ID; i++)
    {
        /* Update Record ID */
        vee_temp.ID = i;
        
        /* Seek for record in data flash */
        if( vee_seek(&vee_temp) == VEE_SUCCESS )
        {
            /* Record was found, update cache */            
            g_vee_cache.entries[i].address = (vee_record_t far *)(vee_temp.pData - offsetof(vee_record_t, pData));
            
            /* Update which VEE Block the record is in */
            g_vee_cache.entries[vee_temp.ID].block = vee_temp.block;
            
            /* Set cache entry as valid */
            g_vee_cache.entries[i].valid = VEE_CACHE_ENTRY_VALID;    
        }        
    }
    
    /* Set 'valid' bit to show that cache has been filled in */
    g_vee_cache.valid = VEE_CACHE_ENTRY_VALID;

    /* Success */                                              
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function g_vee_cache
***********************************************************************************************************************/

#endif /* VEE_CACHE_FILL_ALL */

/***********************************************************************************************************************
* Function Name: vee_seek
* Description  : Tries to find a record in the MCU's data flash
* Arguments    : vee_temp - 
*                    structure with record information
* Return Value : VEE_SUCCESS -
*                    Successful, structure filled in
*                VEE_NOT_FOUND -
*                    Record was not found
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_seek (vee_record_t *vee_temp)
{
    /* Loop variables */
    uint32_t     i;
    /* Structure pointer for VEE Block info */
    vee_block_info_t block_info;
        
    /* Loop through blocks */
    for(i=0;i<g_vee_Sectors[g_vee_RecordLocations[vee_temp->ID]].num_VEE_blocks;i++)
    {
        /* Get block info */
        vee_get_block_info(g_vee_RecordLocations[vee_temp->ID],i, &block_info);
                
        /* Check for block with ACTIVE flag set. */
        if( (block_info.active  == VEE_BLOCK_FLAG_SET) &&            
            (block_info.erasing == VEE_BLOCK_FLAG_NOT_SET ) )
        {
            /* See if record is found in this block */
            if( VEE_SUCCESS == vee_find_record(g_vee_RecordLocations[vee_temp->ID], i, vee_temp) )
            {
                /* Record found! */
                return VEE_SUCCESS;
            }
            else
            {
                /* Record not found */
                return VEE_NOT_FOUND;
            }
        }        
    }

    /* If we get here then record was not found */
    return VEE_NOT_FOUND;                     
}
/***********************************************************************************************************************
End of function vee_seek
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_find_record
* Description  : Searches for a record in a specific VEE Block          
* Arguments    : sector -   
*                    Which sector to search
*                block -   
*                    Which VEE block to search
*                vee_temp -   
*                    Structure with record info
* Return Value : VEE_SUCCESS -
*                    Successful, record found
*                VEE_NOT_FOUND -
*                    Record was not found
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_find_record (uint8_t sector, uint32_t block, vee_record_t *vee_temp)
{
    /* Pointers for searching */
    vee_record_t *ptr, *end_addr;        
    /* Boolean for whether we found valid record or not */
    uint8_t  record_found = 0;
    
    /* Move past flags at beginning */
    ptr = (vee_record_t *)(((uint8_t *)g_vee_Sectors[sector].VEE_block_addr[block]) +
                           ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t)));
    /* Flags are at beginning of block so end is the real end */
    end_addr =  (vee_record_t *)(((uint8_t *)g_vee_Sectors[sector].VEE_block_addr[block]) + 
                                             g_vee_Sectors[sector].VEE_block_size);
    
    /* Search until end of block */
    while(ptr < end_addr )
    {
        /* Make sure this address is not empty */
        if( vee_blank_check_address((uint8_t *)ptr) == VEE_SUCCESS )
        {
            /* Address was blank so search is done */            
            /* Exit loop */
            break;
        }
        
        /* Check to see if ID's match */
        if( vee_temp->ID == ptr->ID )
        {
            /* Check to make sure record is valid */
            if( vee_check_record(ptr) == VEE_SUCCESS )
            {            
                /* We have a match, copy over record info, keep searching for newer versions though */
                vee_temp->check = ptr->check;
                vee_temp->size = ptr->size;
                vee_temp->pData = (uint8_t far *)&ptr->pData;
                vee_temp->block = block;
                /* Record found */
                record_found = 1;
            }
            else
            {
                /* This record is not valid which means that we should not try to continue onwards. Exit loop. */
                break;                  
            }
        }
                   
        /* Move on to next record */
        ptr = (vee_record_t *)(((uint8_t *)&ptr->pData) + ptr->size);
        
        /* Only need to check boundary if minimum write size is larger than 1 byte */
        if( sizeof(vee_var_min_t) > 1 )
        {
            /* Due to program sizes on some data flashes we need to make sure that we are on a program boundary */
            ptr = (vee_record_t *)vee_move_to_boundary((uint32_t)ptr); 
        }
    }
    
    /* Check to see if record was found */
    if( record_found == 1 )
    {
        /* Record found */
        return VEE_SUCCESS;
    }
    else
    {
        /* Record not found */
        return VEE_NOT_FOUND;
    }
}
/***********************************************************************************************************************
End of function vee_find_record
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_Write
* Description  : Attempts to write a VEE Record to the data flash
* Arguments    : vee_temp - 
*                    Structure with record information
* Return Value : VEE_SUCCESS -  
*                    Successful, write in progress
*                VEE_BUSY -     
*                    Defrag in process, try again later
*                VEE_NO_ROOM -
*                    No room, need to call R_VEE_Erase() 
*                VEE_FAILURE -
*                    Failure
*                VEE_INVALID_INPUT -
*                    Record sent in had invalid data.
***********************************************************************************************************************/
uint8_t R_VEE_Write (vee_record_t * vee_temp)
{
    /* Local variables */
    uint8_t sector, ret;

    /* Check record. */
    if (VEE_SUCCESS != vee_check_input_record(vee_temp, false))
    {
        /* Record was invalid. E.g. record ID is out of range. */
        return VEE_INVALID_INPUT;
    }

    /* Try to grab VEE State */
    if( vee_grab_state(VEE_WRITING) != VEE_SUCCESS )
    {
        /* Could not grab state */
        return VEE_BUSY;
    }
        
    /* Get which sector this record will be stored in */
    sector = g_vee_RecordLocations[vee_temp->ID];
    
#ifdef VEE_IGNORE_DUPLICATE_WRITES
    {
    /* Used for checking entries */        
    vee_record_t VEE_existing;
    /* Loop variable */
    vee_var_data_t i;
    /* Boolean for whether records match */
    uint8_t same;
    
    /* Check to see if the data that the user wants to write is the same as the record already in the VEE.  If so, 
       ignore this new write */
       
    /* Check to see if entry is cached */       
    if( g_vee_cache.entries[vee_temp->ID].valid == VEE_CACHE_ENTRY_VALID )
    {
        /* Copy over record information from data flash */
        memcpy(&VEE_existing, g_vee_cache.entries[vee_temp->ID].address, sizeof(VEE_existing));    
               
        /* Fix pData pointer */
        VEE_existing.pData = ((uint8_t far *)g_vee_cache.entries[vee_temp->ID].address) + offsetof(vee_record_t, pData);
    }
    else
    {
        /* Record not in cache, try to find it */
        
        /* Copy over ID */
        VEE_existing.ID = vee_temp->ID;
        
        /* Make pData 0 so we will know if we found the record or not */
        VEE_existing.pData = 0;
        
        /* Seek for record */
        vee_seek(&VEE_existing);
        
        /* If record was found then go ahead and update cache */
        if( (uint32_t)VEE_existing.pData != 0 )
        {
            /* Record was found, update cache */            
            g_vee_cache.entries[VEE_existing.ID].address = (vee_record_t far *)
                                                           (VEE_existing.pData - offsetof(vee_record_t, pData));

            /* Update which VEE Block the record is in */
            g_vee_cache.entries[VEE_existing.ID].block = VEE_existing.block;
                    
            /* Set cache entry as valid */
            g_vee_cache.entries[VEE_existing.ID].valid = VEE_CACHE_ENTRY_VALID;              
        }
    }
    
    /* Was there an existing record with same ID? */
    if( (uint32_t)VEE_existing.pData != 0 )
    {                        
        /* Check size field */
        if( vee_temp->size == VEE_existing.size )
        {
            /* Check 'check' field */
            if( vee_temp->check == VEE_existing.check )
            {
                /* Check data */
                same = 1;         

                for(i = 0; i < vee_temp->size; i++)
                {
                    /* Check byte by byte */
                    if( vee_temp->pData[i] != VEE_existing.pData[i] )
                    {
                        /* Data does not match */
                        same = 0;
                        
                        break;
                    }
                }
                
                /* Did all data match? */
                if( same == 1 )
                {
                    /* Release state */
                    g_vee_state = VEE_READY;
                    
                    /* Data did match */
                    #ifdef VEE_CALLBACK_FUNCTION
                    /* Call user's callback function */
                    VEE_CALLBACK_FUNCTION();
                    #endif
                    
                    /* No need to write */
                    return VEE_SUCCESS;
                }
                /* Data did not match, continue with write */
            }
        }                            
    }
    }
#endif       
    
    /* Check to see if the next available address cached */
    if( g_vee_next_address[sector].valid != VEE_CACHE_ENTRY_VALID )
    {
        /* Next available write address not in cache.  This means that this is the first write that has been issued.  
           At this point error checking be done on this VEE Sector to see if a power down occurred during a program or 
           erasure.  If a error is detected, the API will start the process of fixing the problem. If no error is 
           detected then this write will proceed on. */
        ret = vee_check_sector(sector);
        
        /* Check return value */
        if( ret == VEE_BUSY )
        {
            /* Something had to be fixed or initialized, come back later */
            return VEE_BUSY;
        }                       
        else if( ret == VEE_NOT_FOUND )
        {
            /* In this case no ACTIVE block was found so a blank check is now ongoing. If it returns that the block is 
               blank then it will write the ACTIVE flag and then the record */

            /* Copy over record information to write to global variable */
            memcpy(&g_vee_current_record, vee_temp, sizeof(g_vee_current_record));               

            /* Record will be written once data flash is setup */                           
            return VEE_SUCCESS;               
        }      
    }
    
    /* Is there room for this write? Need to take into consideration VEE structure as well as record data. */
    if( ((uint32_t)g_vee_next_address[sector].address + vee_temp->size + sizeof(vee_record_t) - 
         sizeof(g_vee_current_record.pData)) > 
        (g_vee_Sectors[sector].VEE_block_addr[g_vee_next_address[sector].block] + g_vee_Sectors[sector].VEE_block_size) )
    {
        /* Change state to signify write and defrag */
        g_vee_state = VEE_WRITE_AND_DEFRAG;
        
        /* Copy over record information to write to global variable */
        memcpy(&g_vee_current_record, vee_temp, sizeof(g_vee_current_record));
        
        /* Not enough room, time for a defrag */
        return R_VEE_Defrag(sector);
    }
    else
    {
        /* Room is available, write the record */
        return vee_start_write(vee_temp);
    }

}
/***********************************************************************************************************************
End of function R_VEE_Write
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_start_write
* Description  : Starts writing of record to data flash
* Arguments    : vee_temp - 
*                    Structure with record information
* Return Value : VEE_SUCCESS -  
*                    Successful, write in progress
*                VEE_FAILURE -  
*                    Failure
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_start_write (vee_record_t * vee_temp)
{
    if( g_vee_next_address[g_vee_RecordLocations[vee_temp->ID]].valid != VEE_CACHE_ENTRY_VALID )
    {
        /* Next available address should already have been filled in */
        return VEE_FAILURE;
    }
    
    /* Init R_g_vee_write_address structure */
    g_vee_write_address = (vee_record_t far *) g_vee_next_address[g_vee_RecordLocations[vee_temp->ID]].address;
                       
    /* Set g_vee_current_sector to update NextAddress later */
    g_vee_current_sector = g_vee_RecordLocations[vee_temp->ID];
                       
    /* Copy over record information to write to global variable */
    memcpy(&g_vee_current_record, vee_temp, sizeof(g_vee_current_record));   
    
    /* Fill in 'block' field */
    g_vee_current_record.block = g_vee_next_address[g_vee_RecordLocations[vee_temp->ID]].block;
    
    /* Write 'ID' structure member */
    R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->ID, 
                  (BUF_PTR_TYPE)&g_vee_current_record.ID,
                  sizeof(g_vee_current_record.ID) );                

    /* Next time in the ISR we will write the 'size' field */
    g_vee_write_state = VEE_WRITE_SIZE;

    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function vee_start_write
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name: vee_check_sector
* Description  : Checks for errors in a sector and finds next blank spot
* Arguments    : sector - 
*                    Which VEE Sector to check
* Return Value : VEE_SUCCESS -  
*                    Successful, sector is valid and ready
*                VEE_BUSY -  
*                    Error found, now API is fixing it
*                VEE_NOT_FOUND -  
*                    No active block found, preparing block
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_check_sector (uint8_t sector)
{
    /* Loop variables */
    uint32_t i;
    /* Structure pointer for VEE Block info */
    vee_block_info_t block_info;
    /* The found active block */
    int32_t active_block;
    int32_t nextup_block; 
    int32_t active_and_full_block;
    
    /* Init variables */
    active_and_full_block = -1;
    nextup_block = -1;
    active_block = -1;    
    
    /* Loop through VEE Blocks in this VEE Sector */
    for(i = 0; i < g_vee_Sectors[sector].num_VEE_blocks; i++)
    {
        /* Get information from status flags about this block */
        vee_get_block_info(sector, i, &block_info);
                
        if( block_info.erasing == VEE_BLOCK_FLAG_SET )
        {
            /* Set global variables which will allow us to erase multiple blocks without user intervention */
            g_vee_erase_block_current = g_vee_Sectors[sector].df_blocks[i][0];
            g_vee_erase_block_end = g_vee_Sectors[sector].df_blocks[i][1];             
            
            /* Set state to erasing */
            g_vee_state = VEE_ERASING;
            
            /* This block was interrupted during an erase, start erase again */
            if( R_FlashErase(g_vee_erase_block_end) != FLASH_SUCCESS )
            {            
                /* Something failed in the FlashAPI */
                FlashError();
            } 
            
            /* Erase in process */
            return VEE_BUSY;
        }
        else if( (block_info.active == VEE_BLOCK_FLAG_SET) &&
                 (block_info.full   == VEE_BLOCK_FLAG_NOT_SET) )
        {
            /* ACTIVE block found, keep searching to make sure other blocks
               are okay (e.g. make sure no blocks failed during erasure) */
            active_block = i;            
        }
        else if( (block_info.active == VEE_BLOCK_FLAG_NOT_SET) &&
                 (block_info.nextup == VEE_BLOCK_FLAG_SET) )
        {
            /* In this case a reset interrupted a defrag. To fix this we need to erase the NEXTUP block and try the 
               defrag again. */
            if( active_and_full_block != -1 )                        
            {   
                /* The ACTIVE & FULL block has already been found */         
                vee_erase_and_defrag(sector, (uint32_t)active_and_full_block, i);
                
                /* Write will have to wait */
                return VEE_BUSY;
            }
            else
            {
                /* We now need to find the ACTIVE & FULL block to start Erase & Defrag */
                nextup_block = i;                
            }
        }
        else if( (block_info.active == VEE_BLOCK_FLAG_SET) &&
                 (block_info.full   == VEE_BLOCK_FLAG_SET) )
        { 
            /* We found a block that was ACTIVE and FULL.  A defrag should have been started */
            if( nextup_block != -1 )
            {
                /* The NEXTUP block has already been found. Erase that block and start defrag again */
                vee_erase_and_defrag(sector, i, (uint32_t)nextup_block);
                
                /* Write will have to wait */
                return VEE_BUSY;
            }
            else
            {
                /* Defrag is needed, find the NEXTUP block if there is one */
                active_and_full_block = i;
            }
        }
    }
    
    /* Check results of searching the sector */
    if( (active_block != -1) && ((active_and_full_block != -1) || (nextup_block != -1)) )
    {
        /* This should not happen, there is a block marked ACTIVE and also another block that is marked ACTIVE or 
           NEXTUP. This likely means that an erase failed.  Try to erase the block again */
        if( active_and_full_block != -1 )
        {
            /* Should invalidate the cache in case records in the FULL block have been read. */
            vee_invalidate_cache(sector);

            /* This block was interrupted during an erase, start erase again */
            vee_erase_block(sector, (uint32_t)active_and_full_block);            
        }
        else
        {
            /* This block was interrupted during an erase, start erase again */
            vee_erase_block(sector, (uint32_t)nextup_block);                        
        }   
        
        /* Erase in process */
        return VEE_BUSY;        
    }
    if( (active_and_full_block != -1) && (nextup_block == -1) )
    {
        /* This catches the case between the FULL flag being written in one block and the NEXTUP flag being written in 
           the other block.  In this case just a defrag is needed */        
        /* Release state so defrag can start */
        g_vee_state = VEE_DEFRAG;
        
        /* Init for defrag */
        g_vee_current_record.ID = 0xFF;
        
        /* Start Defrag */
        vee_defrag_block(sector, 
                         (uint32_t)active_and_full_block, 
                         (uint32_t)((active_and_full_block+1)%g_vee_Sectors[sector].num_VEE_blocks));        
        
        /* Return that a process is now ongoing */
        return VEE_BUSY;
    }
    else if( (active_block != -1) && 
             (active_and_full_block == -1) && 
             (nextup_block == -1) )
    {
        /* Active block found, make sure it's ready to write. If there was a power down during a record program then it 
           could cause an error here */        
        if( vee_check_block(sector, (uint32_t)active_block) == VEE_SUCCESS )
        {
            /* Block is good and ready to use */
            return VEE_SUCCESS;
        }
        else
        {
            /* Bad record was found or block was full, defrag needed */
            /* Release state so defrag can start */
            g_vee_state = VEE_DEFRAG;
            
            /* Init for defrag */
            g_vee_current_record.ID = 0xFF;
            
            /* Start Defrag */
            vee_defrag_block(sector, 
                             (uint32_t)active_block, 
                             (uint32_t)((active_block+1)%g_vee_Sectors[sector].num_VEE_blocks));
            
            /* Defrag is process */
            return VEE_BUSY;                
        }
    }
    else if( (active_and_full_block == -1) &&
             (nextup_block == -1) && 
             (active_block == -1) )
    {
        /* This means that the sector should be empty */
        
        /* If user is requesting defrag then we should ignore since sector is empty */
        if( g_vee_state == VEE_DEFRAG )
        {     
            /* No ACTIVE block found, defrag ignored */
            return VEE_NOT_FOUND;
        }           
        
        /* Init some global state for blank checking */
        g_vee_blank_check_block_current = g_vee_Sectors[sector].df_blocks[0][0];
        g_vee_blank_check_block_end = g_vee_Sectors[sector].df_blocks[0][1];
        g_vee_current_sector = sector;
        
        /* Verify that the sector is empty */
        if( vee_blank_check_block( g_vee_blank_check_block_current ) == VEE_SUCCESS )
        {        
            /* No ACTIVE block found, blank check in progress */
            return VEE_NOT_FOUND;
        }
        else
        {
            /* Problem with FlashAPI */
            FlashError();
        }
    }
    else
    {
        /* This should never happen, this means that you found a NEXTUP block that was not yet marked ACTIVE and did not 
           find another block marked ACTIVE or (ACTIVE and FULL). */
        FlashError();
    }
    
    /* Dummy return to make sure nothing happens if FlashError() returns */
    return VEE_BUSY;
}
/***********************************************************************************************************************
End of function vee_check_sector
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_check_block
* Description  : Checks for errors in a VEE Block
* Arguments    : sector - 
*                    Which VEE Sector this block is in
*                block - 
*                    Which VEE Block to check
* Return Value : VEE_SUCCESS -  
*                    Successful, VEE Block is valid and ready
*                VEE_FAILURE -  
*                    Block has a bad record
*                VEE_NO_ROOM -   
*                    Block is valid, but full
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_check_block (uint8_t sector, uint32_t block)
{
    /* Pointers for searching */
    vee_record_t *ptr, *end_addr;        
    
    /* Move past flags at beginning */
    ptr = (vee_record_t *)(((uint8_t *)g_vee_Sectors[sector].VEE_block_addr[block]) +
                           ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t)));
    /* Flags are at beginning of block so end is the real end */
    end_addr =  (vee_record_t *)(((uint8_t *)g_vee_Sectors[sector].VEE_block_addr[block]) + 
                                             g_vee_Sectors[sector].VEE_block_size);
    
    /* Search until end of block */
    while(ptr < end_addr )
    {
        /* Make sure this address is not empty */
        if( vee_blank_check_address((uint8_t *)ptr) == VEE_SUCCESS )
        {
            /* Address was blank so search is done */ 
            /* Fill in NextAddress cache */
            g_vee_next_address[sector].address = (uint8_t *)ptr;
            g_vee_next_address[sector].block = block;
            g_vee_next_address[sector].valid = VEE_CACHE_ENTRY_VALID;
            
            /* Blank address was found which is what we want */
            return VEE_SUCCESS;
        }
        
        /* Check to make sure record is valid */
        if( vee_check_record(ptr) == VEE_FAILURE )
        {            
            /* Record is invalid */
            return VEE_FAILURE;
        }
                   
        /* Move on to next record */
        ptr = (vee_record_t *)(((uint8_t *)&ptr->pData) + ptr->size);
        
        /* Only need to check boundary if minimum write size is larger than 1 byte */
        if( sizeof(vee_var_min_t) > 1 )
        {
            /* Due to program sizes on some data flashes we need to make sure that we are on a program boundary */
            ptr = ( vee_record_t *)vee_move_to_boundary((uint32_t)ptr); 
        }
    }
    
    /* Block is valid, but full */
    return VEE_NO_ROOM;
}
/***********************************************************************************************************************
End of function vee_check_block
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_Erase
* Description  : Attempts to erase a VEE Sector
* Arguments    : sector - 
*                    Which VEE Sector to erase
* Return Value : VEE_SUCCESS -    
*                    Successful, sector erase in progress
*                VEE_BUSY -    
*                    Other flash operation is on going
*                VEE_FAILURE -    
*                    Failure
*                VEE_INVALID_INPUT -
*                    Invalid sector input.
***********************************************************************************************************************/
uint8_t R_VEE_Erase (uint8_t sector)
{
    /* Loop variables */
    uint32_t i;
    /* Structure pointer for VEE Block info */
    vee_block_info_t block_info;

    /* Check input. */
    if (VEE_SUCCESS != vee_check_input_sector(sector))
    {
        /* Invalid sector input. */
        return VEE_INVALID_INPUT;
    }
    
    /* Try to grab VEE State */
    if( vee_grab_state(VEE_ERASING) != VEE_SUCCESS )
    {
        /* Could not grab state */
        return VEE_BUSY;
    }

    /* Invalidate valid indicator for next valid address in this sector. */
    g_vee_next_address[sector].valid = VEE_CACHE_ENTRY_INVALID;
     
    /* Loop through VEE Blocks in sector and erase */
    for(i = 0; i < g_vee_Sectors[sector].num_VEE_blocks; i++)
    {
        /* Get information from status flags about this block */
        vee_get_block_info(sector, i, &block_info);
        
        /* Check to see if block is empty */
        if( (block_info.active  == VEE_BLOCK_FLAG_SET) ||
            (block_info.full    == VEE_BLOCK_FLAG_SET) ||
            (block_info.nextup  == VEE_BLOCK_FLAG_SET) ||
            (block_info.erasing == VEE_BLOCK_FLAG_SET) )
        {
            /* Mark cache entries from this sector as invalid */
            vee_invalidate_cache(sector);

            /* At least onen flag is set so this VEE Block is not empty */
            return vee_erase_block(sector, i);
        }                
    }
    
    /* If we get here then all blocks were already erased */
    /* Release state */
    g_vee_state = VEE_READY;

    /* Still need to call callback function since code might be waiting. */
    #ifdef VEE_CALLBACK_FUNCTION
    /* Call user's callback function */
    VEE_CALLBACK_FUNCTION();
    #endif
    
    /* All blocks are erased */
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function R_VEE_Erase
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_erase_block
* Description  : Attempts to erase a VEE Block
* Arguments    : sector - 
*                    Which VEE Sector the VEE Block is in
*                block  - 
*                    Which VEE Block to erase
* Return Value : VEE_SUCCESS -  
*                    Successful, block erase in progress
*                VEE_FAILURE -  
*                    Failure
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_erase_block (uint8_t sector, uint32_t block)
{
    /* Set global variables which will allow us to erase multiple blocks without user intervention */
    g_vee_erase_block_current = g_vee_Sectors[sector].df_blocks[block][0];
    g_vee_erase_block_end = g_vee_Sectors[sector].df_blocks[block][1];        
    
    /* Set write state so we know to start erase after write is done */
    g_vee_write_state = VEE_WRITE_DONE;
    
    /* Start erase by first writing the ERASING flag in the block that is getting erased */
    if( VEE_SUCCESS != vee_write_flag(sector, block, VEE_BLOCK_STATE_ERASING, VEE_BLOCK_FLAG_ERASING) )
    {
        /* Flag had already been written, proceed with erase */       
        if( R_FlashErase(g_vee_erase_block_end) != FLASH_SUCCESS )
        {            
            /* Something failed in the FlashAPI */
            FlashError();
        }     
    }
    
    /* Block is being erased */
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function vee_erase_block
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_erase_and_defrag
* Description  : Attempts to erase a VEE Block and then resume a defrag
* Arguments    : sector - 
*                    Which VEE Sector the VEE Block is in
*                active - 
*                    Which VEE Block is marked ACTIVE
*                nextup - 
*                    The block was marked as NEXTUP but never became ACTIVE due to reset
* Return Value : VEE_SUCCESS -  
*                    Successful, block erase in progress
*                VEE_FAILURE -  
*                    Failure
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_erase_and_defrag (uint8_t sector, uint32_t active, uint32_t nextup)
{
    /* Mark state */
    g_vee_state = VEE_ERASE_AND_DEFRAG;
    
    /* Set which sector is active for later defrag */
    g_vee_current_sector = sector;
    
    /* Set which blocks are active and nextup */
    g_vee_current_block_active = active;
    g_vee_current_block_nextup = nextup;     
    
    /* Invalidate cache */
    vee_invalidate_cache(sector);

    /* Start erase of old NEXTUP block */
    return vee_erase_block(sector, nextup);
}
/***********************************************************************************************************************
End of function vee_erase_and_defrag
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name: R_VEE_Defrag
* Description  : Defrags a sector. The user might want to do a defrag when not required during idle time so that a 
*                defrag has less chance of happening during a more busy writing phase.  This function only starts the 
*                defrag.
* Arguments    : sector - 
*                    Which VEE Sector to defrag
* Return Value : VEE_SUCCESS -  
*                    Successful, defrag in progress
*                VEE_BUSY -  
*                    Other VEE operation in progress
*                VEE_NOT_FOUND -  
*                    No ACTIVE block found to defrag  
*                VEE_INVALID_INPUT -
*                    Invalid sector input.
***********************************************************************************************************************/
uint8_t R_VEE_Defrag (uint8_t sector)
{
    /* Local variables */
    uint32_t i;
    uint8_t  ret;
    /* Structure pointer for VEE Block info */
    vee_block_info_t block_info;    

    /* Check input. */
    if (VEE_SUCCESS != vee_check_input_sector(sector))
    {
        /* Invalid sector input. */
        return VEE_INVALID_INPUT;
    }
    
    /* Check to see if defrag is already in process */
    if( g_vee_state == VEE_DEFRAG )
    {
        /* Defrag already in process */
        return VEE_BUSY;        
    }
    else
    {
        /* Grab state if we do not already have it */
        if( g_vee_state != VEE_WRITE_AND_DEFRAG )
        {
            /* Try to grab VEE State */
            if( vee_grab_state(VEE_DEFRAG) != VEE_SUCCESS )
            {
                /* Could not grab state */
                return VEE_BUSY;
            }
        }
        
        /* Check to see if the next available address cached */
        if( g_vee_next_address[sector].valid != VEE_CACHE_ENTRY_VALID )
        {
            /* Next available write address not in cache.  This means that this is the first write or defrag that has 
               been issued.  At this point error checking be done on this VEE Sector to see if a power down occurred 
               during a program or erasure.  If a error is detected, the API will start the process of fixing the 
               problem. If no error is detected then this defrag can proceed on. */
            ret = vee_check_sector(sector);
            
            /* Check return value */
            if( ret == VEE_BUSY )
            {
                /* Something had to be fixed or initialized, come back later */
                return VEE_BUSY;
            }                       
            else if( ret == VEE_NOT_FOUND )
            {
                /* Release state */
                g_vee_state = VEE_READY;
                
                /* Sector was blank, nothing to defrag */
                return VEE_NOT_FOUND;
            }      
        }        
        
        /* Find ACTIVE block */
        /* Loop through VEE Blocks in this VEE Sector */
        for(i = 0; i < g_vee_Sectors[sector].num_VEE_blocks; i++)
        {
            /* Get information from status flags about this block */
            vee_get_block_info(sector, i, &block_info);
                
            if( (block_info.active == VEE_BLOCK_FLAG_SET) &&
                (block_info.full   == VEE_BLOCK_FLAG_NOT_SET) )
            {
                /* Found ACTIVE block */
                
                /* Only init state if we do not have a record to write */
                if( g_vee_state != VEE_WRITE_AND_DEFRAG )
                {
                    /* Init global state used */
                    g_vee_current_record.ID = 0xFF;
                }
                
                /* Start defrag process */                                
                return vee_defrag_block( sector, 
                       i,
                       (i+1)%g_vee_Sectors[sector].num_VEE_blocks );
            }
        }
        
        /* If we got to here then no ACTIVE block was found */
        g_vee_state = VEE_READY;
        
        /* Return ACTIVE block was not found */
        return VEE_NOT_FOUND;
    }
}
/***********************************************************************************************************************
End of function R_VEE_Defrag
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_defrag_block
* Description  : This continues the defrag.  It goes through the records one by one to see if they need to be moved.
* Arguments    : sector - 
*                    Which VEE Sector we are dealing with
*                active - 
*                    The ACTIVE VEE Block being defragged
*                nextup - 
*                    The new VEE Block that will become ACTIVE
* Return Value : VEE_SUCCESS -  
*                    Successful, defrag in progress
*                VEE_BUSY -  
*                    Other VEE operation in progress
*                VEE_FAILURE -  
*                    g_vee_state should already be in defrag mode
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_defrag_block (uint8_t sector, uint32_t active, uint32_t nextup)
{
    /* Temp structure */
    vee_record_t vee_temp;
    
    /* Check to see if defrag is already in process */
    if( (g_vee_state != VEE_DEFRAG) &&
        (g_vee_state != VEE_WRITE_AND_DEFRAG) )
    {
        /* The g_vee_state should already be in defrag mode by now */
        return VEE_FAILURE;        
    }
    else if( (g_vee_current_record.ID == 0xFF) ||
             (g_vee_state == VEE_WRITE_AND_DEFRAG) )
    {
        /* This is first time into function, need to write FULL and NEXTUP flags first */      
           
        /* Save which sector and blocks is being defragged */
        g_vee_current_sector = sector;
        g_vee_current_block_active = active;
        g_vee_current_block_nextup = nextup;      

        /* Init some global state for blank checking */
        g_vee_blank_check_block_current = g_vee_Sectors[sector].df_blocks[nextup][0];
        g_vee_blank_check_block_end = g_vee_Sectors[sector].df_blocks[nextup][1];        
                      
        /* Verify NEXTUP block is empty */
        if( vee_blank_check_block( g_vee_blank_check_block_current ) != VEE_SUCCESS)
        {
            /* Flash error when trying to perform blank check */
            FlashError();
        }                                      
    }
    /* Check to see if we have more records to move */
    else if( (g_vee_current_record.ID >= VEE_MAX_RECORD_ID) &&
             (g_vee_current_record.ID != 0xFF) )
    {        
        /* Now that the records have been moved, we need to write the ACTIVE flag in the NEXTUP block and then erase 
           the old ACTIVE block */          
                              
        /* Write ACTIVE flag */                                        
        if(VEE_FAILURE == vee_write_flag(g_vee_current_sector, 
                                         g_vee_current_block_nextup, 
                                         VEE_BLOCK_STATE_ACTIVE,
                                         VEE_BLOCK_FLAG_ACTIVE) )
        {
            /* Problem writing flag */
            FlashError();
        }
        
        /* Set write state to write the ERASING flag next */
        g_vee_write_state = VEE_WRITE_FLAG_ERASING;                                                
    }
    else
    {
        /* Find next record in this sector that might need to be moved */
        while(VEE_MAX_RECORD_ID > g_vee_current_record.ID)
        {
            /* See if this record resides in the sector being defragged */
            if(g_vee_RecordLocations[g_vee_current_record.ID] == g_vee_current_sector)
            {
                /* Found a record to be moved! */
                /* Continue on with defrag */        
                if( g_vee_cache.entries[g_vee_current_record.ID].valid == VEE_CACHE_ENTRY_VALID )
                {
                    /* Check to see if record is already in NEXTUP block */
                    if( g_vee_cache.entries[g_vee_current_record.ID].block != nextup )
                    {
                        /* Record needs to be moved */                                        
                        memcpy((uint8_t far *)(&vee_temp), 
                               (uint8_t far *)(g_vee_cache.entries[g_vee_current_record.ID].address), 
                               sizeof(vee_record_t));
            
                        /* Copy over data so that pData is correct */                           
                        vee_temp.pData = (uint8_t far *)&((vee_record_t far *)
                                         (g_vee_cache.entries[g_vee_current_record.ID].address))->pData;
                        
                        /* Issue write */
                        return vee_start_write(&vee_temp);
                    }
                    /* Else block has already been moved */
                }
                else
                {
                    /* Check to see if entire cache is valid, if so then this record is empty */
                    if(g_vee_cache.valid != VEE_CACHE_ENTRY_VALID)
                    {
                        /* Cache is not filled, we need to seek for record */
                        if( vee_find_record(sector, active, &g_vee_current_record) == VEE_SUCCESS )
                        {
                            /* Record was found, move it to NEXTUP block */
                            g_vee_current_record.block = nextup;
                
                            /* Issue write */
                            return vee_start_write(&g_vee_current_record);                    
                        }
                        /* Else this record was not found in the ACTIVE block */                
                    }
                    /* Else this record is not in the system */
                }
            }
    
            /* This record does not need to be moved, move to next one */
            g_vee_current_record.ID++;
        }

        /* Now that the records have been moved, we need to write the ACTIVE flag in the NEXTUP block and then 
           erase the old ACTIVE block */          

        /* Write NEXTUP flag */                                        
        if(VEE_FAILURE == vee_write_flag(g_vee_current_sector, 
                                         g_vee_current_block_nextup,
                                         VEE_BLOCK_STATE_ACTIVE,
                                         VEE_BLOCK_FLAG_ACTIVE) )
        {
            /* Problem writing flag */
            FlashError();
        }

        /* Set write state to write the ERASING flag next */
        g_vee_write_state = VEE_WRITE_FLAG_ERASING; 
    }
    /* Continue on with defrag */
    return VEE_SUCCESS;
}
/***********************************************************************************************************************
End of function vee_defrag_block
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_write_flag
* Description  : Writes a flag to a VEE Block
* Arguments    : sector - 
*                    Which VEE Sector we are dealing with
*                block - 
*                    Which VEE Block to write to
*                flag - 
*                    Which flag to write
*                value - 
*                    What to write
* Return Value : VEE_SUCCESS -  
*                    Successful, write in progress
*                VEE_FAILURE -  
*                    Problem in FlashAPI or flag already written
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_write_flag (uint8_t sector, uint32_t block, uint8_t flag, vee_var_min_t value)
{
    /* Pointer used for writing flag */
    uint32_t ptr;        
                                        
    /* Get pointer to base of VEE Block */               
    ptr = g_vee_Sectors[sector].VEE_block_addr[block];
    /* Move to flag */
    ptr += (flag*sizeof(vee_var_min_t));        
    
    /* Should perform blank check on flag to make sure we are not writing on already written data which will cause a 
       flash error */
    if( vee_blank_check_address( (uint8_t *)ptr ) != VEE_SUCCESS )
    {
        /* Flag has already been written. Check to see if it is the correct value. The value would already be set when 
           the VEE system is recovering from a unexpected reset. Example, if the FULL flag was written and a power down 
           occured before NEXTUP was written. */
        if (*(vee_var_min_t *)ptr != value)
        {
            /* In this case the flag address has already been written and it is not the correct value. */                    
            return VEE_FAILURE;
        }
        else
        {
            /* The flag value does match the expected value. Continue on as if the write completed successfully. Extra 
               care will need to be taken in this situation since a write was not actually performed and the 
               FlashWriteDone() function will not be called automatically by the Flash API. */
            return VEE_BUSY;
        }
    }  
    
    /* Write flag */
    if( R_FlashWrite( (FLASH_PTR_TYPE)ptr, (BUF_PTR_TYPE)&value, sizeof(value)) == FLASH_SUCCESS )
    {
        /* Write was started successfully */
        return VEE_SUCCESS;
    }
    else
    {
        /* There was a problem with the FlashAPI */
        return VEE_FAILURE;
    }

}
/***********************************************************************************************************************
End of function vee_write_flag
***********************************************************************************************************************/


/***********************************************************************************************************************
* Function Name: FlashEraseDone
* Description  : Callback function from FlashAPI that signifies that the requested erase has finished. This function is 
*                called from a interrupt.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void FlashEraseDone (void)
{
    /* Check to see if there are any more data flash blocks to erase */
    if( g_vee_erase_block_current != g_vee_erase_block_end )
    {
        /* There are more blocks to erase, keep going */
        g_vee_erase_block_end--;
        
        /* Issue next erase */
        if( R_FlashErase(g_vee_erase_block_end) != FLASH_SUCCESS )
        {            
            /* Error has occurred */
            FlashError();
        }        
    }  
    else
    {
        /* Check to see if defrag was also needed */
        if( (g_vee_state == VEE_ERASE_AND_DEFRAG) ||
            (g_vee_state == VEE_WRITE_AND_DEFRAG) )
        {
            if(g_vee_state == VEE_ERASE_AND_DEFRAG) {
                /* Defrag needed now */
                g_vee_state = VEE_DEFRAG;
            }
            
            /* Write NEXTUP flag */                                        
            vee_write_flag(g_vee_current_sector, 
                           g_vee_current_block_nextup,
                           VEE_BLOCK_STATE_NEXTUP,
                           VEE_BLOCK_FLAG_NEXTUP); 
                          
            /* Update next available write location */
            /* Move past flags at beginning */
            g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                (g_vee_Sectors[g_vee_current_sector].VEE_block_addr[g_vee_current_block_nextup] +
                ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t)));
            
            /* Update block and valid fields */
            g_vee_next_address[g_vee_current_sector].block = g_vee_current_block_nextup;
            g_vee_next_address[g_vee_current_sector].valid = VEE_CACHE_ENTRY_VALID;
                                          
            /* Next time we will start moving records */
            g_vee_write_state = VEE_WRITE_START_DEFRAG;
        
            return;
            
        }
        else if( g_vee_state == VEE_ERASE_AND_WRITE )
        {
            /* Block was erased, now write the record */
            g_vee_state = VEE_WRITING;
            
            /* In this state we are about to make a block ACTIVE */
            /* Make first block ACTIVE */
            if( VEE_SUCCESS == vee_write_flag( g_vee_current_sector, 
                                               0,
                                               VEE_BLOCK_STATE_ACTIVE, 
                                               VEE_BLOCK_FLAG_ACTIVE) )
            {
                /* Write record after this */
                g_vee_write_state = VEE_WRITE_ID;
    
                /* Set up next address cache */
                g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                    g_vee_Sectors[g_vee_current_sector].VEE_block_addr[0] + 
                    ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t));
                g_vee_next_address[g_vee_current_sector].block = 0;
                g_vee_next_address[g_vee_current_sector].valid = VEE_CACHE_ENTRY_VALID;            
    
                /* Writing state flag */
                return;                
            }
            else
            {
                /* Problem with Flash API */
                FlashError();
            }            
        }
        else
        {                        
            /* All erases are done, release state */        
            g_vee_state = VEE_READY;    

            #ifdef VEE_CALLBACK_FUNCTION
            /* Call user's callback function */
            VEE_CALLBACK_FUNCTION();
            #endif            
        }
    }
}
/***********************************************************************************************************************
End of function FlashEraseDone
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: FlashWriteDone
* Description  : Callback function from FlashAPI that signifies that the requested write has finished. This function is 
*                called from a interrupt.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void FlashWriteDone (void)
{    
    /* Check Write State to see what needs to be done */
    if( g_vee_write_state != VEE_WRITE_DONE )
    {
        switch( g_vee_write_state )
        {
            case VEE_WRITE_FLAG_ERASING:
            
                /* Start erase */
                if(VEE_FAILURE == vee_erase_block(g_vee_current_sector, g_vee_current_block_active) )
                {
                    /* Problem writing flag */
                    FlashError();
                }
                
                /* Set state so when write is done we know to start erasing */
                g_vee_state = VEE_ERASING;
            
                /* Write is done */
                g_vee_write_state = VEE_WRITE_DONE;
                                        
                return;
            
            case VEE_WRITE_FLAG_NEXTUP:
            
                /* Write NEXTUP flag */                                        
                vee_write_flag(g_vee_current_sector, 
                               g_vee_current_block_nextup,
                               VEE_BLOCK_STATE_NEXTUP,
                               VEE_BLOCK_FLAG_NEXTUP); 
                              
                /* Update next available write location */
                /* Move past flags at beginning */
                g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                    (g_vee_Sectors[g_vee_current_sector].VEE_block_addr[g_vee_current_block_nextup] +
                    ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t)));
                
                /* Update block and valid fields */
                g_vee_next_address[g_vee_current_sector].block = g_vee_current_block_nextup;
                g_vee_next_address[g_vee_current_sector].valid = VEE_CACHE_ENTRY_VALID;
                                              
                /* Next time we will start moving records */
                g_vee_write_state = VEE_WRITE_START_DEFRAG;
            
                return;
                
            case VEE_WRITE_START_DEFRAG:
            
                /* Check to see if we need to write a new record before defrag */
                if( g_vee_state == VEE_WRITE_AND_DEFRAG )
                {
                    /* We need to write a new record before starting defrag */
                    vee_start_write(&g_vee_current_record);
                }
                else
                {                    
                    /* Init defrag search */
                    g_vee_current_record.ID = 0;                                
                
                    /* Start looking for records to be moved */
                    vee_defrag_block(g_vee_current_sector, g_vee_current_block_active, g_vee_current_block_nextup);  
                }              
            
                return;
            
            case VEE_WRITE_ID:
            
                /* Prepare for rest of write and write 'ID' field */
                vee_start_write(&g_vee_current_record);
                
                return;
                
            case VEE_WRITE_SIZE:
            
                /* Write 'size' structure member */
                R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->size, 
                              (BUF_PTR_TYPE)&g_vee_current_record.size,
                              sizeof(g_vee_current_record.size) );

                /* Next time in we will write the 'block' field */
                g_vee_write_state = VEE_WRITE_BLOCK;
                
                return;
            
            case VEE_WRITE_BLOCK:
            
                /* Write 'block' structure member */
                R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->block, 
                              (BUF_PTR_TYPE)&g_vee_current_record.block,
                              sizeof(g_vee_current_record.block) );

                /* Next time in we will write the 'data' field */
                g_vee_write_state = VEE_WRITE_DATA;
                
                return;
                
            case VEE_WRITE_DATA:
            
                /* Write 'pData' structure member */
                if(sizeof(vee_var_min_t) > 1)
                {
                    /* We have to be careful that we send the FlashAPI a buffer size that is an integer multiple of its 
                       programming size. For example: on the RX62N the data flash can program in 8 byte increments.  If 
                       we have 13 bytes of data we actually need to send 16 to the FlashAPI */
                    R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->pData, 
                        (BUF_PTR_TYPE)g_vee_current_record.pData,
                        (uint16_t)(g_vee_current_record.size + 
                        ((sizeof(vee_var_min_t) - (g_vee_current_record.size % sizeof(vee_var_min_t))) % 
                        sizeof(vee_var_min_t))));
                }
                else
                {
                    /* If the program size is 1 then no worries */
                    R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->pData,
                                  (BUF_PTR_TYPE)g_vee_current_record.pData,
                                  g_vee_current_record.size);
                }

                /* Next time in we will write the 'block' field */
                g_vee_write_state = VEE_WRITE_CHECK;
                
                return;
                
            case VEE_WRITE_CHECK:
            
                /* Write 'check' structure member */
                R_FlashWrite( (FLASH_PTR_TYPE)&g_vee_write_address->check, 
                              (BUF_PTR_TYPE)&g_vee_current_record.check,
                              sizeof(g_vee_current_record.check));
                            
                /* Update cache */
                g_vee_cache.entries[g_vee_current_record.ID].address = g_vee_write_address;
                g_vee_cache.entries[g_vee_current_record.ID].block = g_vee_current_record.block;
                g_vee_cache.entries[g_vee_current_record.ID].valid = VEE_CACHE_ENTRY_VALID;
                
                /* Update next available write location */
                if( sizeof(vee_var_min_t) == 1 )
                {
                    /* No need to align write boundary */
                    g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                        ( ((uint8_t far *)&g_vee_write_address->pData) + 
                          g_vee_current_record.size );
                }
                else
                {
                    /* Need to align write boundary */
                    g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                        vee_move_to_boundary(((uint32_t)&g_vee_write_address->pData) + g_vee_current_record.size);
                }
                
                /* Update block and valid fields */
                g_vee_next_address[g_vee_current_sector].block = g_vee_current_record.block;
                g_vee_next_address[g_vee_current_sector].valid = VEE_CACHE_ENTRY_VALID;

                /* Check to see if we need to defrag now */
                if( g_vee_state != VEE_WRITE_AND_DEFRAG )
                {
                    /* Write is done */
                    g_vee_write_state = VEE_WRITE_DONE;
                }
                else
                {
                    /* Write is done, set state to DEFRAG */
                    g_vee_state = VEE_DEFRAG;
                    
                    /* Next time we will start moving records */
                    g_vee_write_state = VEE_WRITE_START_DEFRAG;                    
                }
                
                return;
            
            default:
                
                /* Should never get here */
                FlashError();
                
                return;
                
        }                       
    }
    else
    {
        /* The record has been written, if we were just writing one record then we are done.  If a defrag is occurring 
           then there could be more records to move. */
        if( g_vee_state == VEE_DEFRAG )
        {
            /* Update g_vee_current_record for next record */
            g_vee_current_record.ID++;
            
            /* See if anything else needs to be done */
            vee_defrag_block(g_vee_current_sector, g_vee_current_block_active, g_vee_current_block_nextup);
        }
        else if( (g_vee_state == VEE_ERASING) || 
                 (g_vee_state == VEE_ERASE_AND_DEFRAG) ||
                 (g_vee_state == VEE_ERASE_AND_WRITE) )
        {
            /* ERASING flag has been written, start erase */
            if( R_FlashErase(g_vee_erase_block_end) != FLASH_SUCCESS )
            {            
                /* Something failed in the FlashAPI */
                FlashError();
            }       
        }        
        else
        {
            /* Release State */
            g_vee_state = VEE_READY;
            
            #ifdef VEE_CALLBACK_FUNCTION
            /* Call user's callback function */
            VEE_CALLBACK_FUNCTION();
            #endif
        }
    }
}
/***********************************************************************************************************************
End of function FlashWriteDone
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: FlashBlankCheckDone
* Description  : Callback function from FlashAPI that signifies that the requested blank check has finished.  This 
*                function is called from an interrupt.
* Arguments    : result - 
*                    FLASH_BLANK if block was blank 
*                    FLASH_NOT_BLANK if it was not
* Return Value : none
***********************************************************************************************************************/
void FlashBlankCheckDone (uint8_t result)
{
    /* Used for checking return value. */
    uint8_t ret;

    /* Check to see if we have checked all blocks, if the last block was not blank then it does not matter anyways. */
    if( result == FLASH_BLANK )
    {        
        /* Last block checked was blank, do we have more to check? */
        if( g_vee_blank_check_block_current != g_vee_blank_check_block_end )
        {
            /* Advance to next block to check */
            g_vee_blank_check_block_current++;
            
            /* Check next block */
            if( VEE_SUCCESS != vee_blank_check_block(g_vee_blank_check_block_current))
            {
                /* Flash Error */
                FlashError();
            }
            
            /* Wait for this blank check to finish */
            return;
        }
        else
        {
            /* All blocks were confirmed blank */
            if( g_vee_state == VEE_WRITING )
            {
                /* In this state we are about to make a block ACTIVE */
                /* Make first block ACTIVE */
                if( VEE_SUCCESS == vee_write_flag( g_vee_current_sector, 
                                                   0,
                                                   VEE_BLOCK_STATE_ACTIVE, 
                                                   VEE_BLOCK_FLAG_ACTIVE) )
                {
                    /* Write record after this */
                    g_vee_write_state = VEE_WRITE_ID;
            
                    /* Set up next address cache */
                    g_vee_next_address[g_vee_current_sector].address = (uint8_t far *)
                        g_vee_Sectors[g_vee_current_sector].VEE_block_addr[0] + 
                        ((VEE_BLOCK_STATE_NEXTUP+1)*sizeof(vee_var_min_t));
                    g_vee_next_address[g_vee_current_sector].block = 0;
                    g_vee_next_address[g_vee_current_sector].valid = VEE_CACHE_ENTRY_VALID;            
            
                    /* Writing state flag */
                    return;                
                }
                else
                {
                    /* Problem with Flash API */
                    FlashError();
                }
            }
            else if( (g_vee_state == VEE_DEFRAG) ||
                     (g_vee_state == VEE_WRITE_AND_DEFRAG) )
            {
                /* NEXTUP block is blank */  
                /* Write FULL flag in current ACTIVE */
                ret = vee_write_flag(g_vee_current_sector, 
                                     g_vee_current_block_active,
                                     VEE_BLOCK_STATE_FULL,
                                     VEE_BLOCK_FLAG_FULL);
                                  
                /* Check outcome of writing FULL flag. */
                if (VEE_SUCCESS == ret)
                {
                    /* Flag is being written. */
                    /* Next time in we will write the NEXTUP flag */
                    g_vee_write_state = VEE_WRITE_FLAG_NEXTUP;                                    
                }
                else if (VEE_BUSY == ret)
                {
                    /* Flag was already written. Since Flash API will not be calling FlashWriteDone(), we must set the 
                       state and call it as if the Flash API did. */
                    g_vee_write_state = VEE_WRITE_FLAG_NEXTUP;
                    
                    /* Call FlashWriteDone() as Flash API would have. */
                    FlashWriteDone();
                }
                else
                {
                    /* Error in writing FULL flag */
                    FlashError();
                }                                                                                                     
            }
            else
            {
                /* Error should never get here */
                FlashError();
            }
        }
    }
    else
    {
        /* Block was found to be not blank, erase sector */
        if( g_vee_state == VEE_WRITING )
        {            
            /* Set state to erase and write so that record can be written after erase */
            g_vee_state = VEE_ERASE_AND_WRITE;
            
            /* Sector had no ACTIVE blocks but block 0 was not empty, erase it */               
            vee_erase_block(g_vee_current_sector, 0);
            
            return;
        }
        else if( g_vee_state == VEE_DEFRAG)
        {
            /* NEXTUP block needs to be erased before defrag can occur */
            if( VEE_SUCCESS != vee_erase_and_defrag(g_vee_current_sector, 
                                                    g_vee_current_block_active, 
                                                    g_vee_current_block_nextup) )
            {
                /* Flash Error */
                FlashError();
            }
            
            /* Erase started */
            return;
        }
        else if(g_vee_state == VEE_WRITE_AND_DEFRAG)
        {
            /* NEXTUP block needs to be erased before we can write record, and then defrag */
            if( VEE_SUCCESS != vee_erase_block(g_vee_current_sector, g_vee_current_block_nextup) )
            {
                /* Problem with erase */
                FlashError();
            }
            
            /* Erase started */
            return;
        }
        else
        {
            /* Error should never get here */
            FlashError();
        }
    
    }                   
}
/***********************************************************************************************************************
End of function FlashBlankCheckDone
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
uint32_t R_VEE_GetVersion (void)
{
    /* These version macros are defined in r_vee_if.h. */
    return ((((uint32_t)VEE_VERSION_MAJOR) << 16) | (uint32_t)VEE_VERSION_MINOR);
} 
/***********************************************************************************************************************
End of function R_VEE_GetVersion
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_check_input_record
* Description  : Check an input record to make sure its data is valid.
* Arguments    : vee_temp - 
*                    Structure with record information
*                read_operation -
*                    Is this a read or write operation. Checking differs for each.
* Return Value : VEE_SUCCESS -  
*                    Successful, record appears okay
*                VEE_INVALID_INPUT -
*                    Failure, record has invalid data
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_check_input_record (vee_record_t * vee_temp, bool read_operation)
{
    uint8_t ret;

    ret = VEE_SUCCESS;

    if (vee_temp->ID >= VEE_MAX_RECORD_ID)
    {
        /* Invalid record ID. */
        ret = VEE_FAILURE;
    }

    if ((read_operation == false) && (vee_temp->size == 0))
    {
        /* Must write at least 1 byte. */
        ret = VEE_FAILURE;
    }

    return ret;
} 
/***********************************************************************************************************************
End of function vee_check_input_record
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_check_input_sector
* Description  : Check an input sector number to make sure it is valid
* Arguments    : sector - 
*                    Sector number to use.
* Return Value : VEE_SUCCESS -  
*                    Successful, valid sector number
*                VEE_INVALID_INPUT -
*                    Failure, invalid sector number
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_check_input_sector (uint8_t sector)
{
    uint8_t ret;

    ret = VEE_SUCCESS;

    if (sector >= VEE_NUM_SECTORS)
    {
        ret = VEE_INVALID_INPUT;
    }

    return ret;
} 
/***********************************************************************************************************************
End of function vee_check_input_sector
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: R_VEE_Control
* Description  : Performs various VEE tasks based on input command and data.
* Arguments    : command -
*                    Command to be processed.
*                pdata - 
*                    Pointer to send data in, output data, or both.
* Return Value : VEE_SUCCESS -
*                    Successful, command processed.
*                VEE_BUSY - 
*                    Data flash is busy, try again later
*                VEE_INVALID_INPUT -
*                    Command not supported or bad input data.
***********************************************************************************************************************/
uint8_t R_VEE_Control (vee_command_t command, void * pdata)
{
    uint8_t ret;

    ret = VEE_SUCCESS;

    switch (command)
    {
        case VEE_CMD_RESET:
            /* Reset the VEE to a working state. This assumes that the FCU has already been reset. If using the Flash 
               API then it will reset itself when a flash error is detected. */
            /* VEE_RESET has priority in taking state, but there may already be another task that is inside the 
               vee_grab_state function so we have to make sure we have sole access. */
            if( vee_grab_state(VEE_RESET) != VEE_SUCCESS )
            {
                /* Could not acquire state */
                return VEE_BUSY;
            }

            /* Reset VEE. */
            vee_reset();

            g_vee_state = VEE_READY;
        break;
        default:
            /* Input command not supported. */
            ret = VEE_INVALID_INPUT;
        break;
    }

    return ret;
} 
/***********************************************************************************************************************
End of function R_VEE_GetVersion
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: vee_invalidate_cache
* Description  : Invalidates the record cache for a given sector
* Arguments    : sector - 
*                    Sector number to use.
* Return Value : VEE_SUCCESS -  
*                    Successful, cache is now marked as invalid
***********************************************************************************************************************/
VEE_STATIC uint8_t vee_invalidate_cache (uint8_t sector)
{
    uint8_t  ret;
    uint32_t i;

    ret = VEE_SUCCESS;

    /* Invalidate next address structure. */
    g_vee_next_address[sector].valid = VEE_CACHE_ENTRY_INVALID;

    /* Invalidate cache entries for this sector. */
    g_vee_cache.valid = VEE_CACHE_ENTRY_INVALID;
    
    for (i = 0; i < VEE_MAX_RECORD_ID; i++)
    {
        /* See if this record is in this sector */
        if (g_vee_RecordLocations[i] == sector)
        {
            g_vee_cache.entries[i].valid = VEE_CACHE_ENTRY_INVALID;
        }
    }

    return ret;
} 
/***********************************************************************************************************************
End of function vee_invalidate_cache
***********************************************************************************************************************/


