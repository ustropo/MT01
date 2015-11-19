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
* File Name    : r_usb_hmsc_api.c
* Description  : USB Peripheral Sample Code
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
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_api.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"

/******************************************************************************
Renesas Host MSC Sample Code functions
******************************************************************************/


/******************************************************************************
Function Name   : R_usb_hmsc_Task
Description     : USB HMSC Task
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_Task(void)
{
    usb_hmsc_Task();
}   /* eof R_usb_hmsc_Task() */


/******************************************************************************
Function Name   : R_usb_hmsc_hub_registration
Description     : Hub Data Registration
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hmsc_hub_registration(USB_UTR_t *ptr)
{
    USB_HCDREG_t    driver;

    R_usb_cstd_SetTaskPri(USB_HUB_TSK,  USB_PRI_3);

    /* Driver registration */
    driver.tpl          = (uint16_t*)&usb_ghhub_TPL;        /* Target peripheral list */
    driver.pipetbl      = (uint16_t*)&usb_ghhub_DefEPTbl;   /* Pipe Define Table address */

    R_usb_hhub_Registration(ptr, &driver);
}   /* eof R_usb_hmsc_hub_registration() */

/******************************************************************************
Function Name   : R_usb_hmsc_driver_start
Description     : USB Host Initialize process
Argument        : none
Return          : none
******************************************************************************/
void R_usb_hmsc_driver_start( USB_UTR_t *ptr )
{
    uint16_t    i,j;

    for( i = 0; i < USB_DEVICENUM ; i++ )
    {
        usb_ghmsc_drv_no_tbl[i].use_flag = USB_NOUSE;
    }

    for( i = 0; i < USB_NUM_USBIP ; i++ )
    {
        for( j = 0; j < USB_MAXDEVADDR ; j++ )
        {
            usb_ghmsc_drive_no[i][j] = USB_ERROR;
        }
    }

    R_usb_cstd_SetTaskPri(USB_HMSC_TSK, USB_PRI_3);
    R_usb_cstd_SetTaskPri(USB_HSTRG_TSK, USB_PRI_3);
}   /* eof R_usb_hmsc_driver_start() */

/******************************************************************************
Function Name   : R_usb_hmsc_ClearStall
Description     : Clear Stall
Argument        : uint16_t type             : Data transmit/Data Receive
                : uint16_t msgnum           : Message Number
                : USB_CB_t complete         : Callback Function
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t type, uint16_t msgnum, USB_CB_t complete)
{
    uint16_t        pipeno;

    switch( type ) 
    {
    case USB_DATA_NONE:                         /* Data transmit */
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_OutPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_OutPipe[ptr->ip][msgnum][1] = 0;
        usb_hmsc_ClearStall(ptr, pipeno, complete);
        break;
    case USB_DATA_OK:                           /* Data recieve */
        pipeno = R_usb_hmsc_Information(ptr->ip, usb_ghmsc_InPipe[ptr->ip][msgnum][0]);
        usb_ghmsc_InPipe[ptr->ip][msgnum][1] = 0;
        usb_hmsc_ClearStall(ptr, pipeno, complete);
        break;
    default:
        USB_PRINTF0("### stall error\n");
        break;
    }
}   /* eof R_usb_hmsc_ClearStall() */

/******************************************************************************
Function Name   : R_usb_hmsc_get_pipetbl
Description     : Get pipe table address.
Arguments       : uint16_t devadr           : Device address
Return value    : Pipe table address.
******************************************************************************/
uint16_t* R_usb_hmsc_get_pipetbl(USB_UTR_t *ptr, uint16_t devadr)
{
    if( devadr == usb_ghmsc_Devaddr[ptr->ip] )                      /* Device Address */
    {
        return (uint16_t*)usb_ghmsc_PipeTable[ptr->ip];             /* Pipe Table(DefEP) */
    }

    return (uint16_t*)USB_ERROR;
}   /* eof R_usb_hhid_get_pipetbl() */


