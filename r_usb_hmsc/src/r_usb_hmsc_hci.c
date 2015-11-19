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
* File Name    : r_usb_hmsc_hci.c
* Description  : USB Host MSC BOT driver
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
#include "r_usb_hatapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"   /* Host Mass Storage Class Driver define */
#include "r_usb_hmsc_extern.h"   /* Host MSC grobal define */
#include "r_usb_api.h"
#include "r_usb_hmsc_api.h"
#include "r_usb_hmsc_config.h"

/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SendCbw
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbw(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_CBW_ERR;
    }

    /* Set CBW TAG */
    usb_hmsc_CbwTagCount(ptr, msgnum);

    /* Request CBW */
    /* Device number */
    usb_ghmsc_TransData[ptr->ip][msgnum].keyword     = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[ptr->ip][msgnum].tranadr     = (void*)&usb_ghmsc_Cbw[ptr->ip][msgnum];
    /* Transfer data length */
    usb_ghmsc_TransData[ptr->ip][msgnum].tranlen     = (uint32_t)USB_MSC_CBWLENGTH;
    /* Not control transfer */
    usb_ghmsc_TransData[ptr->ip][msgnum].setup           = 0;
    usb_ghmsc_TransData[ptr->ip][msgnum].segment     = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_TransData[ptr->ip][msgnum].complete    = (USB_CB_t)&usb_cstd_DummyFunction;

    err = usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[ptr->ip][msgnum]);

    return (err);
}   /* eof usb_hmsc_SendCbw() */

/******************************************************************************
Function Name   : usb_hmsc_SendCbwReq
Description     : Send CBW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwReq(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_SUBMIT_ERR;
    }
    /* Call Back Function Info */
    usb_ghmsc_TransData[ptr->ip][msgnum].complete    = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[ptr->ip][msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}   /* eof usb_hmsc_SendCbwReq() */

/******************************************************************************
Function Name   : usb_hmsc_SendCbwCheck
Description     : Check send CBW 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendCbwCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    USB_CLSINFO_t mess;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_CBW_ERR;
    }
    /* NonOS */
    switch( hmsc_retval ) 
    {
    case USB_DATA_NONE:     /* Send CBW */
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_OutPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_OutPipe[ptr->ip][msgnum][1] = usb_ghmsc_TransData[ptr->ip][msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:    /* Stall */
        USB_PRINTF1("*** CBW Transfer STALL(drive:%d) !\n", drvnum);
        usb_shmsc_Process[ptr->ip] = USB_MSG_HMSC_CBW_ERR;
        usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_0;
        mess.keyword = drvnum;

        mess.ip = ptr->ip;
        mess.ipp = ptr->ipp;
        mess.msginfo = usb_shmsc_Process[ptr->ip];
        usb_hmsc_SpecifiedPath(&mess);
        return USB_DATA_STALL;
        break;
    case USB_DATA_TMO:      /* Timeout */
        USB_PRINTF1("### CBW Transfer timeout ERROR(drive:%d) !\n", drvnum);
        pipeno  = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_OutPipe[ptr->ip][msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### CBW Transfer ERROR(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### CBW Transfer error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_CBW_ERR;
}   /* eof usb_hmsc_SendCbwCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetData
Description     : Receive Data request
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t *buff             : Receive Data Buffer Address
                : uint32_t size             : Receive Data Size
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;
    USB_ER_t    err;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        USB_PRINTF1("### usb_hmsc_GetData [usb_hmsc_SmpDrive2Msgnum]error(drvnum:%d) !\n", drvnum);
        return USB_HMSC_DAT_RD_ERR;
    }
    /* Device number */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].keyword   = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].tranadr   = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].tranlen   = size;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].setup     = 0;
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].segment   = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].complete  = (USB_CB_t)&usb_cstd_DummyFunction;

    err = usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[ptr->ip][msgnum]);
    return (err);
}   /* eof usb_hmsc_GetData() */

