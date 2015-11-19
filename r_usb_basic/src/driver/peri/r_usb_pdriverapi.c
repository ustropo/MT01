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
* File Name    : r_usb_pdriverapi.c
* Description  : USB Peripheral Driver API code. PCD (Peripheral Control Driver)
               : USB Function transfer level commands. 
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
extern  void        R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode );
extern  void        R_usb_cstd_SetTaskPri(uint8_t tasknum, uint8_t pri);


/******************************************************************************
Renesas Abstracted Peripheral Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_PcdOpen
Description     : Start PCD(Peripheral Control Driver) task (RTOS version)
                : Initialize pipe information (non-OS version).
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return value    : USB_ER_t       : Error info.
******************************************************************************/
USB_ER_t R_usb_pstd_PcdOpen(USB_UTR_t *ptr)
{
    uint16_t    i;

    R_usb_cstd_SetTaskPri(USB_PCD_TSK,  USB_PRI_0);

    for( i = USB_PIPE0; i <= USB_MAX_PIPE_NO; i++ )
    {
        usb_gpstd_StallPipe[i]  = USB_DONE;
        usb_gcstd_Pipe[ptr->ip][i]      = (USB_UTR_t*)USB_NULL;
    }

    return USB_E_OK;
}/* eof R_usb_pstd_PcdOpen */

/******************************************************************************
Function Name   : R_usb_pstd_PcdClose
Description     : Stop PCD(Peripheral Control Driver) task
Arguments       : USB_UTR_t *ptr : Not used.
Return value    : USB_ER_t       : Error info.
******************************************************************************/
USB_ER_t R_usb_pstd_PcdClose(USB_UTR_t *ptr)
{
    return USB_E_OK;
}/* eof R_usb_pstd_PcdClose */

/******************************************************************************
Function Name   : R_usb_pstd_TransferStart
Description     : Transfer the data of each pipe 
                : Request PCD to transfer data, and the PCD transfers the data 
                  based on the transfer information stored in ptr
Arguments       : USB_UTR_t *ptr : keyword, msghead and msginfo are used for...  $REA
Return value    : USB_ER_t       : Error info.
******************************************************************************/
USB_ER_t R_usb_pstd_TransferStart(USB_UTR_t *ptr)
{
    USB_ER_t        err;
    uint16_t        pipenum;

    pipenum = ptr->keyword;
    if( usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL )
    {
        /* Get PIPE TYPE */
        if( usb_cstd_GetPipeType(ptr, pipenum) != USB_ISO )
        {
            USB_PRINTF1("### R_usb_pstd_TransferStart overlaps %d\n", pipenum);
            return USB_E_QOVR;
        }
    }

    /* Check state ( Configured ) */
    if( usb_pstd_ChkConfigured(ptr) != USB_YES )
    {
        USB_PRINTF0("### R_usb_pstd_TransferStart not configured\n");
        return USB_E_ERROR;
    }

    if( pipenum == USB_PIPE0 )
    {
        USB_PRINTF0("### R_usb_pstd_TransferStart PIPE0 is not support\n");
        return USB_E_ERROR;
    }

    ptr->msghead    = (USB_MH_t)USB_NULL;
    ptr->msginfo    = USB_MSG_PCD_SUBMITUTR;
    /* Send message */
    err = USB_SND_MSG(USB_PCD_MBX, (USB_MSG_t*)ptr);
    if( err != USB_E_OK )
    {
        USB_PRINTF1("### pTransferStart snd_msg error (%ld)\n", err);
    }
    return err;
}/* eof R_usb_pstd_TransferStart() */

/******************************************************************************
Function Name   : R_usb_pstd_TransferEnd
Description     : Force termination of data transfer of specified pipe. Request 
                : PCD to force termination 
                  data transfer, and the PCD forced-terminates data transfer. 
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel. 
                : uint16_t pipe     : Pipe number.
                : uint16_t status   : End status.
Return value    : USB_ER_t          : Error info.
******************************************************************************/
USB_ER_t R_usb_pstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t        info;

    if( usb_gcstd_Pipe[ptr->ip][pipe] == USB_NULL )
    {
        USB_PRINTF0("### R_usb_pstd_TransferEnd overlaps\n");
        return USB_E_QOVR;
    }

    /* check Time out */
    if( status == USB_DATA_TMO )
    {
        info = USB_MSG_PCD_TRANSEND1;
    }
    else
    {
        info = USB_MSG_PCD_TRANSEND2;
    }

    /* PCD Send Mailbox */
    return usb_pstd_PcdSndMbx(ptr, info, pipe, &usb_cstd_DummyFunction);
}/* eof R_usb_pstd_TransferEnd() */

