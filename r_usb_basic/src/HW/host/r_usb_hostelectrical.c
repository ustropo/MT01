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
* File Name    : r_usb_hostelectrical.c
* Description  : USB Host Electrical Test code
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

#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Function Name   : usb_hstd_TestStop
Description     : Host electrical test stop
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t port   : port number.
Return value    : none
******************************************************************************/
void usb_hstd_TestStop(USB_UTR_t *ptr, uint16_t port)
{
    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_SetUact(ptr, port);
}/* eof usb_hstd_TestStop() */

/******************************************************************************
Function Name   : usb_hstd_TestSignal
Description     : Host electrical test signal control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
                : uint16_t command          : command
Return value    : none
******************************************************************************/
void usb_hstd_TestSignal(USB_UTR_t *ptr, uint16_t port, uint16_t command)
{
/* Condition compilation by the difference of user define */
#if ((( USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
   ||(( USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
    uint16_t    buff;

    switch( command )
    {
    case 1: buff = USB_H_TST_J;         break;
    case 2: buff = USB_H_TST_K;         break;
    case 3: buff = USB_H_TST_SE0_NAK;   break;
    case 4: buff = USB_H_TST_PACKET;    break;

    default:
        buff = USB_H_TST_NORMAL;
        usb_creg_set_utst( ptr, buff );
        usb_cstd_SwReset(ptr);
        break;
    }

    usb_hstd_TestUactControl(ptr, port, (uint16_t)USB_UACTOFF);
    usb_creg_set_utst( ptr, buff );
    usb_hstd_TestUactControl(ptr, port, (uint16_t)USB_UACTON);
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
}/* eof usb_hstd_TestSignal() */

/******************************************************************************
Function Name   : usb_hstd_TestUactControl
Description     : Host electrical test SOF control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
                : uint16_t command          : USB_UACTON / OFF
Return value    : none
******************************************************************************/
void usb_hstd_TestUactControl(USB_UTR_t *ptr, uint16_t port, uint16_t command)
{

    if( command == USB_UACTON )
    {
        /* SOF out disable */
        usb_hreg_set_uact( ptr, port );
    }
    else
    {
        /* SOF out disable */
        usb_hreg_clr_uact( ptr, port );
    }
    /* Wait 1ms */
    usb_cpu_DelayXms((uint16_t)1);
}/* eof usb_hstd_TestUactControl() */

/******************************************************************************
Function Name   : usb_hstd_TestVbusControl
Description     : Host electrical test VBUS control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
                : uint16_t command          : USB_UACTON / OFF
Return value    : none
******************************************************************************/
void usb_hstd_TestVbusControl(USB_UTR_t *ptr, uint16_t port, uint16_t command)
{
    if( command == USB_VBON )
    {
        /* VBUS on */
        usb_creg_set_vbout( ptr, port );
    }
    else
    {
        /* VBUS off */
        usb_creg_clr_vbout( ptr, port );
    }
    /* Wait 1ms */
    usb_cpu_DelayXms((uint16_t)1);
}/* eof usb_hstd_TestVbusControl() */

/******************************************************************************
Function Name   : usb_hstd_TestBusReset
Description     : Host electrical test USB-reset signal control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
Return value    : none
******************************************************************************/
void usb_hstd_TestBusReset(USB_UTR_t *ptr, uint16_t port)
{
    /* USBRST=1, UACT=0 */
    usb_creg_rmw_dvstctr( ptr, port, USB_USBRST, (USB_USBRST | USB_UACT) );

    /* Wait 50ms */
    usb_cpu_DelayXms((uint16_t)50);
    /* USBRST=0 */
    usb_creg_clr_dvstctr( ptr, USB_PORT0, USB_USBRST ); //for UTMI
    usb_cpu_Delay1us( 300 );                            //for UTMI

    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_SetUact(ptr, port);
    /* Wait 10ms or more (USB reset recovery) */
    usb_cpu_DelayXms((uint16_t)20);
}/* eof usb_hstd_TestBusReset() */

/******************************************************************************
Function Name   : usb_hstd_TestSuspend
Description     : Host electrical test suspend control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
Return value    : none
******************************************************************************/
void usb_hstd_TestSuspend(USB_UTR_t *ptr, uint16_t port)
{
    /* SOF out disable */
    usb_hreg_clr_uact( ptr, port );
    /* Wait 1ms */
    usb_cpu_DelayXms((uint16_t)1);
}/* eof usb_hstd_TestSuspend() */

/******************************************************************************
Function Name   : usb_hstd_TestResume
Description     : Host electrical test resume control.
Arguments       : USB_UTR_t *ptr   : USB internal structure. Selects USB channel.
                : uint16_t port    : port number
Return value    : none
******************************************************************************/
void usb_hstd_TestResume(USB_UTR_t *ptr, uint16_t port)
{
    /* RESUME bit on */
    usb_hreg_set_resume( ptr, port );
    /* Wait */
    usb_cpu_DelayXms((uint16_t)20);
    /* RESUME bit off */
    usb_hreg_clr_resume( ptr, port );
    /* SOF on */
    usb_hreg_set_uact( ptr, port );
}/* eof usb_hstd_TestResume() */

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End of file
******************************************************************************/
