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
* File Name    : r_usb_hreg_abs.c
* Description  : Call USB Host register access function 
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


/******************************************************************************
Constant macro definitions
******************************************************************************/
/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
    #define USB_FIFOENDIAN      USB_FIFO_LITTLE
#else   /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
    #define USB_FIFOENDIAN      USB_FIFO_BIG
#endif  /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
External variables and functions
******************************************************************************/
extern uint16_t usb_ghstd_RemortPort[];

/******************************************************************************
Function Name   : usb_hstd_SetHubPort
Description     : Set up-port hub
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t addr     : device address
                : uint16_t upphub   : up-port hub address
                : uint16_t hubport  : hub port number
Return value    : none
******************************************************************************/
void usb_hstd_SetHubPort(USB_UTR_t *ptr, uint16_t addr, uint16_t upphub, uint16_t hubport)
{
    if(ptr->ip == USB_USBIP_1)
    {
        usb_hreg_rmw_devadd( ptr, addr, (upphub|hubport), (uint16_t)(USB_UPPHUB | USB_HUBPORT) );
    }
} /* eof usb_hstd_SetHubPort */

/******************************************************************************
Function Name   : usb_hstd_RwupeEnable
Description     : Remote wakeup signal detection enable setting from USB Device
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; root port
Return value    : none
******************************************************************************/
void usb_hstd_RwupeEnable(USB_UTR_t *ptr, uint16_t port)
{

    usb_hreg_set_rwupe( ptr, port );

} /* eof usb_hstd_RwupeEnable */


/******************************************************************************
Function Name   : usb_hstd_RwupeDisable
Description     : Remote wakeup signal detection disable setting from USB Device
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; root port
Return value    : none
******************************************************************************/
void usb_hstd_RwupeDisable(USB_UTR_t *ptr, uint16_t port)
{

    usb_hreg_clr_rwupe( ptr, port );

} /* eof of usb_hstd_RwupeDisable */

