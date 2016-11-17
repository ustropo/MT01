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
* File Name    : r_lvd_config.h
* Description  : Configures LVD FIT code
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 18.07.2013 1.00    First Release
*         : 14.02.2014 1.10    Added support for RX110, RX113, RX210, RX63N
*         : 18.03.2015 1.40    Added support for RX64M, 71M.
*         : 09.07.2015 1.50    Moved LVD_CFG_VDET2_VCC_CMPA2 error checking to r_lvd_rx.h.
***********************************************************************************************************************/
#ifndef LVD_CONFIG_HEADER_FILE
#define LVD_CONFIG_HEADER_FILE

/* Includes board and MCU related header files. */
#include "platform.h"

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING
 * Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting
 * Setting to 1 includes parameter checking; 0 compiles out parameter checking */
#define LVD_CFG_PARAM_CHECKING_ENABLE		(BSP_CFG_PARAM_CHECKING_ENABLE)

/*
* SET LVD INTERRUPT PRIORITY WHEN USING MASKABLE INTERRUPTS
* This #define sets the priority level for the LVD interrupts.
*/
#define LVD_CFG_INTERRUPT_PRIORITY_CHANNEL_1  (3)   // 1 lowest, 15 highest
#define LVD_CFG_INTERRUPT_PRIORITY_CHANNEL_2  (3)   // 1 lowest, 15 highest

/* Specify when LVD stabilization should take place
 * set to 0 for stabilization after Vcc > Vdet detection
 * set to 1 for stabilization after assertion of LVD RESET
 * NOTE: LOCO must be operating for setting 1.
 *       Software standby mode is possible only for setting 0.
 */
#define LVD_CFG_STABILIZATION_CHANNEL_1  (0)
#define LVD_CFG_STABILIZATION_CHANNEL_2  (0)

/* This definition determines if LVD channel 2 Vdet2 is compared to Vcc or the voltage on the CMPA2 pin
 * Set to 0 for Vcc voltage
 * Set to 1 for CMPA2 pin input voltage
 * NOTE: CMPA2 input is only available for RX110, 111, 113, 210 and 231.
 */
#define LVD_CFG_VDET2_VCC_CMPA2  (0)

/*
 * Set this definition to 1 to enable code for the channel
 * Set this definition to 0 to disable code for the channel
 */
#define LVD_CFG_CHANNEL_1_USED   (1)
#define LVD_CFG_CHANNEL_2_USED   (1)

#endif /* LVD_CONFIG_HEADER_FILE */


