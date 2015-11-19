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
* File Name    : r_usb_hmsc_driver.c
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
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* Host MSC grobal define */
#include "r_usb_api.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"

#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#endif

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t usb_ghmsc_DefEpTbl[USB_NUM_USBIP][USB_EPL+1] =
{
  {  /* USB IP No.0 */
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_PDTBLEND /* Pipe end */
  },
  {  /* USB IP No.1 */
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_NONE,
    USB_PDTBLEND /* Pipe end */
  }
};

uint8_t             usb_ghmsc_Data[USB_NUM_USBIP][5120];
USB_UTR_t           usb_ghmsc_TransData[USB_NUM_USBIP][USB_MAXSTRAGE];     /* Send data transfer message */
USB_UTR_t           usb_ghmsc_ReceiveData[USB_NUM_USBIP][USB_MAXSTRAGE];   /* Receive data transfer message */
USB_UTR_t           usb_ghmsc_ControlData[USB_NUM_USBIP];                  /* Control data transfer message */
uint16_t            usb_ghmsc_OutPipe[USB_NUM_USBIP][USB_MAXSTRAGE][2];    /* Pipenumber / Pipectr(SQTGL) */
uint16_t            usb_ghmsc_InPipe[USB_NUM_USBIP][USB_MAXSTRAGE][2];     /* Pipenumber / Pipectr(SQTGL) */
uint16_t            usb_shmsc_MsgNum[USB_NUM_USBIP];
USB_MSC_CBW_t       usb_ghmsc_Cbw[USB_NUM_USBIP][USB_MAXSTRAGE];           /* CBW headder */
USB_MSC_CSW_t       usb_ghmsc_Csw[USB_NUM_USBIP][USB_MAXSTRAGE];           /* CSW headder */
uint32_t            usb_ghmsc_CbwTagNo[USB_NUM_USBIP][USB_MAXSTRAGE];      /* CBW tag number */
uint8_t             usb_ghmsc_AtapiFlag[USB_NUM_USBIP][USB_MAXSTRAGE];
uint16_t            usb_ghmsc_AttSts[USB_MAXSTRAGE];
uint32_t            usb_ghmsc_TransSize[USB_NUM_USBIP];
uint8_t             *usb_ghmsc_Buff[USB_NUM_USBIP];
uint16_t            usb_shmsc_Process[USB_NUM_USBIP];
uint16_t            usb_shmsc_NoDataSeq[USB_NUM_USBIP]                      = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_DataInSeq[USB_NUM_USBIP]                      = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_DataOutSeq[USB_NUM_USBIP]                     = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_StallErrSeq[USB_NUM_USBIP]                    = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_DataStallSeq[USB_NUM_USBIP]                   = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_ghmsc_RootDrive[USB_NUM_USBIP];
uint16_t            usb_ghmsc_CswErrRoop[USB_NUM_USBIP]                     = { USB_OFF, USB_OFF };
uint16_t            usb_ghmsc_drive_no[USB_NUM_USBIP][USB_MAXDEVADDR];
DRIVE_MANAGEMENT_t  usb_ghmsc_drv_no_tbl[USB_DEVICENUM];             /* Drive no. management table */

uint8_t             *usb_ghmsc_DeviceTable[USB_NUM_USBIP];
uint8_t             *usb_ghmsc_ConfigTable[USB_NUM_USBIP];
uint8_t             *usb_ghmsc_InterfaceTable[USB_NUM_USBIP];
uint16_t            usb_ghmsc_Speed[USB_NUM_USBIP];
uint16_t            usb_ghmsc_Devaddr[USB_NUM_USBIP];
uint16_t            usb_shmsc_InitSeq[USB_NUM_USBIP]                        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            *usb_ghmsc_PipeTable[USB_NUM_USBIP];                   /* Pipe Table(DefEP) */

/*----------------*/
/* Storage Driver */
/*----------------*/
USB_UTR_t           usb_shmsc_ClassControl[USB_NUM_USBIP];
USB_CB_t            usb_shmsc_command_result[USB_NUM_USBIP];
uint16_t            usb_ghmsc_DriveChk[USB_MAXDRIVE + 1][6];
uint16_t            usb_shmsc_ClassRequest[USB_NUM_USBIP][5];
uint8_t             usb_shmsc_DeviceReady[USB_MAXUNITNUM];
uint8_t             usb_ghmsc_ClassData[USB_NUM_USBIP][USB_HMSC_CLSDATASIZE];