/******************************************************************************
Function Name   : usb_hstd_InterruptHandler
Description     : Analyzes which USB interrupt is generated
Argument        : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
Return          : none
******************************************************************************/
void usb_hstd_InterruptHandler(USB_UTR_t *ptr)
{
    uint16_t    intsts0, intenb0, ists0;
    uint16_t    intsts1, intenb1, ists1;
    uint16_t    brdysts, brdyenb, bsts;
    uint16_t    nrdysts, nrdyenb, nsts;
    uint16_t    bempsts, bempenb, ests;


    intsts0 = ptr->ipp->INTSTS0.WORD;
    intsts1 = ptr->ipp->INTSTS1.WORD;
    brdysts = ptr->ipp->BRDYSTS.WORD;
    nrdysts = ptr->ipp->NRDYSTS.WORD;
    bempsts = ptr->ipp->BEMPSTS.WORD;
    intenb0 = ptr->ipp->INTENB0.WORD;
    intenb1 = ptr->ipp->INTENB1.WORD;
    brdyenb = ptr->ipp->BRDYENB.WORD;
    nrdyenb = ptr->ipp->NRDYENB.WORD;
    bempenb = ptr->ipp->BEMPENB.WORD;

    /* Interrupt Status Get */
    ptr->keyword = USB_INT_UNKNOWN;
    ptr->status  = 0;

    ists0 = (uint16_t)(intsts0 & intenb0);
    ists1 = (uint16_t)(intsts1 & intenb1);
//  ists2 = (uint16_t)(intsts2 & intenb2);
    bsts  = (uint16_t)(brdysts & brdyenb);
    nsts  = (uint16_t)(nrdysts & nrdyenb);
    ests  = (uint16_t)(bempsts & bempenb);

    /***** Processing Setup transaction *****/
    if( (ists1 & USB_SACK) == USB_SACK )
    {
        /***** Setup ACK *****/
        /* SACK Clear */
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_SACK;
        /* Setup Ignore,Setup Acknowledge disable */
        ptr->ipp->INTENB1.WORD &= (uint16_t)~(USB_SIGNE | USB_SACKE);
        ptr->keyword = USB_INT_SACK;
    }
    else if( (ists1 & USB_SIGN) == USB_SIGN )
    {
        /***** Setup Ignore *****/
        /* SIGN Clear */
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_SIGN;
        /* Setup Ignore,Setup Acknowledge disable */
        ptr->ipp->INTENB1.WORD &= (uint16_t)~(USB_SIGNE | USB_SACKE);
        ptr->keyword = USB_INT_SIGN;
    }
    /***** Processing PIPE0-MAX_PIPE_NO data *****/
    else if( (ists0 & USB_BRDY) == USB_BRDY )       /***** EP0-7 BRDY *****/
    {
        ptr->ipp->BRDYSTS.WORD = (uint16_t)~bsts;
        ptr->keyword = USB_INT_BRDY;
        ptr->status  = bsts;
    }
    else if( (ists0 & USB_BEMP) == USB_BEMP )       /***** EP0-7 BEMP *****/
    {
        ptr->ipp->BEMPSTS.WORD = (uint16_t)~ests;
        ptr->keyword = USB_INT_BEMP;
        ptr->status  = ests;
    }
    else if( (ists0 & USB_NRDY) == USB_NRDY )       /***** EP0-7 NRDY *****/
    {
        ptr->ipp->NRDYSTS.WORD = (uint16_t)~nsts;
        ptr->keyword = USB_INT_NRDY;
        ptr->status  = nsts;
    }

    /***** Processing rootport0 *****/
    else if( (ists1 & USB_OVRCR) == USB_OVRCR )     /***** OVER CURRENT *****/
    {
        /* OVRCR Clear */
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_OVRCR;
        ptr->keyword = USB_INT_OVRCR0;
    }
    else if( (ists1 & USB_ATTCH) == USB_ATTCH )     /***** ATTCH INT *****/
    {
        /* DTCH  interrupt disable */
        usb_hstd_BusIntDisable(ptr, (uint16_t)USB_PORT0);
        ptr->keyword = USB_INT_ATTCH0;
    }
    else if( (ists1 & USB_EOFERR) == USB_EOFERR )   /***** EOFERR INT *****/
    {
        /* EOFERR Clear */
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_EOFERR;
        ptr->keyword = USB_INT_EOFERR0;
    }
    else if( (ists1 & USB_BCHG) == USB_BCHG )       /***** BCHG INT *****/
    {
        /* BCHG  interrupt disable */
        usb_hstd_BchgDisable(ptr, (uint16_t)USB_PORT0);
        ptr->keyword = USB_INT_BCHG0;
    }
    else if( (ists1 & USB_DTCH) == USB_DTCH )       /***** DETACH *****/
    {
        /* DTCH  interrupt disable */
        usb_hstd_BusIntDisable(ptr, (uint16_t)USB_PORT0);
        ptr->keyword = USB_INT_DTCH0;
    }
#ifdef USB_HOST_BC_ENABLE
    else if( (ists1 & USB_PDDETINT) == USB_PDDETINT )        /***** PDDETINT INT *****/
    {
        if(ptr -> ip == USB_USBIP_1)
        {
            /* PDDETINT  interrupt disable */
            ptr->ipp1->INTSTS1.WORD = (uint16_t)~USB_PDDETINT;
            ptr->keyword = USB_INT_PDDETINT0;
        }
    }
#endif
    /***** Processing VBUS/SOF *****/
    else if( (ists0 & USB_VBINT) == USB_VBINT )     /***** VBUS change *****/
    {
        /* Status Clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_VBINT;
        ptr->keyword = USB_INT_VBINT;
    }
    else if( (ists0 & USB_SOFR) == USB_SOFR )       /***** SOFR change *****/
    {
        /* SOFR Clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_SOFR;
        ptr->keyword = USB_INT_SOFR;
    }

    else
    {
    }
} /* eof of usb_hstd_InterruptHandler */

/******************************************************************************
Function Name   : usb_hstd_ChkAttach
Description     : Checks whether USB Device is attached or not and return USB speed
                : of USB Device
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : uint16_t         ; connection status
                :                  ; (USB_ATTACHF/USB_ATTACHL/USB_DETACH/USB_DONE)
Note            : Please change for your SYSTEM
******************************************************************************/
uint16_t usb_hstd_ChkAttach(USB_UTR_t *ptr, uint16_t port)
{
    uint16_t    buf[3];

    usb_hstd_ReadLnst(ptr, port, buf);

    if( (uint16_t)(buf[1] & USB_RHST) == USB_UNDECID )
    {
        if( (buf[0] & USB_LNST) == USB_FS_JSTS )
        {
            /* High/Full speed device */
            USB_PRINTF0(" Detect FS-J\n");
            usb_cstd_SetHse(ptr, port, usb_gcstd_HsEnable[ptr->ip]);
            return USB_ATTACHF;
        }
        else if( (buf[0] & USB_LNST) == USB_LS_JSTS )
        {
            /* Low speed device */
            USB_PRINTF0(" Attach LS device\n");
            usb_cstd_SetHse(ptr, port, USB_HS_DISABLE);
            return USB_ATTACHL;
        }
        else if( (buf[0] & USB_LNST) == USB_SE0 )
        {
            USB_PRINTF0(" Detach device\n");
        }
        else
        {
            USB_PRINTF0(" Attach unknown speed device\n");
        }
    }
    else
    {
        USB_PRINTF0(" Already device attached\n");
        return USB_DONE;
    }
    return USB_DETACH;
} /* eof of usb_hstd_ChkAttach */

/******************************************************************************
Function Name   : usb_hstd_ChkClk
Description     : Checks SOF sending setting when USB Device is detached or suspended
                : , BCHG interrupt enable setting and clock stop processing
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
                : uint16_t event   ; device state
Return value    : none
******************************************************************************/
void usb_hstd_ChkClk(USB_UTR_t *ptr, uint16_t port, uint16_t event)
{

    if( (usb_ghstd_MgrMode[ptr->ip][USB_PORT0] == USB_DETACHED) 
        || (usb_ghstd_MgrMode[ptr->ip][USB_PORT0] == USB_SUSPENDED) )
    {
        usb_hstd_ChkSof( ptr, (uint16_t)USB_PORT0);
        /* Enable port BCHG interrupt */
        usb_hstd_BchgEnable( ptr, (uint16_t)USB_PORT0);
        usb_cstd_StopClock( ptr );
    }

} /* eof of usb_hstd_ChkClk */

/******************************************************************************
Function Name   : usb_hstd_DetachProcess
Description     : Handles the require processing when USB device is detached
                : (Data transfer forcibly termination processing to the connected USB Device,
                : the clock supply stop setting and the USB interrupt dissable setteing etc)
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : none
******************************************************************************/
void usb_hstd_DetachProcess(USB_UTR_t *ptr, uint16_t port)
{
    uint16_t        connect_inf, md, i, addr;

    /* ATTCH interrupt disable */
    usb_hstd_AttchDisable( ptr, port);
    /* DTCH  interrupt disable */
    usb_hstd_DtchDisable( ptr, port);
    usb_hstd_BchgDisable( ptr, (uint16_t)USB_PORT0);

    for( md = 1u; md < (USB_MAXDEVADDR + 1u); md++ )
    {
        addr = (uint16_t)(md << USB_DEVADDRBIT);
        if( usb_hstd_ChkDevAddr(ptr, addr, port) != USB_NOCONNECT )
        {
            if( usb_ghstd_Ctsq[ptr->ip] != USB_IDLEST )
            {
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            }
            for( i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++ )
            {
                /* Not control transfer */
                /* Agreement device address */
                if( usb_cstd_GetDevsel(ptr, i) == addr )
                {
                    /* PID=BUF ? */
                    if( usb_cstd_GetPid(ptr, i) == USB_PID_BUF )
                    {
                        /* End of data transfer (IN/OUT) */
                        usb_cstd_ForcedTermination(ptr, i, (uint16_t)USB_DATA_STOP);
                    }
                    usb_cstd_ClrPipeCnfg(ptr, i);
                }
            }
            usb_hstd_SetDevAddr(ptr, addr, USB_DONE, USB_DONE);
            usb_hstd_SetHubPort(ptr, addr, USB_DONE, USB_DONE);
            USB_PRINTF1("*** Device address %d clear.\n",md);
        }
    }
    /* Decide USB Line state (ATTACH) */
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
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_DETACHED);
        break;
    default:
        /* USB detach */
        usb_hstd_Detach(ptr, port);
        /* Check clock */
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_DETACHED);
        break;
    }
} /* eof of usb_hstd_DetachProcess */

