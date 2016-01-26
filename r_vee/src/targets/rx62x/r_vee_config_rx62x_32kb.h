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
* File Name    : r_vee_config_rx62x.h
* Version      : 1.00
* Description  : Contains different sector and record configurations for the RX62x group.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.07.2011 1.00    First Release
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
    #elif VEE_NUM_SECTORS == 3
const uint8_t g_vee_RecordLocations[VEE_MAX_RECORD_ID] = 
{
    0,  /* Record 0 will be in sector 0 */
    0,  /* Record 1 will be in sector 0 */
    0,  /* Record 2 will be in sector 0 */
    1,  /* Record 3 will be in sector 1 */
    1,  /* Record 4 will be in sector 1 */
    1,  /* Record 5 will be in sector 1 */
    2,  /* Record 6 will be in sector 2 */
    2,  /* Record 7 will be in sector 2 */                            
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
    0x104000  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect0_df_blocks[][2] = 
{ 
    {BLOCK_DB0, BLOCK_DB7},  /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB8, BLOCK_DB15}  /* Start & end DF blocks making up VEE Block 1 */
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
        16384,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect0_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        8,  
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
    0x102000  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect0_df_blocks[][2] = 
{ 
    {BLOCK_DB0, BLOCK_DB3}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB4, BLOCK_DB7}  /* Start & end DF blocks making up VEE Block 1 */
};
/* Sector 1 */
const uint32_t g_vee_sect1_block_addresses[] = 
{ 
    0x104000, /* Start address of VEE Block 0 */
    0x106000 /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect1_df_blocks[][2] = 
{ 
    {BLOCK_DB8,  BLOCK_DB11}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB12, BLOCK_DB15}  /* Start & end DF blocks making up VEE Block 1 */
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
        8192,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect0_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        4,  
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
        8192,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect1_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        4,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect1_df_blocks
    }    
    
    /* To add more sectors copy the one above and change the values */
}; 

#elif VEE_NUM_SECTORS == 3
/* This uses different size sectors */

/* For each VEE Sector to be used the user will need to fill in an array
   entry in the constant array below. */
/* Sector 0 */
const uint32_t g_vee_sect0_block_addresses[] = 
{ 
    0x100000, /* Start address of VEE Block 0 */
    0x102000  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect0_df_blocks[][2] = 
{ 
    {BLOCK_DB0, BLOCK_DB3}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB4, BLOCK_DB7}  /* Start & end DF blocks making up VEE Block 1 */
};
/* Sector 1 */
const uint32_t g_vee_sect1_block_addresses[] = 
{ 
    0x104000, /* Start address of VEE Block 0 */
    0x105800  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect1_df_blocks[][2] = 
{ 
    {BLOCK_DB8,  BLOCK_DB10}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB11, BLOCK_DB13}  /* Start & end DF blocks making up VEE Block 1 */
};

/* Sector 2 */
const uint32_t g_vee_sect2_block_addresses[] = 
{ 
    0x107000, /* Start address of VEE Block 0 */
    0x107800  /* Start address of VEE Block 1 */
};
const uint32_t g_vee_sect2_df_blocks[][2] = 
{ 
    {BLOCK_DB14,  BLOCK_DB14}, /* Start & end DF blocks making up VEE Block 0 */
    {BLOCK_DB15,  BLOCK_DB15}  /* Start & end DF blocks making up VEE Block 1 */
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
        8192,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect0_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        4,  
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
        6144,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect1_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        3,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect1_df_blocks
    }    
    ,
    /* Sector 2 */
    { 
        /* ID is 2 */ 
        2, 
        /* There are 2 VEE Blocks in this sector */
        2,
        /* Size of each VEE Block */
        2048,
        /* Starting addresses for each VEE Block */
        (const uint32_t *)g_vee_sect2_block_addresses, 
        /* Number of data flash blocks per VEE Block
         (End Block # - Start Block # + 1) */
        1,  
        /* Start & end DF blocks making up VEE Blocks */
        g_vee_sect2_df_blocks
    }    
        
    /* To add more sectors copy the one above and change the values */
}; 

    #endif //VEE_NUM_SECTORS

#else //VEE_MEM_DECLARE

extern const uint8_t g_vee_RecordLocations[];
extern const vee_sector_t g_vee_Sectors[];

#endif //VEE_MEM_DECLARE

#endif //VEE_SECTOR_CONFIG_H
