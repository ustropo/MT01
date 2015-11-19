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
* File Name    : r_usb_hdriver.c
* Description  : USB Host Control Driver
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
Private global variables and functions
******************************************************************************/
/* Device driver (registration) */
USB_HCDREG_t    usb_ghstd_DeviceDrv[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Root port, status, config num, interface class, speed, */
uint16_t        usb_ghstd_DeviceInfo[USB_NUM_USBIP][USB_MAXDEVADDR + 1u][8u];
uint16_t        usb_ghstd_RemortPort[2u];
/* Control transfer stage management */
uint16_t        usb_ghstd_Ctsq[USB_NUM_USBIP];
/* Manager mode */
uint16_t        usb_ghstd_MgrMode[USB_NUM_USBIP][2u];
/* DEVSEL & DCPMAXP (Multiple device) */
uint16_t        usb_ghstd_DcpRegister[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Device address */
uint16_t        usb_ghstd_DeviceAddr[USB_NUM_USBIP];
/* Reset handshake result */
uint16_t        usb_ghstd_DeviceSpeed[USB_NUM_USBIP];
/* Device driver number */
uint16_t        usb_ghstd_DeviceNum[USB_NUM_USBIP];
/* Ignore count */
uint16_t        usb_ghstd_IgnoreCnt[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

/******************************************************************************
Static variables and functions
******************************************************************************/
static    USB_HCDINFO_t    *usb_shstd_HcdMsg;
static uint16_t         usb_shstd_ClearStallPipe;
static uint16_t         usb_shstd_ClearStallRequest[5];
static uint8_t          usb_shstd_ClearStallData[10];
static USB_UTR_t        usb_shstd_ClearStallControl;
static USB_CB_t    usb_shstd_ClearStallCall;

static USB_ER_t usb_hstd_SetSubmitutr(USB_UTR_t *ptr);
static void usb_hstd_SetReTransfer(USB_UTR_t *ptr, uint16_t pipe);
static void usb_hstd_ClearStallResult(USB_UTR_t *ptr, uint16_t data1, uint16_t data2 );


/******************************************************************************
External variables and functions
******************************************************************************/
uint16_t        usb_hstd_CmdSubmit( USB_UTR_t *, USB_CB_t );


#ifdef USB_HOST_COMPLIANCE_MODE
extern uint16_t    usb_ghstd_responce_counter;
#endif /* USB_HOST_COMPLIANCE_MODE */

/******************************************************************************
Renesas USB Host Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_get_device_state
Description     : Get USB device status from the specifed device address.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t devaddr          : Device Address.
Return          : uint16_t                  : Device State.
******************************************************************************/
uint16_t usb_hstd_get_device_state(USB_UTR_t *ptr, uint16_t devaddr)
{
    uint16_t        md;
    USB_HCDREG_t    *driver;

    for (md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++)
    {
        driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
        if (driver->devaddr == devaddr)
        {
            return driver->devstate;
        }
    }
    return USB_DETACHED;
}/* eof usb_hstd_get_device_state() */

/******************************************************************************
Function Name   : usb_hstd_DevDescriptor
Description     : Returns buffer header pointer to fetch device descriptor.
Argument        : none
Return          : uint8_t *                 : Device Descriptor Pointer
******************************************************************************/
uint8_t *usb_hstd_DevDescriptor(USB_UTR_t *ptr)
{
    return (uint8_t *)&usb_ghstd_DeviceDescriptor[ptr->ip];
}/* eof usb_hstd_DevDescriptor() */

/******************************************************************************
Function Name   : usb_hstd_ConDescriptor
Description     : Returns buffer header pointer that includes the configuration 
                : descriptor.
Argument        : none
Return          : uint8_t *                 : Configuration Descriptor Pointer
******************************************************************************/
uint8_t *usb_hstd_ConDescriptor(USB_UTR_t *ptr)
{
    return (uint8_t *)&usb_ghstd_ConfigurationDescriptor[ptr->ip];
}/* eof usb_hstd_ConDescriptor() */

/******************************************************************************
Function Name   : usb_hstd_HsFsResult
Description     : Return the connected USB device Speed.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return          : uint16_t       : Device Speed.
******************************************************************************/
uint16_t usb_hstd_HsFsResult(USB_UTR_t *ptr)
{
    return usb_ghstd_DeviceSpeed[ptr->ip];
}/* eof usb_hstd_HsFsResult() */

/******************************************************************************
Function Name   : usb_hstd_ChangeDeviceState
Description     : Request to change status of the connected USB device.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : USB_CB_t complete : callback function.
                : uint16_t msginfo  : Request type.
                : uint16_t member   : Rootport/pipe number.
Return          : USB_ER_t          : USB_E_OK etc.
******************************************************************************/
USB_ER_t usb_hstd_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t member)
{
    USB_ER_t    err;

    err = usb_hstd_HcdSndMbx(ptr, msginfo, member, (uint16_t*)0, complete);

    return err;
}/* eof usb_hstd_ChangeDeviceState() */

/******************************************************************************
Function Name   : usb_hstd_TransferStart
Description     : Send a request for data transfer to HCD (Host Control Driver) 
                : using the specified pipe.
Arguments       : USB_UTR_t *ptr : USB internal structure. Contains message.
Return          : USB_ER_t                  : USB_E_OK/USB_E_QOVR/USB_E_ERROR
******************************************************************************/
USB_ER_t usb_hstd_TransferStart(USB_UTR_t *ptr)
{
    USB_ER_t        err;
    uint16_t        pipenum, devsel, connect_inf;

    pipenum = ptr->keyword;
    /* Pipe Transfer Process check */
    if (usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL)
    {
        /* Check PIPE TYPE */
        if (usb_cstd_GetPipeType(ptr, pipenum) != USB_ISO)
        {
            USB_PRINTF1("### R_usb_hstd_TransferStart overlaps %d\n", pipenum);
            return USB_E_QOVR;
        }
    }

    if (pipenum == USB_PIPE0)
    {
        devsel = (uint16_t)(ptr->setup[4] << USB_DEVADDRBIT);
    }
    else
    {
        /* Get device address from pipe number */
        devsel = usb_cstd_GetDevsel(ptr, pipenum);
    }
    if ((devsel == USB_DEVICE_0) && (pipenum != USB_PIPE0))
    {
        USB_PRINTF1("### R_usb_hstd_TransferStart not configured %x\n", devsel);
        return USB_E_ERROR;
    }

    /* Get device speed from device address */
    connect_inf = usb_hstd_GetDevSpeed(ptr, devsel);
    if (connect_inf == USB_NOCONNECT)
    {
        USB_PRINTF1("### R_usb_hstd_TransferStart not connect %x\n", devsel);
        return USB_E_ERROR;
    }

    ptr->msghead    = (USB_MH_t)USB_NULL;
    ptr->msginfo    = USB_MSG_HCD_SUBMITUTR;
    /* Send message */
    err = USB_SND_MSG(USB_HCD_MBX, (USB_MSG_t*)ptr);
    if (err != USB_E_OK)
    {
        USB_PRINTF1("### R_usb_hstd_TransferStart snd_msg error (%ld)\n", err);
    }
    return err;
}/* eof usb_hstd_TransferStart() */

/******************************************************************************
Function Name   : usb_hstd_DeviceUsbReset
Description     : Send USB bus reset request to MGR task, and move USB device to 
                : default status.
Argument        : uint16_t devaddr          : Device Address
Return          : none
******************************************************************************/
void usb_hstd_DeviceUsbReset(USB_UTR_t *ptr, uint16_t devaddr)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_HCD_USBRESET, devaddr, (uint16_t)0u);
}/* eof usb_hstd_DeviceUsbReset() */

