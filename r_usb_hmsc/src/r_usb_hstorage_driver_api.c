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
* File Name    : r_usb_hstorage_driver.c
* Description  : USB sample data declaration
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config_kernel.h"
#endif


#include "r_usb_basic_if.h"
#include "r_usb_hatapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* MSC global definition */
#include "r_usb_api.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"

/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/
/* Call-back function for read/write completion */
extern void R_tfat_disk_read_write_complete(USB_CLSINFO_t *mess);

/******************************************************************************
Function Name   : R_usb_hmsc_StrgTaskOpen
Description     : Storage task open
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_StrgTaskOpen(USB_UTR_t *ptr)
{
    uint16_t        i;

    USB_PRINTF0("*** Install Host MS device driver ***\n");
    R_usb_hmsc_TaskOpen( ptr, 0, 0 );

    for( i = 0; i < (USB_MAXDRIVE + 1); i++ )
    {
        usb_ghmsc_DriveChk[i][0] = USB_NO;  /* Yes/No */
        usb_ghmsc_DriveChk[i][1] = 0;       /* Unit Number */
        usb_ghmsc_DriveChk[i][2] = 0;       /* Partition Number */
        usb_ghmsc_DriveChk[i][3] = 0;       /* Device address */
        usb_ghmsc_DriveChk[i][4] = 0;       /* Device number */
        usb_ghmsc_DriveChk[i][5] = 0;       /* USB IP number */
    }

    usb_ghmsc_MaxDrive = USB_DRIVE;
    usb_ghmsc_StrgCount = 0;
}   /* eof R_usb_hmsc_StrgTaskOpen() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgTaskClose
Description     : Storage task close
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_StrgTaskClose(USB_UTR_t *ptr)
{
    /* Task close */
    R_usb_hmsc_TaskClose(ptr);
    USB_PRINTF0("*** Release Host MS device driver ***\n");
}   /* eof R_usb_hmsc_StrgTaskClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveTask
Description     : Storage drive task
Arguments       : none
Return value    : none
******************************************************************************/
void R_usb_hmsc_StrgDriveTask(void)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;                    /* Error code */
#ifdef FREE_RTOS_PP
    for( ;; )
    {
#endif
    /* receive message */
    err = R_USB_TRCV_MSG( USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );
    if( err != USB_E_OK )
    {
#ifdef FREE_RTOS_PP
        continue;
#else
        return;
#endif
    }

    switch( mess->msginfo )
    {
    case USB_MSG_CLS_INIT:
        break;
    /* enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
        usb_hmsc_ClassWait(USB_HSTRG_MBX, mess);
        break;
    case USB_MSG_HMSC_STRG_DRIVE_SEARCH:
        /* Drive search */
        usb_hmsc_StrgDriveSearchAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DEV_READ_PARTITION:
        /* Read partition */
        usb_hmsc_SmpDevReadPartitionAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_STRG_USER_COMMAND:
        /* R_usb_hmsc_StrgUserCommand Result */
        usb_hmsc_strg_user_command_result((USB_CLSINFO_t *)mess);
        break;
#ifdef FREE_RTOS_PP
    case USB_NONE:
        R_tfat_disk_read_write_complete((USB_CLSINFO_t *)mess);
        break;
#endif
    default:
        break;
    }

    err = R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)mess);

    if( err != USB_E_OK )
    {
        USB_PRINTF0("### USB Strg Task rel_blk error\n");
    }
#ifdef FREE_RTOS_PP
    }
#endif
}   /* eof R_usb_hmsc_StrgDriveTask() */


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveSearch
Description     : Searches drive SndMsg
Arguments       : uint16_t addr        : Address
Return value    : uint16_t             : Status
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveSearch(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_StrgDriveSearchSeq[ptr->ip] = USB_SEQ_0;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        cp->keyword = addr;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;
        cp->complete = complete;

        /* Send message */
        err = R_USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### StrgDriveSearch function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### StrgDriveSearch function pget_blk error\n");
    }
    return (err);
}   /* eof R_usb_hmsc_StrgDriveSearch() */


