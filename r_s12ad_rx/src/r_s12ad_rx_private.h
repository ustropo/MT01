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
/******************************************************************************
* File Name	   : r_s12ad_rx_private.h
* Description  : Definitions and structures used internally by the driver.
*******************************************************************************/
/******************************************************************************
* History : DD.MM.YYYY Version Description
*           22.07.2013 1.00    Initial Release.
*           21.04.2014 1.20    Updated for RX210 advanced features; RX110/63x support.
*******************************************************************************/

#ifndef S12AD_PRIVATE_H
#define S12AD_PRIVATE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "platform.h"
#include "r_s12ad_rx_if.h"

/******************************************************************************
Macro definitions
*******************************************************************************/

#if (defined(BSP_MCU_RX110) || defined(BSP_MCU_RX111))

#if BSP_PACKAGE_PINS == 64
// valid: 1111 1111 0101 1111; invalid 0000 0000 1010 0000
#define ADC_INVALID_CH_MASK     (0xFFFF00A0)

#elif BSP_PACKAGE_PINS == 48
// valid: 1001 1111 0100 0111; invalid 0110 0000 1011 1000
#define ADC_INVALID_CH_MASK     (0xFFFF60B8)

#elif BSP_PACKAGE_PINS == 40
// valid: 0001 1111 0100 0110; invalid 1110 0000 1011 1001
#define ADC_INVALID_CH_MASK     (0xFFFFE0B9)

#elif BSP_PACKAGE_PINS == 36
// valid: 0001 1111 0000 0110; invalid 1110 0000 1111 1001
#define ADC_INVALID_CH_MASK     (0xFFFFE0F9)

#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

#endif


#ifdef BSP_MCU_RX21_ALL

#if BSP_PACKAGE_PINS == 145
#define ADC_INVALID_CH_MASK     (0xFFFF0000)	// all channels valid (0-15)

#elif BSP_PACKAGE_PINS == 144
#define ADC_INVALID_CH_MASK     (0xFFFF0000)

#elif BSP_PACKAGE_PINS == 100
#define ADC_INVALID_CH_MASK     (0xFFFF0000)

#elif BSP_PACKAGE_PINS == 80
// valid: 0011 1111 1111 1111; invalid 1100 0000 0000 0000
#define ADC_INVALID_CH_MASK     (0xFFFFC000)

#elif BSP_PACKAGE_PINS == 64
// valid: 0011 1111 0101 1111; invalid 1100 0000 1010 0000
#define ADC_INVALID_CH_MASK     (0xFFFFC0A0)

#elif BSP_PACKAGE_PINS == 48
// valid: 0001 1110 0100 0111; invalid 1110 0001 1011 1000
#define ADC_INVALID_CH_MASK     (0xFFFFE1B8)
#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

#endif


#ifdef BSP_MCU_RX63_ALL

#if BSP_PACKAGE_PINS == 177
#define ADC_INVALID_CH_MASK     (0xFFF00000)	// all channels valid (0-20)

#elif BSP_PACKAGE_PINS == 176
#define ADC_INVALID_CH_MASK     (0xFFF00000)

#elif BSP_PACKAGE_PINS == 145
#define ADC_INVALID_CH_MASK     (0xFFF00000)

#elif BSP_PACKAGE_PINS == 144
#define ADC_INVALID_CH_MASK     (0xFFF00000)

#elif BSP_PACKAGE_PINS == 100
#define ADC_INVALID_CH_MASK     (0xFFFFC000)    // channels 0-13 valid

#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

#endif


/******************************************************************************
Typedef definitions
*******************************************************************************/
typedef struct st_adc_ctrl          // ADC Control Block
{
    adc_mode_t      mode;           // operational mode
    bool            opened;
    void          (*callback)(void *p_args);
} adc_ctrl_t;


/* ADCSR register ADCS field (non-63x) */
typedef enum e_adc_adcs
{
    ADC_ADCS_SINGLE_SCAN=0,
    ADC_ADCS_GROUP_SCAN=1,
    ADC_ADCS_CONT_SCAN=2,
    ADC_ADCS_MAX
} adc_adcs_t;

/******************************************************************************
Exported global variables
*******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
*******************************************************************************/

#endif
