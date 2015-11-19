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
* File Name    : r_usb_hsignal.c
* Description  : Host USB signalling
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
#include "r_usb_reg_access.h"


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Constant macro definitions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Renesas Abstracted Host Signal functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_VbusControl
Description     : USB VBUS ON/OFF setting.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t port     : Port number.
                : uint16_t command  : ON / OFF.
Return value    : none
******************************************************************************/
void usb_hstd_VbusControl(USB_UTR_t *ptr, uint16_t port, uint16_t command)
{
    if( command == USB_VBON )
    {
        usb_creg_set_vbout( ptr, port );
#ifdef USB_HOST_BC_ENABLE
        if(USB_BC_SUPPORT_IP == ptr->ip)
        {
            usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_VB](ptr, port);
        }
#endif
    }
    else
    {
        usb_creg_clr_vbout( ptr, port );
    }
}
/******************************************************************************
End of function usb_hstd_VbusControl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SuspendProcess
Description     : Set USB registers as required when USB Device status is moved
                : to "Suspend".  
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t port  : Port number.
Return value    : none
******************************************************************************/
void usb_hstd_SuspendProcess(USB_UTR_t *ptr, uint16_t port)
{
    /* SUSPENDED check */
    if( usb_ghstd_RemortPort[port] == USB_SUSPENDED )
    {
        /* SOF OFF */
        usb_hreg_clr_uact( ptr, port );

        /* Wait */
        usb_cpu_DelayXms((uint16_t)1);
        usb_hstd_ChkSof(ptr, port);
        /* RWUPE=1, UACT=0 */
        usb_hreg_set_rwupe( ptr, port );

        /* Enable port BCHG interrupt */
        usb_hstd_BchgEnable(ptr, port);
        /* Wait */
        usb_cpu_DelayXms((uint16_t)5);
    }
    else
    {
        /* SOF OFF */
        usb_hreg_clr_uact( ptr, port );
        /* Wait */
        usb_cpu_DelayXms((uint16_t)5);
    }
}
/******************************************************************************
End of function usb_hstd_SuspendProcess
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_Attach
Description     : Set USB registers as required when USB device is attached, 
                : and notify MGR (manager) task that attach event occurred.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t result       : Result.
                : uint16_t port         : Port number.
Return value    : none
******************************************************************************/
void usb_hstd_Attach(USB_UTR_t *ptr, uint16_t result, uint16_t port)
{
    /* DTCH  interrupt enable */
    usb_hstd_DtchEnable(ptr, port);
    /* Interrupt Enable */
    usb_cstd_BerneEnable(ptr);
    /* USB Mng API */
    usb_hstd_NotifAtorDetach(ptr, result, port);
#ifdef USB_HOST_BC_ENABLE
    if(USB_BC_SUPPORT_IP == ptr->ip)
    {
        usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_AT](ptr, port);
    }
#endif /* USB_HOST_BC_ENABLE */
}
/******************************************************************************
End of function usb_hstd_Attach
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_Detach
Description     : Set USB register as required when USB device is detached, and 
                  notify MGR (manager) task that detach occurred.
Arguments       : uint16_t port         : Port number.
Return value    : none
******************************************************************************/
void usb_hstd_Detach(USB_UTR_t *ptr, uint16_t port)
{
#ifdef USB_HOST_BC_ENABLE
    if(USB_BC_SUPPORT_IP == ptr->ip)
    {
        usb_hstd_bc_func[g_usb_hstd_bc[ptr->ip].state][USB_BC_EVENT_DT](ptr, port);
    }
#endif /* USB_HOST_BC_ENABLE */

    /* DVSTCTR clear */
    usb_creg_clr_dvstctr( ptr, port, (uint16_t)(USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT) );

    /* ATTCH interrupt enable */
    usb_hstd_AttchEnable(ptr, port);

    /* USB Mng API */
    usb_hstd_NotifAtorDetach(ptr, (uint16_t)USB_DETACH, port);
}
/******************************************************************************
End of function usb_hstd_Detach
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_InitConnect
Description     : Execute attach or detach and return USB connect status.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t port   : Port number.
                : uint16_t else_connect_inf : Else USB Port connect Information.
Return value    : uint16_t         ; connection status
                :                  ; (USB_ATTACHF/USB_ATTACHL/USB_DETACH/USB_DONE)
******************************************************************************/
uint16_t usb_hstd_InitConnect(USB_UTR_t *ptr, uint16_t port, uint16_t else_connect_inf )
{
    uint16_t        connect_inf;

    usb_hreg_clr_sts_attch( ptr, port );
    usb_hreg_clr_sts_dtch( ptr, port );

    /* VBUS out */
    usb_hstd_VbusControl(ptr, port, (uint16_t)USB_VBON);
    
#ifndef USB_HOST_BC_ENABLE
    usb_cpu_DelayXms((uint16_t)100);    /* 100ms wait */
#endif /* ! USB_HOST_BC_ENABLE */
    
    connect_inf = usb_hstd_ChkAttach(ptr, port);

    switch( connect_inf )
    {
    case USB_ATTACHL:
        usb_hstd_Attach(ptr, connect_inf, port);
        break;
    case USB_ATTACHF:
        usb_hstd_Attach(ptr, connect_inf, port);
        break;
    case USB_DETACH:
        /* USB detach */
        usb_hstd_Detach(ptr, port);
        /* Check clock */
#if USB_PORTSEL_PP == USB_1PORT_PP
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_DETACHED);
#else   /* USB_PORTSEL_PP == USB_1PORT_PP */
        usb_hstd_ChkClk2(ptr, else_connect_inf );
#endif  /* USB_PORTSEL_PP == USB_1PORT_PP */
        break;
    default:
        /* USB detach */
        usb_hstd_Detach(ptr, port);
        /* Check clock */
#if USB_PORTSEL_PP == USB_1PORT_PP
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_DETACHED);
#else   /* USB_PORTSEL_PP == USB_1PORT_PP */
        usb_hstd_ChkClk2(ptr, else_connect_inf );
#endif  /* USB_PORTSEL_PP == USB_1PORT_PP */
        break;
    }

    return connect_inf;
}
/******************************************************************************
End of function usb_hstd_InitConnect
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