/******************************************************************************
Function Name   : R_usb_hmsc_StrgDriveClose
Description     : Releases drive
Arguments       : uint16_t side    : Side
Return value    : uint16_t         : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgDriveClose( USB_UTR_t *ptr, uint16_t side )
{
    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_DONE);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts( side ) != USB_HMSC_DEV_ATT )
        {
            USB_PRINTF1("### device det(R_usb_hmsc_StrgDriveClose:side=%d)\n", side);
            return (USB_ERROR);
        }
        usb_hmsc_SmpFsiSectorInitialized(side, (uint32_t)0, (uint16_t)0);
        return (USB_DONE);
    }
}   /* eof R_usb_hmsc_StrgDriveClose() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgReadSector
Description     : Releases drive
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgReadSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Device Status */
    if( R_usb_hmsc_GetDevSts( side ) != USB_HMSC_DEV_ATT )
    {
        USB_PRINTF1("### device det(R_usb_hmsc_StrgReadSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Read10(ptr, side, buff, secno, seccnt, trans_byte);

    return (result);
}   /* eof R_usb_hmsc_StrgReadSector() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgWriteSector
Description     : Writes sector information
Arguments       : uint16_t side        : Side
                : uint8_t  *buff       : Buffer address
                : uint32_t secno       : Sector number
                : uint16_t seccnt      : Sector count
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgWriteSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
    , uint32_t secno, uint16_t seccnt, uint32_t trans_byte )
{
    uint16_t    result;

    /* Device Status */
    if( R_usb_hmsc_GetDevSts( side ) != USB_HMSC_DEV_ATT )
    {
        USB_PRINTF1("### device det(R_usb_hmsc_StrgWriteSector:side=%d)\n", side);
        return (USB_ERROR);
    }
    result = R_usb_hmsc_Write10(ptr, side, buff, secno, seccnt, trans_byte);
    return(result);
}   /* eof R_usb_hmsc_StrgWriteSector() */

/******************************************************************************
Function Name   : R_usb_hmsc_StrgUserCommand
Description     : USB Mass Storage Command
Arguments       : uint16_t side        : Side
                : uint16_t command     : Command
                : uint8_t  *buff       : Buffer address
                : USB_CB_t complete    : callback info
Return value    : uint16_t             : [DONE/ERROR]
******************************************************************************/
uint16_t R_usb_hmsc_StrgUserCommand(USB_UTR_t *ptr, uint16_t side, uint16_t command
    , uint8_t *buff, USB_CB_t complete)
{
    uint16_t    result;

    if( (USB_MEDIADRIVE != USB_DRIVE) && (side == USB_MEDIADRIVE) )
    {   /* Memory device */
        USB_PRINTF1("### not support command(StrgUserCommand:side=%d)\n", side);
        return (USB_ERROR);
    }
    else
    {   /* USB device */
        /* Device Status */
        if( R_usb_hmsc_GetDevSts( side ) != USB_HMSC_DEV_ATT )
        {
            USB_PRINTF1("### device det(StrgUserCommand:side=%d)\n", side);
            return (USB_ERROR);
        }

        switch( command )
        {
        case USB_ATAPI_TEST_UNIT_READY:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Test unit */
            result = R_usb_hmsc_TestUnit(ptr, side);
            break;
        case USB_ATAPI_REQUEST_SENSE:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /*Request sense */
            result = R_usb_hmsc_RequestSense(ptr, side, buff);
            break;
        case USB_ATAPI_FORMAT_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_INQUIRY:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Inquiry */
            result = R_usb_hmsc_Inquiry(ptr, side, buff);
            break;
        case USB_ATAPI_MODE_SELECT6:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Mode select6 */
            result = R_usb_hmsc_ModeSelect6(ptr, side, buff);
            break;
        case USB_ATAPI_MODE_SENSE6:
            return (USB_ERROR);
            break;
        case USB_ATAPI_START_STOP_UNIT:
            return (USB_ERROR);
            break;
        case USB_ATAPI_PREVENT_ALLOW:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Prevent allow */
            result = R_usb_hmsc_PreventAllow(ptr, side, buff);
            break;
        case USB_ATAPI_READ_FORMAT_CAPACITY:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Read format capacity */
            result = R_usb_hmsc_ReadFormatCapacity(ptr, side, buff);
            break;
        case USB_ATAPI_READ_CAPACITY:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Read capacity */
            result = R_usb_hmsc_ReadCapacity(ptr, side, buff);
            break;
        case USB_ATAPI_READ10:
        case USB_ATAPI_WRITE10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_SEEK:
        case USB_ATAPI_WRITE_AND_VERIFY:
        case USB_ATAPI_VERIFY10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SELECT10:
            return (USB_ERROR);
            break;
        case USB_ATAPI_MODE_SENSE10:
            usb_shmsc_StrgProcess[ptr->ip] = USB_MSG_HMSC_STRG_USER_COMMAND;
            usb_shmsc_command_result[ptr->ip] = complete;
            /* Mode sense10 */
            result = R_usb_hmsc_ModeSense10(ptr, side, buff);
            break;
        default:
            return (USB_ERROR);
            break;
        }
        if( result == USB_HMSC_OK )
        {
            return (USB_DONE);
        }
        else
        {
            return (USB_ERROR);
        }
    }
}   /* eof R_usb_hmsc_StrgUserCommand() */


/******************************************************************************
End  Of File
******************************************************************************/
