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
* File Name    : r_usb_pstdrequest.c
* Description  : USB Peripheral standard request code
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
Static variables and functions
******************************************************************************/
static    void        usb_pstd_GetStatus1(USB_UTR_t *ptr);
static    void        usb_pstd_GetDescriptor1(USB_UTR_t *ptr);
static    void        usb_pstd_GetConfiguration1(USB_UTR_t *ptr);
static    void        usb_pstd_GetInterface1(USB_UTR_t *ptr);
static    void        usb_pstd_ClearFeature0(void);
static    void        usb_pstd_ClearFeature3(USB_UTR_t *ptr);
static    void        usb_pstd_SetFeature0(void);
static    void        usb_pstd_SetFeature3(USB_UTR_t *ptr);
static    void        usb_pstd_SetAddress0(void);
static    void        usb_pstd_SetAddress3(USB_UTR_t *ptr);
static    void        usb_pstd_SetDescriptor2(USB_UTR_t *ptr);
static    void        usb_pstd_SetConfiguration0(USB_UTR_t *ptr);
static    void        usb_pstd_SetConfiguration3(USB_UTR_t *ptr);
static    void        usb_pstd_SetInterface0(USB_UTR_t *ptr);
static    void        usb_pstd_SetInterface3(USB_UTR_t *ptr);
static    void        usb_pstd_SynchFrame1(USB_UTR_t *ptr);

/*****************************************************************************
Public Variables
******************************************************************************/