/******************************************************************************
Function Name   : usb_hmsc_GetDataReq
Description     : Get Data request
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t *buff             : Not use
                : uint32_t size             : Not use
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum  = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        USB_PRINTF1("### usb_hmsc_GetDataReq [usb_hmsc_SmpDrive2Msgnum]error(drvnum:%d) !\n", drvnum);
        return USB_HMSC_DAT_RD_ERR;
    }

    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].complete  = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[ptr->ip][msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}   /* eof usb_hmsc_GetDataReq() */

/******************************************************************************
Function Name   : usb_hmsc_GetDataCheck
Description     : Check Get Data 
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t pipeno, msgnum;
    
    msgnum  = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        USB_PRINTF1("### usb_hmsc_GetDataCheck 1 [usb_hmsc_SmpDrive2Msgnum]error(drvnum:%d) !\n", drvnum);
        return USB_HMSC_DAT_RD_ERR;
    }

    /* NonOS */
    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_InPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_InPipe[ptr->ip][msgnum][1] = usb_ghmsc_ReceiveData[ptr->ip][msgnum].pipectr;
        return  USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        USB_PRINTF1("*** GetData Read STALL(drive:%d) !\n", drvnum);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_OK, msgnum, (USB_CB_t)usb_hmsc_ClearStallCheck2);
        return  USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        USB_PRINTF1("### hmsc_Data Read timeout ERROR(drive:%d) !\n", drvnum);
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_InPipe[ptr->ip][msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### hmsc_Data Read ERROR(drive:%d) !\n", drvnum);
        break;
    case USB_DATA_OVR:
        USB_PRINTF1("### hmsc_Data receive over(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### hmsc_Data Read error(drive:%d) !\n", drvnum);
        break;
    }
    USB_PRINTF1("### usb_hmsc_GetDataCheck 2 [usb_hmsc_SmpDrive2Msgnum]error(drvnum:%d) !\n", drvnum);
    return USB_HMSC_DAT_RD_ERR;
}   /* eof usb_hmsc_GetDataCheck() */

/******************************************************************************
Function Name   : usb_hmsc_SendData
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t  *buff            : Data Info Address
                : uint32_t size             : Data Size
Return value    : uint16_t                  : Error Code(USB_DONE)
******************************************************************************/
uint16_t usb_hmsc_SendData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    uint16_t    msgnum;
    USB_ER_t    err;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_DAT_WR_ERR;
    }
    /* Device number */
    usb_ghmsc_TransData[ptr->ip][msgnum].keyword     = msgnum;
    /* Transfer data address */
    usb_ghmsc_TransData[ptr->ip][msgnum].tranadr     = (void*)buff;
    /* Transfer data length */
    usb_ghmsc_TransData[ptr->ip][msgnum].tranlen     = size;
    /* Not control transfer */
    usb_ghmsc_TransData[ptr->ip][msgnum].setup       = 0;
    usb_ghmsc_TransData[ptr->ip][msgnum].segment     = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_TransData[ptr->ip][msgnum].complete
        = (USB_CB_t)&usb_cstd_DummyFunction;

    err = usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_NONE
        , &usb_ghmsc_TransData[ptr->ip][msgnum]);
    return err;
}   /* eof usb_hmsc_SendData() */

/******************************************************************************
Function Name   : usb_hmsc_SendDataReq
Description     : Send Pipe Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint8_t  *buff            : Data Info Address
                : uint32_t size             : Data Size
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_DAT_WR_ERR;
    }
    /* Call Back Function Info */
    usb_ghmsc_TransData[ptr->ip][msgnum].complete    = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_NONE, &usb_ghmsc_TransData[ptr->ip][msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}   /* eof usb_hmsc_SendDataReq() */

