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
* File Name    : r_usb_hstdfunction.c
* Description  : USB Host standard request related functions.
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
extern void (*usb_ghstd_EnumarationProcess[8])(USB_UTR_t *,uint16_t, uint16_t);    
/* Enumeration Table */

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Renesas Abstracted Host Standard functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_Bchg0Function
Description     : Execute the process appropriate to the status of the connected 
                : USB device when a BCHG interrupt occurred.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void usb_hstd_Bchg0Function(USB_UTR_t *ptr)
{
    uint16_t    buf, connect_inf;

    /* SUSPENDED check */
    if( usb_ghstd_RemortPort[USB_PORT0] == USB_SUSPENDED )
    {
        /* Device State Control Register - Resume enable check */
        buf = usb_creg_read_dvstctr( ptr, USB_PORT0 );

        if( (uint16_t)(buf & USB_RESUME) == USB_RESUME )
        {
            USB_PRINTF0("remote wakeup port0\n");
            usb_ghstd_RemortPort[USB_PORT0] = USB_DEFAULT;
            /* Change device state to resume */
            usb_hstd_DeviceResume(ptr, (uint16_t)(USB_PORT0 + USB_DEVICEADDR));
        }
        else
        {
            /* Decide USB Line state (ATTACH) */
            connect_inf = usb_hstd_ChkAttach(ptr, (uint16_t)USB_PORT0);
            if( connect_inf == USB_DETACH )
            {
                usb_ghstd_RemortPort[USB_PORT0] = USB_DEFAULT;
                /* USB detach process */
                usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT0);
            }
            else
            {
                /* Enable port BCHG interrupt */
                usb_hstd_BchgEnable(ptr, (uint16_t)USB_PORT0);
                /* Check clock */
                usb_hstd_ChkClk(ptr, (uint16_t)USB_PORT0, (uint16_t)USB_SUSPENDED);
            }
        }
    }
    else
    {
        /* USB detach process */
        usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT0);
    }
}
/******************************************************************************
End of function usb_hstd_Bchg0Function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_LsConnectFunction
Description     : Low-speed device connect.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_LsConnectFunction(USB_UTR_t *ptr)
{
    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
}
/******************************************************************************
End of function usb_hstd_LsConnectFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_AttachFunction
Description     : Device attach.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_AttachFunction(void)
{
    /* 100ms wait */
    usb_cpu_DelayXms((uint16_t)100);
}
/******************************************************************************
End of function usb_hstd_AttachFunction
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_Ovrcr0Function
Description     : Set USB registers as required due to an OVRCR (over-current)
                : interrupt, and notify the MGR (manager) task about this.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void usb_hstd_Ovrcr0Function(USB_UTR_t *ptr)
{
    /* Over-current bit check */
    USB_PRINTF0(" OVCR int port0\n");
    /* OVRCR interrupt disable */
    /* Notification over current */
    usb_hstd_OvcrNotifiation(ptr, (uint16_t)USB_PORT0);
}
/******************************************************************************
End of function usb_hstd_Ovrcr0Function
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumFunction1
Description     : Device enumeration function nr 1.
Arguments       : none
Return value    : uint16_t              : USB_DONE
******************************************************************************/
uint16_t usb_hstd_EnumFunction1(void)
{
    return USB_DONE;
}
/******************************************************************************
End of function usb_hstd_EnumFunction1
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumFunction2
Description     : Device enumeration function nr 2.
Arguments       : uint16_t *enummode    : Enumeration mode.
Return value    : uint16_t              : USB_YES
******************************************************************************/
uint16_t usb_hstd_EnumFunction2(uint16_t* enummode)
{
    return USB_YES;
}
/******************************************************************************
End of function usb_hstd_EnumFunction2
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumFunction3
Description     : Device enumeration function nr 3.
Arguments       : uint16_t devaddr      : Device address
                : uint16_t enum_seq     : 
Return value    : none
******************************************************************************/
void usb_hstd_EnumFunction3(USB_UTR_t *ptr, uint16_t devaddr, uint16_t enum_seq)
{
    (*usb_ghstd_EnumarationProcess[5])(ptr, devaddr, enum_seq);
}
/******************************************************************************
End of function usb_hstd_EnumFunction3
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumFunction4
Description     : Device enumeration function nr 4.
Arguments       : uint16_t *reqnum      : Request number.
                : uint16_t *enummode    : 
                : uint16_t devaddr      : Device address.
Return value    : none
******************************************************************************/
void usb_hstd_EnumFunction4(uint16_t* reqnum, uint16_t* enummode, uint16_t devaddr)
{
}
/******************************************************************************
End of function usb_hstd_EnumFunction4
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumFunction5
Description     : Device enumeration function nr 5.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_EnumFunction5(void)
{
    USB_PRINTF0(" Get_DeviceDescrip(8-2)\n");
}
/******************************************************************************
End of function usb_hstd_EnumFunction5
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