/******************************************************************************
Function Name   : R_usb_hmsc_TaskOpen
Description     : Open Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskOpen(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    USB_PRINTF0("*** Install Host MSCD device driver ***\n");
}   /* eof R_usb_hmsc_TaskOpen() */

/******************************************************************************
Function Name   : R_usb_hmsc_TaskClose
Description     : Close Mass storage class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_TaskClose(USB_UTR_t *ptr)
{
    USB_PRINTF0("*** Release Host MS device driver ***\n");
}   /* eof R_usb_hmsc_TaskClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_Initialized
Description     : initialized USB_HMSC_TSK
Arguments       : uint16_t data1 : 
                : uint16_t data2 : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_Initialized(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    uint16_t    i;

    /* 0x00 */

    for( i = 0; i < USB_MAXSTRAGE; i++ )
    {
        usb_ghmsc_AttSts[i] = USB_HMSC_DEV_DET;
        usb_ghmsc_CbwTagNo[ptr->ip][i] = (uint16_t)1;

        usb_ghmsc_Cbw[ptr->ip][i].dCBWSignature = USB_MSC_CBW_SIGNATURE;
        usb_ghmsc_Cbw[ptr->ip][i].dCBWTag = usb_ghmsc_CbwTagNo[ptr->ip][i];
        usb_ghmsc_Cbw[ptr->ip][i].dCBWDTL_Lo = 0;
        usb_ghmsc_Cbw[ptr->ip][i].dCBWDTL_ML = 0;
        usb_ghmsc_Cbw[ptr->ip][i].dCBWDTL_MH = 0;
        usb_ghmsc_Cbw[ptr->ip][i].dCBWDTL_Hi = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bmCBWFlags.CBWdir = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bmCBWFlags.reserved7 = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bCBWLUN.bCBWLUN = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bCBWLUN.reserved4 = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bCBWCBLength.bCBWCBLength = 0;
        usb_ghmsc_Cbw[ptr->ip][i].bCBWCBLength.reserved3 = 0;

        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[0]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[1]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[2]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[3]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[4]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[5]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[6]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[7]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[8]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[9]  = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[10] = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[11] = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[12] = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[13] = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[14] = 0;
        usb_ghmsc_Cbw[ptr->ip][i].CBWCB[15] = 0;
    }
}   /* eof R_usb_hmsc_Initialized() */

/******************************************************************************
Function Name   : R_usb_hmsc_ClassCheck
Description     : check connected device
Arguments       : uint16_t **table : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_ClassCheck(USB_UTR_t *ptr, uint16_t **table)
{

    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_ghmsc_DeviceTable[ptr->ip]       = (uint8_t*)(table[0]);
    usb_ghmsc_ConfigTable[ptr->ip]       = (uint8_t*)(table[1]);
    usb_ghmsc_InterfaceTable[ptr->ip]    = (uint8_t*)(table[2]);
    usb_ghmsc_Speed[ptr->ip]             = *table[6];
    usb_ghmsc_Devaddr[ptr->ip]           = *table[7];
    usb_ghmsc_PipeTable[ptr->ip]         = (uint16_t*)(table[8]);        /* Pipe Table(DefEP) */
    *table[3]                   = USB_DONE;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSC_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_INIT;
        usb_shmsc_InitSeq[ptr->ip] = USB_SEQ_0;

        cp->ip = ptr->ip;
        cp->ipp = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HMSC_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### ClassCheck function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### ClassCheck function pget_blk error\n");
    }   
}   /* eof R_usb_hmsc_ClassCheck() */


/******************************************************************************
Function Name   : R_usb_hmsc_Read10
Description     : Read10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Read10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Read10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw(ptr, (uint16_t)USB_ATAPI_READ10, secno, seccnt, trans_byte, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, trans_byte);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Read10() */