/******************************************************************************
Function Name   : usb_hmsc_SendDataCheck
Description     : Check Send Data
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_SendDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t    pipeno, msgnum;
    
    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_DAT_WR_ERR;
    }

    switch( hmsc_retval ) 
    {
    case USB_DATA_NONE:
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_OutPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_OutPipe[ptr->ip][msgnum][1] = usb_ghmsc_TransData[ptr->ip][msgnum].pipectr;
        return USB_HMSC_OK;
        break;
    case USB_DATA_STALL:
        USB_PRINTF1("*** hmsc_Data Write STALL(drive:%d) !\n", drvnum);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_NONE, msgnum, (USB_CB_t)usb_hmsc_ClearStallCheck2);
        return USB_HMSC_STALL;
        break;
    case USB_DATA_TMO:
        USB_PRINTF1("### hmsc_Data Write timeout ERROR(drive:%d) !\n", drvnum);
        pipeno  = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_OutPipe[ptr->ip][msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### hmsc_Data Write ERROR(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### hmsc_Data Write error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_DAT_WR_ERR;
}   /* eof usb_hmsc_SendDataCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetCsw
Description     : Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCsw(USB_UTR_t *ptr, uint16_t drvnum)
{
    uint16_t    msgnum;
    USB_ER_t    err;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_CSW_ERR;
    }

    /* Request */
    /* Device number */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].keyword   = msgnum;
    /* Transfer data address */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].tranadr   = (void*)&usb_ghmsc_Csw[ptr->ip][msgnum];
    /* Transfer data length */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Not control transfer */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].setup     = 0;
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].segment   = USB_TRAN_END;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].complete  = (USB_CB_t)&usb_cstd_DummyFunction;

    err = usb_hmsc_Submitutr(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[ptr->ip][msgnum]);
    return err;
}   /* eof usb_hmsc_GetCsw() */

/******************************************************************************
Function Name   : usb_hmsc_GetCswReq
Description     : Request Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswReq(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_ER_t    err;
    uint16_t    msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_CSW_ERR;
    }

    /* Transfer data length */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].tranlen   = (uint32_t)USB_MSC_CSW_LENGTH;
    /* Call Back Function Info */
    usb_ghmsc_ReceiveData[ptr->ip][msgnum].complete  = &usb_hmsc_CheckResult;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_DATA_OK, &usb_ghmsc_ReceiveData[ptr->ip][msgnum]);
    if( err != USB_E_OK ) 
    {
        USB_PRINTF1("### Mass Storage Device Class submit error(drive:%d) !\n", drvnum);
        return USB_HMSC_SUBMIT_ERR;
    }
    return (err);
}   /* eof usb_hmsc_GetCswReq() */

