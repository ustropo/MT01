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
* File Name    : r_lvd_rx_if.h
* Description  : This file contains the interface functions and enumerations required by the user to use the
*                r_lvd_rx module. This file has to be included by the user application in order to use the module API
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 18.07.2013 1.00    First Release
*         : 14.02.2014 1.10    Added support for RX110, RX113, RX210, RX63N
*         : 22.12.2014 1.30    Added support for RX113.
*         : 18.03.2015 1.40    Added support for RX64M, 71M.
*         : 09.07.2015 1.50    Added support for RX231.
***********************************************************************************************************************/

#ifndef LVD_INTERFACE_HEADER_FILE
#define LVD_INTERFACE_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
/* Includes board and MCU related header files. */
#include "platform.h"
/* Used for configuring the LVD module */
#include "r_lvd_rx_config.h"
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define LVD_RX_VERSION_MAJOR           (1)
#define LVD_RX_VERSION_MINOR           (50)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/*  LVD API Error codes */
typedef enum _lvd_err
{
    LVD_SUCCESS,
    LVD_ERR_VDET,                                   // Illegal value attempted for VDet level.
    LVD_ERR_NOT_INITIALIZED,                        // Channel has not been Opened yet
    LVD_ERR_ILL_REINITIALIZATION,                   // Attempt to Open channel again without first closing
    LVD_ERR_ILL_PARAM,                              // illegal argument not specified in enum
    LVD_ERR_VCC_BELOW_AND_NOT_CROSSED   = 0x10,     // Vcc/CMPA2 is below Vdet and not crossed yet
    LVD_ERR_VCC_BELOW_AND_CROSSED       = 0x11,     // Vcc/CMPA2 is below Vdet and has crossed at least once
    LVD_ERR_VCC_ABOVE_AND_NOT_CROSSED   = 0x12,     // Vcc/CMPA2 is above Vdet and not crossed yet
    LVD_ERR_VCC_ABOVE_AND_CROSSED       = 0x13,     // Vcc/CMPA2 is above Vdet and has crossed at least once
    LVD_ERR_DISABLED                                // LVD Channel is disabled
}lvd_err_t;

typedef enum _lvd_channel_t                     // LVD Channels
{
    LVD_CHANNEL_1 = 1,
    LVD_CHANNEL_2,
    LVD_CHANNEL_INVALID
} lvd_channel_t;

typedef enum                                    // LVD Voltage levels
{
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1))
    LVD_VOLTAGE_CH2_MIN   = 0,
    LVD_VOLTAGE_CH2_2_90v = 0,
    LVD_VOLTAGE_CH2_2_60v,
    LVD_VOLTAGE_CH2_2_00v,
    LVD_VOLTAGE_CH2_1_80v,
    LVD_VOLTAGE_CH2_MAX = LVD_VOLTAGE_CH2_1_80v,

    LVD_VOLTAGE_CH1_MIN   = 4,
    LVD_VOLTAGE_CH1_3_10v = 4,
    LVD_VOLTAGE_CH1_3_00v,
    LVD_VOLTAGE_CH1_2_90v,
    LVD_VOLTAGE_CH1_2_79v,
    LVD_VOLTAGE_CH1_2_68v,
    LVD_VOLTAGE_CH1_2_58v,
    LVD_VOLTAGE_CH1_2_48v,
    LVD_VOLTAGE_CH1_2_06v,
    LVD_VOLTAGE_CH1_1_96v,
    LVD_VOLTAGE_CH1_1_86v,
    LVD_VOLTAGE_CH1_MAX = LVD_VOLTAGE_CH1_1_86v,
    LVD_VOLTAGE_INVALID,
#endif

#if (BSP_MCU_RX210 == 1)
    LVD_VOLTAGE_CH1_MIN  = 0,
    LVD_VOLTAGE_CH1_4_15 = 0,
    LVD_VOLTAGE_CH1_4_00,
    LVD_VOLTAGE_CH1_3_85,
    LVD_VOLTAGE_CH1_3_70,
    LVD_VOLTAGE_CH1_3_55,
    LVD_VOLTAGE_CH1_3_40,
    LVD_VOLTAGE_CH1_3_25,
    LVD_VOLTAGE_CH1_3_10,
    LVD_VOLTAGE_CH1_2_95,
    LVD_VOLTAGE_CH1_2_80,
    LVD_VOLTAGE_CH1_2_65,
    LVD_VOLTAGE_CH1_2_50,
    LVD_VOLTAGE_CH1_2_35,
    LVD_VOLTAGE_CH1_2_20,
    LVD_VOLTAGE_CH1_2_05,
    LVD_VOLTAGE_CH1_1_90,
    LVD_VOLTAGE_CH1_MAX = LVD_VOLTAGE_CH1_1_90,
#if (LVD_CFG_VDET2_VCC_CMPA2 == 0)
    LVD_VOLTAGE_CH2_MIN  = 0,
    LVD_VOLTAGE_CH2_4_15 = 0,
    LVD_VOLTAGE_CH2_4_00,
    LVD_VOLTAGE_CH2_3_85,
    LVD_VOLTAGE_CH2_3_70,
    LVD_VOLTAGE_CH2_3_55,
    LVD_VOLTAGE_CH2_3_40,
    LVD_VOLTAGE_CH2_3_25,
    LVD_VOLTAGE_CH2_3_10,
    LVD_VOLTAGE_CH2_2_95,
    LVD_VOLTAGE_CH2_2_80,
    LVD_VOLTAGE_CH2_2_65,
    LVD_VOLTAGE_CH2_2_50,
    LVD_VOLTAGE_CH2_2_35,
    LVD_VOLTAGE_CH2_2_20,
    LVD_VOLTAGE_CH2_2_05,
    LVD_VOLTAGE_CH2_1_90,
    LVD_VOLTAGE_CH2_MAX = LVD_VOLTAGE_CH2_1_90,