/* Yes/No, Unit Number, Partition Number, device address, EPtbl offset */
uint32_t            usb_ghmsc_MaxLUN[USB_NUM_USBIP];
uint16_t            usb_ghmsc_StrgCount;
uint16_t            usb_ghmsc_MaxDrive;
uint16_t            usb_shmsc_StrgProcess[USB_NUM_USBIP]                    = { USB_NONE, USB_NONE };
uint16_t            usb_shmsc_StrgDriveSearchSeq[USB_NUM_USBIP]             = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_StrgDriveSearchErrCount[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_StrgDriveSearchCount[USB_NUM_USBIP]           = { USB_SEQ_0, USB_SEQ_0 };

/* Read Sector */
uint16_t            usb_shmsc_DevReadSectorSizeSeq[USB_NUM_USBIP]           = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_DevReadSectorSizeErrCount[USB_NUM_USBIP]      = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_DevReadPartitionSeq[USB_NUM_USBIP]            = { USB_SEQ_0, USB_SEQ_0 };

/* Drive Open */
uint16_t            usb_shmsc_StrgDriveOpenSeq[USB_NUM_USBIP]               = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_StrgDriveOpenCount[USB_NUM_USBIP]             = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_StrgDriveOpenParCount[USB_NUM_USBIP]          = { USB_SEQ_0, USB_SEQ_0 };

/* Partition */
uint32_t            usb_shmsc_PartitionLba[USB_NUM_USBIP][USB_BOOTPARTNUM + 1u];
uint16_t            usb_ghmsc_RootDevaddr[USB_NUM_USBIP];
uint16_t            usb_shmsc_NewDrive[USB_NUM_USBIP];
uint16_t            usb_shmsc_LoopCont[USB_NUM_USBIP]                       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t            usb_shmsc_Unit[USB_NUM_USBIP];
uint16_t            usb_ghmsc_PartTransSize[USB_NUM_USBIP];
uint8_t             usb_shmsc_PartitionInfo[USB_NUM_USBIP][USB_BOOTPARTNUM];


/******************************************************************************
Renesas Abstracted HMSC Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_Task
Description     : USB HMSC Task
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_Task(void)
{
    USB_UTR_t       *mess;
    /* Error code */
    USB_ER_t        err;
#ifdef FREE_RTOS_PP
    for( ;; )
	{
#endif
    /* Receive message */
    err = R_USB_TRCV_MSG( USB_HMSC_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );
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
        usb_hmsc_ClassCheck(mess, (USB_CLSINFO_t *)mess);
        break;

    case USB_MSG_HMSC_NO_DATA:
        usb_hmsc_NoDataAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_IN:
        usb_hmsc_DataInAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_OUT:
        usb_hmsc_DataOutAct((USB_CLSINFO_t *)mess);
        break;
    case USB_MSG_HMSC_DATA_STALL:
        usb_hmsc_DataStall((USB_UTR_t *)mess);
        break;
    case USB_MSG_HMSC_CBW_ERR:
        usb_hmsc_StallErr((USB_UTR_t *)mess);
        break;      
    case USB_MSG_HMSC_CSW_PHASE_ERR:
        usb_hmsc_StallErr((USB_UTR_t *)mess);
        break;
/* enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
        usb_hmsc_ClassWait(USB_HMSC_MBX, mess);
        break;
    default:
        break;
    }
    err = R_USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)mess);
    if( err != USB_E_OK )
    {
        USB_PRINTF0("### USB Strg Task rel_blk error\n");
    }
#ifdef FREE_RTOS_PP
	}
#endif
}   /* eof usb_hmsc_Task() */


/******************************************************************************
Function Name   : usb_hmsc_SetRwCbw
Description     : CBW parameter initialization for the READ10/WRITE10 command
Arguments       : uint16_t command : 
                : uint32_t secno : 
                : uint16_t seccnt : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetRwCbw(USB_UTR_t *ptr, uint16_t command, uint32_t secno, uint16_t seccnt,
     uint32_t trans_byte, uint16_t side)
{
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, side);
    if( USB_ERROR == msgnum )
    {
        return;
    }

    /* CBW parameter set */
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWTag = usb_ghmsc_CbwTagNo[ptr->ip][msgnum];
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(ptr, side);
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.reserved3 = 0;

    /* ATAPI_COMMAND */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[0] = (uint8_t)command;
    /* LUN */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[1] = 0x00;
    /* sector address */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[2] = (uint8_t)(secno >> 24);
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[3] = (uint8_t)(secno >> 16);
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[4] = (uint8_t)(secno >> 8);
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[5] = (uint8_t)secno;
    /* Reserved */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[6] = 0x00;
    /* Sector length */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[7] = (uint8_t)(seccnt >> 8);
    /* Block address */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[8] = (uint8_t)seccnt;
    /* Control data */
    usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[9] = (uint8_t)0x00;

    /* ATAPI command check */
    switch( command )
    {
    case USB_ATAPI_TEST_UNIT_READY:
    case USB_ATAPI_REQUEST_SENSE:
    case USB_ATAPI_INQUIRY:
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
    case USB_ATAPI_START_STOP_UNIT:
    case USB_ATAPI_PREVENT_ALLOW:
    case USB_ATAPI_READ_FORMAT_CAPACITY:
    case USB_ATAPI_READ_CAPACITY:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Initialized READ CBW TAG */
    case USB_ATAPI_READ10:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        /* 10bytes */
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Initialized WRITE CBW TAG */
    case USB_ATAPI_WRITE10:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 0;
        /* 10bytes */
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
    case USB_ATAPI_MODE_SELECT10:
    case USB_ATAPI_MODE_SENSE10:
    default:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    }

    if( usb_ghmsc_AtapiFlag[ptr->ip][msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}   /* eof usb_hmsc_SetRwCbw() */

/******************************************************************************
Function Name   : usb_hmsc_SetElsCbw
Description     : CBW parameter initialization for other commands
Arguments       : uint8_t *data : 
                : uint32_t trans_byte : 
                : uint16_t side : 
Return value    : none
******************************************************************************/
void usb_hmsc_SetElsCbw(USB_UTR_t *ptr, uint8_t *data, uint32_t trans_byte, uint16_t side)
{
    uint8_t     i;
    uint16_t    msgnum;

    /* Data IN */
    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, side);
    if( USB_ERROR == msgnum )
    {
        return;
    }

    /* CBW parameter set */
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWTag = usb_ghmsc_CbwTagNo[ptr->ip][msgnum];
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_Lo = (uint8_t)trans_byte;
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_ML = (uint8_t)(trans_byte >> 8);
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_MH = (uint8_t)(trans_byte >> 16);
    usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWDTL_Hi = (uint8_t)(trans_byte >> 24);
    /* Receive */
    usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.reserved7 = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWLUN.bCBWLUN = usb_hmsc_SmpDrive2Unit(ptr, side);
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWLUN.reserved4 = 0;
    usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.reserved3 = 0;

    for( i = 0; i < 12; i++ )
    {
        usb_ghmsc_Cbw[ptr->ip][msgnum].CBWCB[i] = data[i];
    }

    /* ATAPI command check */
    switch( data[0] )
    {
    /* No data */
    case USB_ATAPI_TEST_UNIT_READY:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_REQUEST_SENSE:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Send */
    case USB_ATAPI_FORMAT_UNIT:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Receive */
    case USB_ATAPI_INQUIRY:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    case USB_ATAPI_MODE_SELECT6:
    case USB_ATAPI_MODE_SENSE6:
        break;
    /* No data */
    case USB_ATAPI_START_STOP_UNIT:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* No data */
    case USB_ATAPI_PREVENT_ALLOW:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 6;
        break;
    /* Receive */
    case USB_ATAPI_READ_FORMAT_CAPACITY:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    /* Receive */
    case USB_ATAPI_READ_CAPACITY:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    case USB_ATAPI_READ10:
    case USB_ATAPI_WRITE10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    case USB_ATAPI_SEEK:
    case USB_ATAPI_WRITE_AND_VERIFY:
    case USB_ATAPI_VERIFY10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Send */
    case USB_ATAPI_MODE_SELECT10:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    /* Receive */
    case USB_ATAPI_MODE_SENSE10:
        usb_ghmsc_Cbw[ptr->ip][msgnum].bmCBWFlags.CBWdir = 1;
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength = 10;
        break;
    default:
        USB_PRINTF0("### Non-mounted command demand generating !\n");
        break;
    }

    if( usb_ghmsc_AtapiFlag[ptr->ip][msgnum] == USB_ATAPI )
    {
        /* 12bytes */
        usb_ghmsc_Cbw[ptr->ip][msgnum].bCBWCBLength.bCBWCBLength
        = USB_MSC_CBWCB_LENGTH;
    }
}   /* eof usb_hmsc_SetElsCbw() */

/******************************************************************************
Function Name   : usb_hmsc_CbwTagCount
Description     : Updates tag information
Arguments       : uint16_t msgnum : 
Return value    : none
******************************************************************************/
void usb_hmsc_CbwTagCount(USB_UTR_t *ptr, uint16_t msgnum)
{
    usb_ghmsc_CbwTagNo[ptr->ip][msgnum]++;
    if( usb_ghmsc_CbwTagNo[ptr->ip][msgnum] == (uint16_t)0 )
    {
        usb_ghmsc_CbwTagNo[ptr->ip][msgnum] = (uint16_t)1;
    }
}   /* eof usb_hmsc_CbwTagCount() */

/******************************************************************************
Function Name   : usb_hmsc_CheckCsw
Description     : CSW check
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_CheckCsw(USB_UTR_t *ptr, uint16_t drvnum)
{
    uint16_t    msgnum;
    USB_CLSINFO_t *mess;
    
    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        USB_HMSC_CSW_ERR;
    }

    /* CSW Check */
    if( usb_ghmsc_Csw[ptr->ip][msgnum].dCSWSignature != USB_MSC_CSW_SIGNATURE )
    {
        USB_PRINTF2("### CSW signature error 0x%08x:SIGN=0x%08x.\n",
             usb_ghmsc_Csw[ptr->ip][msgnum].dCSWSignature, USB_MSC_CSW_SIGNATURE);
        return USB_HMSC_CSW_ERR;
    }
    if( usb_ghmsc_Csw[ptr->ip][msgnum].dCSWTag != usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWTag )
    {
        USB_PRINTF2("### CSW Tag error 0x%08x:CBWTAG=0x%08x.\n",
             usb_ghmsc_Csw[ptr->ip][msgnum].dCSWTag, usb_ghmsc_Cbw[ptr->ip][msgnum].dCBWTag);
        return USB_HMSC_CSW_ERR;
    }
    switch( usb_ghmsc_Csw[ptr->ip][msgnum].bCSWStatus )
    {
    case USB_MSC_CSW_OK:
        return USB_HMSC_OK;
        break;
    case USB_MSC_CSW_NG:
        return USB_HMSC_CSW_ERR;
        break;
    case USB_MSC_CSW_PHASE_ERR:
        usb_shmsc_Process[ptr->ip] = USB_MSG_HMSC_CSW_PHASE_ERR;
        usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_0;
        mess->keyword = drvnum;
        mess->msginfo = usb_shmsc_Process[ptr->ip];
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess );
        return USB_HMSC_CSW_PHASE_ERR;
        break;
    default:
        break;
    }
    USB_PRINTF1("### CSW status error 0x%2x.\n", usb_ghmsc_Csw[ptr->ip][msgnum].bCSWStatus);
    return USB_HMSC_CSW_ERR;
}   /* eof usb_hmsc_CheckCsw() */

