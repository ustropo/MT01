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
* File Name    : r_usb_clibusbip.c
* Description  : USB IP Host and Peripheral low level library
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


/******************************************************************************
External variables and functions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Renesas Abstracted Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_NrdyEnable
Description     : Enable NRDY interrupt of the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : none
******************************************************************************/
void usb_cstd_NrdyEnable(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
        /*  At the peripheral operation, interrupt is disabled, */
        /*  because handler becomes busy. */
    }
    else
    {
        /* Enable NRDY */
        usb_creg_set_nrdyenb( ptr, pipe );
    }
}
/******************************************************************************
End of function usb_cstd_NrdyEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_BerneEnable
Description     : Enable BRDY/NRDY/BEMP interrupt.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void usb_cstd_BerneEnable(USB_UTR_t *ptr)
{
    /* Enable BEMP, NRDY, BRDY */
    usb_creg_set_intenb( ptr, (USB_BEMPE|USB_NRDYE|USB_BRDYE) );
}
/******************************************************************************
End of function usb_cstd_BerneEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_SwReset
Description     : Request USB IP software reset
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void usb_cstd_SwReset(USB_UTR_t *ptr)
{
    /* USB Enable */
    usb_creg_set_usbe( ptr );
    /* USB Reset */
    usb_creg_clr_usbe( ptr );
    /* USB Enable */
    usb_creg_set_usbe( ptr );
}
/******************************************************************************
End of function usb_cstd_SwReset
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_GetPid
Description     : Fetch specified pipe's PID.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint16_t PID-bit status
******************************************************************************/
uint16_t usb_cstd_GetPid(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t    buf;

    /* PIPE control reg read */
    buf = usb_creg_read_pipectr( ptr, pipe );
    return (uint16_t)(buf & USB_PID);
}
/******************************************************************************
End of function usb_cstd_GetPid
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_DoSqtgl
Description     : Toggle setting of the toggle-bit for the specified pipe by 
                : argument.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe     : Pipe number.
                : uint16_t toggle   : Current toggle status.
Return value    : none
******************************************************************************/
void usb_cstd_DoSqtgl(USB_UTR_t *ptr, uint16_t pipe, uint16_t toggle)
{
    /* Check toggle */
    if( (toggle & USB_SQMON) == USB_SQMON )
    {
        /* Do pipe SQSET */
        usb_creg_set_sqset(ptr, pipe);
    }
    else
    {
        /* Do pipe SQCLR */
        usb_creg_set_sqclr(ptr, pipe);
    }
}
/******************************************************************************
End of function usb_cstd_DoSqtgl
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_GetMaxPacketSize
Description     : Fetch MaxPacketSize of the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint16_t MaxPacketSize
******************************************************************************/
uint16_t usb_cstd_GetMaxPacketSize(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t    size, buffer;

    if( pipe == USB_PIPE0 )
    {
        buffer = usb_creg_read_dcpmaxp( ptr );
    }
    else
    {
        /* Pipe select */
        usb_creg_write_pipesel( ptr, pipe );
        buffer = usb_creg_read_pipemaxp( ptr );
    }
    /* Max Packet Size */
    size = (uint16_t)(buffer & USB_MXPS);

    return size;
}
/******************************************************************************
End of function usb_cstd_GetMaxPacketSize
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_GetDevsel
Description     : Get device address from pipe number
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint16_t DEVSEL-bit status
******************************************************************************/
uint16_t usb_cstd_GetDevsel(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t    devsel, buffer;

    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        /* Peripheral Function */
        /* USB address */
        buffer = usb_creg_read_usbaddr( ptr );
        /* Device address */
        devsel = (uint16_t)(buffer & USB_USBADDR_MASK);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        if( pipe == USB_PIPE0 )
        {
            buffer = usb_creg_read_dcpmaxp( ptr );
        }
        else
        {
            /* Pipe select */
            usb_creg_write_pipesel( ptr, pipe );
            buffer = usb_creg_read_pipemaxp( ptr );
        }
        /* Device address */
        devsel = (uint16_t)(buffer & USB_DEVSEL);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }

    return devsel;
}
/******************************************************************************
End of function usb_cstd_GetDevsel
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_GetPipeDir
Description     : Get PIPE DIR
Arguments       : uint16_t pipe  : Pipe number.
Return value    : uint16_t Pipe direction.
******************************************************************************/
uint16_t usb_cstd_GetPipeDir(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        buffer;

    /* Pipe select */
    usb_creg_write_pipesel( ptr, pipe );
    /* Read Pipe direction */
    buffer = usb_creg_read_pipecfg( ptr );
    return (uint16_t)(buffer & USB_DIRFIELD);
}
/******************************************************************************
End of function usb_cstd_GetPipeDir
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_GetPipeType
Description     : Fetch and return PIPE TYPE.
Arguments       : uint16_t pipe            : Pipe number.
Return value    : uint16_t Pipe type
******************************************************************************/
uint16_t usb_cstd_GetPipeType(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        buffer;

    /* Pipe select */
    usb_creg_write_pipesel( ptr, pipe );
    /* Read Pipe direction */
    buffer = usb_creg_read_pipecfg( ptr );
    return (uint16_t)(buffer & USB_TYPFIELD);
}
/******************************************************************************
End of function usb_cstd_GetPipeType
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_is_host_mode
Description     : Check current function
Arguments       : none
Return value    : uint16_t    : YES = Host
                :             : NO = Peripheral
******************************************************************************/
uint16_t usb_cstd_is_host_mode(USB_UTR_t *ptr)
{
    uint16_t    buf;
    buf = usb_creg_read_syscfg( ptr, USB_PORT0 );
    if( (buf & USB_DCFM) == USB_DCFM )
    {
        /* Host Function mode */
        return USB_YES;
    }
    else
    {
        /* Peripheral Function mode */
        return USB_NO;
    }
}
/******************************************************************************
End of function usb_cstd_is_host_mode
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_DoAclrm
Description     : Set the ACLRM-bit (Auto Buffer Clear Mode) of the specified 
                : pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : none
******************************************************************************/
void usb_cstd_DoAclrm(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Control ACLRM */
    usb_creg_set_aclrm( ptr, pipe );
    usb_creg_clr_aclrm( ptr, pipe );
}
/******************************************************************************
End of function usb_cstd_DoAclrm
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_SetBuf
Description     : Set PID (packet ID) of the specified pipe to BUF.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : none
******************************************************************************/
void usb_cstd_SetBuf(USB_UTR_t *ptr, uint16_t pipe)
{
    /* PIPE control reg set */
    usb_creg_set_pid( ptr, pipe, USB_PID_BUF );
}
/******************************************************************************
End of function usb_cstd_SetBuf
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ClrStall
Description     : Set up to NAK the specified pipe, and clear the STALL-bit set
                : to the PID of the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : none
Note            : PID is set to NAK.
******************************************************************************/
void usb_cstd_ClrStall(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Set NAK */
    usb_cstd_SetNak(ptr, pipe);
    /* Clear STALL */
    usb_creg_clr_pid( ptr, pipe, USB_PID_STALL );
}
/******************************************************************************
End of function usb_cstd_ClrStall
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_Epadr2Pipe
Description     : Get the associated pipe no. of the specified endpoint.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t dir_ep : Direction + endpoint number.
Return value    : uint16_t        : OK    : Pipe number.
                :                 : ERROR : Error.
******************************************************************************/
uint16_t usb_cstd_Epadr2Pipe(USB_UTR_t *ptr, uint16_t dir_ep)
{
    uint16_t        i, direp, tmp, *table;

    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        uint16_t conf;

        conf = usb_gpstd_ConfigNum;
        if( conf < (uint16_t)1 )
        {
            /* Address state */
            conf = (uint16_t)1;
        }

        /* Peripheral */
        /* Get PIPE Number from Endpoint address */
        table = (uint16_t*)((uint16_t**)(usb_gpstd_Driver.pipetbl[conf - 1]));
        direp = (uint16_t)(((dir_ep & 0x80) >> 3) | (dir_ep & 0x0F));
        /* EP table loop */
        for( i = 0; table[i] != USB_PDTBLEND; i += USB_EPL )
        {
            tmp = (uint16_t)(table[i + 1] & (USB_DIRFIELD | USB_EPNUMFIELD));
            /* EP table endpoint dir check */
            if( direp == tmp )
            {
                return table[i];
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        uint16_t        md;
        USB_HCDREG_t    *driver;

        /* Host */
        /* Get PIPE Number from Endpoint address */
        for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
        {
            if( (usb_ghstd_DeviceDrv[ptr->ip][md].ifclass != USB_IFCLS_NOT) &&
                (usb_ghstd_DeviceDrv[ptr->ip][md].devaddr != USB_NODEVICE) )
            {
                driver = (USB_HCDREG_t*)&usb_ghstd_DeviceDrv[ptr->ip][md];
                table = (uint16_t*)(driver->pipetbl);
                direp = (uint16_t)((((dir_ep & 0x80) ^ 0x80) >> 3) | (dir_ep & 0x0F));
                /* EP table loop */
                for( i = 0; table[i] != USB_PDTBLEND; i += USB_EPL )
                {
                    tmp = (uint16_t)(table[i + 1] & (USB_DIRFIELD | USB_EPNUMFIELD));
                    /* EP table endpoint dir check */
                    if( direp == tmp )
                    {
                        return table[i];
                    }
                }
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_cstd_Epadr2Pipe
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_Pipe2Epadr
Description     : Get the associated endpoint value of the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint8_t        : OK    : Endpoint nr + direction.
                :                : ERROR : Error.
******************************************************************************/
uint8_t usb_cstd_Pipe2Epadr(USB_UTR_t *ptr, uint16_t pipe)
{

    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
        /* Peripheral */
        USB_PRINTF0("Not support peripheral function\n");
        return (uint8_t)USB_ERROR;
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        uint16_t        buffer,direp;

        /* Pipe select */
        usb_creg_write_pipesel( ptr, pipe );
        /* Read Pipe direction */
        buffer = usb_creg_read_pipecfg( ptr );
        direp = (uint16_t)((((buffer & USB_DIRFIELD) ^ USB_DIRFIELD) << 3) + (buffer & USB_EPNUMFIELD));
        return (uint8_t)(direp);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
    return (uint8_t)USB_ERROR;
}
/******************************************************************************
End of function usb_cstd_Pipe2Epadr
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_Pipe2Fport
Description     : Get port No. from the specified pipe No. by argument
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint16_t       : FIFO port selector.
******************************************************************************/
uint16_t usb_cstd_Pipe2Fport(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        i, *table;

    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        uint16_t    conf;

        conf = usb_gpstd_ConfigNum;
        if( conf < (uint16_t)1 )
        {
            /* Address state */
            conf = (uint16_t)1;
        }
        /* Peripheral */
        /* Get FIFO port from PIPE number */
        table = (uint16_t*)((uint16_t**)
            (usb_gpstd_Driver.pipetbl[conf - 1]));
        /* EP table loop */
        for( i = 0; table[i] != USB_PDTBLEND; i += USB_EPL )
        {
            if( table[i] == pipe )
            {
                return table[i + 5];
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        uint16_t        md;
        USB_HCDREG_t    *driver;

        /* Host */
        /* Get FIFO port from PIPE number */
        for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
        {
            if( (usb_ghstd_DeviceDrv[ptr->ip][md].ifclass != USB_IFCLS_NOT) &&
                (usb_ghstd_DeviceDrv[ptr->ip][md].devaddr != USB_NODEVICE) )
            {
                driver = (USB_HCDREG_t*)&usb_ghstd_DeviceDrv[ptr->ip][md];
                table = (uint16_t*)(driver->pipetbl);
                /* EP table loop */
                for( i = 0; table[i] != USB_PDTBLEND; i += USB_EPL )
                {
                    if( table[i] == pipe)
                    {
                        return table[i + 5];
                    }
                }
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }

    return USB_ERROR;
}
/******************************************************************************
End of function usb_cstd_Pipe2Fport
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_GetDeviceAddress
Description     : Get the device address associated with the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : uint16_t DEVSEL-bit status
******************************************************************************/
uint16_t usb_cstd_GetDeviceAddress(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        buffer;

    /* Check current function */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        /* Peripheral */
        /* USB address */
        buffer = usb_creg_read_usbaddr( ptr );
        /* Device address */
        return (uint16_t)(buffer & USB_USBADDR_MASK);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        uint16_t        i, md;
        USB_HCDREG_t    *driver;

        /* Host */
        if( pipe == USB_PIPE0 )
        {
            buffer = usb_creg_read_dcpmaxp( ptr );
            /* Device address */
            return (uint16_t)(buffer & USB_DEVSEL);
        }
        else
        {
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                if( (usb_ghstd_DeviceDrv[ptr->ip][md].ifclass != USB_IFCLS_NOT) &&
                    (usb_ghstd_DeviceDrv[ptr->ip][md].devaddr != USB_NODEVICE) )
                {
                    driver = (USB_HCDREG_t*)&usb_ghstd_DeviceDrv[ptr->ip][md];
                    /* EP table loop */
                    for( i = 0; driver->pipetbl[i] != USB_PDTBLEND; i
                        += USB_EPL )
                    {
                        if( driver->pipetbl[i] == pipe )
                        {
                            buffer = driver->pipetbl[i + 3];
                            /* Device address */
                            return (uint16_t)(buffer & USB_DEVSEL);
                        }
                    }
                }
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_cstd_GetDeviceAddress
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ClearIntEnb
Description     : Clear teh INTENB register.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void    usb_cstd_ClearIntEnb( USB_UTR_t *ptr )
{
    usb_creg_write_intenb( ptr, 0 );
    /* Conditional compile dep. on difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    usb_hreg_write_intenb( ptr, USB_PORT0, 0 );
    usb_hreg_write_intenb( ptr, USB_PORT1, 0 );
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
}
/******************************************************************************
End of function usb_cstd_ClearIntEnb
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ClearIntSts
Description     : Clear the INTSTS register.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void    usb_cstd_ClearIntSts( USB_UTR_t *ptr )
{
    usb_creg_write_intsts( ptr, 0 );
/* Conditional compile dep. on difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    usb_hreg_write_intsts( ptr, USB_PORT0, 0 );
    usb_hreg_write_intsts( ptr, USB_PORT1, 0 );
#endif  /* USB_FUNCSEL_USBIP0_PP != USB_PERI_PP && USB_FUNCSEL_USBIP1_PP != USB_PERI_PP */
}
/******************************************************************************
End of function usb_cstd_ClearIntSts
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ClearDline
Description     : Clear DRPD/DPRPU; host pulldown of resistors for D+ D- lines.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return value    : none
******************************************************************************/
void    usb_cstd_ClearDline( USB_UTR_t *ptr )
{
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    usb_creg_clr_cnen( ptr );
#endif /* #if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    usb_hreg_clr_drpd( ptr, USB_PORT0 );
    usb_hreg_clr_drpd( ptr, USB_PORT1 );
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
    usb_preg_clr_dprpu( ptr );
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
}
/******************************************************************************
End of function usb_cstd_ClearDline
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_PortSpeed
Description     : Get USB-speed of the specified port.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t port  : Root port
Return value    : uint16_t       : HSCONNECT : Hi-Speed
                :                : FSCONNECT : Full-Speed
                :                : LSCONNECT : Low-Speed
                :                : NOCONNECT : not connect
******************************************************************************/
uint16_t usb_cstd_PortSpeed(USB_UTR_t *ptr, uint16_t port)
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
}
/******************************************************************************
End of function usb_cstd_PortSpeed
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_HiSpeedEnable
Description     : Check if set to Hi-speed.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.    ;
                : uint16_t port  : Root port
Return value    : uint16_t       : YES; Hi-Speed enabled.
                :                : NO; Hi-Speed disabled.
******************************************************************************/
uint16_t usb_cstd_HiSpeedEnable(USB_UTR_t *ptr, uint16_t port)
{
    uint16_t    buf;

    buf = usb_creg_read_syscfg( ptr, port );

    if( (buf & USB_HSE) == USB_HSE )
    {
        /* Hi-Speed Enable */
        return USB_YES;
    }
    else
    {
        /* Hi-Speed Disable */
        return USB_NO;
    }
}
/******************************************************************************
End of function usb_cstd_HiSpeedEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_SetHse
Description     : Set/clear the HSE-bit of the specified port.
Arguments       : uint16_t port     : Root port.
                : uint16_t speed    : HS_ENABLE/HS_DISABLE.
Return value    : none
******************************************************************************/
void usb_cstd_SetHse(USB_UTR_t *ptr, uint16_t port, uint16_t speed)
{
    if( speed == USB_HS_DISABLE )
    {
        /* HSE = disable */
        usb_creg_clr_hse( ptr, port );
    }
    else
    {
        /* HSE = enable */
        usb_creg_set_hse( ptr, port );
    }
}
/******************************************************************************
End of function usb_cstd_SetHse
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_DummyFunction
Description     : dummy function
Arguments       : uint16_t data1    : Not used.
                : uint16_t data2    : Not used.
Return value    : none
******************************************************************************/
void usb_cstd_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
}
/******************************************************************************
End of function usb_cstd_DummyFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_DummyTrn
Description     : dummy function
Arguments       : USB_REQUEST_t *data1  : Not used.
                : uint16_t data2        : Not used.
Return value    : none
******************************************************************************/
void usb_cstd_DummyTrn(USB_UTR_t *ptr, USB_REQUEST_t *data1, uint16_t data2)
{
}
/******************************************************************************
End of function usb_cstd_DummyTrn
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_set_usbip_mode
Description     : Set the Host mode or Peripheral mode to USB H/W
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t function : Host mode/Peripheral mode
Return value    : none
******************************************************************************/
void usb_cstd_set_usbip_mode(USB_UTR_t *ptr, uint16_t function)
{
    /* USB interrupt message initialize */
    usb_cstd_InitUsbMessage(ptr, function);
    /* Select HW function */
    usb_cstd_set_usbip_mode_sub(ptr, function);
}/* eof usb_cstd_set_usbip_mode() */


#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
Function Name   : usb_cstd_set_sofcfg_intl
Description     : Set Interrupt sence mode(Level sence) for SOFCFG.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_cstd_set_sofcfg_intl( USB_UTR_t *ptr )
{
    usb_creg_set_sofcfg( ptr, USB_INTL );
} /* eof usb_cstd_set_sofcfg_intl() */
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */


/******************************************************************************
End  Of File
******************************************************************************/
