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
* File Name     : r_usb_hmsc_api.h
* Description   : USB Host MSC Driver API declaration
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

#ifndef __R_USB_HMSC_API_H__
#define __R_USB_HMSC_API_H__

/*****************************************************************************
Public Functions (API)
******************************************************************************/

uint16_t    R_usb_hmsc_DriveSpeed(USB_UTR_t *ptr, uint16_t side);
uint16_t    R_usb_hmsc_GetDevSts( uint16_t side );
uint16_t    R_usb_hmsc_Information(uint16_t ipno, uint16_t PipeOffset);
uint16_t    R_usb_hmsc_Inquiry(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSelect6(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ModeSense10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_PreventAllow(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_Read10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff,
                uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_ReadCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_ReadFormatCapacity(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_RequestSense(USB_UTR_t *ptr, uint16_t side, uint8_t *buff);
uint16_t    R_usb_hmsc_SetDevSts( uint16_t side, uint16_t data );
uint16_t    R_usb_hmsc_StrgDriveClose(USB_UTR_t *ptr, uint16_t side);
USB_ER_t    R_usb_hmsc_StrgDriveOpen(USB_UTR_t *ptr, uint16_t side, USB_CB_t complete );
uint16_t    R_usb_hmsc_StrgDriveSearch(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete);
uint16_t    R_usb_hmsc_StrgReadSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_StrgUserCommand(USB_UTR_t *ptr, uint16_t side, uint16_t command, uint8_t *buff, USB_CB_t complete);
uint16_t    R_usb_hmsc_StrgWriteSector(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
uint16_t    R_usb_hmsc_TestUnit(USB_UTR_t *ptr, uint16_t side);
uint16_t    R_usb_hmsc_Write10(USB_UTR_t *ptr, uint16_t side, uint8_t *buff
                , uint32_t secno, uint16_t seccnt, uint32_t trans_byte);
void        R_usb_hmsc_ClassCheck(USB_UTR_t *ptr, uint16_t **table);
void        R_usb_hmsc_DriveClose(USB_UTR_t *ptr, uint16_t addr, uint16_t data2);
void        R_usb_hmsc_Initialized(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        R_usb_hmsc_Release(USB_UTR_t *ptr);
void        R_usb_hmsc_StrgTaskClose(USB_UTR_t *ptr);
void        R_usb_hmsc_StrgTaskOpen(USB_UTR_t *ptr);
void        R_usb_hmsc_TaskClose(USB_UTR_t *ptr);
void        R_usb_hmsc_TaskOpen(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        R_usb_hmsc_driver_start( USB_UTR_t *ptr );

void        R_usb_hmsc_ClearStall(USB_UTR_t *ptr, uint16_t type, uint16_t msgnum, USB_CB_t complete);
USB_ER_t    R_usb_hmsc_MassStorageReset(USB_UTR_t *ptr, uint16_t drvnum, USB_CB_t complete);
USB_ER_t    R_usb_hmsc_GetMaxUnit(USB_UTR_t *ptr, uint16_t addr, USB_CB_t complete);

#endif  /* __R_USB_HMSC_API_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