/******************************************************************************
Function Name   : usb_hmsc_SmpBotDescriptor
Description     : BOT Descriptor
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpBotDescriptor(USB_UTR_t *ptr, uint8_t *table, uint16_t msgnum)
{
    /* Check Descriptor */
    switch( table[1] )
    {
    /* Device Descriptor */
    case USB_DT_DEVICE:
        USB_PRINTF0("### Not Interface descriptor (Device Desc).\n");
        return USB_ERROR;
        break;
    /* Configuration Descriptor */
    case USB_DT_CONFIGURATION:
        USB_PRINTF0("### Not Interface descriptor (Config Desc).\n");
        return USB_ERROR;
        break;
    /* String Descriptor */
    case USB_DT_STRING:
        USB_PRINTF0("### Not Interface descriptor (String Desc).\n");
        return USB_ERROR;
        break;
    /* Interface Descriptor ? */
    case USB_DT_INTERFACE:
        /* Check Interface Descriptor (deviceclass) */
        if( table[5] != USB_IFCLS_MAS )
        {
            USB_PRINTF1("### Interface deviceclass is %x , not support.\n", table[5]);
            return USB_ERROR;
        }
        /* Check Interface Descriptor (subclass) */
        if( table[6] == USB_ATAPI )
        {
            USB_PRINTF0("    Interface subclass  : SFF-8070i\n");
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[ptr->ip][msgnum] = USB_ATAPI;
        }
        else if (table[6] == USB_SCSI)
        {
            USB_PRINTF0(
                "Interface subclass  : SCSI transparent command set\n");
            /* SCSI Command */
            usb_ghmsc_AtapiFlag[ptr->ip][msgnum] = USB_SCSI;
        }
        else if (table[6] == USB_ATAPI_MMC5)
        {
            USB_PRINTF0("   Interface subclass   : ATAPI command set\n");
            /* ATAPI Command */
            usb_ghmsc_AtapiFlag[ptr->ip][msgnum] = USB_ATAPI_MMC5;
        }
        else
        {
            USB_PRINTF1("### Interface subclass is %x , not support.\n", table[6]);
            /* Unknown Command */
            usb_ghmsc_AtapiFlag[ptr->ip][msgnum] = USB_NONE;
            return USB_ERROR;
        }
        /* Check Interface Descriptor (protocol) */
        if( table[7] == USB_BOTP )
        {
            USB_PRINTF0("   Interface protocol   : BOT \n");
        }
        else
        {
            USB_PRINTF1("### Interface protocol is %x , not support.\n", table[7]);
            return USB_ERROR;
        }
        /* Check Endpoint number */
        if( table[4] < USB_TOTALEP )
        {
            USB_PRINTF1("### Endpoint number is %x , less than 2.\n", table[4]);
            return USB_ERROR;
        }
        return USB_DONE;
        break;
    /* Endpoint Descriptor */
    case USB_DT_ENDPOINT:
        USB_PRINTF0("### Not Interface descrip (Endpoint Desc).\n");
        return USB_ERROR;
        break;
    /* Device Qualifier Descriptor */
    case USB_DT_DEVICE_QUALIFIER:
        USB_PRINTF0("### Not Interface descrip (Dev Qualifier Desc).\n");
        return USB_ERROR;
        break;
    /* Other Speed Configuration Descriptor */
    case USB_DT_OTHER_SPEED_CONF:
        USB_PRINTF0("### Not Interface descrip (Other Speed Config Desc).\n");
        return USB_ERROR;
        break;
    /* Interface Power Descriptor */
    case USB_DT_INTERFACE_POWER:
        USB_PRINTF0("### Not Interface descrip (Interface Power Desc).\n");
        return USB_ERROR;
        break;
    /* Not Descriptor */
    default:
        USB_PRINTF0("### Not Interface descrip ( Not Standard Desc ).\n");
        break;
    }
    return USB_ERROR;
}   /* eof usb_hmsc_SmpBotDescriptor() */