/******************************************************************************
Function Name   : R_usb_hmsc_Write10
Description     : Write10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Write10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff, uint32_t secno,
     uint16_t seccnt, uint32_t trans_byte)
{
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Write10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* set CBW parameter */
    usb_hmsc_SetRwCbw(ptr, (uint16_t)USB_ATAPI_WRITE10, secno, seccnt, trans_byte, side);

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(ptr, side, buff, trans_byte);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Write10() */

/******************************************************************************
Function Name   : R_usb_hmsc_PreventAllow
Description     : PreventAllow
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_PreventAllow(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(PreventAllow:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_PREVENT_ALLOW;
    /* Reserved */
    data[4] = buff[0];

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(ptr, side);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_PreventAllow() */

/******************************************************************************
Function Name   : R_usb_hmsc_TestUnit
Description     : TestUnit
Arguments       : uint16_t side : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_TestUnit(USB_UTR_t *ptr, uint16_t side)
{
    uint8_t     data[12];
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(TestUnit:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_TEST_UNIT_READY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)0, side);

    /* No Data */
    hmsc_retval = usb_hmsc_NoData(ptr, side);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_TestUnit() */

/******************************************************************************
Function Name   : R_usb_hmsc_RequestSense
Description     : RequestSense
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_RequestSense(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(RequestSense:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_REQUEST_SENSE;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_RequestSense() */

/******************************************************************************
Function Name   : R_usb_hmsc_Inquiry
Description     : Inquiry
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Inquiry(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 36;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(Inquiry:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_INQUIRY;
    /* Allocation length */
    data[4] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_Inquiry() */

/******************************************************************************
Function Name   : R_usb_hmsc_ReadCapacity
Description     : ReadCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 8;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(ReadCapacity:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_CAPACITY;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ReadCapacity() */

/******************************************************************************
Function Name   : R_usb_hmsc_ReadFormatCapacity
Description     : ReadFormatCapacity
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ReadFormatCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 0x20;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(read_format:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_READ_FORMAT_CAPACITY;
    /* Allocation length */
    data[8] = length;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ReadFormatCapacity() */

/******************************************************************************
Function Name   : R_usb_hmsc_ModeSense10
Description     : ModeSense10
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSense10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 26;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(ModeSense10:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SENSE10;
    /* Set LUN / DBD=1 */
    data[1] = 0x08;
    /* Allocation length */
    data[7] = 0x00;
    /* Allocation length */
    data[8] = 0x02;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data IN */
    hmsc_retval = usb_hmsc_DataIn(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ModeSense10() */

/******************************************************************************
Function Name   : R_usb_hmsc_ModeSelect6
Description     : ModeSelect6
Arguments       : uint16_t side : 
                : uint8_t *buff : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_ModeSelect6(USB_UTR_t *ptr, uint16_t side, uint8_t *buff)
{
    uint8_t     data[12];
    uint8_t     length = 18;
    uint16_t    hmsc_retval, unit;

    unit = usb_hmsc_SmpDrive2Unit(ptr, side);
    if( unit == USB_ERROR )
    {
        USB_PRINTF2("### unit error(mode_sense6:side=%d,unit=%d)\n", side, unit);
        return (USB_ERROR);
    }

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)12, data);

    /* Data set */
    /* Command */
    data[0] = USB_ATAPI_MODE_SELECT6;
    /* SP=0 */
    data[1] = 0x10;
    /* Parameter list length ??? */
    data[4] = 0x18;

    /* Set CBW parameter */
    usb_hmsc_SetElsCbw(ptr, (uint8_t*)&data, (uint32_t)length, side);

    /* Data clear */
    usb_hmsc_ClrData((uint16_t)24, buff);

    /* Data set */
    buff[3] = 0x08;
    buff[10] = 0x02;
    buff[12] = 0x08;
    buff[13] = 0x0A;

    /* Data OUT */
    hmsc_retval = usb_hmsc_DataOut(ptr, side, buff, (uint32_t)length);
    return (hmsc_retval);
}   /* eof R_usb_hmsc_ModeSelect6() */

