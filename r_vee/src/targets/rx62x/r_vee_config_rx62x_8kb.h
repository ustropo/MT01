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
* File Name    : r_vee_config_rx62x_8kb.h
* Version      : 1.00
* Description  : Contains different sector and record configurations for RX62x MCUs with 8KB of data flash.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 02.01.2013 1.00    First Release
***********************************************************************************************************************/

#ifndef VEE_SECTOR_CONFIG_H
#define VEE_SECTOR_CONFIG_H

/***********************************************************************************************************************
VEE Sector Configuration
***********************************************************************************************************************/
/* Make structures extern unless VEE_MEM_DECLARE is defined */
#ifdef VEE_MEM_DECLARE

/***********************************************************************************************************************
Constant data structure that contains information about VEE Record locations.
Each entry in the array corresponds to which sector the record will be located in. Records can only be allocated to 
valid sectors. For example, if VEE_NUM_SECTORS is defined to 1 then there is only 1 sector therefore every record
will be located in sector 0.
***********************************************************************************************************************/
#if VEE_MAX_RECORD_ID == 8
    #if VEE_NUM_SECTORS == 1 
const uint8_t g_vee_RecordLocations[VEE_MAX_RECORD_ID] = 
{
    0,  /* Record 0 will be in sector 0 */
    0,  /* Record 1 will be in sector 0 */
    0,  /* Record 2 will be in sector 0 */
    0,  /* Record 3 will be in sector 0 */
    0,  /* Record 4 will be in sector 0 */
    0,  /* Record 5 will be in sector 0 */
    0,  /* Record 6 will be in sector 0 */
    0,  /* Record 7 will be in sector 0 */                            
};
    #elif VEE_NUM_SECTORS == 2
const uint8_t g_vee_RecordLocations[VEE_MAX_RECORD_ID] = 
{
    0,  /* Record 0 will be in sector 0 */
    0,  /* Record 1 will be in sector 0 */
    0,  /* Record 2 will be in sector 0 */
    0,  /* Record 3 will be in sector 0 */
    1,  /* Record 4 will be in sector 1 */
    1,  /* Record 5 will be in sector 1 */
    1,  /* Record 6 will be in sector 1 */
    1,  /* Record 7 will be in sector 1 */                            
};
    #else
    #error "A configuration with this number of sectors and records is not yet specified. Please specify your own."
    #endif /* VEE_NUM_SECTORS */
#else
    /* If the user specifies a different number of records from the default (8) then they will need to make copy the 
       g_vee_RecordLocations[] array from above and specify where records will be located. */
    #error "g_vee_RecordLocations[] array is not defined for this number of records (specified by VEE_MAX_RECORD_ID)"           
#endif /* VEE_MAX_RECORD_ID */

/***********************************************************************************************************************
Constant data structure that contains information about VEE Sector setup
***********************************************************************************************************************/
#if VEE_NUM_SECTORS == 1 

/* For each VEE Sector to be used the user will need to fill in an array entry in the constant array below. */
/* Sector 0 */
const uint32_t g_vee_sect0_block_addresses[] = 
{ 
    0x100000, /* Start address of VEE Block 0 */
    0x101000  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect0_df_blocks[][2] = 
{ 
    {BLOCK_DB0, BLOCK_DB1},  /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB2, BLOCK_DB3}  /* Start & end DF blocks making up VEE Block 1 */
};
/* To add more sectors copy the constant arrays below and change the values */

const vee_sector_t g_vee_Sectors[ VEE_NUM_SECTORS ] = 
{
    /* Sector 0 */
    { 
        /* ID is 0 */ 
        0, 
        /* There are 2 VEE Blocks in this sector */
        2,
        /* Size of each VEE Block */
        4096,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect0_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        2,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect0_df_blocks
    }
    
    /* To add more sectors copy the one above and change the values */
}; 

#elif VEE_NUM_SECTORS == 2

/* For each VEE Sector to be used the user will need to fill in an array
   entry in the constant array below. */
/* Sector 0 */
const uint32_t g_vee_sect0_block_addresses[] = 
{ 
    0x100000, /* Start address of VEE Block 0 */
    0x100800  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect0_df_blocks[][2] = 
{ 
    {BLOCK_DB0, BLOCK_DB0}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB1, BLOCK_DB1}  /* Start & end DF blocks making up VEE Block 1 */
};
/* Sector 1 */
const uint32_t g_vee_sect1_block_addresses[] = 
{ 
    0x101000, /* Start address of VEE Block 0 */
    0x101800 /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect1_df_blocks[][2] = 
{ 
    {BLOCK_DB2, BLOCK_DB2}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB3, BLOCK_DB3}  /* Start & end DF blocks making up VEE Block 1 */
};

/* To add more sectors copy the constant arrays below and change the values */

const vee_sector_t g_vee_Sectors[ VEE_NUM_SECTORS ] = 
{
    /* Sector 0 */
    { 
        /* ID is 0 */ 
        0, 
        /* There are 2 VEE Blocks in this sector */
        2,
        /* Size of each VEE Block */
        2048,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect0_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        1,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect0_df_blocks
    }
    ,
    /* Sector 1 */
    { 
        /* ID is 1 */ 
        1, 
        /* There are 2 VEE Blocks in this sector */
        2,
        /* Size of each VEE Block */
        2048,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect1_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        1,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect1_df_blocks
    }    
    
    /* To add more sectors copy the one above and change the values */
}; 

#endif //VEE_NUM_SECTORS

#else //VEE_MEM_DECLARE

extern const uint8_t g_vee_RecordLocations[];
extern const vee_sector_t g_vee_Sectors[];

#endif //VEE_MEM_DECLARE

#endif //VEE_SECTOR_CONFIG_H