/******************************************************************************
Function Name   : usb_hstd_DeviceResume
Description     : Send request for RESUME signal output to USB device to MGR task.
Argument        : uint16_t devaddr          : Device Address
Return          : none
******************************************************************************/
void usb_hstd_DeviceResume(USB_UTR_t *ptr, uint16_t devaddr)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_HCD_RESUME, devaddr, (uint16_t)0u);
}/* eof usb_hstd_DeviceResume() */

/******************************************************************************
Function Name   : usb_hstd_HcdSndMbx
Description     : Send specified message to HCD (Host Control Driver) task.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t  msginfo : Message info..
                : uint16_t  dat     : Pipe no.
                : uint16_t  *adr    : Address.
                : USB_CB_t  callback: Callback function pointer.
Return          : USB_ER_t          : USB_E_OK etc.
******************************************************************************/
USB_ER_t usb_hstd_HcdSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t *adr, USB_CB_t callback)
{
    USB_MH_t        p_blf;
    USB_ER_t        err, err2;
    USB_HCDINFO_t   *hp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HCD_MPL, &p_blf);
    if (err == USB_E_OK)
    {
        hp = (USB_HCDINFO_t*)p_blf;
        hp->msghead     = (USB_MH_t)USB_NULL;
        hp->msginfo     = msginfo;
        hp->keyword     = dat;
        hp->tranadr     = adr;
        hp->complete    = callback;
        hp->ipp         = ptr->ipp;
        hp->ip          = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HCD_MBX, (USB_MSG_t*)p_blf);
        if (err != USB_E_OK)
        {
            USB_PRINTF1("### hHcdSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HCD_MPL, (USB_MH_t)p_blf);
            if (err2 != USB_E_OK)
            {
                USB_PRINTF1("### hHcdSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hHcdSndMbx pget_blk error (%ld)\n", err);
    }
    return err;
}/* eof usb_hstd_HcdSndMbx() */

/******************************************************************************
Function Name   : usb_hstd_MgrSndMbx
Description     : Send the message to MGR(Manager) task
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t msginfo  : Message info.
                : uint16_t dat      : Port no.
                : uint16_t res               : Result
Return          : none
******************************************************************************/
void usb_hstd_MgrSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t res)
{
    USB_MH_t        p_blf;
    USB_ER_t        err, err2;
    USB_MGRINFO_t   *mp;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_MGR_MPL, &p_blf);
    if (err == USB_E_OK)
    {
        mp = (USB_MGRINFO_t *)p_blf;
        mp->msghead = (USB_MH_t)USB_NULL;
        mp->msginfo = msginfo;
        mp->keyword = dat;
        mp->result  = res;
        mp->ipp     = ptr->ipp;
        mp->ip      = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t *)p_blf);
        if (err != USB_E_OK)
        {
            USB_PRINTF1("### hMgrSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)p_blf);
            if (err2 != USB_E_OK)
            {
                USB_PRINTF1("### hMgrSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hMgrSndMbx pget_blk error (%ld)\n", err);
    }
}/* eof usb_hstd_MgrSndMbx */

/******************************************************************************
Function Name   : usb_hstd_HcdRelMpl
Description     : Release the secured memory block.
Argument        : uint16_t n        : Error no.
Return          : none
******************************************************************************/
void usb_hstd_HcdRelMpl(USB_UTR_t *ptr, uint16_t n)
{
    USB_ER_t        err;

    /* Memory Pool Release */
    err = USB_REL_BLK(USB_HCD_MPL, (USB_MH_t)ptr);
    if (err != USB_E_OK)
    {
        USB_PRINTF1("### USB HCD rel_blk error: %d\n", n);
    }
}/* eof usb_hstd_HcdRelMpl() */

/******************************************************************************
Function Name   : usb_hstd_Suspend
Description     : Request suspend for USB device.
Argument        : uint16_t port     : Port no.
Return          : none
******************************************************************************/
void usb_hstd_Suspend(USB_UTR_t *ptr, uint16_t port)
{
    USB_HCDINFO_t*    hp;

    /* Control transfer idle stage ? */
    if (usb_ghstd_Ctsq[ptr->ip] == USB_IDLEST)
    {
        /* USB suspend process */
        usb_hstd_SuspendProcess(ptr, port);
        /* Check clock */
        usb_hstd_ChkClk(ptr, port, (uint16_t)USB_SUSPENDED);
        /* Callback */
        hp = (USB_HCDINFO_t*)ptr;
        (hp->complete)(ptr, port, ptr->msginfo);
    }
    else
    {
        /* 1ms wait */
        usb_cpu_DelayXms((uint16_t)1);
        /* Change device state request */
        usb_hstd_ChangeDeviceState(ptr, &usb_hstd_StatusResult, ptr->msginfo, port);
    }
}/* eof usb_hstd_Suspend() */

/******************************************************************************
Function Name   : usb_hstd_SetSubmitutr
Description     : Submit utr: Get the device address via the specified pipe num-
                : ber and do a USB transfer.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Also used in 
                : this function to get device address, and specifies keyword and
                : USB channel.
Return          : USB_ER_t                      : USB_DONE
******************************************************************************/
USB_ER_t usb_hstd_SetSubmitutr(USB_UTR_t *ptr)
{
    uint16_t    pipenum, devsel, connect_inf;
    uint16_t    end_flag;
    USB_UTR_t    *pp;

    pipenum = ptr->keyword;
    usb_gcstd_Pipe[ptr->ip][pipenum] = ptr;

    /* Get device address from pipe number */
    if (pipenum == USB_PIPE0)
    {
        devsel = (uint16_t)(ptr->setup[4] << USB_DEVADDRBIT);
    }
    else
    {
        /* Get device address from pipe number */
        devsel = usb_cstd_GetDevsel(ptr, pipenum);
    }
    if ((devsel == USB_DEVICE_0) && (pipenum != USB_PIPE0))
    {
        /* End of data transfer (IN/OUT) */
        usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_ERR);
        return USB_DONE;
    }

    /* Get device speed from device address */
    connect_inf = usb_hstd_GetDevSpeed(ptr, devsel);
    if (connect_inf == USB_NOCONNECT)
    {
        if (pipenum == USB_PIPE0)
        {
            /* Control Read/Write End */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
        }
        else
        {
            /* End of data transfer (IN/OUT) */
            usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_ERR);
        }
        return USB_DONE;
    }

    /* Control Transfer */
    if (pipenum == USB_PIPE0)
    {
        /* Control transfer idle stage ? */
        if (usb_ghstd_Ctsq[ptr->ip] == USB_IDLEST)
        {            
            usb_hstd_SetupStart(ptr);
        }
        /* Control Read Data */
        else if (usb_ghstd_Ctsq[ptr->ip] == USB_DATARDCNT)
        {
            pp = usb_gcstd_Pipe[ptr->ip][USB_PIPE0];
            /* Control read start */
            usb_hstd_ControlReadStart(ptr, pp->tranlen, (uint8_t*)pp->tranadr);
        }
        /* Control Write Data */
        else if (usb_ghstd_Ctsq[ptr->ip] == USB_DATAWRCNT)
        {
            pp = usb_gcstd_Pipe[ptr->ip][USB_PIPE0];
            /* Control write start */
            end_flag = usb_hstd_ControlWriteStart(ptr, pp->tranlen, (uint8_t*)pp->tranadr);
            if (end_flag == USB_FIFOERROR)
            {
                USB_PRINTF0("### FIFO access error \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            }
        }
        else
        {
            USB_PRINTF0("### Control transfer seaquence error \n");
            /* Control Read/Write End */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
        }
    }
    else
    {
        /* Data Transfer */
        usb_hstd_SetReTransfer(ptr, pipenum);
    }
    return USB_DONE;
}/* eof usb_hstd_SetSubmitutr() */

/******************************************************************************
Function Name   : usb_hstd_SetReTransfer
Description     : Start IN/OUT transfer based on the specified pipe.
Argument        : uint16_t pipe             : Pipe number
Return          : none
******************************************************************************/
void usb_hstd_SetReTransfer(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Data Transfer */
    if (usb_cstd_GetPipeDir(ptr, pipe) == USB_DIR_H_IN)
    {
        /* IN Transfer */
        usb_cstd_ReceiveStart(ptr, pipe);
    }
    else
    {
        /* OUT Transfer */
        usb_cstd_SendStart(ptr, pipe);
    }
}/* eof usb_hstd_SetReTransfer() */

/******************************************************************************
Function Name   : usb_hstd_BusIntDisable
Description     : Disable USB Bus Interrupts OVRCR, ATTCH, DTCH, and BCHG.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t  port : Port number.  //$REA - redundant!
Return          : none
******************************************************************************/
void usb_hstd_BusIntDisable(USB_UTR_t *ptr, uint16_t port)
{
    /* ATTCH interrupt disable */
    usb_hstd_AttchDisable(ptr, port);
    /* DTCH     interrupt disable */
    usb_hstd_DtchDisable(ptr, port);
    /* BCHG     interrupt disable */
    usb_hstd_BchgDisable(ptr, port);
}/* eof usb_hstd_BusIntDisable() */

/******************************************************************************
Function Name   : usb_hstd_Interrupt
Description     : Execute appropriate process depending on which USB interrupt 
                : occurred.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
Return          : none
******************************************************************************/
void usb_hstd_Interrupt(USB_UTR_t *ptr)
{
    uint16_t    intsts, end_flag;
    USB_UTR_t    *pp;

    intsts = ptr->keyword;
//    bitsts = ptr->status;

    switch( intsts)
    {

    /***** Processing PIPE0-MAX_PIPE_NO data *****/
    case USB_INT_BRDY:
        usb_hstd_BrdyPipe(ptr);
    break;
    case USB_INT_BEMP:
        usb_hstd_BempPipe(ptr);
    break;
    case USB_INT_NRDY:
        usb_hstd_NrdyPipe(ptr);
    break;

    /***** Processing Setup transaction *****/
    case USB_INT_SACK:
        switch( usb_ghstd_Ctsq[ptr->ip])
        {
        case USB_SETUPRD:
            /* Next stage to Control read data */
            /* continue */
        case USB_SETUPRDCNT:
            /* Next stage to Control read data */
            pp = usb_gcstd_Pipe[ptr->ip][USB_PIPE0];
            /* Control read start */
            usb_hstd_ControlReadStart(ptr, pp->tranlen, (uint8_t*)pp->tranadr);
        break;
        case USB_SETUPWR:
            /* Next stage to Control Write data */
            /* continue */
        case USB_SETUPWRCNT:
            /* Next stage to Control Write data */
            pp = usb_gcstd_Pipe[ptr->ip][USB_PIPE0];
            /* Control write start */
            end_flag = usb_hstd_ControlWriteStart(ptr, pp->tranlen, (uint8_t*)pp->tranadr);
            if (end_flag == USB_FIFOERROR)
            {
                USB_PRINTF0("### FIFO access error \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            }
        break;
        case USB_SETUPNDC:
            /* Next stage to Control write no data */
            usb_hstd_StatusStart(ptr);
        break;
        default:
        break;
        }
    break;
    case USB_INT_SIGN:
        USB_PRINTF0("***SIGN\n");
#ifdef USB_HOST_COMPLIANCE_MODE
        USB_COMPLIANCE_DISP(ptr, USB_COMP_ERR,USB_NO_ARG);
#endif /* USB_HOST_COMPLIANCE_MODE */
        /* Ignore count */
        usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0]++;
        USB_PRINTF2("### IGNORE Pipe %d is %d times (Setup) \n", USB_PIPE0, usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0]);
        if (usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0] == USB_PIPEERROR)
        {
            /* Setup Device Ignore count over */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
        }
        else
        {
            /* Interrupt enable */
            /* 5ms wait */
            usb_cpu_DelayXms((uint16_t)5u);
            /* Status Clear */
            usb_hreg_clr_sts_sign( ptr);
            usb_hreg_clr_sts_sack( ptr);
            /* Setup Ignore,Setup Acknowledge enable */
            usb_hreg_set_enb_signe( ptr);
            usb_hreg_set_enb_sacke( ptr);
            /* SETUP request send */
            /* Send SETUP request */
            usb_hreg_set_sureq( ptr );
        }
    break;

    /***** Processing rootport0 *****/
    case USB_INT_OVRCR0:
        /* Port0 OVCR interrupt function */
        usb_hstd_Ovrcr0Function(ptr);
    break;
    case USB_INT_EOFERR0:
        /* User program */
    break;
    case USB_INT_ATTCH0:
        /* Port0 ATCH interrupt function */
        usb_hstd_AttachProcess(ptr, (uint16_t)USB_PORT0);
    break;
    case USB_INT_BCHG0:
        USB_PRINTF0("BCHG int port0\n");
        /* Port0 BCHG interrupt function */
        usb_hstd_Bchg0Function(ptr);
    break;
    case USB_INT_DTCH0:
        USB_PRINTF0("DTCH int port0\n");
        /* USB detach process */
        usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT0);
    break;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    /***** Processing rootport1 *****/
    case USB_INT_OVRCR1:
        if ((usb_creg_read_syssts( ptr, USB_PORT0 ) & USB_OVCBIT) == 0)
        {
            USB_PRINTF0(" OVCR int port1\n");
            /* OVRCR interrupt disable */
            usb_hstd_OvrcrDisable(ptr, (uint16_t)USB_PORT1);
            /* Notification over current */
            usb_hstd_OvcrNotifiation(ptr, (uint16_t)USB_PORT1);
        }
    break;
    case USB_INT_EOFERR1:
        /* User program */
    break;
    case USB_INT_ATTCH1:
        USB_PRINTF0("ATTCH int port1\n");
        /* USB attach / detach */
        usb_hstd_AttachProcess(ptr, (uint16_t)USB_PORT1);
    break;
    case USB_INT_BCHG1:
        /* SUSPENDED check */
        if (usb_ghstd_RemortPort[USB_PORT1] == USB_SUSPENDED)
        {
            if (USB_RESUME == (usb_creg_read_dvstctr( ptr, USB_PORT1 ) & USB_RESUME ))
            {
                USB_PRINTF0("BCHG(RWUP) int port1\n");
                usb_ghstd_RemortPort[USB_PORT1] = USB_DEFAULT;
                /* Change device state to resume */
                usb_hstd_DeviceResume(ptr, (uint16_t)(USB_PORT1 + USB_DEVICEADDR));
            }
            else
            {
                /* Decide USB Line state (ATTACH) */
                if (USB_DETACH == usb_hstd_ChkAttach(ptr, (uint16_t)USB_PORT1))
                {
                    usb_ghstd_RemortPort[USB_PORT1] = USB_DEFAULT;
                    /* USB detach process */
                    usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT1);
                }
                else
                {
                    /* Enable port BCHG interrupt */
                    usb_hstd_BchgEnable(ptr, (uint16_t)USB_PORT1);
                    /* Check clock */
                    usb_hstd_ChkClk(ptr, (uint16_t)USB_PORT1, (uint16_t)USB_SUSPENDED);
                }
            }
        }
        else
        {
            /* USB detach process */
            usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT1);
        }
    break;
    case USB_INT_DTCH1:
        USB_PRINTF0("DTCH int port1\n");
        /* USB detach process */
        usb_hstd_DetachProcess(ptr, (uint16_t)USB_PORT1);
    break;
