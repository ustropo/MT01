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
* File Name    : r_vee_config.h
* Description  : Virtual EEPROM user configuration header file
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.07.2011 1.00    First Release
*         : 03.01.2012 1.50    Updated for internal CS and for FIT. Added support for RX63x Groups.
*         : 14.09.2012 1.60    Updated for FIT v0.7 Spec. Fixed bug found when reset occurred after FULL flag was 
*                              written and before NEXTUP was written. VEE now handles this event.
***********************************************************************************************************************/

#ifndef VEE_USER_CONFIG_H
#define VEE_USER_CONFIG_H

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Number of VEE Sectors to be used. When setting this value there must be an associated sector configuration in the
   header file r_vee_config_*MCU Group*.h. For example, if this is set to 2, and the RX62N is chosen, then there must
   be a 2 sector configuration in r_vee_config_rx62x.h. If this file does not contain a sector configuration for the
   chosen amount of sectors then it should be created. This file can also be editted to change the size of sectors. */
#define VEE_NUM_SECTORS         1

/* The number of unique records to be used. It is not recommended to put many more entries than you will use since there 
   is a cache entry for each possible unique ID. If you change this value from the default (8) then you need to define
   a new g_vee_RecordLocations[] array in the r_vee_config_*MCU Group*.h header file for your port. */
#define VEE_MAX_RECORD_ID       (8)

/* This option allows the user to select to ignore record writes where the record being written is the same as the 
   record already stored in the VEE. This will save space but takes extra time to check when performing a write. */
#define VEE_IGNORE_DUPLICATE_WRITES    

/* This configuration value lets you choose whether you want to fill the cache all at once, or one record at a time.  
   If this DEFINE is uncommmented then when a read is performed and a record is not found in the cache (e.g. first read 
   after reset) then all the records will be searched for and the entire cache will be filled in at once.  If this is 
   commented out then only the record which was requested will be searched for.  This comes down to do you want to 
   spread out the time required for searching or do you want to get it all of out of the way at the very beginning 
   so that subsequent reads will not require searching? */
#define VEE_CACHE_FILL_ALL

/* Lets the user choose whether to use the default R_VEE_GenerateCheck() and vee_check_record() functions or whether 
   they want to write their own. For example, if the user wanted to use a Checksum instead of the supplied CRC-16 check 
   then they could comment out this define and write their own functions. If the user writes their own function they 
   must still use the same function name and arguments. */
#define VEE_USE_DEFAULT_CHECK_FUNCTIONS   

/* If this is defined then when a VEE operation finishes the defined function will be called. For example, if you have 
   this:
   #define  VEE_CALLBACK_FUNCTION    Foo 
   then when a program finishes the function Foo() will be called.
   */
#define     VEE_CALLBACK_FUNCTION   VEE_OperationDone_Callback

#endif //VEE_USER_CONFIG_H