/******************************************************************************
Function Name   : R_usb_pstd_ChangeDeviceState
Description     : Change USB Device to the status specified by argument
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t state            : New device status.
                : uint16_t port_no          : Pipe number etc.
                : USB_CB_t complete         : Callback function.
Return value    : USB_ER_t                  : Error info.
******************************************************************************/
USB_ER_t R_usb_pstd_ChangeDeviceState(USB_UTR_t *ptr, uint16_t state, uint16_t port_no, USB_CB_t complete)
{
    USB_ER_t        err;

    /* PCD Send Mailbox */
    err = usb_pstd_PcdSndMbx(ptr, state, port_no, complete);
    
    return err;
}/* eof R_usb_pstd_ChangeDeviceState() */

/******************************************************************************
Function Name   : R_usb_pstd_DeviceInformation
Description     : Get USB Device information such as USB Device status and con-
                : figuration No. etc. 
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t *tbl  : Device information storage pointer TBL. This 
                : pointer is used to provide the caller with the device's sta-
                : tus, speed, configuration and interface number, and the value
                : of the remote wakeup flag.
Return value    : none
******************************************************************************/
void R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl)
{
    /* Device status */
    tbl[0] = usb_creg_read_intsts( ptr ) & (uint16_t)(USB_VBSTS|USB_DVSQ);

    /* Speed */
    tbl[1] = usb_cstd_PortSpeed(ptr, (uint16_t)USB_PORT0);

    /* Configuration number */
    tbl[2] = usb_gpstd_ConfigNum;

    /* Interface number */
    tbl[3] = usb_pstd_GetInterfaceNum(usb_gpstd_ConfigNum);

    /* Remote Wakeup Flag */
    tbl[4] = usb_gpstd_RemoteWakeup;
}/* eof R_usb_pstd_DeviceInformation() */

/******************************************************************************
Function Name   : R_usb_pstd_DriverRegistration
Description     : Register pipe information table, descriptor information table, 
                : callback function, etc. This info is specified by the data in
                : the structure USB_PCDREG_t.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : USB_PCDREG_t *registinfo : Class driver structure.
Return value    : none
******************************************************************************/
void R_usb_pstd_DriverRegistration(USB_UTR_t *ptr, USB_PCDREG_t *registinfo)
{
    USB_PCDREG_t    *driver;

    driver = &usb_gpstd_Driver;
    /* Pipe define table address */
    driver->pipetbl     = registinfo->pipetbl;
    /* Device descriptor table address */
    driver->devicetbl   = registinfo->devicetbl;
    /* Qualifier descriptor table address */
    driver->qualitbl    = registinfo->qualitbl;
    /* Configuration descriptor table address */
    driver->configtbl   = registinfo->configtbl;
    /* Other configuration descriptor table address */
    driver->othertbl    = registinfo->othertbl;
    /* String descriptor table address */
    driver->stringtbl   = registinfo->stringtbl;
    /* Driver init */
    driver->classinit   = registinfo->classinit;
    /* Device default */
    driver->devdefault  = registinfo->devdefault;
    /* Device configured */
    driver->devconfig   = registinfo->devconfig;
    /* Device detach */
    driver->devdetach   = registinfo->devdetach;
    /* Device suspend */
    driver->devsuspend  = registinfo->devsuspend;
    /* Device resume */
    driver->devresume   = registinfo->devresume;
    /* Interfaced change */
    driver->interface   = registinfo->interface;
    /* Control transfer */
    driver->ctrltrans   = registinfo->ctrltrans;
    /* Initialized device driver */
    (*driver->classinit)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
}/* eof R_usb_pstd_DriverRegistration() */

/******************************************************************************
Function Name   : R_usb_pstd_DriverRelease
Description     : Clear the information registered in the structure USB_PCDREG_t.
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_pstd_DriverRelease(void)
{
    USB_PCDREG_t    *driver;

    driver = &usb_gpstd_Driver;
    /* Pipe define table address */
    driver->pipetbl     = (uint16_t**)0u;
    /* Device descriptor table address */
    driver->devicetbl   = (uint8_t*)0u;
    /* Qualifier descriptor table address */
    driver->qualitbl    = (uint8_t*)0u;
    /* Configuration descriptor table address */
    driver->configtbl   = (uint8_t**)0u;
    /* Other configuration descriptor table address */
    driver->othertbl    = (uint8_t**)0u;
    /* String descriptor table address */
    driver->stringtbl   = (uint8_t**)0u;
    /* Driver init */
    driver->classinit   = &usb_cstd_DummyFunction;
    /* Device default */
    driver->devdefault  = &usb_cstd_DummyFunction;
    /* Device configured */
    driver->devconfig   = &usb_cstd_DummyFunction;
    /* Device detach */
    driver->devdetach   = &usb_cstd_DummyFunction;
    /* Device suspend */
    driver->devsuspend  = &usb_cstd_DummyFunction;
    /* Device resume */
    driver->devresume   = &usb_cstd_DummyFunction;
    /* Interfaced change */
    driver->interface   = &usb_cstd_DummyFunction;
    /* Control transfer */
    driver->ctrltrans   = &usb_cstd_DummyTrn;
}/* eof R_usb_pstd_DriverRelease() */