/******************************************************************************
Function Name   : usb_hmsc_SmpPipeInfo
Description     : Pipe Information
Arguments       : uint8_t *table : 
                : uint16_t msgnum : 
                : uint16_t speed : 
                : uint16_t length : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_SmpPipeInfo(USB_UTR_t *ptr, uint8_t *table, uint16_t msgnum, uint16_t speed, uint16_t length)
{
    uint16_t        dirflag = 0;
    uint16_t        ofdsc, offset;
    uint16_t        retval;

    /* Check Descriptor */
    if( table[1] != USB_DT_INTERFACE )
    {
        /* Configuration Descriptor */
        USB_PRINTF0("### Not Interface descriptor.\n");
        return USB_ERROR;
    }

    offset = (uint16_t)(2u * USB_EPL * msgnum);

    /* Check Endpoint Descriptor */
    ofdsc = table[0];
    /* Pipe initial */
    usb_ghmsc_OutPipe[ptr->ip][msgnum][0]    = USB_NOPORT;
    usb_ghmsc_InPipe[ptr->ip][msgnum][0]     = USB_NOPORT;
    /* Toggle clear */
    usb_ghmsc_OutPipe[ptr->ip][msgnum][1]    = 0;
    usb_ghmsc_InPipe[ptr->ip][msgnum][1]     = 0;
    while( ofdsc < (length - table[0]) )
    {
        /* Search within Interface */
        switch( table[ofdsc + 1] )
        {
        /* Device Descriptor ? */
        case USB_DT_DEVICE:
        /* Configuration Descriptor ? */
        case USB_DT_CONFIGURATION:
        /* String Descriptor ? */
        case USB_DT_STRING:
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Endpoint Descriptor error.\n");
            return USB_ERROR;
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            /* Bulk Endpoint */
            if( (table[ofdsc + 3] & USB_EP_TRNSMASK) == USB_EP_BULK )
            {
                switch( dirflag )
                {
                case 0:
                    retval = R_usb_hstd_ChkPipeInfo(speed, &usb_ghmsc_PipeTable[ptr->ip][offset], &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[ptr->ip][msgnum][0] = offset;
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[ptr->ip][msgnum][0] = offset;
                    }
                    dirflag++;
                    break;
                case 1:
                    retval = R_usb_hstd_ChkPipeInfo(speed, &usb_ghmsc_PipeTable[ptr->ip][offset + USB_EPL],
                                &table[ofdsc]);
                    if( retval == USB_DIR_H_OUT )
                    {
                        usb_ghmsc_OutPipe[ptr->ip][msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( retval == USB_DIR_H_IN )
                    {
                        usb_ghmsc_InPipe[ptr->ip][msgnum][0] = (uint16_t)(offset + USB_EPL);
                    }
                    if( (usb_ghmsc_InPipe[ptr->ip][msgnum][0] != USB_NOPORT) &&
                        (usb_ghmsc_OutPipe[ptr->ip][msgnum][0] != USB_NOPORT) )
                    {
                        return USB_DONE;
                    }
                    USB_PRINTF0("### Endpoint Descriptor error.\n");
                    break;
                default:
                    break;
                }
            }
            ofdsc += table[ofdsc];
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Endpoint Descriptor error.\n");
            return USB_ERROR;
            break;
        /* Antoher Descriptor */
        default:
            ofdsc += table[ofdsc];
            break;
        }
    }
    return USB_ERROR;
}   /* eof usb_hmsc_SmpPipeInfo() */

/******************************************************************************
Function Name   : usb_hmsc_NoData
Description     : HMSC No data
Arguments       : uint16_t drvnum : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoData(USB_UTR_t *ptr, uint16_t drvnum)
{
    USB_CLSINFO_t mess;
#ifdef FREE_RTOS_PP
    taskENTER_CRITICAL();
#endif

    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;
    usb_shmsc_Process[ptr->ip] = USB_MSG_HMSC_NO_DATA;
    usb_shmsc_NoDataSeq[ptr->ip] = USB_SEQ_0;
    mess.msginfo = usb_shmsc_Process[ptr->ip];
#ifdef FREE_RTOS_PP
    taskEXIT_CRITICAL();
#endif
    usb_hmsc_SpecifiedPath(&mess);
    return USB_DONE;
}   /* eof usb_hmsc_NoData() */

/******************************************************************************
Function Name   : usb_hmsc_NoDataAct
Description     : No Data Request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_NoDataAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    
    drvnum = usb_ghmsc_RootDrive[mess->ip];
    result = mess -> result;
    
    switch( usb_shmsc_NoDataSeq[mess->ip] )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive[mess->ip] = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq[mess->ip]++;
        mess->msginfo = usb_shmsc_Process[mess->ip];
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq[mess->ip]++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_NoDataSeq[mess->ip] = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### NoData : SendCbw error(drive:%d) \n", drvnum);
            usb_shmsc_NoDataSeq[mess->ip] = USB_SEQ_0;
            usb_hmsc_CommandResult(mess,hmsc_retval);
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);
            
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_NO_DATA;
            usb_shmsc_NoDataSeq[mess->ip]++;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_NO_DATA;
        usb_shmsc_NoDataSeq[mess->ip]++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum, result);

        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop[mess->ip] == USB_ON)
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** NoData : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop[mess->ip] = USB_ON;
            drvnum = usb_ghmsc_RootDrive[mess->ip];
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t *)usb_ghmsc_Data[mess->ip]);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** NoData : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop[mess->ip] == USB_ON)
            {
                usb_ghmsc_CswErrRoop[mess->ip]  =USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### NoData : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        usb_shmsc_NoDataSeq[mess->ip] = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess, hmsc_retval);
        usb_shmsc_NoDataSeq[mess->ip] = USB_SEQ_0;
        break;
    }
    return (hmsc_retval);
}   /* eof usb_hmsc_NoDataAct() */

/******************************************************************************
Function Name   : usb_hmsc_DataIn
Description     : HMSC Data In
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataIn(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_CLSINFO_t mess;
#ifdef FREE_RTOS_PP
    taskENTER_CRITICAL();
#endif
    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;

    usb_ghmsc_Buff[ptr->ip] = buff;
    usb_ghmsc_TransSize[ptr->ip] = size;

    usb_shmsc_Process[ptr->ip] = USB_MSG_HMSC_DATA_IN;
    usb_shmsc_DataInSeq[ptr->ip] = USB_SEQ_0;
    mess.msginfo = usb_shmsc_Process[ptr->ip];
#ifdef FREE_RTOS_PP
    taskEXIT_CRITICAL();
#endif
    usb_hmsc_SpecifiedPath(&mess);
    return USB_DONE;
}   /* eof usb_hmsc_DataIn() */

/******************************************************************************
Function Name   : usb_hmsc_DataInAct
Description     : Receive Data request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataInAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint32_t    size;
    uint8_t     *buff;

    drvnum = usb_ghmsc_RootDrive[mess->ip];
    buff = usb_ghmsc_Buff[mess->ip];
    size = usb_ghmsc_TransSize[mess->ip];
    result = mess -> result;

    switch( usb_shmsc_DataInSeq[mess->ip] )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive[mess->ip] = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq[mess->ip]++;
        mess->msginfo = usb_shmsc_Process[mess->ip];
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_DataInSeq[mess->ip]++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataInSeq[mess->ip] = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataIN : SendCBW error(drive:%d) \n", drvnum);
            usb_shmsc_DataInSeq[mess->ip] = USB_SEQ_0;
            usb_hmsc_CommandResult(mess, hmsc_retval);
        }
        else
        {
/* Data */
            hmsc_retval = usb_hmsc_GetData(mess, drvnum, buff, size);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq[mess->ip]++;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetDataReq(mess, drvnum, buff, size);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq[mess->ip]++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_GetDataCheck(mess, drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq[mess->ip]++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataIN : GetData error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            usb_shmsc_DataInSeq[mess->ip] = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);

            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
            usb_shmsc_DataInSeq[mess->ip]++;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_IN;
        usb_shmsc_DataInSeq[mess->ip]++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum, result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if(usb_ghmsc_CswErrRoop[mess->ip] == USB_ON)
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataIN : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop[mess->ip] = USB_ON;
            drvnum = usb_ghmsc_RootDrive[mess->ip];
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)&usb_ghmsc_Data[mess->ip]);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** DataIN : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if(usb_ghmsc_CswErrRoop[mess->ip] == USB_ON)
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataIN : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        
        usb_shmsc_DataInSeq[mess->ip] = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess, hmsc_retval);
        usb_shmsc_DataInSeq[mess->ip] = USB_SEQ_0;
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}   /* eof usb_hmsc_DataInAct() */