/******************************************************************************
Function Name   : usb_hmsc_GetCswCheck
Description     : Check Receive CSW
Arguments       : uint16_t drvnum           : Drive Number
                : uint16_t hmsc_retval      : Return Value
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetCswCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval)
{
    uint16_t        pipeno, msgnum;

    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return USB_HMSC_CSW_ERR;
    }

    switch( hmsc_retval ) 
    {
    case USB_DATA_SHT:
        /* Continue */
    case USB_DATA_OK:
        /* CSW Check */
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_InPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_InPipe[ptr->ip][msgnum][1] = usb_ghmsc_ReceiveData[ptr->ip][msgnum].pipectr;
        return usb_hmsc_CheckCsw(ptr, drvnum);
        break;
    case USB_DATA_STALL:
        /* Stall */
        USB_PRINTF1("*** GetCSW Transfer STALL(drive:%d) !\n", drvnum);
        return USB_MSG_HMSC_DATA_STALL;
        break;
    case USB_DATA_TMO:
        /* Timeout */
        USB_PRINTF1("### usb_hmscCSW Transfer timeout ERROR(drive:%d) !\n", drvnum);
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_InPipe[ptr->ip][msgnum][0]);
        R_usb_hstd_TransferEnd(ptr, pipeno, (uint16_t)USB_DATA_TMO);
        break;
    case USB_DATA_ERR:
        USB_PRINTF1("### usb_hmscCSW Transfer ERROR(drive:%d) !\n"
            , drvnum);
        break;
    case USB_DATA_OVR:
        USB_PRINTF1("### usb_hmscCSW receive over(drive:%d) !\n", drvnum);
        break;
    default:
        USB_PRINTF1("### usb_hmscCSW Transfer error(drive:%d) !\n", drvnum);
        break;
    }
    return USB_HMSC_CSW_ERR;
}   /* eof usb_hmsc_GetCswCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringInfoCheck
Description     : Check Get string descriptor infomation
Arguments       : uint16_t devaddr          : Device Address
Return value    : uint16_t                  : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetStringInfoCheck(USB_UTR_t *ptr, uint16_t devaddr)
{
/* Condition compilation by the difference of useful function */
 #ifdef USB_DEBUGPRINT_PP
    uint32_t    j;
    uint8_t     pdata[32];
 #endif /* USB_DEBUGPRINT_PP */

    if( usb_ghmsc_ClassData[ptr->ip][0] < (uint8_t)(30 * 2 + 2) ) 
    {
        USB_PRINTF0("*** Serial Number short\n");
        usb_ghmsc_ClassData[ptr->ip][0]  = (uint8_t)(usb_ghmsc_ClassData[ptr->ip][0] / 2);
        usb_ghmsc_ClassData[ptr->ip][0]  = (uint8_t)(usb_ghmsc_ClassData[ptr->ip][0] - 1);
    } 
    else 
    {
        usb_ghmsc_ClassData[ptr->ip][0]  = 30;
    }
/* Condition compilation by the difference of useful function */
 #ifdef USB_DEBUGPRINT_PP
    for( j = (uint16_t)0; j < usb_ghmsc_ClassData[ptr->ip][0]; j++ ) 
    {
        pdata[j]    = usb_ghmsc_ClassData[ptr->ip][j * (uint16_t)2 + (uint16_t)2];
    }
    pdata[usb_ghmsc_ClassData[ptr->ip][0]] = 0;
    USB_PRINTF1("    Serial Number : %s\n", pdata);
    if( usb_ghmsc_ClassData[ptr->ip][0] < (uint8_t)(12 * 2 + 2) ) 
    {
    }
 #endif /* USB_DEBUGPRINT_PP */
    return USB_DONE;
}   /* eof usb_hmsc_GetStringInfoCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetMaxUnitCheck
Description     : Check Get Max LUN request
Argument        : uint16_t err              : Error Code
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_GetMaxUnitCheck(USB_UTR_t *ptr, uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        USB_PRINTF0("*** GetMaxUnit not support !\n");
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        USB_PRINTF0("*** GetMaxUnit not support(time out) !\n");
        usb_hmsc_ControlEnd(ptr, (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        USB_PRINTF1(
            "### [GetMaxUnit] control transfer error(%d) !\n", err);
        return USB_ERROR;
    } 
    else 
    {
    }
    return (usb_ghmsc_Data[ptr->ip][0]);
}   /* eof usb_hmsc_GetMaxUnitCheck() */

