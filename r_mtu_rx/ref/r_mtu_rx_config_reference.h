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
/***********************************************************************************************************************
* File Name     : r_mtu_rx_config.h
* Description   : Common configuration file for all FIT MTU modules.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 23.09.2014 1.00     First Release
***********************************************************************************************************************/
#ifndef MTU_CONFIG_HEADER_FILE
#define MTU_CONFIG_HEADER_FILE

#include "platform.h"

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Checking of arguments passed to MTU API functions can be enable or disabled.
 * Disabling argument checking is provided for systems that absolutely require faster and smaller code.
 * By default the module is configured to use the setting of the system-wide BSP_CFG_PARAM_CHECKING_ENABLE macro.
 * This can be overridden for the local module by redefining MTU_CFG_PARAM_CHECKING_ENABLE.
 * To control parameter checking locally, set MTU_CFG_PARAM_CHECKING_ENABLE to 1 to enable it,
 * otherwise set to 0 skip checking.
 */
#define MTU_CFG_PARAM_CHECKING_ENABLE  (BSP_CFG_PARAM_CHECKING_ENABLE)

#define MTU_CFG_REQUIRE_LOCK (1)

/* Enable the MTU channels to use in this build. (0) = not used. (1) = used. */
#define MTU_CFG_USE_CH0  (1)
#define MTU_CFG_USE_CH1  (1)
#define MTU_CFG_USE_CH2  (1)
#ifndef BSP_MCU_RX110
#define MTU_CFG_USE_CH3  (1)
#define MTU_CFG_USE_CH4  (1)
#endif

/* Code for unused MTU functions can be excluded from the build to reduce size. (0) = not used. (1) = used. */
#define MTU_CFG_USE_TIMER   (1)
#define MTU_CFG_USE_CAPTURE (1)
#define MTU_CFG_USE_PWM     (1)

/* Set interrupt priority levels for each channel present.
 * Priority is shared by all interrupt sources in a channel.
 * Values must be in the range 0 (interrupt disabled) to 15 (highest)*/
#define MTU_IR_PRIORITY_CHAN0 (3)
#define MTU_IR_PRIORITY_CHAN1 (3)
#define MTU_IR_PRIORITY_CHAN2 (3)
#define MTU_IR_PRIORITY_CHAN3 (3)
#define MTU_IR_PRIORITY_CHAN4 (3)
#define MTU_IR_PRIORITY_CHAN5 (3)

/* Set the MTU input capture noise filter clock for each channels to used for input capture.
 * See r_mtu_timer_rx_if.h for allowable selections. The following settings are default and
 * should be changed as needed. The available settings are defined in "r_mtu_timer_rx_if.h" */
#define MTU_CFG_FILT_CHAN0  (MTU_FILTER_PCLK_DIV_1)
#define MTU_CFG_FILT_CHAN1  (MTU_FILTER_PCLK_DIV_1)
#define MTU_CFG_FILT_CHAN2  (MTU_FILTER_PCLK_DIV_1)
#define MTU_CFG_FILT_CHAN3  (MTU_FILTER_PCLK_DIV_1)
#define MTU_CFG_FILT_CHAN4  (MTU_FILTER_PCLK_DIV_1)


/**********************************************************************************************************************/
#endif /* MTU_CONFIG_HEADER_FILE */