#endif    /* USB_PORTSEL_PP == USB_2PORT_PP */
#ifdef USB_HOST_BC_ENABLE
    case USB_INT_PDDETINT0:
        /* Port0 PDDETINT interrupt function */
        if(USB_BC_SUPPORT_IP == ptr->ip)
        {
            usb_hstd_pddetint_process(ptr, (uint16_t)USB_PORT0);
        }
    break;
#endif
    case USB_INT_VBINT:
        /* User program */
        usb_creg_clr_enb_vbse( ptr );
    break;
    case USB_INT_SOFR:
#ifdef USB_HOST_COMPLIANCE_MODE
        usb_ghstd_responce_counter++;
        if(usb_ghstd_responce_counter == USB_RESPONCE_COUNTER_VALUE)
        {
            usb_creg_clr_enb_sofe( ptr );
            USB_COMPLIANCE_DISP(ptr, USB_COMP_NOTRESP, USB_NO_ARG);
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_STOP);
        }
#else  /* USB_HOST_COMPLIANCE_MODE */
        /* User program */
        usb_creg_clr_enb_sofe( ptr );
#endif /* USB_HOST_COMPLIANCE_MODE */
    break;

    /*** ERROR ***/
    case USB_INT_UNKNOWN:
        USB_PRINTF0("hINT_UNKNOWN\n");
    break;
    default:
        USB_PRINTF1("hINT_default %X\n", intsts);
    break;
    }
}/* eof usb_hstd_Interrupt() */