/******************************************************************************
Function Name   : usb_hmsc_MassStorageResetCheck
Description     : Check Mass Strage Reset request
Argument        : uint16_t err              : Error Code
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
uint16_t usb_hmsc_MassStorageResetCheck(USB_UTR_t *ptr, uint16_t err)
{
    if( err == (uint16_t)USB_DATA_STALL ) 
    {
        USB_PRINTF0("*** MassStorageReset not support !\n");
        return USB_ERROR;
    } 
    else if( err == (uint16_t)USB_DATA_TMO ) 
    {
        USB_PRINTF0("*** MassStorageReset not support(time out) !\n");
        usb_hmsc_ControlEnd(ptr, (uint16_t)USB_DATA_TMO);
        return USB_ERROR;
    } 
    else if( err != (uint16_t)USB_CTRL_END ) 
    {
        USB_PRINTF1("### [MassStorageReset] control transfer error(%d) !\n", err);
        return USB_ERROR;
    } 
    else 
    {
    }
    return USB_DONE;
}   /* eof usb_hmsc_MassStorageResetCheck() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStall
Description     : Clear Stall
Arguments       : uint16_t Pipe : 
                : USB_CB_t complete : 
Return value    : uint16_t
******************************************************************************/
USB_ER_t usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t Pipe, USB_CB_t complete)
{
    USB_ER_t    err;

    err = R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_t)complete, USB_DO_CLR_STALL, Pipe);
    return err;
}   /* eof usb_hmsc_ClearStall() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck
Description     : Clear Stall Check
Arguments       : uint16_t errcheck : 
Return value    : uint16_t
******************************************************************************/
void usb_hmsc_ClearStallCheck(USB_UTR_t *ptr, uint16_t errcheck)
{
    uint16_t retval;

    retval = usb_hmsc_StdReqCheck(errcheck);
    if( retval == USB_DONE )
    {
        /* Clear STALL */
        usb_cstd_ClrStall(ptr, (uint16_t)USB_PIPE0 );

        /* SQCLR */
        R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_t)&usb_cstd_DummyFunction, USB_DO_CLR_SQTGL, (uint16_t)USB_PIPE0 );
    }
}   /* eof usb_hmsc_ClearStallCheck() */

/******************************************************************************
Function Name   : usb_hmsc_ClearStallCheck2
Description     : Clear Stall Check 2
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClearStallCheck2(USB_UTR_t *mess)
{
    uint16_t errcheck;

    errcheck = mess->status;
    usb_hmsc_ClearStallCheck(mess, errcheck);

    mess->msginfo = usb_shmsc_Process[mess->ip];
    usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
}   /* eof usb_hmsc_ClearStallCheck2() */

/******************************************************************************
Function Name   : usb_hmsc_Submitutr
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : USB_UTR_t *utr_table     : Information Table
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
USB_ER_t usb_hmsc_Submitutr(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table)
{
    uint16_t    msgnum, pipeno, offset2;
    USB_ER_t    err;

    usb_shmsc_MsgNum[ptr->ip] = utr_table->keyword;

    switch( type ) 
    {
    case USB_CTRL_END:                      /* Control transfer */
        break;
    case USB_DATA_NONE:                     /* Data transmit */
        msgnum              = utr_table->keyword;
        offset2             = usb_ghmsc_OutPipe[ptr->ip][msgnum][0];
        if( USB_NOPORT == offset2 )
        {
            USB_PRINTF3("### USB_NOPORT error IP[%d] MSGNUM[%d] OFFSET[%x]\n", ptr->ip, msgnum, offset2 );
            return USB_ERROR;
        }
        pipeno              = R_usb_hmsc_Information(ptr->ip, offset2);
        if( USB_PIPE5 < pipeno )
        {
            USB_PRINTF1("### PIPE No. error[%d]\n", pipeno);
            return USB_ERROR;
        }
        utr_table->keyword  = pipeno;
        R_usb_hstd_SetPipeInfo((uint16_t*)&usb_ghmsc_DefEpTbl[0], (uint16_t*)&usb_ghmsc_PipeTable[ptr->ip][offset2], 
            (uint16_t)USB_EPL);
        err = R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    case USB_DATA_OK:                           /* Data recieve */
        msgnum              = utr_table->keyword;
        offset2             = usb_ghmsc_InPipe[ptr->ip][msgnum][0];
        if( USB_NOPORT == offset2 )
        {
            USB_PRINTF3("### USB_NOPORT error IP[%d] MSGNUM[%d] OFFSET[%x]\n", ptr->ip, msgnum, offset2 );
            return USB_ERROR;
        }
        pipeno              = R_usb_hmsc_Information(ptr->ip, offset2);
        if( USB_PIPE5 < pipeno )
        {
            USB_PRINTF1("### PIPE No. error[%d]\n", pipeno);
            return USB_ERROR;
        }
        utr_table->keyword  = pipeno;
        R_usb_hstd_SetPipeInfo((uint16_t*)&usb_ghmsc_DefEpTbl[0], (uint16_t*)&usb_ghmsc_PipeTable[ptr->ip][offset2], 
            (uint16_t)USB_EPL);
        err = R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghmsc_DefEpTbl, pipeno);
        break;
    default:
        break;
    }
    return err;
}   /* eof usb_hmsc_Submitutr() */