/******************************************************************************
Function Name   : usb_hmsc_DataOut
Description     : HMSC Data Out
Arguments       : uint16_t drvnum : 
                : uint8_t *buff : 
                : uint32_t size : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOut(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size)
{
    USB_CLSINFO_t mess;
#ifdef FREE_RTOS_PP
    taskENTER_CRITICAL();
#endif
    mess.ip = ptr->ip;
    mess.ipp = ptr->ipp;

    mess.keyword = drvnum;
    usb_ghmsc_Buff[ptr->ip] = buff;
    usb_ghmsc_TransSize[ptr->ip] = size;
    usb_shmsc_Process[ptr->ip] = USB_MSG_HMSC_DATA_OUT;
    usb_shmsc_DataOutSeq[ptr->ip] = USB_SEQ_0;
    mess.msginfo = usb_shmsc_Process[ptr->ip];
#ifdef FREE_RTOS_PP
    taskEXIT_CRITICAL();
#endif
    usb_hmsc_SpecifiedPath(&mess);
    return USB_DONE;
}   /* eof usb_hmsc_DataOut() */

/******************************************************************************
Function Name   : usb_hmsc_DataOutAct
Description     : Send Data request
Arguments       : USB_CLSINFO_t *mess : 
Return value    : uint16_t : 
******************************************************************************/
uint16_t usb_hmsc_DataOutAct(USB_CLSINFO_t *mess)
{
    uint16_t    hmsc_retval, result;
    uint16_t    drvnum;
    uint8_t     *buff;
    uint32_t    size;

    drvnum = usb_ghmsc_RootDrive[mess->ip];
    buff = usb_ghmsc_Buff[mess->ip];
    size = usb_ghmsc_TransSize[mess->ip];
    result = mess -> result;
    
    switch( usb_shmsc_DataOutSeq[mess->ip] )
    {
    case USB_SEQ_0:
        /* CBW */
        drvnum = mess -> keyword;
        usb_ghmsc_RootDrive[mess->ip] = drvnum;
        hmsc_retval = usb_hmsc_SendCbw(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq[mess->ip]++;
        mess->msginfo = usb_shmsc_Process[mess->ip];
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_1:
        hmsc_retval = usb_hmsc_SendCbwReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq[mess->ip]++;
        break;
    case USB_SEQ_2:
        hmsc_retval = usb_hmsc_SendCbwCheck(mess, drvnum, result);
        if( hmsc_retval == USB_DATA_STALL )
        {
            usb_shmsc_DataOutSeq[mess->ip] = USB_SEQ_0;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataOUT : SendCBW error(drive:%d) \n", drvnum);
            usb_shmsc_DataOutSeq[mess->ip] = USB_SEQ_0;
            usb_hmsc_CommandResult(mess, hmsc_retval);
        }
        else
        {
            /* Data */
            usb_hmsc_SendData(mess, drvnum, buff, size);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq[mess->ip]++;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_3:
        usb_hmsc_SendDataReq(mess, drvnum, buff, size);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq[mess->ip]++;
        break;
    case USB_SEQ_4:
        hmsc_retval = usb_hmsc_SendDataCheck(mess, drvnum, result);
        if( hmsc_retval == USB_HMSC_STALL )
        {
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq[mess->ip]++;
        }
        else if( hmsc_retval != USB_HMSC_OK )
        {
            USB_PRINTF1("### DataOUT : SendData error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            usb_shmsc_DataOutSeq[mess->ip] = USB_SEQ_0;
        }
        else
        {
            /* CSW */
            hmsc_retval = usb_hmsc_GetCsw(mess, drvnum);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
            usb_shmsc_DataOutSeq[mess->ip]++;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        }
        break;
    case USB_SEQ_5:
        hmsc_retval = usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_OUT;
        usb_shmsc_DataOutSeq[mess->ip]++;
        break;
    case USB_SEQ_6:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum,result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop[mess->ip] == USB_ON )
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataOUT : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop[mess->ip] = USB_ON;
            drvnum = usb_ghmsc_RootDrive[mess->ip];
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)usb_ghmsc_Data[mess->ip]);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            USB_PRINTF1("*** DataOUT : CSW-STALL(drive:%d) \n", drvnum);
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
            mess->keyword = drvnum;
            mess->msginfo = usb_shmsc_Process[mess->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            break;
        default:
            if( usb_ghmsc_CswErrRoop[mess->ip] == USB_ON )
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataOUT : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }

        usb_shmsc_DataOutSeq[mess->ip] = USB_SEQ_0;
        break;
    default:
        usb_shmsc_DataOutSeq[mess->ip] = USB_SEQ_0;
        usb_hmsc_CommandResult(mess, hmsc_retval);
        break;
    }
    /* Data read error */
    return (hmsc_retval);
}   /* eof usb_hmsc_DataOutAct() */

