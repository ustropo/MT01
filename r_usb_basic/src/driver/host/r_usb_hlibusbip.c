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
* File Name    : r_usb_hlibusbip.c
* Description  : USB IP Host library.
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
Renesas Abstracted Host Lib IP functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_SetDevAddr
Description     : Set USB speed (Full/Hi) of the connected USB Device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t addr             : device address
                : uint16_t speed            : device speed
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_SetDevAddr(USB_UTR_t *ptr, uint16_t addr, uint16_t speed, uint16_t port)
{
    if( addr == USB_DEVICE_0 )
    {
        usb_creg_write_dcpmxps( ptr, (uint16_t)(USB_DEFPACKET + USB_DEVICE_0));
    }
    usb_hreg_set_usbspd( ptr, addr, (speed | port) );
}
/******************************************************************************
End of function usb_hstd_SetDevAddr
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_BchgEnable
Description     : Enable BCHG interrupt for the specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_BchgEnable(USB_UTR_t *ptr, uint16_t port)
{

    usb_hreg_clr_sts_bchg( ptr, port );
    usb_hreg_set_enb_bchge( ptr, port );

}
/******************************************************************************
End of function usb_hstd_BchgEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_BchgDisable
Description     : Disable BCHG interrupt for specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_BchgDisable(USB_UTR_t *ptr, uint16_t port)
{

    usb_hreg_clr_sts_bchg( ptr, port );
    usb_hreg_clr_enb_bchge( ptr, port );

}
/******************************************************************************
End of function usb_hstd_BchgDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SetUact
Description     : Start sending SOF to the connected USB device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_SetUact(USB_UTR_t *ptr, uint16_t port)
{
    usb_creg_rmw_dvstctr( ptr, port, USB_UACT, (USB_USBRST | USB_RESUME | USB_UACT) );
}
/******************************************************************************
End of function usb_hstd_SetUact
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_OvrcrEnable
Description     : Enable OVRCR interrupt of the specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_OvrcrEnable(USB_UTR_t *ptr, uint16_t port)
{

    usb_hreg_clr_sts_ovrcr( ptr, port );
    usb_hreg_set_enb_ovrcre( ptr, port );

}
/******************************************************************************
End of function usb_hstd_OvrcrEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_OvrcrDisable
Description     : Disable OVRCR interrupt of the specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_OvrcrDisable(USB_UTR_t *ptr, uint16_t port)
{
    /* OVRCR Clear(INT_N edge sense) */
    usb_hreg_clr_sts_ovrcr( ptr, port );
    /* Over-current disable */
    usb_hreg_clr_enb_ovrcre( ptr, port );
}
/******************************************************************************
End of function usb_hstd_OvrcrDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_AttchEnable
Description     : Enable ATTCH (attach) interrupt of the specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_AttchEnable(USB_UTR_t *ptr, uint16_t port)
{

    /* ATTCH status Clear */
    usb_hreg_clr_sts_attch( ptr, port );
    /* Attach enable */
    usb_hreg_set_enb_attche( ptr, port );

}
/******************************************************************************
End of function usb_hstd_AttchEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_AttchDisable
Description     : Disable ATTCH (attach) interrupt of the specified USB port.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_AttchDisable(USB_UTR_t *ptr, uint16_t port)
{

    /* ATTCH Clear(INT_N edge sense) */
    usb_hreg_clr_sts_attch( ptr, port );
    /* Attach disable */
    usb_hreg_clr_enb_attche( ptr, port );

}
/******************************************************************************
End of function usb_hstd_AttchDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DtchEnable
Description     : Enable DTCH (detach) interrupt of the specified USB port. 
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_DtchEnable(USB_UTR_t *ptr, uint16_t port)
{

    /* DTCH Clear */
    usb_hreg_clr_sts_dtch( ptr, port );
    /* Detach enable */
    usb_hreg_set_enb_dtche( ptr, port );

}
/******************************************************************************
End of function usb_hstd_DtchEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DtchDisable
Description     : Disable DTCH (detach) interrupt of the specified USB port.  
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t port             : root port
Return value    : none
******************************************************************************/
void usb_hstd_DtchDisable(USB_UTR_t *ptr, uint16_t port)
{

    /* DTCH Clear(INT_N edge sense) */
    usb_hreg_clr_sts_dtch( ptr, port );
    /* Detach disable */
    usb_hreg_clr_enb_dtche( ptr, port );

}
/******************************************************************************
End of function usb_hstd_DtchDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SetPipeRegister
Description     : Set up USB registers to use specified pipe (given in infor-
                : mation table).
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t pipe_no          : pipe number
                : uint16_t *tbl     : pipe information table 
Return value    : none
******************************************************************************/
void usb_hstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t pipe_no, uint16_t *tbl)
{
    uint16_t        i, pipe, buf;

    /* PIPE USE check */
    if( pipe_no == USB_USEPIPE )
    {
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE,  USB_NO);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
        /* EP Table loop */
        for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
        {
            /* PipeNo Number */
            pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
            /* PIPE Setting */
            usb_cstd_pipe_init(ptr, pipe, tbl, i);
        }
    }
    else
    {
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
        buf = usb_creg_read_fifosel( ptr, USB_D0USE );
        if( (buf & USB_CURPIPE) == pipe_no )
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        }
        buf = usb_creg_read_fifosel( ptr, USB_D1USE );
        if( (buf & USB_CURPIPE) == pipe_no )
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
        }
        /* EP Table loop */
        for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
        {
            /* PipeNo Number */
            pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
            if( pipe == pipe_no )
            {
                /* PIPE Setting */
                usb_cstd_pipe_init(ptr, pipe, tbl, i);
            }
        }
    }
}
/******************************************************************************
End of function usb_hstd_SetPipeRegister
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_GetRootport
Description     : Get USB port no. set in the USB register based on the speci-
                : fied USB Device address.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
                : uint16_t addr             : device address
Return value    : uint16_t                  : root port number
******************************************************************************/
uint16_t usb_hstd_GetRootport(USB_UTR_t *ptr, uint16_t addr)
{
    uint16_t    buffer;

    /* Get device address configuration register from device address */
    buffer = usb_hreg_read_devadd( ptr, addr );
    if( buffer != USB_ERROR )
    {
        /* Return root port number */
        return (uint16_t)(buffer & USB_RTPORT);
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_hstd_GetRootport
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ChkDevAddr
Description     : Get USB speed set in USB register based on the specified USB 
                : Device address and USB port no.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t addr             : device address
                : uint16_t rootport         : root port
Return value    : uint16_t                  : USB speed etc
******************************************************************************/
uint16_t usb_hstd_ChkDevAddr(USB_UTR_t *ptr, uint16_t addr, uint16_t rootport)
{
    uint16_t    buffer;

    /* Get device address configuration register from device address */
    buffer = usb_hreg_read_devadd( ptr, addr );
    if( buffer != USB_ERROR )
    {
        if( (uint16_t)(buffer & USB_RTPORT) == rootport )
        {
            /* Return Address check result */
            return (uint16_t)(buffer & USB_USBSPD);
        }
    }
    return USB_NOCONNECT;
}
/******************************************************************************
End of function usb_hstd_ChkDevAddr
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_GetDevSpeed
Description     : Get USB speed set in USB register based on the specified USB 
                : Device address.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t addr             : device address
Return value    : uint16_t                  : device speed
Note            : Use also to a connection check is possible
******************************************************************************/
uint16_t usb_hstd_GetDevSpeed(USB_UTR_t *ptr, uint16_t addr)
{
    uint16_t    buffer;

    /* Get device address configuration register from device address */
    buffer = usb_hreg_read_devadd( ptr, addr );
    if( buffer != USB_ERROR )
    {
        /* Return device speed */
        return (uint16_t)(buffer & USB_USBSPD);
    }
    return USB_NOCONNECT;
}
/******************************************************************************
End of function usb_hstd_GetDevSpeed
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