/******************************************************************************
Function Name   : usb_hstd_ReadLnst
Description     : Reads LNST register two times, checks whether these values
                : are equal and returns the value of DVSTCTR register that correspond to
                : the port specified by 2nd argument.
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
                : uint16_t *buf    ; Pointer to the buffer to store DVSTCTR register
Return value    : none
Note            : Please change for your SYSTEM
******************************************************************************/
void usb_hstd_ReadLnst(USB_UTR_t *ptr, uint16_t port, uint16_t *buf)
{
    do
    {
        buf[0] = usb_creg_read_syssts( ptr, port );
        /* 30ms wait */
        usb_cpu_DelayXms((uint16_t)30);
        buf[1] = usb_creg_read_syssts( ptr, port );
        if( (buf[0] & USB_LNST) == (buf[1] & USB_LNST) )
        {
            /* 20ms wait */
            usb_cpu_DelayXms((uint16_t)20);
            buf[1] = usb_creg_read_syssts( ptr, port );
        }
    }
    while( (buf[0] & USB_LNST) != (buf[1] & USB_LNST) );
    buf[1] = usb_creg_read_dvstctr( ptr, port );
} /* eof of usb_hstd_ReadLnst */

/******************************************************************************
Function Name   : usb_hstd_AttachProcess
Description     : Interrupt disable setting when USB Device is attached and
                : handles the required interrupt disable setting etc when USB device
                : is attached.
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : none
Note            : Please change for your SYSTEM
******************************************************************************/
void usb_hstd_AttachProcess(USB_UTR_t *ptr, uint16_t port)
{
    uint16_t        connect_inf;

    /* ATTCH interrupt disable */
    usb_hstd_AttchDisable(ptr, port);
    /* DTCH  interrupt disable */
    usb_hstd_DtchDisable(ptr, port);
    usb_hstd_BchgDisable(ptr, (uint16_t)USB_PORT0);
    /* Decide USB Line state (ATTACH) */
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
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_DETACHED);
        break;
    default:
        usb_hstd_Attach(ptr, (uint16_t)USB_ATTACHF, port);
        break;
    }
} /* eof of usb_hstd_AttachProcess */

