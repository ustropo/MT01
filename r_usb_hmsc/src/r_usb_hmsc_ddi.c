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
* File Name    : r_usb_hmsc_ddi.c
* Description  : USB Host MSC BOT ddi
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_hatapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver define */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_api.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_ClassCheck
Description     : check class
Arguments       : USB_CLSINFO_t *mess : message
Return value    : none
******************************************************************************/
void usb_hmsc_ClassCheck(USB_UTR_t *ptr, USB_CLSINFO_t *mess)
{
    uint16_t ofset, result;
    uint16_t strage_drive_no;

    result = USB_DONE;
    switch( usb_shmsc_InitSeq[ptr->ip] )
    {
    case USB_SEQ_0:
        /* Check device count */
        if( usb_ghmsc_StrgCount == USB_MAXSTRAGE )
        {
            USB_PRINTF0("### max device count over(HMSC) !\n");
            result = USB_ERROR;
        }

        /* Check drive count */
        if( usb_ghmsc_MaxDrive >= USB_MAXDRIVE )
        {
            USB_PRINTF1("    max drive over %d(HMSC) .\n", usb_ghmsc_MaxDrive);
            result = USB_ERROR;
        }

        strage_drive_no = R_usb_hmsc_alloc_drvno( ptr->ip, usb_ghmsc_Devaddr[ptr->ip] );

        /* Descriptor check */
        ofset = usb_hmsc_SmpBotDescriptor(ptr, usb_ghmsc_InterfaceTable[ptr->ip], strage_drive_no);
        if( ofset == USB_ERROR )
        {
            USB_PRINTF0("### Descriptor search error(HMSC) !\n");
            result = USB_ERROR;
        }

        /* Serial number check */
        if( result != USB_ERROR )
        {
            /* no string device (STALL) */
            if( usb_ghmsc_DeviceTable[ptr->ip][14] == 0
                && usb_ghmsc_DeviceTable[ptr->ip][15] == 0
                && usb_ghmsc_DeviceTable[ptr->ip][16] == 0 ) {

                ofset = usb_hmsc_SmpPipeInfo(ptr, usb_ghmsc_InterfaceTable[ptr->ip]
                            , strage_drive_no, usb_ghmsc_Speed[ptr->ip]
                            , (uint16_t)usb_ghmsc_ConfigTable[ptr->ip][2]);
                if( ofset == USB_ERROR ) {
                    USB_PRINTF0("### Device information error !\n");
                }
                R_usb_hstd_ReturnEnuMGR(ptr, ofset);    /* return to MGR */
                usb_shmsc_InitSeq[ptr->ip] = USB_SEQ_0;
                return;
            }

            ofset = usb_hmsc_GetStringDescriptor1(ptr, usb_ghmsc_Devaddr[ptr->ip],
                     (uint16_t)usb_ghmsc_DeviceTable[ptr->ip][16],
                     (USB_CB_t)usb_hmsc_class_check_result );
            usb_shmsc_InitSeq[ptr->ip]++;
        }
        break;

    case USB_SEQ_1:
        ofset = usb_hmsc_GetStringDescriptor1Check(ptr, mess->result);
        if( ofset == USB_ERROR )
        {
            result = USB_ERROR;
        }
        else
        {
            ofset = usb_hmsc_GetStringDescriptor2(ptr, usb_ghmsc_Devaddr[ptr->ip],
                     (uint16_t)usb_ghmsc_DeviceTable[ptr->ip][15],
                     (USB_CB_t)usb_hmsc_class_check_result );
            usb_shmsc_InitSeq[ptr->ip]++;
        }
        break;

    case USB_SEQ_2:
        /* Serial number check */
        ofset = usb_hmsc_GetStringDescriptor2Check(ptr, mess->result);
        if( ofset == USB_ERROR )
        {
            result = USB_ERROR;
        }
        
        ofset = usb_hmsc_GetStringInfoCheck(ptr, usb_ghmsc_Devaddr[ptr->ip]);
        if( ofset == USB_ERROR )
        {
            USB_PRINTF0("*** This device is No Serial Number\n");
            result = USB_ERROR;
        }
        
        if( result != USB_ERROR )
        {

            strage_drive_no = R_usb_hmsc_ref_drvno( ptr->ip, usb_ghmsc_Devaddr[ptr->ip] );

            /* Pipe Information table set */
            ofset = usb_hmsc_SmpPipeInfo(ptr, usb_ghmsc_InterfaceTable[ptr->ip],
                     strage_drive_no, usb_ghmsc_Speed[ptr->ip],
                     (uint16_t)usb_ghmsc_ConfigTable[ptr->ip][2]);
            if( ofset == USB_ERROR )
            {
                USB_PRINTF0("### Device information error !\n");
            }
            /* Return to MGR */
            R_usb_hstd_ReturnEnuMGR(ptr, ofset);
            usb_shmsc_InitSeq[ptr->ip] = USB_SEQ_0;
        }
        break;

    default:
        result = USB_ERROR;
        break;
    }
    
    if( result == USB_ERROR )
    {
        usb_shmsc_InitSeq[ptr->ip] = USB_SEQ_0;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR(ptr, USB_ERROR);
    }
}   /* eof usb_hmsc_ClassCheck() */


/******************************************************************************
Function Name   : usb_hmsc_ClrData
Description     : data clear
Arguments       : uint16_t len : 
                : uint8_t *buf : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClrData(uint16_t len, uint8_t *buf)
{
    uint16_t    i;

    for( i = 0; i < len; ++i )
    {
        *buf++ = 0x00;
    }
}   /* eof usb_hmsc_ClrData() */


/******************************************************************************
End  Of File
******************************************************************************/
