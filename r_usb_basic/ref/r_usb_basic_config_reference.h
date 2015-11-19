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
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_basic_config_reference.h
* Description  : USB User definition
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/

#ifndef __R_USB_CONFIG_H__
#define __R_USB_CONFIG_H__

#include    "r_usb_defvalue.h"
#include    "r_usb_fixed_config.h"

/*****************************************************************************
Macro definitions (USER DEFINE)
******************************************************************************/
/* 
    Select using DTC module
*/
//    #define USB_DTC_ENABLE                /* defined: Use DTC, undefined: Don't use DTC */


/******************************************************************************
MCU separate setting
*******************************************************************************/

/*
    Select USB mode to USB IP(USBb)

    USB_HOST_PP  : USB Host Mode
    USB_PERI_PP  : USB Peripheral Mode
    USB_NOUSE_PP : Not Used (USBb)
*/
    #define USB_FUNCSEL_USBIP0_PP   USB_PERI_PP

/*
    Select USB mode to USB IP(USBAa/USBA)
    
    USB_HOST_PP  : USB Host Mode
    USB_PERI_PP  : USB Peripheral Mode
    USB_NOUSE_PP : Not Used (USBAa/USBA)
*/
    #define USB_FUNCSEL_USBIP1_PP   USB_HOST_PP

/*
    Select SPEED mode setting for USBAa IP
    
    RX71M: USB_HS_PP / USB_FS_PP
    RX64M: USB_FS_PP (Don't set "USB_HS_PP" when usign RX64M)
*/
    #define USB_SPEED_MODE_PP       USB_FS_PP

/*
    Battery Charging setting ( USB Host Mode )
*/
//    #define USB_HOST_BC_ENABLE            /* defined: Use BC function, undefined: No use BC function */
//    #define USB_BC_DCP_ENABLE            /* defined: Host DCP mode, undefined: Host CDP mode */

/*    Set the user function when USB device is attached if neccesary. */
//    #define USB_BC_ATTACH(ptr, data1, data2)       usb_cstd_DummyFunction(ptr, data1, data2)

/*
    Compliance Test Setting ( USB Host Mode )
*/
//    #define USB_HOST_COMPLIANCE_MODE     /* defined: Host COMPLIANCE mode, undefined: Host normal mode */
//    #define USB_HS_EL_TEST               /* defined: Host ELECTRICAL TEST mode, undefine: Host Normal mode */

/*    Set the user function when USB device is attached if neccesary. */
//    #define USB_COMPLIANCE_DISP(ptr, data1, data2)       usb_cstd_DummyFunction(ptr, data1, data2)

#define USB_RESPONCE_COUNTER_VALUE     (6000u)

/*
    Overcurrent Setting ( USB Host Mode )
*/
#define USB_OVERCURRENT(ptr, data1, data2)               usb_cstd_DummyFunction(ptr, data1, data2)

/*
    Battery Charging setting ( USB Peripheral Mode )
*/
//    #define USB_PERI_BC_ENABLE

/*
    CPU byte endian select

    USB_BYTE_LITTLE_PP : Setting Little endian to USB IP.
    USB_BYTE_BIG_PP    : Setting Big endian to USB IP.
*/
#define USB_CPUBYTE_PP                  USB_BYTE_LITTLE_PP

/* 
    Select CPU Low Power Mode

    USB_LPWR_NOT_USE_PP : No used CPU Low power mode
    USB_LPWR_USE_PP     : Used CPU Low power mode
*/
#define USB_CPU_LPW_PP                  USB_LPWR_NOT_USE_PP

/*
    Output debugging message in a console of IDE.

    USB_DEBUG_OFF_PP : No output the debugging message
    USB_DEBUG_ON_PP  : Output the debugging message
*/
#define USB_DEBUG_OUTPUT_PP             USB_DEBUG_OFF_PP

/* 
    Debug Hook function call setting
*/
#define USB_DEBUG_HOOK_USE /* Undefined : No call the debug Hook Function */

/*****************************************************************************
Macro definitions (Peripheral Mode)
******************************************************************************/
/* Max of string descriptor */
#define USB_STRINGNUM                   (7u)

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
********************************************************************************/
#include "r_usb_sysdef.h"

#endif  /* __R_USB_CONFIG_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
