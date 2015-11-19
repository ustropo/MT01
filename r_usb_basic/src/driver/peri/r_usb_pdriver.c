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
* File Name    : r_usb_pdriver.c
* Description  : USB Peripheral driver code.
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
Private global variables and functions
******************************************************************************/

uint16_t        usb_gpstd_StallPipe[USB_MAX_PIPE_NO + 1u];  /* Stall Pipe info */
USB_CB_t        usb_gpstd_StallCB;                          /* Stall Callback function */
uint16_t        usb_gpstd_ConfigNum = 0;                    /* Current configuration number */
uint16_t        usb_gpstd_AltNum[USB_ALT_NO];               /* Alternate number */
uint16_t        usb_gpstd_RemoteWakeup = USB_NO;            /* Remote wakeup enable flag */

#if ((( USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
    ||(( USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
uint16_t        usb_gpstd_TestModeSelect;                   /* Test mode selectors */
uint16_t        usb_gpstd_TestModeFlag = USB_NO;            /* Test mode flag */
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */

uint16_t        usb_gpstd_EpTblIndex[2][USB_MAX_EP_NO + 1u];   /* Index of endpoint information table */
uint16_t        usb_gpstd_ReqType;                          /* Request type */
uint16_t        usb_gpstd_ReqTypeType;                      /* Request type TYPE */
uint16_t        usb_gpstd_ReqTypeRecip;                     /* Request type RECIPIENT */
uint16_t        usb_gpstd_ReqRequest;                       /* Request */
uint16_t        usb_gpstd_ReqValue;                         /* Value */
uint16_t        usb_gpstd_ReqIndex;                         /* Index */
uint16_t        usb_gpstd_ReqLength;                        /* Length */
uint16_t        usb_gpstd_intsts0;                          /* INTSTS0 */

/* Driver registration */
USB_PCDREG_t    usb_gpstd_Driver = 
{
    (uint16_t**)&usb_cstd_DummyFunction,    /* Pipe define table address */
    (uint8_t*)  &usb_cstd_DummyFunction,    /* Device descriptor table address */
    (uint8_t*)  &usb_cstd_DummyFunction,    /* Qualifier descriptor table address */
    (uint8_t**) &usb_cstd_DummyFunction,    /* Configuration descriptor table address */
    (uint8_t**) &usb_cstd_DummyFunction,    /* Other configuration descriptor table address */
    (uint8_t**) &usb_cstd_DummyFunction,    /* String descriptor table address */
    &usb_cstd_DummyFunction,                /* Driver init */
    &usb_cstd_DummyFunction,                /* Device default */
    &usb_cstd_DummyFunction,                /* Device configured */
    &usb_cstd_DummyFunction,                /* Device detach */
    &usb_cstd_DummyFunction,                /* Device suspend */
    &usb_cstd_DummyFunction,                /* Device resume */
    &usb_cstd_DummyFunction,                /* Interfaced change */
    &usb_cstd_DummyTrn,                     /* Control transfer */
};

USB_REQUEST_t   usb_gpstd_ReqReg;           /* Device Request - Request structure */

/******************************************************************************
Static variables and functions
******************************************************************************/
static USB_PCDINFO_t    *usb_spstd_PcdMsg;  /* Pcd Task receive message */

static  USB_ER_t usb_pstd_SetSubmitutr(USB_UTR_t *ptr, USB_UTR_t *utrmsg);
static  void usb_pstd_SetReTransfer(USB_UTR_t *ptr, uint16_t pipe);


/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_PcdSndMbx
Description     : PCD Send Mailbox
Arguments       : USB_UTR_t *ptr            : USB system internal structure.
                : uint16_t msginfo          : USB system internal structure.
                : uint16_t keyword          : USB system internal structure.
                : USB_CB_t complete         : Callback function.
Return value    : USB_ER_t                  : Error Info
******************************************************************************/
USB_ER_t usb_pstd_PcdSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t keyword, USB_CB_t complete)
{
    USB_MH_t        p_blf;
    USB_ER_t        err, err2;
    USB_PCDINFO_t   *pp;

    /* Get Memory pool for send message */
    err = USB_PGET_BLK(USB_PCD_MPL, &p_blf);
    if( err == USB_E_OK )
    {
        pp = (USB_PCDINFO_t*)p_blf;
        pp->msghead     = (USB_MH_t)USB_NULL;
        pp->msginfo     = msginfo;
        pp->keyword     = keyword;
        pp->complete    = complete;

        pp->ipp         = ptr->ipp;
        pp->ip          = ptr->ip;

        /* Send message for usb_pstd_PcdTask */
        err = USB_SND_MSG(USB_PCD_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### pPcdSndMbx snd_msg error (%ld)\n", err);
            err2 = USB_REL_BLK(USB_PCD_MPL,(USB_MH_t)p_blf);
            if( err2 != USB_E_OK )
            {
                USB_PRINTF1("### pPcdSndMbx rel_blk error (%ld)\n", err2);
            }
        }
    }
    else
    {
        USB_PRINTF1("### pPcdSndMbx pget_blk error\n", err);
    }
    return err;
}
/******************************************************************************
End of function usb_pstd_PcdSndMbx
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_PcdRelMpl
Description     : PCD REL_BLK send                          $REA
Arguments       : uint16_t n                                $REA
Return value    : none
******************************************************************************/
void usb_pstd_PcdRelMpl(uint16_t n)
{
    USB_ER_t        err;

    /* PCD memory pool release */
    err = USB_REL_BLK(USB_PCD_MPL, (USB_MH_t)usb_spstd_PcdMsg);
    if( err != USB_E_OK )
    {
        USB_PRINTF2("### usb_pstd_PcdRelMpl (%d) rel_blk error: %d\n", n, err);
    }
}
/******************************************************************************
End of function usb_pstd_PcdRelMpl
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_PcdTask
Description     : The Peripheral Control Driver(PCD) task.
Arguments       : USB_VP_INT stacd
Return value    : none
******************************************************************************/
void usb_pstd_PcdTask(USB_VP_INT stacd)
{
    USB_UTR_t       *mess, *ptr;
    /* Error code */
    USB_ER_t        err;
    uint16_t        pipenum;

    err = USB_TRCV_MSG(USB_PCD_MBX, (USB_MSG_t**)&mess, (USB_TM_t)10000);
    if( (err != USB_E_OK) )
    {
        return;
    }

    ptr = (USB_UTR_t *)mess;

    usb_spstd_PcdMsg = (USB_PCDINFO_t*)mess;
    pipenum = usb_spstd_PcdMsg->keyword;

    /* Detach is all device */
    switch( usb_spstd_PcdMsg->msginfo )
    {
    case USB_MSG_PCD_INT:
        /* USB INT */
        usb_pstd_Interrupt((USB_UTR_t*)usb_spstd_PcdMsg);
        break;
    case USB_MSG_PCD_PCUTINT:
        /* Start Oscillation : Interrupt wakeup */
        usb_cstd_InterruptClock(ptr);
        ptr = (USB_UTR_t*)usb_spstd_PcdMsg;
        /* USB interrupt Handler */
        usb_pstd_InterruptHandler(ptr);
        /* USB INT */
        usb_pstd_Interrupt((USB_UTR_t*)usb_spstd_PcdMsg);
        ptr->msginfo = USB_MSG_PCD_INT;
        break;

    case USB_MSG_PCD_SUBMITUTR:
        /* USB Submit utr */
        usb_pstd_SetSubmitutr(ptr, (USB_UTR_t*)usb_spstd_PcdMsg);
        break;
    case USB_MSG_PCD_REMOTEWAKEUP:
        usb_cstd_SelfClock(ptr);
        usb_pstd_RemoteWakeup(ptr);
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_REMOTEWAKEUP);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)1u);
        break;

    case USB_MSG_PCD_CLRSEQBIT:
        usb_creg_set_sqclr(ptr, pipenum);
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_CLRSEQBIT);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)2u);
        break;
    case USB_MSG_PCD_SETSTALL:
        usb_pstd_SetStall(ptr, pipenum);
        usb_gpstd_StallPipe[pipenum] = USB_YES;
        usb_gpstd_StallCB = usb_spstd_PcdMsg->complete;
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)3u);
        break;

    case USB_MSG_PCD_TRANSEND1:
        /* End of all pipes */
        if( usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL )
        {
            /* Transfer timeout */
            usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_TMO);
        }
        else
        {
            USB_PRINTF1("### Peri not transferd-1 %d\n", pipenum);
        }
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)4u);
        break;
    case USB_MSG_PCD_TRANSEND2:
        /* End of all pipes */
        if( usb_gcstd_Pipe[ptr->ip][pipenum] != USB_NULL )
        {
            /* Transfer stop */
            usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_STOP);
        }
        else
        {
            USB_PRINTF1("### Peri not transferd-2 %d\n", pipenum);
        }
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)5u);
        break;

    case USB_MSG_PCD_DETACH:
        usb_cstd_SelfClock(ptr);
        /* USB detach */
        usb_pstd_DetachProcess(ptr);
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_DETACH);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)6u);
        break;
    case USB_MSG_PCD_ATTACH:
        usb_cstd_SelfClock(ptr);
        usb_pstd_AttachProcess(ptr);
        /* Process Done Callback function */