/******************************************************************************
Function Name   : usb_hstd_ClearFeature
Description     : Send ClearFeature command to the connected USB device.
Arguments       : uint16_t addr     : Device address.
                : uint16_t epnum    : Endpoint number.
                : USB_CB_t complete : Callback function.
Return value    : uint16_t          : Error info.
******************************************************************************/
USB_ER_t usb_hstd_ClearFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete)
{
    USB_ER_t    ret_code;

    if (epnum == 0xFF)
    {
        /* ClearFeature(Device) */
        usb_shstd_ClearStallRequest[0]    = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
        usb_shstd_ClearStallRequest[1]    = USB_DEV_REMOTE_WAKEUP;
        usb_shstd_ClearStallRequest[2]    = (uint16_t)0x0000;
    }
    else
    {
        /* ClearFeature(endpoint) */
        usb_shstd_ClearStallRequest[0]    = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_ENDPOINT;
        usb_shstd_ClearStallRequest[1]    = USB_ENDPOINT_HALT;
        usb_shstd_ClearStallRequest[2]    = epnum;
    }
    usb_shstd_ClearStallRequest[3]    = (uint16_t)0x0000;
    usb_shstd_ClearStallRequest[4]    = addr;

    usb_shstd_ClearStallControl.tranadr    = (void*)usb_shstd_ClearStallData;
    usb_shstd_ClearStallControl.complete    = complete;
    usb_shstd_ClearStallControl.tranlen    = (uint32_t)usb_shstd_ClearStallRequest[3];
    usb_shstd_ClearStallControl.keyword    = USB_PIPE0;
    usb_shstd_ClearStallControl.setup    = usb_shstd_ClearStallRequest;
    usb_shstd_ClearStallControl.segment    = USB_TRAN_END;

    usb_shstd_ClearStallControl.ip    = ptr->ip;
    usb_shstd_ClearStallControl.ipp    = ptr->ipp;

    ret_code = usb_hstd_TransferStart(&usb_shstd_ClearStallControl);

    return ret_code;
}/* eof usb_hstd_ClearFeature() */