/******************************************************************************
Renesas Abstracted Peripheral standard request functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq0
Description     : The idle and setup stages of a standard request from host.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_StandReq0(USB_UTR_t *ptr)
{
    switch( usb_gpstd_ReqRequest )
    {
        case USB_CLEAR_FEATURE:
            /* Clear Feature0 */
            usb_pstd_ClearFeature0();
        break;
        case USB_SET_FEATURE:
            /* Set Feature0 */
            usb_pstd_SetFeature0();
        break;
        case USB_SET_ADDRESS:
            /* Set Address0 */
            usb_pstd_SetAddress0();
        break;
        case USB_SET_CONFIGURATION:
            /* Set Configuration0 */
            usb_pstd_SetConfiguration0(ptr);
        break;
        case USB_SET_INTERFACE:
            /* Set Interface0 */
            usb_pstd_SetInterface0(ptr);
        break;
        default:
        break;
    }
}
/******************************************************************************
End of function usb_pstd_StandReq0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq1
Description     : The control read data stage of a standard request from host.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_StandReq1(USB_UTR_t *ptr)
{
    switch( usb_gpstd_ReqRequest )
    {
        case USB_GET_STATUS:
            /* Get Status1 */
            usb_pstd_GetStatus1(ptr);
        break;
        case USB_GET_DESCRIPTOR:
            /* Get Descriptor1 */
            usb_pstd_GetDescriptor1(ptr);
        break;
        case USB_GET_CONFIGURATION:
            /* Get Configuration1 */
            usb_pstd_GetConfiguration1(ptr);
        break;
        case USB_GET_INTERFACE:
            /* Get Interface1 */
            usb_pstd_GetInterface1(ptr);
        break;
        case USB_SYNCH_FRAME:
            /* Synch Frame */
            usb_pstd_SynchFrame1(ptr);
        break;
        default:
            /* Set pipe USB_PID_STALL */
            usb_pstd_SetStallPipe0( ptr );
        break;
    }
}
/******************************************************************************
End of function usb_pstd_StandReq1
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq2
Description     : The control write data stage of a standard request from host.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_StandReq2(USB_UTR_t *ptr)
{
    if( usb_gpstd_ReqRequest == USB_SET_DESCRIPTOR )
    {
        /* Set Descriptor2 */
        usb_pstd_SetDescriptor2(ptr);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_StandReq2
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq3
Description     : Standard request process. This is for the status stage of a 
                : control write where there is no data stage.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_StandReq3(USB_UTR_t *ptr)
{
    switch( usb_gpstd_ReqRequest )
    {
        case USB_CLEAR_FEATURE:
            /* ClearFeature3 */
            usb_pstd_ClearFeature3(ptr);
        break;
        case USB_SET_FEATURE:
            /* SetFeature3 */
            usb_pstd_SetFeature3(ptr);
        break;
        case USB_SET_ADDRESS:
            /* SetAddress3 */
            usb_pstd_SetAddress3(ptr);
        break;
        case USB_SET_CONFIGURATION:
            /* SetConfiguration3 */
            usb_pstd_SetConfiguration3(ptr);
        break;
        case USB_SET_INTERFACE:
            /* SetInterface3 */
            usb_pstd_SetInterface3(ptr);
        break;
        default:
            /* Set pipe USB_PID_STALL */
            usb_pstd_SetStallPipe0( ptr );
        break;
    }
    /* Control transfer stop(end) */
    usb_pstd_ControlEnd(ptr, (uint16_t)USB_CTRL_END);
}
/******************************************************************************
End of function usb_pstd_StandReq3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq4
Description     : The control read status stage of a standard request from host.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_StandReq4(USB_UTR_t *ptr)
{
    switch( usb_gpstd_ReqRequest )
    {
        case USB_GET_STATUS:
            /* GetStatus4 */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
        case USB_GET_DESCRIPTOR:
            /* GetDescriptor4 */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
        case USB_GET_CONFIGURATION:
            /* GetConfiguration4 */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
        case USB_GET_INTERFACE:
            /* GetInterface4 */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
        case USB_SYNCH_FRAME:
            /* SynchFrame4 */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
        default:
            /* Set pipe USB_PID_STALL */
            usb_pstd_SetStallPipe0( ptr );
        break;
    }
    /* Control transfer stop(end) */
    usb_pstd_ControlEnd(ptr, (uint16_t)USB_CTRL_END);
}
/******************************************************************************
End of function usb_pstd_StandReq4
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_StandReq5
Description     : The control write status stage of a standard request from host.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_StandReq5(USB_UTR_t *ptr)
{
    if( usb_gpstd_ReqRequest == USB_SET_DESCRIPTOR )
    {
        /* Set pipe PID_BUF */
        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_SetStallPipe0( ptr );
    }
    /* Control transfer stop(end) */
    usb_pstd_ControlEnd(ptr, (uint16_t)USB_CTRL_END);
}
/******************************************************************************
End of function usb_pstd_StandReq5
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_GetStatus1
Description     : Analyze a Get Status command and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_GetStatus1(USB_UTR_t *ptr)
{
    static uint8_t  tbl[2];
    uint16_t        ep;
    uint16_t        buffer, pipe;

    if( (usb_gpstd_ReqValue == 0) && (usb_gpstd_ReqLength == 2) )
    {
        tbl[0] = 0;
        tbl[1] = 0;
        /* Check request type */
        switch( usb_gpstd_ReqTypeRecip )
        {
            case USB_DEVICE:
                if( usb_gpstd_ReqIndex == 0 )
                {
                    /* Self powered / Bus powered */
                    tbl[0] = usb_pstd_GetCurrentPower();
                    /* Support remote wakeup ? */
                    if( usb_gpstd_RemoteWakeup == USB_YES )
                    {
                        tbl[0] |= USB_GS_REMOTEWAKEUP;
                    }
                    /* Control read start */
                    usb_pstd_ControlRead(ptr, (uint32_t)2, tbl);
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            case USB_INTERFACE:
                if( usb_pstd_ChkConfigured(ptr) == USB_YES )
                {
                    if( usb_gpstd_ReqIndex < usb_pstd_GetInterfaceNum(usb_gpstd_ConfigNum) )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)2, tbl);
                    }
                    else
                    {
                        /* Request error (not exist interface) */
                        usb_pstd_SetStallPipe0( ptr );
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            case USB_ENDPOINT:
                /* Endpoint number */
                ep = (uint16_t)(usb_gpstd_ReqIndex & USB_EPNUMFIELD);
                /* Endpoint 0 */
                if( ep == 0 )
                {
                    buffer = usb_creg_read_dcpctr( ptr );
                    if( (buffer & USB_PID_STALL) != (uint16_t)0 )
                    {
                       /* Halt set */
                       tbl[0] = USB_GS_HALT;
                    }
                    /* Control read start */
                    usb_pstd_ControlRead(ptr, (uint32_t)2, tbl);
                }
                /* EP1 to max */
                else if( ep <= USB_MAX_EP_NO )
                {
                    if( usb_pstd_ChkConfigured(ptr) == USB_YES )
                    {
                        pipe = usb_cstd_Epadr2Pipe(ptr, usb_gpstd_ReqIndex);
                        if( pipe == USB_ERROR )
                        {
                            /* Set pipe USB_PID_STALL */
                            usb_pstd_SetStallPipe0( ptr );
                        }
                        else
                        {
                            buffer = usb_cstd_GetPid(ptr, pipe);
                            if( (buffer & USB_PID_STALL) != (uint16_t)0 )
                            {
                                /* Halt set */
                                tbl[0] = USB_GS_HALT;
                            }
                            /* Control read start */
                            usb_pstd_ControlRead(ptr, (uint32_t)2, tbl);
                        }
                    }
                    else
                    {
                        /* Set pipe USB_PID_STALL */
                        usb_pstd_SetStallPipe0( ptr );
                    }
                }
                else
                {
                    /* Set pipe USB_PID_STALL */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            default:
                /* Set pipe USB_PID_STALL */
                usb_pstd_SetStallPipe0( ptr );
            break;
        }
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_GetStatus1
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_GetDescriptor1
Description     : Analyze a Get Descriptor command from host and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_GetDescriptor1(USB_UTR_t *ptr)
{
    uint16_t    len;
    uint16_t    idx;
    uint8_t     *table;

    if(usb_gpstd_ReqTypeRecip == USB_DEVICE )
    {
        idx = (uint16_t)(usb_gpstd_ReqValue & USB_DT_INDEX);
        switch( (uint16_t)USB_GET_DT_TYPE(usb_gpstd_ReqValue) )
        {
            /*---- Device descriptor ----*/
            case USB_DT_DEVICE:
                if((usb_gpstd_ReqIndex == (uint16_t)0) && (idx == (uint16_t)0))
                {
                    table = usb_gpstd_Driver.devicetbl;
                    if( usb_gpstd_ReqLength < table[0] )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)usb_gpstd_ReqLength, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)table[0], table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            /*---- Configuration descriptor ----*/
            case USB_DT_CONFIGURATION:
                if(( usb_gpstd_ReqIndex == 0 ) && (idx == (uint16_t)0))
                {
                    table = usb_gpstd_Driver.configtbl[idx];
                    len   = (uint16_t)(*(uint8_t*)((uint32_t)table + (uint32_t)3));
                    len   = (uint16_t)(len << 8);
                    len   += (uint16_t)(*(uint8_t*)((uint32_t)table + (uint32_t)2));
                    /* Descriptor > wLength */
                    if( usb_gpstd_ReqLength < len )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)usb_gpstd_ReqLength, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            /*---- String descriptor ----*/
            case USB_DT_STRING:
                if( idx < USB_STRINGNUM )
                {
                    table = usb_gpstd_Driver.stringtbl[idx];
                    len   = (uint16_t)(*(uint8_t*)((uint32_t)table + (uint32_t)0));
                    if( usb_gpstd_ReqLength < len )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)usb_gpstd_ReqLength, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            /*---- Interface descriptor ----*/
            case USB_DT_INTERFACE:
                /* Set pipe USB_PID_STALL */
                usb_pstd_SetStallPipe0( ptr );
            break;
            /*---- Endpoint descriptor ----*/
            case USB_DT_ENDPOINT:
                /* Set pipe USB_PID_STALL */
                usb_pstd_SetStallPipe0( ptr );
            break;
            case USB_DT_DEVICE_QUALIFIER:
                if( ((usb_cstd_HiSpeedEnable(ptr, (uint16_t)USB_PORT0) == USB_YES)
                    && (usb_gpstd_ReqIndex == 0)) && (idx == 0) )
                {
                    table = usb_gpstd_Driver.qualitbl;
                    if( usb_gpstd_ReqLength < table[0] )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)usb_gpstd_ReqLength, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)table[0], table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            case USB_DT_OTHER_SPEED_CONF:
                if( (usb_cstd_HiSpeedEnable(ptr, (uint16_t)USB_PORT0) == USB_YES)
                    && (usb_gpstd_ReqIndex == 0) && (idx == (uint16_t)0))
                {
                    table = usb_gpstd_Driver.othertbl[idx];
                    len     = (uint16_t)(*(uint8_t*)((uint32_t)table + (uint32_t)3));
                    len     = (uint16_t)(len << 8);
                    len += (uint16_t)(*(uint8_t*)((uint32_t)table + (uint32_t)2));
                    /* Descriptor > wLength */
                    if( usb_gpstd_ReqLength < len )
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)usb_gpstd_ReqLength, table);
                    }
                    else
                    {
                        /* Control read start */
                        usb_pstd_ControlRead(ptr, (uint32_t)len, table);
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            case USB_DT_INTERFACE_POWER:
                /* Not support */
                usb_pstd_SetStallPipe0( ptr );
            break;
            default:
                /* Set pipe USB_PID_STALL */
                usb_pstd_SetStallPipe0( ptr );
            break;
        }
    }
    else if( usb_gpstd_ReqTypeRecip == USB_INTERFACE )
    {
        usb_gpstd_ReqReg.ReqType        = usb_gpstd_ReqType;
        usb_gpstd_ReqReg.ReqTypeType    = usb_gpstd_ReqTypeType;
        usb_gpstd_ReqReg.ReqTypeRecip   = usb_gpstd_ReqTypeRecip;
        usb_gpstd_ReqReg.ReqRequest     = usb_gpstd_ReqRequest;
        usb_gpstd_ReqReg.ReqValue       = usb_gpstd_ReqValue;
        usb_gpstd_ReqReg.ReqIndex       = usb_gpstd_ReqIndex;
        usb_gpstd_ReqReg.ReqLength      = usb_gpstd_ReqLength;
        (*usb_gpstd_Driver.ctrltrans)(ptr, (USB_REQUEST_t *)&usb_gpstd_ReqReg, (uint16_t)USB_NO_ARG);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_GetDescriptor1
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_GetConfiguration1
Description     : Analyze a Get Configuration command and process it.
                : (for control read data stage)
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_GetConfiguration1(USB_UTR_t *ptr)
{
    static uint8_t    tbl[2];

    /* check request */
    if( (((usb_gpstd_ReqTypeRecip == USB_DEVICE) 
        && (usb_gpstd_ReqValue == 0)) 
        && (usb_gpstd_ReqIndex == 0)) 
        && (usb_gpstd_ReqLength == 1) )
    {
        tbl[0] = (uint8_t)usb_gpstd_ConfigNum;
        /* Control read start */
        usb_pstd_ControlRead(ptr, (uint32_t)1, tbl);
    }
    else
    {
        /* Set pipe USB_PID_STALL */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_GetConfiguration1
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_GetInterface1
Description     : Analyze a Get Interface command and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_GetInterface1(USB_UTR_t *ptr)
{
    static uint8_t    tbl[2];

    /* check request */
    if( ((usb_gpstd_ReqTypeRecip == USB_INTERFACE) && (usb_gpstd_ReqValue == 0)) && (usb_gpstd_ReqLength == 1) )
    {
        if( usb_gpstd_ReqIndex < USB_ALT_NO )
        {
            tbl[0] = (uint8_t)usb_gpstd_AltNum[usb_gpstd_ReqIndex];
            /* Start control read */
            usb_pstd_ControlRead(ptr, (uint32_t)1, tbl);
        }
        else
        {
            /* Request error */
            usb_pstd_SetStallPipe0( ptr );
        }
    }
    else
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_GetInterface1
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ClearFeature0
Description     : Clear Feature0
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_ClearFeature0(void)
{
}
/******************************************************************************
End of function usb_pstd_ClearFeature0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ClearFeature3
Description     : Analyze a Clear Feature command and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_ClearFeature3(USB_UTR_t *ptr)
{
    uint16_t        pipe;
    uint16_t        ep;

    if( usb_gpstd_ReqLength == 0 )
    {
        /* check request type */
        switch( usb_gpstd_ReqTypeRecip )
        {
            case USB_DEVICE:
                if( (usb_gpstd_ReqValue == USB_DEV_REMOTE_WAKEUP)
                    && (usb_gpstd_ReqIndex == 0) )
                {
                    if( usb_pstd_ChkRemote() == USB_YES )
                    {
                        usb_gpstd_RemoteWakeup = USB_NO;
                        /* Set pipe PID_BUF */
                        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                    }
                    else
                    {
                        /* Not support remote wakeup */
                        /* Request error */
                        usb_pstd_SetStallPipe0( ptr );
                    }
                }
                else
                {
                    /* Not specification */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            case USB_INTERFACE:
                /* Request error */
                usb_pstd_SetStallPipe0( ptr );
            break;
            case USB_ENDPOINT:
                /* Endpoint number */
                ep = (uint16_t)(usb_gpstd_ReqIndex & USB_EPNUMFIELD);
                if( usb_gpstd_ReqValue == USB_ENDPOINT_HALT )
                {
                    /* EP0 */
                    if( ep == 0 )
                    {
                        /* Stall clear */
                        usb_cstd_ClrStall(ptr, (uint16_t)USB_PIPE0);
                        /* Set pipe PID_BUF */
                        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                    }
                    /* EP1 to max */
                    else if( ep <= USB_MAX_EP_NO )
                    {
                        pipe = usb_cstd_Epadr2Pipe(ptr, usb_gpstd_ReqIndex);
                        if( pipe == USB_ERROR )
                        {
                            /* Request error */
                            usb_pstd_SetStallPipe0( ptr );
                        }
                        else
                        {
                            if( usb_cstd_GetPid(ptr, pipe) == USB_PID_BUF )
                            {
                                usb_cstd_SetNak(ptr, pipe);
                                /* SQCLR=1 */
                                usb_creg_set_sqclr(ptr, pipe);
                                /* Set pipe PID_BUF */
                                usb_cstd_SetBuf(ptr, pipe);
                            }
                            else
                            {
                                usb_cstd_ClrStall(ptr, pipe);
                                /* SQCLR=1 */
                                usb_creg_set_sqclr(ptr, pipe);
                            }
                            /* Set pipe PID_BUF */
                            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                            if( usb_gpstd_StallPipe[pipe] == USB_YES )
                            {
                                usb_gpstd_StallPipe[pipe] = USB_DONE;
                                (*usb_gpstd_StallCB)(ptr, pipe, (uint16_t)0);
                            }
                        }
                    }
                    else
                    {
                        /* Request error */
                        usb_pstd_SetStallPipe0( ptr );
                    }
                }
                else
                {
                    /* Request error */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;
            default:
                usb_pstd_SetStallPipe0( ptr );
            break;
        }
    }
    else
    {
        /* Not specification */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_ClearFeature3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetFeature0
Description     : Set Feature0 (for idle/setup stage)
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SetFeature0(void)
{
}
/******************************************************************************
End of function usb_pstd_SetFeature0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetFeature3
Description     : Analyze a Set Feature command and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetFeature3(USB_UTR_t *ptr)
{
    uint16_t    pipe;
    uint16_t    ep;

    if( usb_gpstd_ReqLength == 0 )
    {
        /* check request type */
        switch( usb_gpstd_ReqTypeRecip )
        {
            case USB_DEVICE:
                switch( usb_gpstd_ReqValue )
                {
                    case USB_DEV_REMOTE_WAKEUP:
                        if( usb_gpstd_ReqIndex == 0 )
                        {
                            if( usb_pstd_ChkRemote() == USB_YES )
                            {
                                usb_gpstd_RemoteWakeup = USB_YES;
                                /* Set pipe PID_BUF */
                                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                            }
                            else
                            {
                                /* Not support remote wakeup */
                                /* Request error */
                                usb_pstd_SetStallPipe0( ptr );
                            }
                        }
                        else
                        {
                            /* Not specification */
                            usb_pstd_SetStallPipe0( ptr );
                        }
                    break;
#if ((( USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
    ||(( USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
                    case USB_TEST_MODE:
                        if( usb_cstd_PortSpeed(ptr, (uint16_t)USB_PORT0) == USB_HSCONNECT )
                        {
                            if( (usb_gpstd_ReqIndex < USB_TEST_RESERVED) || (USB_TEST_VSTMODES <= usb_gpstd_ReqIndex) )
                            {
                                usb_gpstd_TestModeFlag = USB_YES;
                                usb_gpstd_TestModeSelect = usb_gpstd_ReqIndex;
                                /* Set pipe PID_BUF */
                                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                            }
                            else
                            {
                                /* Not specification */
                                usb_pstd_SetStallPipe0( ptr );
                            }
                        }
                        else
                        {
                            /* Not specification */
                            usb_pstd_SetStallPipe0( ptr );
                        }
                    break;
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */

                    default:
                        usb_pstd_SetFeatureFunction(ptr);
                    break;
                }
            break;
            case USB_INTERFACE:
                /* Set pipe USB_PID_STALL */
                usb_pstd_SetStallPipe0( ptr );
            break;
            case USB_ENDPOINT:
                /* Endpoint number */
                ep = (uint16_t)(usb_gpstd_ReqIndex & USB_EPNUMFIELD);
                if( usb_gpstd_ReqValue == USB_ENDPOINT_HALT )
                {
                    /* EP0 */
                    if( ep == 0 )
                    {
                        /* Set pipe PID_BUF */
                        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                    }
                    /* EP1 to max */
                    else if( ep <= USB_MAX_EP_NO )
                    {
                        pipe = usb_cstd_Epadr2Pipe(ptr, usb_gpstd_ReqIndex);
                        if( pipe == USB_ERROR )
                        {
                            /* Request error */
                            usb_pstd_SetStallPipe0( ptr );
                        }
                        else
                        {
                            /* Set pipe USB_PID_STALL */
                            usb_pstd_SetStall(ptr, pipe);
                            /* Set pipe PID_BUF */
                            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                        }
                    }
                    else
                    {
                        /* Request error */
                        usb_pstd_SetStallPipe0( ptr );
                    }
                }
                else
                {
                    /* Not specification */
                    usb_pstd_SetStallPipe0( ptr );
                }
            break;

            default:
                /* Request error */
                usb_pstd_SetStallPipe0( ptr );
            break;
        }
    }
    else
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SetFeature3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetAddress0
Description     : Set Address0 (for idle/setup stage).
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SetAddress0(void)
{
}
/******************************************************************************
End of function usb_pstd_SetAddress0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetAddress3
Description     : Analyze a Set Address command and process it.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetAddress3(USB_UTR_t *ptr)
{
    if( usb_gpstd_ReqTypeRecip == USB_DEVICE )
    {
        if( (usb_gpstd_ReqIndex == 0) && (usb_gpstd_ReqLength == 0) )
        {
            if( usb_gpstd_ReqValue <= 127 )
            {
                /* Set pipe PID_BUF */
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
            }
            else
            {
                /* Not specification */
                usb_pstd_SetStallPipe0( ptr );
            }
        }
        else
        {
            /* Not specification */
            usb_pstd_SetStallPipe0( ptr );
        }
    }
    else
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SetAddress3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetDescriptor2
Description     : Return STALL in response to a Set Descriptor command.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetDescriptor2(USB_UTR_t *ptr)
{
    /* Not specification */
    usb_pstd_SetStallPipe0( ptr );
}
/******************************************************************************
End of function usb_pstd_SetDescriptor2
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_SetConfiguration0
Description     : Call callback function to notify the reception of SetConfiguration command
                : (for idle /setup stage)
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetConfiguration0(USB_UTR_t *ptr)
{
    uint16_t config_num;

    config_num = usb_gpstd_ConfigNum;

    /* Configuration number set */
    usb_pstd_SetConfigNum(usb_gpstd_ReqValue);

    if( usb_gpstd_ReqValue != config_num )
    {
        /* Registration open function call */
        (*usb_gpstd_Driver.devconfig)(ptr, usb_gpstd_ConfigNum, (uint16_t)USB_NO_ARG);
    }
}
/******************************************************************************
End of function usb_pstd_SetConfiguration0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetConfiguration3
Description     : Analyze a Set Configuration command and process it. This is
                : for the status stage of a control write where there is no data
                : stage.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetConfiguration3(USB_UTR_t *ptr)
{
    uint16_t    i, j;
    uint16_t    ifc, cfgnum, cfgok;
    uint16_t    *table;
    uint8_t     *table2;

    if( usb_gpstd_ReqTypeRecip == USB_DEVICE )
    {
        cfgnum  = usb_pstd_GetConfigNum();
        cfgok   = USB_NG;

        for ( j = 0; j < cfgnum; j++ )
        {
            table2 = usb_gpstd_Driver.configtbl[j];

            if( (((usb_gpstd_ReqValue == table2[5]) || (usb_gpstd_ReqValue == 0))
                && (usb_gpstd_ReqIndex == 0)) && (usb_gpstd_ReqLength == 0) )
            {
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                cfgok    = USB_OK;

                if( ( usb_gpstd_ReqValue > 0 ) && ( usb_gpstd_ReqValue != usb_gpstd_ConfigNum ) )
                {
                    usb_pstd_ClearEpTblIndex();
                    ifc = usb_pstd_GetInterfaceNum(usb_gpstd_ReqValue);
                    for( i = 0; i < ifc; ++i )
                    {
                        /* Pipe Information Table ("endpoint table") initialize */
                        usb_pstd_SetEpTblIndex(usb_gpstd_ReqValue, i, (uint16_t)0);
                    }
                    table = usb_gpstd_Driver.pipetbl[usb_gpstd_ReqValue - 1];
                    /* Clear pipe configuration register */
                    usb_pstd_SetPipeRegister(ptr, (uint16_t)USB_CLRPIPE, table);
                    /* Set pipe configuration register */
                    usb_pstd_SetPipeRegister(ptr, (uint16_t)USB_PERIPIPE, table);
                }
                break;
            }
        }
        if( cfgok == USB_NG )
        {
            /* Request error */
            usb_pstd_SetStallPipe0( ptr );
        }
    }
    else
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SetConfiguration3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetInterface0
Description     : Call callback function to notify reception of SetInterface com-
                : mand. For idle/setup stage.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_SetInterface0(USB_UTR_t *ptr)
{
    /* Interfaced change function call */
    (*usb_gpstd_Driver.interface)(ptr, usb_gpstd_AltNum[usb_gpstd_ReqIndex], (uint16_t)USB_NO_ARG);
}
/******************************************************************************
End of function usb_pstd_SetInterface0
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetInterface3
Description     : Analyze a Set Interface command and request the process for 
                : the command. This is for a status stage of a control write 
                : where there is no data stage.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SetInterface3(USB_UTR_t *ptr)
{
    uint16_t    *table;
    uint16_t    conf;

    conf = usb_gpstd_ConfigNum;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Configured ? */
    if( (usb_pstd_ChkConfigured(ptr) == USB_YES) 
        && (usb_gpstd_ReqTypeRecip == USB_INTERFACE) )
    {
        if( (usb_gpstd_ReqIndex <= usb_pstd_GetInterfaceNum(usb_gpstd_ConfigNum)) && (usb_gpstd_ReqLength == 0) )
        {
            if( usb_gpstd_ReqValue <= usb_pstd_GetAlternateNum(usb_gpstd_ConfigNum, usb_gpstd_ReqIndex) )
            {
                usb_gpstd_AltNum[usb_gpstd_ReqIndex] = (uint16_t)(usb_gpstd_ReqValue & USB_ALT_SET);
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
                usb_pstd_ClearEpTblIndex();
                /* Search endpoint setting */
                usb_pstd_SetEpTblIndex(usb_gpstd_ConfigNum, usb_gpstd_ReqIndex, usb_gpstd_AltNum[usb_gpstd_ReqIndex]);
                table = usb_gpstd_Driver.pipetbl[conf - 1];
                /* Set pipe configuration register */
                usb_pstd_SetPipeRegister(ptr, (uint16_t)USB_PERIPIPE, table);
            }
            else
            {
                /* Request error */
                usb_pstd_SetStallPipe0( ptr );
            }
        }
        else
        {
            /* Request error */
            usb_pstd_SetStallPipe0( ptr );
        }
    }
    else
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SetInterface3
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SynchFrame1
Description     : Return STALL response to SynchFrame command.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SynchFrame1(USB_UTR_t *ptr)
{
    /* Set pipe USB_PID_STALL */
    usb_pstd_SetStallPipe0( ptr );
}
/******************************************************************************
End of function usb_pstd_SynchFrame1
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/