/******************************************************************************
Function Name   : usb_hstd_ChkSof
Description     : Checks whether SOF is sended or not
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : none
******************************************************************************/
void usb_hstd_ChkSof(USB_UTR_t *ptr, uint16_t port)
{
#if 0
    uint16_t    buf;

    do
    {
        /* Wait 640ns */
        usb_cpu_Delay1us((uint16_t)1);

        buf = usb_creg_read_syssts( ptr, port );
    }
    while( (uint16_t)(buf & USB_SOFEA) == USB_SOFEA );
#endif
    usb_cpu_Delay1us((uint16_t)1);  /* Wait 640ns */
} /* eof of usb_hstd_ChkSof */

/******************************************************************************
Function Name   : usb_hstd_BusReset
Description     : Setting USB register when BUS Reset
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : none
******************************************************************************/
void usb_hstd_BusReset(USB_UTR_t *ptr, uint16_t port)
{
    uint16_t        buf, i;

    /* USBRST=1, UACT=0 */
    usb_creg_rmw_dvstctr( ptr, port, USB_USBRST, (USB_USBRST | USB_UACT) );

    /* Wait 50ms */
    usb_cpu_DelayXms((uint16_t)50);
    if(ptr->ip == USB_USBIP_1)
    {
        /* USBRST=0 */
        usb_creg_clr_dvstctr( ptr, USB_PORT0, USB_USBRST ); //for UTMI
        usb_cpu_Delay1us( 300 );                            //for UTMI
    }
    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_SetUact(ptr, port);
    /* Wait 10ms or more (USB reset recovery) */
    usb_cpu_DelayXms((uint16_t)20);
    for( i = 0, buf = USB_HSPROC; (i < 3) && (buf == USB_HSPROC); ++i )
    {
        /* DeviceStateControlRegister - ResetHandshakeStatusCheck */
        buf = usb_creg_read_dvstctr( ptr, port );
        buf = (uint16_t)(buf & USB_RHST);
        if( buf == USB_HSPROC )
        {
            /* Wait */
            usb_cpu_DelayXms((uint16_t)10);
        }
    }
    /* 30ms wait */
    usb_cpu_DelayXms((uint16_t)30);
} /* eof of usb_hstd_BusReset */

/******************************************************************************
Function Name   : usb_hstd_ResumeProcess
Description     : Setting USB register when RESUME signal is detected
Arguments       : USB_UTR_t *ptr   ; USB system internal structure. Selects channel.
                : uint16_t port    ; port number
Return value    : none
******************************************************************************/
void usb_hstd_ResumeProcess(USB_UTR_t *ptr, uint16_t port)
{
    usb_hstd_BchgDisable( ptr, port );
    /* RESUME=1, RWUPE=0 */
    usb_creg_rmw_dvstctr( ptr, port, USB_RESUME, (USB_RESUME | USB_RWUPE) );
    /* Wait */
    usb_cpu_DelayXms((uint16_t)20);
    /* USBRST=0, RESUME=0, UACT=1 */
    usb_hstd_SetUact(ptr, port);
    /* Wait */
    usb_cpu_DelayXms((uint16_t)5);
} /* eof of usb_hstd_ResumeProcess */

/******************************************************************************
Function Name   : usb_hstd_support_speed_check
Description     : Get USB-speed of the specified port.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t port  : Root port
Return value    : uint16_t       : HSCONNECT : Hi-Speed
                :                : FSCONNECT : Full-Speed
                :                : LSCONNECT : Low-Speed
                :                : NOCONNECT : not connect
******************************************************************************/
uint16_t usb_hstd_support_speed_check( USB_UTR_t *ptr, uint16_t port )
{
    uint16_t    buf, ConnInf;

    buf = usb_creg_read_dvstctr( ptr, port );

    /* Reset handshake status get */
    buf = (uint16_t)(buf & USB_RHST);

    switch( buf )
    {
    /* Get port speed */
    case USB_HSMODE:    ConnInf = USB_HSCONNECT;    break;
    case USB_FSMODE:    ConnInf = USB_FSCONNECT;    break;
    case USB_LSMODE:    ConnInf = USB_LSCONNECT;    break;
    case USB_HSPROC:    ConnInf = USB_NOCONNECT;    break;
    default:        ConnInf = USB_NOCONNECT;    break;
    }

    return (ConnInf);
} /* eof of usb_hstd_support_speed_check */

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End of file
******************************************************************************/
