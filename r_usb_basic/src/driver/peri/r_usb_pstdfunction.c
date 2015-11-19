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
* File Name    : r_usb_pStdFunction.c
* Description  : USB Peripheral standard function code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_reg_access.h"


#if (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP)

/******************************************************************************
External variables and functions
******************************************************************************/
void    usb_pstd_SetStallPipe0( USB_UTR_t *ptr );

/******************************************************************************
Renesas Abstracted Peripheral standard function functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetFeatureFunction
Description     : Process a SET_FEATURE request.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetFeatureFunction(USB_UTR_t *ptr)
{
    /* Request error */
    usb_pstd_SetStallPipe0(ptr);
}
/******************************************************************************
End of function usb_pstd_SetFeatureFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ChkVbsts
Description     : Return the VBUS status.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return          : uint16_t connection status(ATTACH/DETACH)
******************************************************************************/
uint16_t usb_pstd_ChkVbsts(USB_UTR_t *ptr)
{
    uint16_t    buf1, buf2, buf3;
    uint16_t    connect_info;

    /* VBUS chattering cut */
    do
    {
        buf1 = usb_creg_read_intsts( ptr );
        usb_cpu_Delay1us((uint16_t)10);
        buf2 = usb_creg_read_intsts( ptr );
        usb_cpu_Delay1us((uint16_t)10);
        buf3 = usb_creg_read_intsts( ptr );
    }
    while( ((buf1 & USB_VBSTS) != (buf2 & USB_VBSTS))
          || ((buf2 & USB_VBSTS) != (buf3 & USB_VBSTS)) );

    /* VBUS status judge */
    if( (buf1 & USB_VBSTS) != (uint16_t)0 )
    {
        /* Attach */
        connect_info = USB_ATTACH;
    }
    else
    {
        /* Detach */
        connect_info = USB_DETACH;
    }
    return connect_info;
}
/******************************************************************************
End of function usb_pstd_ChkVbsts
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_AttachFunction
Description     : Processing for attach detect.(Waiting time of stabilize VBUS.)
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_AttachFunction(USB_UTR_t *ptr)
{
    /* Delay about 10ms(Waiting time of stabilize VBUS.) */
    usb_cpu_DelayXms((uint16_t)10);
}
/******************************************************************************
End of function usb_pstd_AttachFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_BusresetFunction
Description     : Processing for USB bus reset detection.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_BusresetFunction(USB_UTR_t *ptr)
{
}
/******************************************************************************
End of function usb_pstd_BusresetFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SuspendFunction
Description     : Processing for suspend signal detection.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SuspendFunction(USB_UTR_t *ptr)
{
}
/******************************************************************************
End of function usb_pstd_SuspendFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_InitFunction
Description     : Call function that checks VBUS status.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
uint16_t usb_pstd_InitFunction(USB_UTR_t *ptr)
{
    /* Wait USB_VBSTS */
    return usb_pstd_ChkVbsts( ptr );
}
/******************************************************************************
End of function usb_pstd_InitFunction
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
