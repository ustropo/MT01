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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_s12ad_rx_if.h
* Description  : 12-bit A/D Converter driver interface file.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           22.07.2013 1.00    Initial Release.
*           21.04.2014 1.20    Updated for RX210 advanced features; RX110/63x support.
*           05.06.2014 1.30    Fixed channel mask bug in adc_enable_chans()
***********************************************************************************************************************/

#ifndef S12AD_IF_H
#define S12AD_IF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"

/* MCU SPECIFIC USER INTERFACE FILES */

#if (defined(BSP_MCU_RX110) || defined(BSP_MCU_RX111))
#include "./src/targets/rx110_rx111/r_s12ad_rx110_rx111_if.h"
#elif defined(BSP_MCU_RX210)
#include "./src/targets/rx210/r_s12ad_rx210_if.h"
#elif defined(BSP_MCU_RX63_ALL)
#include "./src/targets/rx63x/r_s12ad_rx63x_if.h"
#endif

/******************************************************************************
Macro definitions
*******************************************************************************/
/* Version Number of API. */
#define ADC_VERSION_MAJOR       (1)
#define ADC_VERSION_MINOR       (30)

/*****************************************************************************
Typedef definitions
******************************************************************************/

typedef enum e_adc_err      // ADC API error codes
{
    ADC_SUCCESS = 0,
    ADC_ERR_AD_LOCKED,      // Open() call is in progress elsewhere
    ADC_ERR_AD_NOT_CLOSED,  // peripheral still running in another mode
    ADC_ERR_MISSING_PTR,    // missing required pointer argument
    ADC_ERR_INVALID_ARG,    // argument is not valid for parameter
    ADC_ERR_ILLEGAL_ARG,    // argument is illegal for mode
    ADC_ERR_SCAN_NOT_DONE,  // default, Group A, or Group B scan not done
    ADC_ERR_UNKNOWN         // unknown hardware error
} adc_err_t;


/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */

typedef enum e_adc_cb_evt           // callback function events
{
    ADC_EVT_SCAN_COMPLETE,          // normal/Group A scan complete
    ADC_EVT_SCAN_COMPLETE_GROUPB    // Group B scan complete
} adc_cb_evt_t;

typedef struct st_adc_cb_args       // callback arguments
{
    adc_cb_evt_t   event;
} adc_cb_args_t;


/*****************************************************************************
Public Functions
******************************************************************************/
adc_err_t R_ADC_Open(adc_mode_t const   mode,
                     adc_cfg_t * const  p_cfg,
                     void               (* const p_callback)(void *p_args));

adc_err_t R_ADC_Control(adc_cmd_t const cmd,
                        void * const    p_args);

adc_err_t R_ADC_Read(adc_reg_t const    reg_id,
                     uint16_t * const   p_data);

adc_err_t R_ADC_ReadAll(adc_data_t * const  p_all_data);

void R_ADC_Close(void);

uint32_t  R_ADC_GetVersion(void);

                                    
#endif /* S12AD_IF_H */