/******************************************************************************
Function Name   : usb_hmsc_SubmitutrReq
Description     : Data transfer request
Argument        : uint16_t type             : Data Transmit/Data Receive
                : USB_UTR_t *utr_table      : Information Table
Return value    : USB_ER_t                  : Error Code
******************************************************************************/
USB_ER_t usb_hmsc_SubmitutrReq(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table)
{
    uint16_t    msgnum, pipeno;
    uint16_t    toggle;
    USB_ER_t    err;

    msgnum = usb_shmsc_MsgNum[ptr->ip];

    utr_table->ip = ptr->ip;
    utr_table->ipp = ptr->ipp;

    switch( type ) 
    {
    case USB_CTRL_END:                      /* Control transfer */
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_DATA_NONE:                     /* Data transmit */
        pipeno = utr_table->keyword;

        if( (USB_SQMON & usb_ghmsc_OutPipe[ptr->ip][msgnum][1]) == USB_SQMON )
        {
            toggle = USB_SQ_DATA1;
        }
        else
        {
            toggle = USB_SQ_DATA0;
        }
        usb_hmsc_DoSqtgl(utr_table, pipeno, toggle);
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    case USB_DATA_OK:                       /* Data recieve */
        pipeno = utr_table->keyword;

        if( (USB_SQMON & usb_ghmsc_InPipe[ptr->ip][msgnum][1]) == USB_SQMON )
        {
            toggle = USB_SQ_DATA1;
        }
        else
        {
            toggle = USB_SQ_DATA0;
        }
        usb_hmsc_DoSqtgl(utr_table, pipeno, toggle);
        err = R_usb_hstd_TransferStart(utr_table);
        break;
    default:
        USB_PRINTF0("### submit error\n");
        err = USB_HMSC_SUBMIT_ERR;
        break;
    }
    return err;
}   /* eof usb_hmsc_SubmitutrReq() */


/******************************************************************************
Function Name   : usb_hmsc_DoSqtgl
Description     : Set SQSET bit or SQCLR bit for Pipe control regstor(PIPEnCTR).
Argument        : USB_UTR_t *ptr        : The app's USB Comm. Structure.
                : uint16_t Pipe         : Pipe No.
                : uint16_t toggle       : Sequence Toggle bit(DATA0/DATA1)
Return value    : none
******************************************************************************/
void usb_hmsc_DoSqtgl(USB_UTR_t *ptr, uint16_t Pipe, uint16_t toggle)
{
    uint16_t msginfo;

    if( toggle == USB_SQ_DATA1 )
    {
        msginfo = USB_DO_SET_SQTGL;
    }
    else
    {
        msginfo = USB_DO_CLR_SQTGL;
    }

    R_usb_hstd_ChangeDeviceState(ptr, (USB_CB_t)&usb_cstd_DummyFunction, msginfo, Pipe);
}   /* eof usb_hmsc_DoSqtgl() */

/******************************************************************************
Function Name   : usb_hmsc_ControlEnd
Description     : Control end function
Argument        : uint16_t  sts             : Status
Return value    : none
******************************************************************************/
void usb_hmsc_ControlEnd(USB_UTR_t *ptr, uint16_t sts)
{
        R_usb_hstd_TransferEnd(ptr, USB_PIPE0, sts);
}   /* eof usb_hmsc_ControlEnd() */

/******************************************************************************
End  Of File
******************************************************************************/