/******************************************************************************
Function Name   : usb_hmsc_SpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_SpecifiedPath(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *ptr;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSC_MPL, &p_blf) == USB_E_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = mess->msginfo;
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->result;

        ptr->ip         = mess->ip;
        ptr->ipp        = mess->ipp;

        /* Send message */
        err = R_USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
}   /* eof usb_hmsc_SpecifiedPath() */

/******************************************************************************
Function Name   : usb_hmsc_CheckResult
Description     : Hub class check result
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_CheckResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *ptr;

    ptr = mess;
    if( mess->status == USB_DATA_STALL )
    {
        ptr->msginfo    = usb_shmsc_Process[mess->ip];
    }
    
    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSC_MPL, &p_blf) == USB_E_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = usb_shmsc_Process[mess->ip];
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->status;

        ptr->ip         = mess->ip;
        ptr->ipp        = mess->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HMSC_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
    }
}   /* eof usb_hmsc_CheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_class_check_result
Description     : Hub class check result
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_class_check_result(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *ptr;

    ptr = mess;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSC_MPL, &p_blf) == USB_E_OK )
    {
        ptr = (USB_CLSINFO_t*)p_blf;
        ptr->msginfo    = USB_MSG_CLS_INIT;
        ptr->keyword    = mess->keyword;
        ptr->result     = mess->status;

        ptr->ip         = mess->ip;
        ptr->ipp        = mess->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HMSC_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HMSC_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### usb_hmsc_class_check_resultn snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### usb_hmsc_class_check_result pget_blk error\n");
    }
}   /* eof usb_hmsc_class_check_result() */