/******************************************************************************
Function Name   : R_usb_hmsc_DriveSpeed
Description     : DriveSpeed
Arguments       : uint16_t side : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_DriveSpeed(USB_UTR_t *ptr, uint16_t side)
{
    uint16_t    tbl[10];

    R_usb_hstd_DeviceInformation(ptr, side, (uint16_t *)tbl);
    return (tbl[3]);            /* Speed */
}   /* eof R_usb_hmsc_DriveSpeed() */

/******************************************************************************
Function Name   : R_usb_hmsc_Release
Description     : Release Mass Strage Class driver
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_Release(USB_UTR_t *ptr)
{
    R_usb_hstd_DriverRelease(ptr, (uint8_t)USB_IFCLS_MAS);
}   /* eof R_usb_hmsc_Release() */


/******************************************************************************
Function Name   : R_usb_hmsc_SetDevSts
Description     : Sets HMSCD operation state
Arguments       : uint16_t data : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_SetDevSts( uint16_t side, uint16_t data )
{
    usb_ghmsc_AttSts[side] = data;
    return USB_DONE;
}   /* eof R_usb_hmsc_SetDevSts() */

/******************************************************************************
Function Name   : R_usb_hmsc_GetDevSts
Description     : Responds to HMSCD operation state
Arguments       : none
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_GetDevSts( uint16_t side )
{
    return( usb_ghmsc_AttSts[side] );
}   /* eof R_usb_hmsc_GetDevSts() */

/******************************************************************************
Function Name   : R_usb_hmsc_Information
Description     : EP Table Information
Arguments       : uint16_t pipe_offset : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t R_usb_hmsc_Information(uint16_t ipno, uint16_t pipe_offset)
{
    return(usb_ghmsc_PipeTable[ipno][pipe_offset]);
}   /* eof R_usb_hmsc_Information() */

/******************************************************************************
Function Name   : R_usb_hmsc_GetMaxUnit
Description     : Get Max LUN request
Argument        : uint16_t addr             : Device Address
                : USB_CB_t complete         : CallBack Function
Return value    : USB_ER_t                  : Error Code
******************************************************************************/
USB_ER_t R_usb_hmsc_GetMaxUnit(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete)
{
    USB_ER_t err;
    static uint16_t get_max_lun_table[5] = {0xFEA1, 0x0000, 0x0000, 0x0001, 0x0000};

    /* Device address set */
    get_max_lun_table[4] = addr;

    /* Recieve MaxLUN */
    usb_ghmsc_ControlData[ptr->ip].keyword   = USB_PIPE0;
    usb_ghmsc_ControlData[ptr->ip].tranadr   = (void*)usb_ghmsc_Data[ptr->ip];
    usb_ghmsc_ControlData[ptr->ip].tranlen   = (uint32_t)1;
    usb_ghmsc_ControlData[ptr->ip].setup     = get_max_lun_table;
    usb_ghmsc_ControlData[ptr->ip].complete  = complete;
    usb_ghmsc_ControlData[ptr->ip].segment   = USB_TRAN_END;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_CTRL_END, &usb_ghmsc_ControlData[ptr->ip]);
    return err;
}   /* eof R_usb_hmsc_GetMaxUnit() */