/******************************************************************************
Function Name   : usb_hstd_ClearStall
Description     : Clear Stall
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe      : Pipe number.
                : USB_CB_t complete        : Callback function
Return value    : uint16_t           : Error info.
******************************************************************************/
USB_ER_t usb_hstd_ClearStall(USB_UTR_t *ptr, uint16_t pipe, USB_CB_t complete)
{
    USB_ER_t    err;
    uint8_t     dir_ep;
    uint16_t    devsel;

    dir_ep = usb_cstd_Pipe2Epadr(ptr, pipe);
    devsel = usb_cstd_GetDeviceAddress(ptr, pipe);

    err = usb_hstd_ClearFeature(ptr, (uint16_t)(devsel >> USB_DEVADDRBIT), (uint16_t)dir_ep, complete);
    return err;
}/* eof usb_hstd_ClearStall() */

/******************************************************************************
Function Name   : usb_hstd_ClearStallResult
Description     : Callback function to notify HCD task that usb_hstd_ClearStall function is completed
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t      data1       : Not Use
                : uint16_t      data2       : Not Use
Return value    : uint16_t                 : error info
******************************************************************************/
void usb_hstd_ClearStallResult(USB_UTR_t *ptr, uint16_t data1, uint16_t data2 )
{
    USB_MH_t        p_blf;
    USB_ER_t        err, err2;
    USB_UTR_t        *up;

    /* Get mem pool blk */
    err = USB_PGET_BLK(USB_HCD_MPL, &p_blf);
    if (err == USB_E_OK)
    {
        up = (USB_UTR_t*)p_blf;
        up->msghead    = (USB_MH_t)USB_NULL;
        up->msginfo    = USB_MSG_HCD_CLR_STALL_RESULT;
        up->status    = ptr->status;

        up->ipp    = ptr->ipp;
        up->ip    = ptr->ip;

        /* Send message */
        err = USB_SND_MSG(USB_HCD_MBX, (USB_MSG_t*)p_blf);
        if (err != USB_E_OK)
        {
            USB_PRINTF1("### hHcdSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_HCD_MPL, (USB_MH_t)p_blf);
            if (err2 != USB_E_OK)
            {
                USB_PRINTF1("### hHcdSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### hHcdSndMbx pget_blk error (%ld)\n", err);
    }
}/* eof usb_hstd_ClearStallResult() */

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
Function Name   : usb_hstd_HcdTask
Description     : USB Host Control Driver Task.
Argument        : USB_VP_INT stacd  : Task Start Code.
Return          : none
******************************************************************************/
void usb_hstd_HcdTask(USB_VP_INT stacd)
{
#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)
    USB_UTR_t       *mess, *ptr;
    USB_ER_t        err;
    uint16_t        rootport, pipenum, msginfo;
    uint16_t        connect_inf;
    uint16_t        retval;
    USB_HCDINFO_t*  hp;
#ifdef FREE_RTOS_PP
    for( ;; )
    {
#endif
    /* Receive message */
    err = USB_TRCV_MSG(USB_HCD_MBX, (USB_MSG_t**)&mess, (USB_TM_t)10000);
    if ( (err != USB_E_OK) && (err != USB_E_TMOUT))
    {
#ifdef FREE_RTOS_PP
        continue;
#else
        return;
#endif
    }
    else
    {
        ptr = (USB_UTR_t *)mess;
        hp  = (USB_HCDINFO_t*)mess;

        /* Peripheral Function */
        if (usb_cstd_is_host_mode(ptr) == USB_NO)
        {
#ifdef FREE_RTOS_PP
            continue;
#else
            return;
#endif
        }

        rootport    = ptr->keyword;
        pipenum     = ptr->keyword;

        /* Branch Hcd Task receive Message Command */
        msginfo = ptr->msginfo;
        switch( msginfo)
        {
            case USB_MSG_HCD_INT:
                /* USB INT */
                usb_hstd_Interrupt(ptr);
            break;

            case USB_MSG_HCD_PCUTINT:
                /* Start Oscillation: Interrupt wakeup */
                usb_cstd_InterruptClock(ptr);
                ptr = (USB_UTR_t*)usb_shstd_HcdMsg;
                /* USB interrupt Handler */
                usb_hstd_InterruptHandler(ptr);
                /* USB INT */
                usb_hstd_Interrupt(ptr);
                ptr->msginfo = USB_MSG_HCD_INT;
            break;

            case USB_MSG_HCD_SUBMITUTR:
                /* USB Submit utr */
                usb_hstd_SetSubmitutr(ptr);
            break;

            case USB_MSG_HCD_ATTACH:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* USB attach / detach */
                usb_hstd_AttachProcess(ptr, rootport);
                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_ATTACH);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_ATTACH_MGR:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* USB attach / detach */
                usb_hstd_AttachProcess(ptr, rootport);
                connect_inf = usb_cstd_PortSpeed(ptr, rootport);
                /* Callback */
                (hp->complete)(ptr, rootport, connect_inf);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_DETACH:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* USB detach process */
                usb_hstd_DetachProcess(ptr, rootport);

                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_DETACH);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_DETACH_MGR:
                usb_creg_clr_dvstctr( ptr, USB_PORT0, (USB_RWUPE | USB_USBRST | USB_RESUME | USB_UACT));

                usb_cpu_DelayXms(1);
                /* interrupt disable */
                usb_hstd_AttchDisable(ptr, rootport);
                usb_hstd_DtchDisable(ptr, rootport);
                usb_hstd_BchgDisable(ptr, rootport);
                (hp->complete)(ptr, rootport, USB_MSG_HCD_DETACH_MGR);
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_USBRESET:
                /* USB bus reset */
                usb_hstd_BusReset(ptr, rootport);
                /* Check current port speed */
                connect_inf = usb_cstd_PortSpeed(ptr, rootport);
                /* Callback */
                (hp->complete)(ptr, rootport, connect_inf);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_REMOTE:
                /* Suspend device */
                usb_ghstd_RemortPort[rootport] = USB_SUSPENDED;
                usb_hstd_Suspend(ptr, rootport);
                /* CallBack */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_REMOTE);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_SUSPEND:
                /* Suspend device */
                usb_hstd_Suspend(ptr, rootport);
                (hp->complete)(ptr, rootport, USB_MSG_HCD_SUSPEND);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_RESUME:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* USB resume */
                usb_hstd_ResumeProcess(ptr, rootport);
                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_RESUME);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_VBON:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* Interrupt Enable */
                usb_hstd_OvrcrEnable(ptr, rootport);
                /* USB VBUS control ON */
                usb_hstd_VbusControl(ptr, rootport, (uint16_t)USB_VBON);
#ifndef USB_HOST_BC_ENABLE
                /* 100ms wait */
                usb_cpu_DelayXms((uint16_t)100u);
#endif /* ! USB_HOST_BC_ENABLE */
                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_VBON);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_VBOFF:
                /* Start Oscillation */
                usb_cstd_SelfClock(ptr);
                /* USB VBUS control OFF */
                usb_hstd_VbusControl(ptr, rootport, (uint16_t)USB_VBOFF);
                usb_hstd_OvrcrDisable(ptr, rootport);

                /* 100ms wait */
                usb_cpu_DelayXms((uint16_t)100u);
                /* Callback */
                (hp->complete)(ptr, rootport, USB_MSG_HCD_VBOFF);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_CLR_STALLBIT:
                /* STALL */
                usb_cstd_ClrStall(ptr, pipenum);
                /* Callback */
                (hp->complete)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_MSG_HCD_CLR_STALLBIT);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_SQTGLBIT:
                pipenum = ptr->keyword & USB_PIPENM;
                /* SQ toggle */
                usb_cstd_DoSqtgl(ptr, pipenum, ptr->keyword);
                /* Callback */
                (hp->complete)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_MSG_HCD_SQTGLBIT);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_CLR_STALL:
                usb_shstd_ClearStallCall = hp->complete;
                usb_shstd_ClearStallPipe = pipenum;
                err = usb_hstd_ClearStall(ptr, pipenum, (USB_CB_t)&usb_hstd_ClearStallResult);
                if( USB_E_QOVR == err )
                {
                    USB_WAI_MSG( USB_HCD_MBX, ptr, 1000 );  /* Retry */
                }
                else
                {
                    /* Release Memory Block */
                    usb_hstd_HcdRelMpl(ptr, msginfo);
                }
            break;
            case USB_MSG_HCD_CLR_STALL_RESULT:
                ptr = (USB_UTR_t*)mess;
                retval = ptr -> status;
    
                if (retval == USB_DATA_TMO)
                {
                    USB_PRINTF0("*** Standard Request Timeout error !\n");
                }
                else if (retval == USB_DATA_STALL)
                {
                    USB_PRINTF0("*** Standard Request STALL !\n");
                }
                else if (retval != USB_CTRL_END)
                {
                    USB_PRINTF0("*** Standard Request error !\n");
                }
                else
                {
                    usb_cstd_ClrStall(ptr, usb_shstd_ClearStallPipe);
                    usb_creg_set_sqclr(ptr, usb_shstd_ClearStallPipe);    /* SQCLR */
                }
                (*usb_shstd_ClearStallCall)(ptr, retval, USB_MSG_HCD_CLR_STALL);
            
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_CLRSEQBIT:
                /* SQCLR */
                usb_creg_set_sqclr(ptr, pipenum);
                /* Callback */
                (hp->complete)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_MSG_HCD_CLRSEQBIT);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_SETSEQBIT:
                /* SQSET */
                usb_creg_set_sqset(ptr, pipenum);
                /* Callback */
                (hp->complete)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_MSG_HCD_SETSEQBIT);
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_TRANSEND1:
                /* Pipe Transfer Process check */
                if (usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL) 
                {
                    /* Control Transfer stop */
                    if (pipenum == USB_PIPE0)
                    {
                        /* Control Read/Write End */
                        usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_TMO);
                    }
                    else
                    {
                        /* Transfer stop */
                        usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_TMO);
                    }
                }
                else
                {
                    USB_PRINTF1("### Host not transferd %d\n",pipenum);
                }
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

            case USB_MSG_HCD_TRANSEND2:
                /* Pipe Transfer Process check */
                if (usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL)
                {
                    /* Control Transfer stop */
                    if (pipenum == USB_PIPE0)
                    {
                        /* Control Read/Write End */
                        usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_STOP);
                    }
                    else
                    {
                        /* Transfer stop */
                        usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_STOP);
                    }
                }
                else
                {
                    USB_PRINTF1("### Host not transferd %d\n",pipenum);
                }
                /* Release Memory Block */
                usb_hstd_HcdRelMpl(ptr, msginfo);
            break;

#ifdef USB_DTC_ENABLE
            case USB_MSG_HCD_D0FIFO_INT:
                usb_cstd_D0fifoInt(ptr);
            break;
#endif /* USB_DTC_ENABLE */

            case USB_MSG_HCD_D1FIFO_INT:
            break;

            case USB_MSG_HCD_RESM_INT:
            break;

            default:
            break;
        }
    }
#ifdef FREE_RTOS_PP
	}

#endif
#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */
}/* eof usb_hstd_HcdTask() */
/******************************************************************************
End  Of File
******************************************************************************/