/******************************************************************************
Function Name   : usb_hmsc_StallErr
Description     : HMSC Stall Error
Arguments       : USB_UTR_t *ptr : 
Return value    : none
******************************************************************************/
void usb_hmsc_StallErr(USB_UTR_t *ptr)
{
    uint16_t    drvnum, msgnum, result;
    uint16_t    hub_addr,hub_port_no;
    USB_ER_t    err,err2;
    USB_MH_t    p_blf;
    USB_UTR_t   *cp;
    USB_UTR_t   devadr;

    drvnum = usb_ghmsc_RootDrive[ptr->ip];
    result = ptr->status;
    msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, drvnum);
    if( USB_ERROR == msgnum )
    {
        return;
    }

    switch( usb_shmsc_StallErrSeq[ptr->ip] )
    {
    case USB_SEQ_0:
        drvnum = ptr->keyword;
        /* Device address set */
        usb_hmsc_SmpDrive2Addr( drvnum, &devadr );

        /* Get connected Hub address from Device address. */
        hub_addr = R_usb_hhub_get_hub_addr(ptr, devadr.keyword);

        /* Get connected Hub port number from Device address. */
        hub_port_no = R_usb_hhub_get_hub_port_no(ptr, devadr.keyword);

        /* Transfer Get Port Status when get Hub address and connected Hub port no. */
        if( (hub_addr != USB_ERROR) && (hub_port_no != USB_ERROR) )
        {
            /* Get Port Status(GET_STATUS) */
            R_usb_hhub_GetPortInformation(ptr, hub_addr, hub_port_no, (USB_CB_t)usb_hmsc_CheckResult );
            usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_4;
        }
        else
        {
            usb_ghmsc_RootDrive[ptr->ip] = drvnum;

            err = R_usb_hmsc_MassStorageReset(ptr, drvnum, (USB_CB_t)usb_hmsc_CheckResult);
            /* Control Transfer overlaps */
            if( err == USB_E_QOVR )
            {
                /* Resend message */
                err = R_USB_PGET_BLK(USB_HMSC_MPL, &p_blf);
                if( err == USB_E_OK )
                {
                    cp = (USB_UTR_t*)p_blf;
                    cp->msginfo = ptr -> msginfo;
                    cp->keyword = ptr -> keyword;
                    cp->status  = ptr -> status;

                    cp->ip      = ptr->ip;
                    cp->ipp     = ptr->ipp;

                    /* Send message */
                    err = USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### StallErr snd_msg error (%ld)\n", err);
                        err2 = R_USB_REL_BLK(USB_HMSC_MPL, (USB_MH_t)p_blf);
                        if( err2 != USB_E_OK )
                        {
                            USB_PRINTF1("### StallErr rel_blk error (%ld)\n", err2);
                        }
                    }
                }
                else
                {
                    USB_PRINTF1("### StallErr pget_blk error (%ld)\n", err);
                }
            }
            else
            {
                /* Control Transfer not overlaps */
                usb_shmsc_StallErrSeq[ptr->ip]++;
            }
        }
        break;

    case USB_SEQ_1:
        usb_hmsc_MassStorageResetCheck(ptr, result);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_NONE, msgnum,
             usb_hmsc_CheckResult);
        usb_shmsc_StallErrSeq[ptr->ip]++;
        break;
    case USB_SEQ_2:
        usb_hmsc_ClearStallCheck(ptr, result);
        R_usb_hmsc_ClearStall(ptr, (uint16_t)USB_DATA_OK, msgnum,
             usb_hmsc_CheckResult);
        usb_shmsc_StallErrSeq[ptr->ip]++;
        break;
    case USB_SEQ_3:
        usb_hmsc_ClearStallCheck(ptr, result);
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult(ptr, result);
        usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_0;
        break;

    case USB_SEQ_4:
        drvnum = (uint16_t)ptr->usr_data;                               /* Device no. set */
        /* Device address set */
        usb_hmsc_SmpDrive2Addr( drvnum, &devadr );

        /* Get connected Hub address from Device address. */
        hub_addr = R_usb_hhub_get_hub_addr(ptr, devadr.keyword);

        /* Check device connect status for after transfer complete GET_STATUS(Get Port Status) */
        if( R_usb_hhub_chk_connect_status(ptr, hub_addr) == USB_DEV_NO_CONNECT )
        {
            if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
            {
                result = USB_HMSC_CSW_PHASE_ERR;
            }
            else
            {
                result = USB_HMSC_CBW_ERR;
            }
            usb_hmsc_CommandResult(ptr, result);
            usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_0;
        }
        else
        {
            usb_ghmsc_RootDrive[ptr->ip] = drvnum;

            err = R_usb_hmsc_MassStorageReset(ptr, drvnum, (USB_CB_t)usb_hmsc_CheckResult);
            /* Control Transfer overlaps */
            if( err == USB_E_QOVR )
            {
                /* Resend message */
                err = R_USB_PGET_BLK(USB_HMSC_MPL, &p_blf);
                if( err == USB_E_OK )
                {
                    cp = (USB_UTR_t*)p_blf;
                    cp->msginfo = ptr -> msginfo;
                    cp->keyword = ptr -> keyword;
                    cp->status  = ptr -> status;

                    cp->ip      = ptr->ip;
                    cp->ipp     = ptr->ipp;

                    /* Send message */
                    err = R_USB_SND_MSG(USB_HMSC_MBX, (USB_MSG_t*)p_blf);
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### StallErr snd_msg error (%ld)\n", err);
                        err2 = R_USB_REL_BLK(USB_HMSC_MPL, (USB_MH_t)p_blf);
                        if( err2 != USB_E_OK )
                        {
                            USB_PRINTF1("### StallErr rel_blk error (%ld)\n", err2);
                        }
                    }
                }
                else
                {
                    USB_PRINTF1("### StallErr pget_blk error (%ld)\n", err);
                }
            }
            else
            {
                /* Control Transfer not overlaps */
                usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_1;
            }
        }
        break;

    default:
        if( ptr->msginfo == USB_MSG_HMSC_CSW_PHASE_ERR )
        {
            result = USB_HMSC_CSW_PHASE_ERR;
        }
        else
        {
            result = USB_HMSC_CBW_ERR;
        }
        usb_hmsc_CommandResult(ptr, result);
        usb_shmsc_StallErrSeq[ptr->ip] = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_StallErr() */

