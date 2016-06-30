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
* File Name	   : r_crc_rx.c
* Description  : Uses the RX CRC peripheral
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 28.02.2012 1.00    First Release            
*         : 10.05.2012 1.10    Updated to be compliant with FIT Module Spec v0.7
*         : 13.02.2013 1.20    Updated to be compliant with FIT Module Spec v1.02. Changed API for R_CMT_Compute() 
*                              because existing API had no way of informing user if lock was not able to be obtained.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed-size integer typedefs. */
#include <stdint.h>
/* bool support. */
#include <stdbool.h>
/* Has intrinsic support. Includes xchg() which is used in this code. */
#include <machine.h>
/* Includes board and MCU related header files. */
#include "platform.h"
/* Configuration for this package. */
#include "r_crc_rx_config.h"
/* Header file for this package. */
#include "r_crc_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Error checking on configuration options. */
#if (CRC_CFG_POLY_X8_X2_X_1 + CRC_CFG_POLY_X16_X15_X2_1 + CRC_CFG_POLY_X16_X12_X5_1) > 1
    #error "Error! Only choose one CRC polynomial in r_crc_rx_config.h"
#endif

#if defined(CRC_CFG_MSB_FIRST) && defined(CRC_CFG_LSB_FIRST)
    #error "Error! Only choose MSB-first or LSB-first in r_crc_rx_config.h"
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Determine whether CRC peripheral has been enabled. */
static bool    g_crc_enabled = false;

/* Internal functions. */
static bool crc_acquire_state(void);
static void crc_release_state(void);

/***********************************************************************************************************************
* Function Name: R_CRC_Init
* Description  : Initializes the CRC for given input.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void R_CRC_Init (void)
{
    /* Enable the CRC peripheral if needed. */
#if defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX11_ALL)
    /* Enable writing to MSTP registers. */
    SYSTEM.PRCR.WORD = 0xA502;
#endif

    /* Enable the CRC peripheral */
    MSTP(CRC) = 0;
    
#if defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX63_ALL) || defined(BSP_MCU_RX11_ALL)
    /* Disable writing to MSTP registers. */
    SYSTEM.PRCR.WORD = 0xA500;
#endif

    /* Set peripheral as initialized. */
    g_crc_enabled = true;
    
    /* Set polynomial. */
#if   defined(CRC_CFG_POLY_X8_X2_X_1)
    CRC.CRCCR.BIT.GPS = 1;
#elif defined(CRC_CFG_POLY_X16_X15_X2_1)
    CRC.CRCCR.BIT.GPS = 2;
#elif defined(CRC_CFG_POLY_X16_X12_X5_1)
    CRC.CRCCR.BIT.GPS = 3;
#else
    #error "Error! Choose CRC Polynomial in r_crc_rx_config.h";
#endif

    /* Set MSB-first or LSB-first. */
#if   defined(CRC_CFG_MSB_FIRST)
    CRC.CRCCR.BIT.LMS = 1;
#elif defined(CRC_CFG_LSB_FIRST)
    CRC.CRCCR.BIT.LMS = 0;
#else
    #error "Error! Choose MSB or LSB first for CRC code in r_crc_rx_config.h";
#endif

    /* Perform register clear on CRCDOOR. */
    CRC.CRCCR.BIT.DORCLR = 1;
}

/***********************************************************************************************************************
* Function Name: R_CRC_Compute
* Description  : Compute the CRC for the input data given.
* Arguments    : seed - 
*                    Data to initialize the CRC calculation with
*                data - 
*                    Address of data to use
*                data_bytes - 
*                    Number of bytes of data
*                crc_out - 
*                    Address of where to store computed CRC value.
* Return Value : true -
*                    CRC value computed.
*                false -
*                    CRC peripheral is busy with another request or it was not initialized.
***********************************************************************************************************************/
bool R_CRC_Compute (uint16_t seed, uint8_t * data, uint32_t data_bytes, uint16_t * const crc_out)
{
    /* Loop variable. */
    uint32_t i;
    /* Used for CRC calculation. */
    uint16_t crc_read;

    /* Check to make sure peripheral has been initialized. */
    if (g_crc_enabled == false)
    {
        /* Must initialize peripheral first. */
        return false;
    }

    /* Grab state to make sure we do not interfere with another operation. */
    if (crc_acquire_state() != true)
    {
        /* Another operation is already in progress */
        return false;
    }
       
    /* Seed CRC */
    CRC.CRCDOR = seed;  
      
    /* Compute CRC-16 on data */
    for(i = 0; i < data_bytes; i++)
    {
        /* Update CRC value */
        CRC.CRCDIR = data[i];
    }

    /* Check generated CRC versus stored */
    crc_read = CRC.CRCDOR;
        
    /* Release state so other operations can be performed. */
    crc_release_state();

#if   defined(CRC_CFG_POLY_X8_X2_X_1)
    /* If 8-bit CRC is used then only the low order byte of CRCDOR is updated. Mask off top byte to make sure nothing
       is there. */
    crc_read = (crc_read & 0x00FF);
#endif

    /* Store CRC reading into output pointer. */
    *crc_out = crc_read;

    return true;    
} 

/***********************************************************************************************************************
* Function Name: crc_acquire_state
* Description  : Attempt to acquire the state so that we right to perform an operation.
* Arguments    : none
* Return Value : true - 
*                    Lock was obtained
*                false - 
*                    Lock was not obtained because code is busy with another on-going operation.
***********************************************************************************************************************/
static bool crc_acquire_state (void)
{
    /* Attempt to acquire lock. */
    return R_BSP_HardwareLock(BSP_LOCK_CRC);   
}

/***********************************************************************************************************************
* Function Name: crc_release_state
* Description  : Release lock so that other operations can be performed.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
static void crc_release_state (void)
{
    /* Release lock. */
    R_BSP_HardwareUnlock(BSP_LOCK_CRC);    
}

/***********************************************************************************************************************
* Function Name: R_CRC_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
#pragma inline(R_CRC_GetVersion)
uint32_t R_CRC_GetVersion (void)
{
    /* These version macros are defined in r_cmt_rx_if.h. */
    return ((((uint32_t)CRC_RX_VERSION_MAJOR) << 16) | (uint32_t)CRC_RX_VERSION_MINOR);
} 

