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
* File Name    : r_usb_hmsc_extern.h
* Description  : USB common uItron header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/


#ifndef __R_USB_HMSC_EXTERN_H__
#define __R_USB_HMSC_EXTERN_H__

#include "r_usb_hmsc_define.h"      /* USB  Mass Storage Class Header */

/*****************************************************************************
Public Valiables
******************************************************************************/
extern USB_UTR_t            usb_ghmsc_ControlData[USB_NUM_USBIP];
extern USB_UTR_t            usb_ghmsc_TransData[USB_NUM_USBIP][USB_MAXSTRAGE];     /* Send data transfer message */
extern USB_UTR_t            usb_ghmsc_ReceiveData[USB_NUM_USBIP][USB_MAXSTRAGE];   /* Receive data transfer message */
extern USB_MSC_CBW_t        usb_ghmsc_Cbw[USB_NUM_USBIP][USB_MAXSTRAGE];
extern USB_MSC_CSW_t        usb_ghmsc_Csw[USB_NUM_USBIP][USB_MAXSTRAGE];
extern DRIVE_MANAGEMENT_t   usb_ghmsc_drv_no_tbl[USB_DEVICENUM];                    /* Drive no. management table */
extern uint32_t             usb_ghmsc_CbwTagNo[USB_NUM_USBIP][USB_MAXSTRAGE];
extern uint32_t             usb_ghmsc_TransSize[];
extern uint16_t             usb_ghmsc_DefEpTbl[USB_NUM_USBIP][USB_EPL+1];
extern uint16_t             *usb_ghmsc_PipeTable[USB_NUM_USBIP];                    /* Pipe Table(DefEP) */
extern uint8_t              *usb_ghmsc_DeviceTable[USB_NUM_USBIP];
extern uint8_t              *usb_ghmsc_ConfigTable[USB_NUM_USBIP];
extern uint8_t              *usb_ghmsc_InterfaceTable[USB_NUM_USBIP];
extern const   uint16_t     usb_ghhub_TPL[];
extern uint16_t             usb_ghmsc_AttSts[USB_MAXSTRAGE];
extern uint16_t             usb_ghmsc_DriveChk[USB_MAXDRIVE + 1][6];
extern uint16_t             usb_ghmsc_InPipe[USB_NUM_USBIP][USB_MAXSTRAGE][2];
extern uint16_t             usb_ghmsc_MaxDrive;
extern uint16_t             usb_ghmsc_OutPipe[USB_NUM_USBIP][USB_MAXSTRAGE][2];
extern uint16_t             usb_shmsc_StallErrSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_Process[USB_NUM_USBIP];
extern uint16_t             usb_ghhub_DefEPTbl[];
extern uint16_t             usb_ghmsc_Devaddr[USB_NUM_USBIP];
extern uint16_t             usb_ghmsc_Speed[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_InitSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgProcess[];
extern uint16_t             usb_ghmsc_StrgCount;
extern uint16_t             usb_shmsc_MsgNum[USB_NUM_USBIP];
extern uint8_t              usb_ghmsc_Data[USB_NUM_USBIP][5120];
extern uint8_t              usb_ghmsc_ClassData[USB_NUM_USBIP][USB_HMSC_CLSDATASIZE];
extern uint16_t             usb_ghmsc_drive_no[USB_NUM_USBIP][USB_MAXDEVADDR];

/*----------------*/
/* Storage Driver */
/*----------------*/
extern USB_UTR_t            usb_shmsc_ClassControl[USB_NUM_USBIP];
extern USB_CB_t             usb_shmsc_command_result[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_ClassRequest[USB_NUM_USBIP][5];
extern uint8_t              usb_shmsc_DeviceReady[USB_MAXUNITNUM];
extern uint8_t              usb_ghmsc_ClassData[USB_NUM_USBIP][USB_HMSC_CLSDATASIZE];
extern uint32_t             usb_ghmsc_MaxLUN[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgProcess[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveSearchSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveSearchErrCount[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveSearchCount[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_DevReadSectorSizeSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_DevReadSectorSizeErrCount[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_DevReadPartitionSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveOpenSeq[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveOpenCount[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_StrgDriveOpenParCount[USB_NUM_USBIP];
extern uint32_t             usb_shmsc_PartitionLba[USB_NUM_USBIP][USB_BOOTPARTNUM + 1u];
extern uint16_t             usb_ghmsc_RootDevaddr[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_NewDrive[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_LoopCont[USB_NUM_USBIP];
extern uint16_t             usb_shmsc_Unit[USB_NUM_USBIP];
extern uint16_t             usb_ghmsc_PartTransSize[USB_NUM_USBIP];
extern uint8_t              usb_shmsc_PartitionInfo[USB_NUM_USBIP][USB_BOOTPARTNUM];

/*****************************************************************************
Public Functions
******************************************************************************/
uint16_t    usb_hmsc_CheckCsw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_DataIn(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_DataOut(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetCsw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_GetData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetMaxUnitCheck(USB_UTR_t *ptr, uint16_t Err);
uint16_t    usb_hmsc_NoData(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_SendCbw(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_SendData(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_SmpBotDescriptor(USB_UTR_t *ptr, uint8_t *Table, uint16_t msgnum);
uint16_t    usb_hmsc_SmpDevCheckBootRecord(uint8_t *Data, uint32_t *ParLBA, uint8_t *ParInfo, uint16_t flag);
uint16_t    usb_hmsc_SmpDevNextDriveSearch(USB_UTR_t *ptr);
uint16_t    usb_hmsc_SmpDevReadPartition(USB_UTR_t *ptr, uint16_t unit, uint32_t trans_byte);
void        usb_hmsc_SmpDrive2Addr(uint16_t side, USB_UTR_t *devadr);
uint16_t    usb_hmsc_SmpDrive2Msgnum(USB_UTR_t *ptr, uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Part(USB_UTR_t *ptr, uint16_t side);
uint16_t    usb_hmsc_SmpDrive2Unit(USB_UTR_t *ptr, uint16_t side);
uint16_t    usb_hmsc_SmpFsiFileSystemInitialized(uint16_t side, uint8_t *Data, uint32_t Offset);
uint32_t    usb_hmsc_SmpFsiOffsetSectorRead(uint16_t side);
uint16_t    usb_hmsc_SmpPipeInfo(USB_UTR_t *ptr, uint8_t *table, uint16_t msgnum, uint16_t speed, uint16_t length);
uint16_t    usb_hmsc_SmpTotalDrive(void);
USB_ER_t    usb_hmsc_Submitutr(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table);
USB_ER_t    usb_hmsc_SubmitutrReq(USB_UTR_t *ptr, uint16_t type, USB_UTR_t *utr_table);
void        usb_hmsc_CbwTagCount(USB_UTR_t *ptr, uint16_t msgnum);
void        usb_hmsc_ClassWait(USB_ID_t id, USB_UTR_t *mess);
void        usb_hmsc_ClrData(uint16_t len, uint8_t *buf);
void        usb_hmsc_DoSqtgl(USB_UTR_t *ptr, uint16_t Pipe, uint16_t toggle);
void        usb_hmsc_SetElsCbw(USB_UTR_t *ptr, uint8_t *data, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SetRwCbw(USB_UTR_t *ptr, uint16_t command, uint32_t secno, uint16_t seccnt, uint32_t trans_byte, uint16_t side);
void        usb_hmsc_SmpFsiDriveClear(USB_UTR_t *ptr, uint16_t addr);
void        usb_hmsc_SmpFsiSectorInitialized(uint16_t side, uint32_t offset, uint16_t size);
uint16_t    usb_hmsc_GetStringInfo(USB_UTR_t *ptr, uint16_t devaddr, uint8_t *table);
void        usb_hmsc_ControlEnd(USB_UTR_t *ptr, uint16_t sts);
USB_ER_t    usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t Pipe, USB_CB_t complete);
uint16_t    usb_hmsc_DataInAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_DataOutAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_GetCswCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetCswReq(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_GetDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_GetDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_GetStringInfoCheck(USB_UTR_t *ptr, uint16_t devaddr);
uint16_t    usb_hmsc_MassStorageResetCheck(USB_UTR_t *ptr, uint16_t Err);
uint16_t    usb_hmsc_NoDataAct(USB_CLSINFO_t *mess);
uint16_t    usb_hmsc_SendCbwCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendCbwReq(USB_UTR_t *ptr, uint16_t drvnum);
uint16_t    usb_hmsc_SendDataCheck(USB_UTR_t *ptr, uint16_t drvnum, uint16_t hmsc_retval);
uint16_t    usb_hmsc_SendDataReq(USB_UTR_t *ptr, uint16_t drvnum, uint8_t *buff, uint32_t size);
uint16_t    usb_hmsc_SmpDevReadPartitionAct(USB_CLSINFO_t *mess);
void        usb_hmsc_CheckResult(USB_UTR_t *mess, uint16_t, uint16_t);
void        usb_hmsc_class_check_result(USB_UTR_t *mess, uint16_t, uint16_t);
void        usb_hmsc_ClassCheck(USB_UTR_t *ptr, USB_CLSINFO_t *mess);
void        usb_hmsc_ClearStallCheck(USB_UTR_t *ptr, uint16_t errcheck);
void        usb_hmsc_ClearStallCheck2(USB_UTR_t *mess);
void        usb_hmsc_CommandResult(USB_UTR_t *ptr, uint16_t result);
void        usb_hmsc_DataStall(USB_UTR_t *mess);
void        usb_hmsc_SpecifiedPath(USB_CLSINFO_t *mess);
void        usb_hmsc_StallErr(USB_UTR_t *mess);
void        usb_hmsc_strg_user_command_result(USB_CLSINFO_t *mess);
void        usb_hmsc_Task(void);
uint16_t    usb_hmsc_GetStringDescriptor(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index);
uint16_t    usb_hmsc_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t    usb_hmsc_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t    usb_hmsc_GetStringDescriptor1Check(USB_UTR_t *ptr, uint16_t errcheck);
uint16_t    usb_hmsc_GetStringDescriptor2Check(USB_UTR_t *ptr, uint16_t errcheck);
uint16_t    usb_hmsc_StdReqCheck(uint16_t errcheck);
uint16_t    usb_hmsc_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete);
uint16_t    usb_hmsc_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete);
void        usb_hmsc_StrgCheckResult(USB_UTR_t *mess);
void        usb_hmsc_StrgSpecifiedPath(USB_CLSINFO_t *mess);
void        usb_hmsc_StrgDriveSearchAct(USB_CLSINFO_t *mess);
void        usb_hmsc_SmpStrgDriveOpenAct( USB_CLSINFO_t *mess );

#endif  /* __R_USB_HMSC_EXTERN_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