/******************************************************************************
Function Name   : usb_hmsc_DataStall
Description     : HMSC Data Stall
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_DataStall(USB_UTR_t *mess)
{
    uint16_t result, status, drvnum, msgnum, hmsc_retval;
    USB_CLSINFO_t *ptr;

    drvnum = usb_ghmsc_RootDrive[mess->ip];
    ptr = (USB_CLSINFO_t*)mess;
    result = ptr->result;
    status = mess->status;

    msgnum = usb_hmsc_SmpDrive2Msgnum(mess, drvnum);
    if( USB_ERROR == msgnum )
    {
        return;
    }

    switch( usb_shmsc_DataStallSeq[mess->ip] )
    {
    case USB_SEQ_0:
        drvnum = mess->keyword;
        usb_ghmsc_RootDrive[mess->ip] = drvnum;

        R_usb_hmsc_ClearStall(mess, (uint16_t)USB_DATA_OK, msgnum, usb_hmsc_CheckResult);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq[mess->ip]++;
        break;
    case USB_SEQ_1:
        usb_hmsc_ClearStallCheck(mess, status);
        usb_hmsc_GetCsw(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
        mess->msginfo = usb_shmsc_Process[ptr->ip];
        usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
        usb_shmsc_DataStallSeq[mess->ip]++;
        break;
    case USB_SEQ_2:
        usb_hmsc_GetCswReq(mess, drvnum);
        usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_DATA_STALL;
        usb_shmsc_DataStallSeq[mess->ip]++;
        break;
    case USB_SEQ_3:
        hmsc_retval = usb_hmsc_GetCswCheck(mess, drvnum,result);
        switch( hmsc_retval )
        {
        case USB_HMSC_OK:
            if( usb_ghmsc_CswErrRoop[mess->ip] == USB_ON )
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        case USB_HMSC_CSW_ERR:
            USB_PRINTF1("*** DataOUT : CSW-NG(drive:%d) \n", drvnum);
            usb_ghmsc_CswErrRoop[mess->ip] = USB_ON;
            drvnum = usb_ghmsc_RootDrive[mess->ip];
            R_usb_hmsc_RequestSense(mess, drvnum, (uint8_t*)usb_ghmsc_Data[mess->ip]);
            break;
        case USB_MSG_HMSC_DATA_STALL:
            usb_shmsc_Process[mess->ip] = USB_MSG_HMSC_CSW_PHASE_ERR;
            mess->keyword = drvnum;
            mess->msginfo = usb_shmsc_Process[ptr->ip];
            usb_hmsc_SpecifiedPath((USB_CLSINFO_t *)mess);
            USB_PRINTF1("*** DataOUT : Phase error(drive:%d) \n", drvnum);
            break;
        default:
            if( usb_ghmsc_CswErrRoop[mess->ip] == USB_ON )
            {
                usb_ghmsc_CswErrRoop[mess->ip] = USB_OFF;
                hmsc_retval = USB_HMSC_CSW_ERR;
            }
            USB_PRINTF1("### DataOUT : GetCSW error(drive:%d) \n", drvnum);
            usb_hmsc_CommandResult(mess, hmsc_retval);
            break;
        }
        usb_shmsc_DataStallSeq[mess->ip] = USB_SEQ_0;
        break;
    default:
        usb_hmsc_CommandResult(mess,USB_HMSC_CSW_ERR);
        usb_shmsc_DataStallSeq[mess->ip] = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_DataStall() */

/******************************************************************************
Function Name   : usb_hmsc_CommandResult
Description     : Hub class check result
Arguments       : uint16_t result     : Result
Return value    : none
******************************************************************************/
void usb_hmsc_CommandResult(USB_UTR_t *ptr, uint16_t result)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess[ptr->ip];
        cp->result  = result;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
    }
}   /* eof usb_hmsc_CommandResult() */

/******************************************************************************
End  Of File
******************************************************************************/