#else
    LVD_VOLTAGE_CH2_MIN   = 1,
    LVD_VOLTAGE_CH2_1_33  = 1,
    LVD_VOLTAGE_CH2_MAX   = LVD_VOLTAGE_CH2_1_33,
#endif
    LVD_VOLTAGE_INVALID,
#endif

#if (BSP_MCU_RX231 == 1)
    LVD_VOLTAGE_CH1_MIN  = 0,
    LVD_VOLTAGE_CH1_4_29 = 0,
    LVD_VOLTAGE_CH1_4_14,
    LVD_VOLTAGE_CH1_4_02,
    LVD_VOLTAGE_CH1_3_84,
    LVD_VOLTAGE_CH1_3_10,
    LVD_VOLTAGE_CH1_3_00,
    LVD_VOLTAGE_CH1_2_90,
    LVD_VOLTAGE_CH1_2_79,
    LVD_VOLTAGE_CH1_2_68,
    LVD_VOLTAGE_CH1_2_58,
    LVD_VOLTAGE_CH1_2_48,
    LVD_VOLTAGE_CH1_2_20,
    LVD_VOLTAGE_CH1_1_96,
    LVD_VOLTAGE_CH1_1_86,
    LVD_VOLTAGE_CH1_MAX = LVD_VOLTAGE_CH1_1_86,
#if (LVD_CFG_VDET2_VCC_CMPA2 == 0)
    LVD_VOLTAGE_CH2_MIN  = 0,
    LVD_VOLTAGE_CH2_4_29 = 0,
    LVD_VOLTAGE_CH2_4_14,
    LVD_VOLTAGE_CH2_4_02,
    LVD_VOLTAGE_CH2_3_84,
    LVD_VOLTAGE_CH2_MAX = LVD_VOLTAGE_CH2_3_84,
#endif
    LVD_VOLTAGE_INVALID,
#endif

#if (BSP_MCU_RX63N == 1)
    LVD_VOLTAGE_CH1_MIN  = 10,
    LVD_VOLTAGE_CH1_2_95 = 10,
    LVD_VOLTAGE_CH1_MAX  = LVD_VOLTAGE_CH1_2_95,
    LVD_VOLTAGE_CH2_MIN  = 10,
    LVD_VOLTAGE_CH2_2_95 = 10,
    LVD_VOLTAGE_CH2_MAX  = LVD_VOLTAGE_CH2_2_95,
    LVD_VOLTAGE_INVALID,
#endif

#if ((BSP_MCU_RX64M == 1) || (BSP_MCU_RX71M == 1))
    LVD_VOLTAGE_CH2_MIN  = 9,
    LVD_VOLTAGE_CH2_2_99 = 9,
    LVD_VOLTAGE_CH2_2_92 = 10,
    LVD_VOLTAGE_CH2_2_85 = 11,
    LVD_VOLTAGE_CH2_MAX  = LVD_VOLTAGE_CH2_2_85,

    LVD_VOLTAGE_CH1_MIN  = 9,
    LVD_VOLTAGE_CH1_2_99 = 9,
    LVD_VOLTAGE_CH1_2_92 = 10,
    LVD_VOLTAGE_CH1_2_85 = 11,
    LVD_VOLTAGE_CH1_MAX  = LVD_VOLTAGE_CH1_2_85,
    LVD_VOLTAGE_INVALID,
#endif
} lvd_voltage_level_t;

typedef enum           // LVD event type
{
    LVD_ACTION_RESET = 0,
    LVD_ACTION_NMI,
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
    LVD_ACTION_IRQ,
#endif
    LVD_ACTION_POLL,
    LVD_ACTION_INVALID
} lvd_action_t;

typedef enum          // LVD trigger type
{
    LVD_TRIGGER_RISE = 0,
    LVD_TRIGGER_FALL,
    LVD_TRIGGER_BOTH,
    LVD_TRIGGER_INVALID,
} lvd_trigger_t;

typedef struct _lvd_config
{
    lvd_action_t e_action;
    lvd_voltage_level_t e_voltage_level;
    lvd_trigger_t e_trigger;
}lvd_config_t;

typedef enum
{
    LVD_CMD_LEVEL_SET,
    LVD_CMD_STATUS_GET,
    LVD_CMD_STATUS_CLEAR,
    LVD_CMD_INVALID
}lvd_cmd_t;

typedef struct st_lvd_lvl_cfg
{
    lvd_channel_t         e_channel;       // LVD Channel
    lvd_voltage_level_t   e_voltage_lvl;   // New voltage level
} lvd_lvl_cfg_t;

typedef struct st_lvd_status
{
    lvd_channel_t         e_channel;       // LVD Channel
} lvd_status_t;

typedef struct
{
    bsp_int_src_t vector;         //Which LVD vector caused this interrupt
} lvd_int_cb_args_t;
/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
uint32_t         R_LVD_GetVersion (void);
lvd_err_t        R_LVD_Open(lvd_channel_t e_channel, lvd_config_t *p_cfg, void (*pcallback)(void *));
lvd_err_t        R_LVD_Control(lvd_cmd_t const e_cmd, void *param);
lvd_err_t        R_LVD_Close(lvd_channel_t e_channel);



#endif /* LVD_INTERFACE_HEADER_FILE */