/******************************************************************************
Function Name   : R_usb_hmsc_MassStorageReset
Description     : Mass Strage Reset request
Argument        : uint16_t drvnum           : Drive Number
                : USB_CB_t complete        : Callback Funtion
Return value    : USB_ER_t                 : Error Code
******************************************************************************/
USB_ER_t R_usb_hmsc_MassStorageReset(USB_UTR_t *ptr, uint16_t drvnum, USB_CB_t complete)
{
    USB_ER_t err;
    USB_UTR_t    dev_adr;

    static uint16_t mass_storage_reset_table[5] = {0xFF21, 0x0000, 0x0000, 0x0000, 0x0000};

    /* Device address set */
    usb_hmsc_SmpDrive2Addr(drvnum, &dev_adr);
    mass_storage_reset_table[4] = dev_adr.keyword;

    /* Set MassStorageReset */
    usb_ghmsc_ControlData[ptr->ip].keyword   = USB_PIPE0;
    usb_ghmsc_ControlData[ptr->ip].tranadr   = (void*)usb_ghmsc_Data[ptr->ip];
    usb_ghmsc_ControlData[ptr->ip].tranlen   = (uint32_t)0;
    usb_ghmsc_ControlData[ptr->ip].setup     = mass_storage_reset_table;
    usb_ghmsc_ControlData[ptr->ip].complete  = complete;
    usb_ghmsc_ControlData[ptr->ip].segment   = USB_TRAN_END;

    err = usb_hmsc_SubmitutrReq(ptr, (uint16_t)USB_CTRL_END, &usb_ghmsc_ControlData[ptr->ip]);
    return err;
}   /* eof R_usb_hmsc_MassStorageReset() */

/******************************************************************************
Function Name   : R_usb_hmsc_DriveClose
Description     : drive close
Arguments       : uint16_t addr : 
                : uint16_t data2 : 
Return value    : none
******************************************************************************/
void R_usb_hmsc_DriveClose(USB_UTR_t *ptr, uint16_t addr, uint16_t data2)
{
    uint16_t    strage_drive_no;

    usb_hmsc_SmpFsiDriveClear(ptr, addr);
    strage_drive_no = R_usb_hmsc_ref_drvno(ptr->ip, addr);
    R_usb_hmsc_free_drvno(strage_drive_no);
}   /* eof R_usb_hmsc_DriveClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_alloc_drvno
Description     : Get Drive no.
Arguments       : uint16_t ipno         : USB IP No. 
                : uint16_t devadr       : Device address
Return value    : Drive no.
******************************************************************************/
uint16_t    R_usb_hmsc_alloc_drvno(uint16_t ipno, uint16_t devadr)
{
    uint16_t    drv_no;

    /* Initialeze hid device state */
    for( drv_no = 0; drv_no < USB_DEVICENUM ; drv_no++ )
    {
        if( USB_NOUSE == usb_ghmsc_drv_no_tbl[drv_no].use_flag )
        {
            usb_ghmsc_drv_no_tbl[drv_no].use_flag = USB_YES;
            usb_ghmsc_drv_no_tbl[drv_no].dev_adr = devadr;
            usb_ghmsc_drv_no_tbl[drv_no].ip = ipno;
            usb_ghmsc_drive_no[ipno][devadr] = drv_no;
            return drv_no;
        }
    }

    return USB_ERROR;
} /* eof R_usb_hmsc_alloc_drvno() */

/******************************************************************************
Function Name   : R_usb_hmsc_free_drvno
Description     : Release Drive no.
Arguments       : uint16_t drvno        : Drive no.
Return value    : none
******************************************************************************/
void R_usb_hmsc_free_drvno( uint16_t drvno )
{
    uint16_t    ip;
    uint16_t    devadr;

    ip = usb_ghmsc_drv_no_tbl[drvno].ip;
    devadr = usb_ghmsc_drv_no_tbl[drvno].dev_adr;

    usb_ghmsc_drv_no_tbl[drvno].use_flag = USB_NOUSE;
    if( usb_ghmsc_drive_no[ip][devadr] == drvno )
    {
        usb_ghmsc_drive_no[ip][devadr] = USB_ERROR;
    }
} /* eof R_usb_hmsc_free_drvno() */

/******************************************************************************
Function Name   : R_usb_hmsc_ref_drvno
Description     : Get Drive no.
Arguments       : uint16_t ipno         : USB IP No. 
                : uint16_t devadr       : Device address
Return value    : Drive no.
******************************************************************************/
uint16_t    R_usb_hmsc_ref_drvno(uint16_t ipno, uint16_t devadr)
{
    return usb_ghmsc_drive_no[ipno][devadr];
} /* eof R_usb_hmsc_ref_drvno() */


/******************************************************************************
End  Of File
******************************************************************************/