#ifdef USB_PERI_BC_ENABLE
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)g_usb_bc_detect, USB_MSG_PCD_ATTACH);
#else
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_ATTACH);
#endif
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)7u);
        break;

    case USB_MSG_PCD_DP_ENABLE:
        usb_pstd_DpEnable(ptr);
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_DP_ENABLE);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)8u);
        break;
    case USB_MSG_PCD_DP_DISABLE:
        usb_pstd_DpDisable(ptr);
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_DP_DISABLE);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)9u);
        break;
    case USB_MSG_PCD_DM_ENABLE:
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_DM_ENABLE);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)10u);
        break;
    case USB_MSG_PCD_DM_DISABLE:
        /* Process Done Callback function */
        (usb_spstd_PcdMsg->complete)(ptr, (uint16_t)USB_NO_ARG, USB_MSG_PCD_DM_DISABLE);
        /* PCD memory pool release */
        usb_pstd_PcdRelMpl((uint16_t)11u);
        break;

#ifdef USB_DTC_ENABLE
    case USB_MSG_PCD_D0FIFO_INT:
        usb_cstd_D0fifoInt(ptr);
        break;
#endif /* USB_DTC_ENABLE */

    case USB_MSG_PCD_D1FIFO_INT:
        break;

    case USB_MSG_PCD_RESM_INT:
        break;

    default:
        while( 1 )
        {
        };
        break;
    }
}
/******************************************************************************
End of function usb_pstd_PcdTask
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_SetSubmitutr
Description     : USB Peripheral Submit utr.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : USB_UTR_t *utrmsg             $REA
Return value    : USB_ER_t
******************************************************************************/
USB_ER_t usb_pstd_SetSubmitutr(USB_UTR_t *ptr, USB_UTR_t *utrmsg)
{
    uint16_t    pipenum;

    pipenum = utrmsg->keyword;
    usb_gcstd_Pipe[ptr->ip][pipenum] = utrmsg;

    /* Check state ( Configured ) */
    if( usb_pstd_ChkConfigured(ptr) == USB_YES )
    {
        /* Data transfer */
        usb_pstd_SetReTransfer(ptr, pipenum);
    }
    else
    {
        /* Transfer stop */
        usb_cstd_ForcedTermination(ptr, pipenum, (uint16_t)USB_DATA_ERR);
    }
    return USB_DONE;
}
/******************************************************************************
End of function usb_pstd_SetSubmitutr
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetReTransfer
Description     : Start transmission/reception of data transfer based on the 
                : specified transfer direction.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t pipe     : Pipe nr.
Return value    : none
******************************************************************************/
void usb_pstd_SetReTransfer(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Data transfer */
    if( usb_cstd_GetPipeDir(ptr, pipe) == USB_DIR_P_OUT )
    {   /* Out transfer */
        usb_cstd_ReceiveStart(ptr, pipe);
    }
    else
    {
        /* In transfer */
        usb_cstd_SendStart(ptr, pipe);
    }
}
/******************************************************************************
End of function usb_pstd_SetReTransfer
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_Interrupt
Description     : Analyze the USB Peripheral interrupt event and execute the
                : appropriate process.
Arguments       : USB_UTR_t *p    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_Interrupt(USB_UTR_t *ptr)
{
    uint16_t    intsts, status, stginfo;

    intsts = ptr->keyword;
    status = ptr->status;

    usb_gpstd_intsts0 = usb_creg_read_intsts( ptr );

    /* check interrupt status */
    switch( intsts )
    {

    /* BRDY, BEMP, NRDY */
    case USB_INT_BRDY:
        usb_pstd_BrdyPipe(ptr, status);
        break;
    case USB_INT_BEMP:
        usb_pstd_BempPipe(ptr, status);
        break;
    case USB_INT_NRDY:
        usb_pstd_NrdyPipe(ptr, status);
        break;
    /* Resume */
    case USB_INT_RESM:
        USB_PRINTF0("RESUME int peri\n");
        /* Callback */
        (*usb_gpstd_Driver.devresume)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
        usb_pstd_ResumeProcess(ptr);
        break;
    /* VBUS */
    case USB_INT_VBINT:
        usb_creg_set_cnen( ptr );
        if( usb_pstd_ChkVbsts(ptr) == USB_ATTACH )
        {
            USB_PRINTF0("VBUS int attach\n");
            /* USB attach */
            usb_pstd_AttachProcess(ptr);
        }
        else
        {
            USB_PRINTF0("VBUS int detach\n");
            /* USB detach */
            usb_pstd_DetachProcess(ptr);
        }
        break;
    /* SOF */
    case USB_INT_SOFR:
        /* User program */
        break;

    /* DVST */
    case USB_INT_DVST:
        switch( (uint16_t)(status & USB_DVSQ) )
        {
        /* Power state  */
        case USB_DS_POWR:
            break;
        /* Default state  */
        case USB_DS_DFLT:
            USB_PRINTF0("USB-reset int peri\n");
            usb_pstd_BusReset(ptr);
            break;
        /* Address state  */
        case USB_DS_ADDS:
            break;
        /* Configured state  */
        case USB_DS_CNFG:
            USB_PRINTF0("Device configuration int peri\n");
            break;
        /* Power suspend state */
        case USB_DS_SPD_POWR:
            /* Continue */
        /* Default suspend state */
        case USB_DS_SPD_DFLT:
            /* Continue */
        /* Address suspend state */
        case USB_DS_SPD_ADDR:
            /* Continue */
        /* Configured Suspend state */
        case USB_DS_SPD_CNFG:
            USB_PRINTF0("SUSPEND int peri\n");
            usb_pstd_SuspendProcess(ptr);
            break;
        /* Error */
        default:
            break;
        }
        break;

    /* CTRT */
    case USB_INT_CTRT:
        stginfo = (uint16_t)(status & USB_CTSQ);
        if( (stginfo == USB_CS_IDST) )
        {
#if ((( USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
   ||(( USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
            /* check Test mode */
            if( usb_gpstd_TestModeFlag == USB_YES )
            {
                /* Test mode */
                usb_pstd_TestMode( ptr );
            }
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
        }
        else
        {
            if( ((stginfo == USB_CS_RDDS) || (stginfo == USB_CS_WRDS)) || (stginfo == USB_CS_WRND) )
            {
                /* Save request register */
                usb_pstd_SaveRequest(ptr);
            }
        }

        if( usb_gpstd_ReqTypeType == USB_STANDARD )
        {
                /* Switch on the control transfer stage (CTSQ). */
            switch( stginfo )
            {
            /* Idle or setup stage */
            case USB_CS_IDST:
                usb_pstd_StandReq0(ptr);
                break;
            /* Control read data stage */
            case USB_CS_RDDS:
                usb_pstd_StandReq1(ptr);
                break;
            /* Control write data stage */
            case USB_CS_WRDS:
                usb_pstd_StandReq2(ptr);
                break;
                    /* Status stage of a control write where there is no data stage. */
            case USB_CS_WRND:
                usb_pstd_StandReq3(ptr);
                break;
            /* Control read status stage */
            case USB_CS_RDSS:
                usb_pstd_StandReq4(ptr);
                break;
            /* Control write status stage */
            case USB_CS_WRSS:
                usb_pstd_StandReq5(ptr);
                break;
            /* Control sequence error */
            case USB_CS_SQER:
                usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                break;
            /* Illegal */
            default:
                usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                break;
            }
        }
        else
        {
            /* Vender Specific */
            usb_gpstd_ReqReg.ReqType        = usb_gpstd_ReqType;
            usb_gpstd_ReqReg.ReqTypeType    = usb_gpstd_ReqTypeType;
            usb_gpstd_ReqReg.ReqTypeRecip   = usb_gpstd_ReqTypeRecip;
            usb_gpstd_ReqReg.ReqRequest     = usb_gpstd_ReqRequest;
            usb_gpstd_ReqReg.ReqValue       = usb_gpstd_ReqValue;
            usb_gpstd_ReqReg.ReqIndex       = usb_gpstd_ReqIndex;
            usb_gpstd_ReqReg.ReqLength      = usb_gpstd_ReqLength;
            /* Callback */
            (*usb_gpstd_Driver.ctrltrans)(ptr, (USB_REQUEST_t *)&usb_gpstd_ReqReg, stginfo);
        }
        break;

    /* Error */
    case USB_INT_UNKNOWN:
        USB_PRINTF0("pINT_UNKNOWN\n");
        break;
    default:
        USB_PRINTF1("pINT_default %X\n", intsts);
        break;
    }
}
/******************************************************************************
End of function usb_pstd_Interrupt
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ClearAlt
Description     : Zero-clear the alternate table (buffer).
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_ClearAlt(void)
{
    uint16_t    i;

    for( i = 0; i < USB_ALT_NO; ++i )
    {
        /* Alternate table clear */
        usb_gpstd_AltNum[i] = 0;
    }
}
/******************************************************************************
End of function usb_pstd_ClearAlt
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClearMem
Description     : Initialize global variables defined for peripheral mode.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_ClearMem(void)
{
    /* Configuration number */
    usb_gpstd_ConfigNum = 0;
    /* Remote wakeup enable flag */
    usb_gpstd_RemoteWakeup = USB_NO;
    usb_gcstd_XckeMode = USB_NO;
    /* Alternate setting clear */
    usb_pstd_ClearAlt();
}
/******************************************************************************
End of function usb_pstd_ClearMem
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_SetConfigNum
Description     : Set specified configuration number.
Arguments       : uint16_t value    : Configuration number
Return value    : none
******************************************************************************/
void usb_pstd_SetConfigNum(uint16_t value)
{
    /* Set configuration number */
    usb_gpstd_ConfigNum = value;
    /* Alternate setting clear */
    usb_pstd_ClearAlt();
}
/******************************************************************************
End of function usb_pstd_SetConfigNum
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ClearEpTblIndex
Description     : Clear Endpoint Index Table (buffer).
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_ClearEpTblIndex(void)
{
    uint16_t    i;

    for( i = 0; i <= USB_MAX_EP_NO; ++i )
    {
        /* Endpoint index table clear */
        usb_gpstd_EpTblIndex[0][i] = USB_ERROR;
        usb_gpstd_EpTblIndex[1][i] = USB_ERROR;
    }
}
/******************************************************************************
End of function usb_pstd_ClearEpTblIndex
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_GetConfigNum
Description     : Get number of possible configurations.
Arguments       : none
Return value    : uint16_t : Number of possible configurations.
                : (bNumConfigurations)
******************************************************************************/
uint16_t usb_pstd_GetConfigNum(void)
{
    /* Configuration Number */
    return (uint16_t)(usb_gpstd_Driver.devicetbl[USB_DEV_NUM_CONFIG]);
}
/******************************************************************************
End of function usb_pstd_GetConfigNum
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_get_confignum_to_tblindex
Description     : Get Configuration descriptor table index 
Arguments       : uint16_t con_num : Configuration Number
Return value    : uint16_t : Configuration descriptor table index 
******************************************************************************/
uint16_t usb_pstd_get_confignum_to_tblindex(uint16_t con_num)
{
    uint16_t    conf;
    uint16_t    i;
    uint16_t    tbl_index = 0;

    conf = con_num;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Configuration Descriptor search loop */
    for( i = 0; i < con_num; i++ )
    {
        /* Check Configuration Number.   5:bConfigurationValue */
        if( *(uint8_t*)(usb_gpstd_Driver.configtbl[i] + USB_DEV_B_CONFIGURATION_VALUE) == con_num )
        {
            /* Set Configuration tabile index */
            tbl_index = i;
            break;
        }
    }

    return tbl_index;
}
/******************************************************************************
End of function usb_pstd_get_confignum_to_tblindex
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_GetInterfaceNum
Description     : Get interface number 
Arguments       : uint16_t con_num : Configuration Number
Return value    : uint16_t : Number of this interface 
                : (bNumInterfaces)
******************************************************************************/
uint16_t usb_pstd_GetInterfaceNum(uint16_t con_num)
{
    uint16_t    conf;
    uint16_t    num_if = 0;
    uint16_t    tbl_index;

    conf = con_num;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Get Configuration descriptor table index */
    tbl_index = usb_pstd_get_confignum_to_tblindex( conf );

    /* Get NumInterfaces.        4:bNumInterfaces */
    num_if = *(uint8_t*)(usb_gpstd_Driver.configtbl[tbl_index] + USB_DEV_B_NUM_INTERFACES);

    return num_if;
}
/******************************************************************************
End of function usb_pstd_GetInterfaceNum
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_GetAlternateNum
Description     : Get Alternate Setting Number
Arguments       : uint16_t con_num : Configuration Number
                : uint16_t int_num : Interface Number
Return value    : uint16_t : Value used to select this alternate
                : (bAlternateSetting)
******************************************************************************/
uint16_t usb_pstd_GetAlternateNum(uint16_t con_num, uint16_t int_num)
{
    uint16_t    i, conf;
    uint16_t    alt_num = 0;
    uint8_t     *ptr;
    uint16_t    length;
    uint16_t    tbl_index;

    conf = con_num;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Get Configuration descriptor table index */
    tbl_index = usb_pstd_get_confignum_to_tblindex( conf );

    ptr = usb_gpstd_Driver.configtbl[tbl_index];
    i = ptr[0];
    /* Interface descriptor[0] */
    ptr = (uint8_t*)((uint32_t)ptr + ptr[0]);
    length = (uint16_t)(*(uint8_t*)((uint32_t)  usb_gpstd_Driver.configtbl[tbl_index] + (uint16_t)2u));
    length |= (uint16_t)((uint16_t)(*(uint8_t*)((uint32_t)usb_gpstd_Driver.configtbl[tbl_index] + (uint16_t)3u)) << 8u);
    
    /* Search descriptor table size */
    for(  ; i < length;  )
    {
        /* Descriptor type ? */
        switch( ptr[1] )
        {
        /* Interface */
        case USB_DT_INTERFACE:
            if( int_num == ptr[2] )
            {
                /* Alternate number count */
                alt_num = (uint16_t)ptr[3];
            }
            i += ptr[0];
            /* Interface descriptor[0] */
            ptr =(uint8_t*)((uint32_t)ptr + ptr[0]);
            break;
        /* Device */
        case USB_DT_DEVICE:
            /* Continue */
        /* Configuration */
        case USB_DT_CONFIGURATION:
            /* Continue */
        /* String */
        case USB_DT_STRING:
            /* Continue */
        /* Endpoint */
        case USB_DT_ENDPOINT:
            /* Continue */
        /* Class, Vendor, else */
        default:
            i += ptr[0];
            /* Interface descriptor[0] */
            ptr =(uint8_t*)((uint32_t)ptr + ptr[0]);
            break;
        }
    }
    return alt_num;
}
/******************************************************************************
End of function usb_pstd_GetAlternateNum
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_SetEpTblIndex
Description     : Set endpoint index in table (buffer) region based on config-
                : uration descriptor. In other words, set which endpoints to 
                : use based on specified configuration, 
Arguments       : uint16_t con_num : Configuration Number.
                : uint16_t int_num : Interface Number.
                : uint16_t alt_num : Alternate Setting.
Return value    : none
******************************************************************************/
void usb_pstd_SetEpTblIndex(uint16_t con_num, uint16_t int_num, uint16_t alt_num)
{
    uint8_t         *ptr;
    uint16_t        i, j, length, conf;
    uint16_t        start, numbers, ep;
    uint16_t    tbl_index;
    uint16_t    dir;

    conf = con_num;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Get Configuration descriptor table index */
    tbl_index = usb_pstd_get_confignum_to_tblindex( conf );

    /* Configuration descriptor */
    ptr = usb_gpstd_Driver.configtbl[tbl_index];
    i = *ptr;
    length = (uint16_t)(*(uint8_t*)((uint32_t)ptr + (uint32_t)3u));
    length = (uint16_t)(length << 8);
    length += (uint16_t)(*(uint8_t*)((uint32_t)ptr + (uint32_t)2u));
    ptr =(uint8_t*)((uint32_t)ptr + *ptr);
    start = 0;
    numbers = 0;
    j = 0;

    for(  ; i < length;  )
    {
        /* Descriptor type ? */
        switch(*(uint8_t*)((uint32_t)ptr + (uint32_t)1u) )
        {
        /* Interface */
        case USB_DT_INTERFACE:
            if((*(uint8_t*)((uint32_t)ptr + (uint32_t)2u) == int_num)
                && (*(uint8_t*)((uint32_t)ptr + (uint32_t)3u) == alt_num))
            {
                numbers = *(uint8_t*)((uint32_t)ptr + (uint32_t)4u);
            }
            else
            {
                start += *(uint8_t*)((uint32_t)ptr + (uint32_t)4u);
            }
            i += *ptr;
            ptr =(uint8_t*)((uint32_t)ptr + *ptr);
            break;
        /* Endpoint */
        case USB_DT_ENDPOINT:
            if( j < numbers )
            {
                ep = (uint16_t)*(uint8_t*)((uint32_t)ptr + (uint32_t)2u);
                if( USB_EP_IN == (ep & USB_EP_DIRMASK) )
                {
                    dir = 1;    /* IN */
                }
                else
                {
                    dir = 0;    /* OUT */
                }
                ep &= (uint16_t)0x0f;
                usb_gpstd_EpTblIndex[dir][ep] = (uint8_t)(start + j);
                ++j;
            }
            i += *ptr;
            ptr = (uint8_t*)((uint32_t)ptr + *ptr);
            break;
        /* Device */
        case USB_DT_DEVICE:
            /* Continue */
        /* Configuration */
        case USB_DT_CONFIGURATION:
            /* Continue */
        /* String */
        case USB_DT_STRING:
            /* Continue */
        /* Class, Vendor, else */
        default:
            i += *ptr;
            ptr = (uint8_t*)((uint32_t)ptr + *ptr);
            break;
        }
    }
}
/******************************************************************************
End of function usb_pstd_SetEpTblIndex
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ChkRemote
Description     : Check if the RemoteWakeUp bit for the configuration descrip-
                : tor is set.
Arguments       : none
Return value    : uint16_t : remote wakeup status (YES/NO).
******************************************************************************/
uint16_t usb_pstd_ChkRemote(void)
{
    uint8_t atr;
    uint16_t    tbl_index;

    if( usb_gpstd_ConfigNum == 0 )
    {
        return USB_NO;
    }

    /* Get Configuration descriptor table index */
    tbl_index = usb_pstd_get_confignum_to_tblindex( usb_gpstd_ConfigNum );

    /* Get Configuration Descriptor - bmAttributes */
    atr = *(uint8_t*)((uint32_t)usb_gpstd_Driver.configtbl[tbl_index] + (uint32_t)7u);
    /* Remote Wakeup check(= D5) */
    if( (atr & USB_CF_RWUPON) == USB_CF_RWUPON )
    {
        return USB_YES;
    }
    return USB_NO;
}
/******************************************************************************
End of function usb_pstd_ChkRemote
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_GetCurrentPower
Description     : Find out how the peripheral is powered by looking at the con-
                : figuration descriptor.
Arguments       : none
Return value    : uint8_t : Current power means; self-powered or bus-powered
                : (GS_SELFPOWERD/GS_BUSPOWERD).
******************************************************************************/
uint8_t usb_pstd_GetCurrentPower(void)
{
    /*
     * Please answer the currently power of your system.
     */

    uint8_t tmp, currentpower, conf;
    uint16_t    tbl_index;

    conf = (uint8_t)usb_gpstd_ConfigNum;
    if( conf < (uint8_t)1 )
    {
        /* Address state */
        conf = (uint8_t)1;
    }

    /* Get Configuration descriptor table index */
    tbl_index = usb_pstd_get_confignum_to_tblindex( conf );

    /* Standard configuration descriptor */
    tmp = *(uint8_t*)((uint32_t)usb_gpstd_Driver.configtbl[tbl_index] + (uint32_t)7u);
    if( (tmp & USB_CF_SELFP) == USB_CF_SELFP )
    {
        /* Self Powered */
        currentpower = USB_GS_SELFPOWERD;
    }
    else
    {
        /* Bus Powered */
        currentpower = USB_GS_BUSPOWERD;
    }

    /* Check currently powered */

    return currentpower;
}
/******************************************************************************
End of function usb_pstd_GetCurrentPower
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SetPipeRegister
Description     : Configure specified pipe.
Arguments       : uint16_t pipe_number  : Pipe number.
                : uint16_t *tbl         : DEF_EP table pointer.
Return value    : none
******************************************************************************/
void usb_pstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t pipe_number, uint16_t *tbl)
{
    uint16_t        i, pipe, ep;
#ifdef USB_DTC_ENABLE
    uint16_t        buf;
#endif    /* USB_DTC_ENABLE */
    uint16_t    dir;

    switch( pipe_number )
    {
    /* All pipe initialized */
    case USB_USEPIPE:
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE,  USB_NO);
#ifdef USB_DTC_ENABLE
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
#endif    /* USB_DTC_ENABLE */
        for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
        {
            /* Pipe number */
            pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
            usb_cstd_pipe_init(ptr, pipe, tbl, i);
        }
        break;
    /* Peripheral pipe initialized */
    case USB_PERIPIPE:
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE,  USB_NO);
#ifdef USB_DTC_ENABLE
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
#endif    /* USB_DTC_ENABLE */
        for( ep = USB_EP1; ep <= USB_MAX_EP_NO; ++ep )
        {
            for( dir = 0; dir <2; dir++ )
            {
                if( usb_gpstd_EpTblIndex[dir][ep] != USB_ERROR )
                {
                    i = (uint16_t)(USB_EPL * usb_gpstd_EpTblIndex[dir][ep]);
                    /* Pipe number */
                    pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
                    usb_cstd_pipe_init(ptr, pipe, tbl, i);
                }
            }
        }
        break;
    /* Clear Peripheral pipe register */
    case USB_CLRPIPE:
        /* Current FIFO port Clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE,  USB_NO);
#ifdef USB_DTC_ENABLE
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
#endif    /* USB_DTC_ENABLE */
        for( ep = USB_EP1; ep <= USB_MAX_EP_NO; ++ep )
        {
            for( dir = 0; dir <2; dir++ )
            {
                if( usb_gpstd_EpTblIndex[dir][ep] != USB_ERROR )
                {
                    i = (uint16_t)(USB_EPL * usb_gpstd_EpTblIndex[dir][ep]);
                    /* Pipe number */
                    pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
                    usb_cstd_ClrPipeCnfg(ptr, pipe);
                }
            }
        }
        break;
    /* Pipe initialized */
    default:
        /* Current FIFO port clear */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
#ifdef USB_DTC_ENABLE
        /* D0FIFO */
        buf = usb_creg_read_fifosel( ptr, USB_D0DMA );
        if( (buf & USB_CURPIPE) == pipe_number )
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        }
        /* D1FIFO */
        buf = usb_creg_read_fifosel( ptr, USB_D1DMA );
        if( (buf & USB_CURPIPE) == pipe_number )
        {
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
        }
#endif    /* USB_DTC_ENABLE */
        for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
        {
            /* Pipe number */
            pipe = (uint16_t)(tbl[i + 0] & USB_CURPIPE);
            if( pipe == pipe_number )
            {
                usb_cstd_pipe_init(ptr, pipe, tbl, i);
            }
        }
        break;
    }
}
/******************************************************************************
End of function usb_pstd_SetPipeRegister
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