/******************************************************************************
Function Name   : R_usb_pstd_SetPipeRegister
Description     : Set specified pipe configuration of specified pipe no.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t pipe_number  : pipe number
                : uint16_t *tbl         : DEF_EP table pointer
Return value    : none
******************************************************************************/
void R_usb_pstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t pipe_number, uint16_t *tbl)
{
    usb_pstd_SetPipeRegister( ptr, pipe_number, tbl);
}/* eof R_usb_pstd_SetPipeRegister() */

/******************************************************************************
Function Name   : R_usb_pstd_SetPipeStall
Description     : Pipe Stall Set
Arguments       : USB_UTR_t *ptr
Return value    : none
******************************************************************************/
void R_usb_pstd_SetPipeStall(USB_UTR_t *ptr, uint16_t pipeno)
{
    usb_pstd_SetStall(ptr, pipeno);
}
/******************************************************************************
End of function R_usb_pstd_SetPipeStall
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_SetStall
Description     : Set pipe stall request
Arguments       : USB_CB_t complete ; callback function
                : uint16_t pipe ; pipe number
Return value    : USB_ER_t Error Info
******************************************************************************/
USB_ER_t R_usb_pstd_SetStall(USB_UTR_t *ptr, USB_CB_t complete, uint16_t pipe)
{
    /* PCD Send Mailbox */
    return usb_pstd_PcdSndMbx(ptr, (uint16_t)USB_MSG_PCD_SETSTALL, pipe, complete);
}
/******************************************************************************
End of function R_usb_pstd_SetStall
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_ControlRead
Description     : Start control read transfer (API). When a valid control read 
                : request is received from host, the ControlRead function gen-
                : erates data for transmission to the host and writes it to the 
                : FIFO.
Arguments       : uint32_t bsize    : Read size in bytes.
                : uint8_t *table    : Start address of read data buffer.
Return value    : uint16_t          : USB_WRITESHRT/USB_WRITE_END/USB_WRITING/
                :                   : USB_FIFOERROR.
******************************************************************************/
uint16_t R_usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table)
{
    uint16_t    end_flag;

    end_flag = usb_pstd_ControlRead( ptr, bsize, table);

    return (end_flag);
}
/******************************************************************************
End of function R_usb_pstd_ControlRead
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_ControlWrite
Description     : Start control write transfer (API). When a valid control write 
                : request from host is received, the ControlWrite function 
                : enables data reception from the host.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint32_t bsize    : Write size in bytes.
                : uint8_t *table    : Start address of write data buffer.
Return value    : none
******************************************************************************/
void R_usb_pstd_ControlWrite(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table)
{
    usb_gcstd_DataCnt[ptr->ip][USB_PIPE0] = bsize;
    usb_gcstd_DataPtr[ptr->ip][USB_PIPE0] = table;

    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
    /* Buffer clear */
    usb_creg_set_bclr( ptr, USB_CUSE );

    /* Interrupt enable */
    /* Enable ready interrupt */
    usb_creg_set_brdyenb(ptr, (uint16_t)USB_PIPE0);
    /* Enable not ready interrupt */
    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);

    /* Set PID=BUF */
    usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
}
/******************************************************************************
End of function R_usb_pstd_ControlWrite
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_ControlEnd
Description     : End control transfer.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t status   : Control transfer status.
Return value    : none
******************************************************************************/
void R_usb_pstd_ControlEnd(USB_UTR_t *ptr, uint16_t status)
{
    usb_pstd_ControlEnd( ptr, status);
}
/******************************************************************************
End of function R_usb_pstd_ControlEnd
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_pstd_usbdriver_start
Description     :  Start peripheral USB low-level driver task.
Argument        : none
Return          : none
******************************************************************************/
void R_usb_pstd_usbdriver_start( USB_UTR_t *ptr )
{
    /* Task Start Processing */
    R_usb_pstd_PcdOpen(ptr);            /* Pcd open */
}
/******************************************************************************
End of function R_usb_pstd_usbdriver_start()
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
Function Name   : R_usb_pstd_PcdTask
Description     : Call PCD (Peripheral Control Driver) task (API for nonOS).
Arguments       : USB_VP_INT stacd: Start Code
Return value    : none
******************************************************************************/
void R_usb_pstd_PcdTask(USB_VP_INT stacd)
{
#if (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP)
    usb_pstd_PcdTask( stacd );
#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */
}/* eof R_usb_pstd_PcdTask() */

/******************************************************************************
End  Of File
******************************************************************************/
