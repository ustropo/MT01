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
* File Name    : r_vee_if.h
* Description  : Interface file Virtual EEPROM implementation using MCU's data flash memory.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 12.07.2011 1.00    First Release
*         : 03.01.2012 1.50    Updated for internal CS and for FIT. Added support for RX63x Groups.
*         : 14.09.2012 1.60    Updated for FIT v0.7 Spec. Fixed bug found when reset occurred after FULL flag was 
*                              written and before NEXTUP was written. VEE now handles this event.
*         : 03.01.2013 1.70    Added R_VEE_Open() function to initialize or reset VEE. Created r_vee_target.h to replace
*                              multiple r_vee_<mcu>.h files that had duplicate information. Updated to be compliant with
*                              FIT v1.00 specification. This means that config file is now in 'ref' folder. Tested with
*                              RX62G, RX210, and RX63T. Added R_VEE_Control() function.
***********************************************************************************************************************/

#ifndef VEE_IF_H
#define VEE_IF_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* This includes user configurable items for the VEE project */
#include "r_vee_config.h"
/* From r_bsp. Gives us MCU information used to configure VEE project. */
#include "platform.h"
/* Private VEE header file. Used for getting vee_var_*_t typedefs. */
#include "r_vee.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define VEE_VERSION_MAJOR           (1)
#define VEE_VERSION_MINOR           (70)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* VEE Record Structure */
typedef struct
{
    /* Unique record identifier, cannot be 0xFF! */
    vee_var_data_t    ID;
    /* Number of bytes of data for this record */
    vee_var_data_t    size;
    /* Valid or error checking field */
    vee_var_data_t    check;
    /* Which VEE Block this record is located in. THIS IS FOR VEE USE ONLY - USERS SHOULD NOT MODIFY THIS! */
    vee_var_data_t    block;
    /* Pointer to record data */
    uint8_t    far  * pData;    
} vee_record_t;

/* Commands that can be sent to R_VEE_Control() function. */
typedef enum  
{ 
    /* This command will reset the VEE even if it is in the middle of an operation. This should only be used when a 
       flash error (e.g. data flash access during VEE operation) has occurred and you need to return the VEE to a 
       working state. */
    VEE_CMD_RESET
} vee_command_t;

/* Return values for functions */
typedef enum  
{ 
    VEE_SUCCESS,
    VEE_FAILURE,
    VEE_BUSY,
    VEE_NO_ROOM,
    VEE_NOT_FOUND,
    VEE_ERROR_FOUND,
    VEE_INVALID_INPUT
} vee_return_values_t;

/* Defines the possible states of the VEE */
typedef enum 
{ 
    VEE_READY, 
    VEE_READING, 
    VEE_WRITING, 
    VEE_ERASING,
    VEE_DEFRAG,
    VEE_ERASE_AND_DEFRAG,
    VEE_WRITE_AND_DEFRAG,
    VEE_ERASE_AND_WRITE,
    VEE_OPENING,
    VEE_RESET
} vee_states_t;

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
uint8_t      R_VEE_Read(vee_record_t * VEE_temp);
uint8_t      R_VEE_Write(vee_record_t * VEE_temp);
uint8_t      R_VEE_Defrag(uint8_t sector);
uint8_t      R_VEE_Erase(uint8_t sector);
vee_states_t R_VEE_GetState(void);
uint8_t      R_VEE_ReleaseState(void);
uint8_t      R_VEE_GenerateCheck(vee_record_t *record);
uint8_t      R_VEE_Open(void);
uint8_t      R_VEE_Control(vee_command_t command, void * pdata);
/* If a callback function is used (decided by setting in r_vee_config.h) then make a prototype for it. */
#ifdef      VEE_CALLBACK_FUNCTION
/* Callback function prototype */
void         VEE_CALLBACK_FUNCTION(void);
#endif

#endif //VEE_IF_H


