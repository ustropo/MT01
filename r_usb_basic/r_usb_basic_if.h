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
* File Name    : r_usb_basic_if.h
* Description  : Interface file for USB basic API for RX
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/

#ifndef _USB_BASIC_H
#define _USB_BASIC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Used to get which MCU is currently being used. */
#include "platform.h"
/* User specific options for Flash API */
#include "r_usb_basic_config.h"

#include "r_usb_ctypedef.h"
#include "r_usb_cdefusbip.h"
#include "r_usb_cextern.h"
#include "r_usb_ckernelid.h"
#include "r_usb_cmacprint.h"
#include "r_usb_cmacsystemcall.h"
#include "r_usb_cusb_bitdefine.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* Version Number of API. */
#define USB_VERSION_MAJOR               (0)
#define USB_VERSION_MINOR               (90)

/*****************************************************************************
Typedef definitions
******************************************************************************/
typedef enum e_usb_err_t
{
    USB_SUCCESS = 0,        
    USB_ERR_OPENED,         /* USB was initialized already */
    USB_ERR_NOT_OPEN,       /* USB module is not initialized yet */
    USB_ERR_INVALID_ARG,    /* Arguments are invalid */
    USB_ERR_NULL_PTR,       /* Argument pointers are NULL */
    USB_ERR_BUSY,           /* The USB resources is locked by another process */
} usb_err_t;

typedef enum e_usb_ip       /* USB IP type */
{
    USB_IP0 = 0,
    USB_IP1 = 1,
} usb_ip_t;

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
//uint8_t R_USBHS_Open(uint32_t channel);

uint32_t    R_USB_GetVersion(void);
usb_err_t   R_USB_Open( usb_ip_t ip_type );
usb_err_t   R_USB_Close( usb_ip_t ip_type );


/* USB API (Host) */
uint16_t    R_usb_hstd_allocatePipe(USB_UTR_t *ptr, uint16_t type);
void        R_usb_hstd_freePipe(USB_UTR_t *ptr, uint8_t pipeno);
USB_ER_t    R_usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint8_t addr, uint8_t cnt_value, USB_CB_t complete);
USB_ER_t    R_usb_hstd_MgrEnumSetConfiguration(USB_UTR_t *ptr, uint8_t devadr, uint8_t config_val, USB_CB_t complete);
USB_ER_t    R_usb_hstd_TransferStart(USB_UTR_t *utr_table);
USB_ER_t    R_usb_hstd_SetPipeRegistration(USB_UTR_t *ptr, uint16_t *table, uint16_t pipe);
USB_ER_t    R_usb_hstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
USB_ER_t    R_usb_hstd_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t member);
USB_ER_t    R_usb_hstd_MgrOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_hstd_MgrClose(void);
void        R_usb_hstd_DriverRegistration(USB_UTR_t *ptr, USB_HCDREG_t *callback);
void        R_usb_hstd_DriverRelease(USB_UTR_t *ptr, uint8_t devclass);
uint16_t    R_usb_hstd_ChkPipeInfo(uint16_t speed, uint16_t *EpTbl, uint8_t *Descriptor);
void        R_usb_hstd_SetPipeInfo(uint16_t *dst_ep_tbl, uint16_t *src_ep_tbl, uint16_t length);
void        R_usb_hstd_DeviceInformation(USB_UTR_t *ptr, uint16_t addr, uint16_t *tbl);
void        R_usb_hstd_ReturnEnuMGR(USB_UTR_t *ptr, uint16_t cls_result);
void        R_usb_hstd_EnuWait(USB_UTR_t *ptr, uint8_t taskID);
uint16_t    R_usb_hstd_DetachControl(uint16_t port);

USB_ER_t    R_usb_hstd_HcdOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_hstd_HcdClose(void);

/* USB API (Peripheral) */
uint16_t    R_usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        R_usb_pstd_ControlWrite(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        R_usb_pstd_ControlEnd(USB_UTR_t *ptr, uint16_t status);
USB_ER_t    R_usb_pstd_PcdOpen(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_PcdClose(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_TransferStart(USB_UTR_t *ptr);
USB_ER_t    R_usb_pstd_TransferEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status);
USB_ER_t    R_usb_pstd_ChangeDeviceState(USB_UTR_t *ptr, uint16_t state, uint16_t port_no, USB_CB_t complete);
void        R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl);
void        R_usb_pstd_DriverRegistration(USB_UTR_t *ptr, USB_PCDREG_t *callback);
void        R_usb_pstd_DriverRelease(void);
void        R_usb_pstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t PipeNo, uint16_t *tbl);
void        R_usb_pstd_SetPipeStall(USB_UTR_t *ptr, uint16_t pipeno);
void        R_usb_pstd_usbdriver_start( USB_UTR_t *ptr );

/* USB API (Other) */
void        R_usb_cstd_ClearHwFunction(USB_UTR_t *ptr);
void        R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode );
uint8_t     R_usb_cstd_CheckSchedule(void);
void        R_usb_ScheInit( void );
USB_REGADR_t    R_usb_cstd_GetUsbIpAdr( uint16_t ipno );
void        R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode );
void        R_usb_cstd_SetRegDvstctr0( USB_UTR_t *ptr, uint16_t val );
void        R_usb_cstd_SetRegPipeCtr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t val );
void        R_usb_cstd_SetBuf(USB_UTR_t *ptr, uint16_t pipe);



void        R_usb_hhub_Open(USB_UTR_t *ptr, uint16_t devaddr, uint16_t data2);
void        R_usb_hhub_Close(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t data2);
void        R_usb_hhub_Registration(USB_UTR_t *ptr, USB_HCDREG_t *callback);
USB_ER_t    R_usb_hhub_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t devaddr);
uint16_t    R_usb_hhub_GetHubInformation(USB_UTR_t *ptr, uint16_t hubaddr, USB_CB_t complete);
uint16_t    R_usb_hhub_GetPortInformation(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, USB_CB_t complete);

uint16_t    R_usb_hhub_get_hub_addr(USB_UTR_t *ptr, uint16_t devadr);
uint16_t    R_usb_hhub_get_hub_port_no(USB_UTR_t *ptr, uint16_t devadr);
uint16_t    R_usb_hhub_chk_connect_status(USB_UTR_t *ptr, uint16_t hub_adr);

void        R_usb_pstd_PcdTask(USB_VP_INT_t);
void        R_usb_hhub_Task(USB_VP_INT_t);
void        R_usb_hstd_MgrTask(USB_VP_INT_t);
void        R_usb_hstd_HcdTask(USB_VP_INT_t);
void        R_usb_hstd_HubRegistAll(USB_UTR_t *ptr);

/* for NonOS Scheduler */
USB_ER_t    R_usb_cstd_RecMsg( uint8_t id, USB_MSG_t** mess, USB_TM_t tm );
USB_ER_t    R_usb_cstd_SndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    R_usb_cstd_iSndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    R_usb_cstd_WaiMsg( uint8_t id, USB_MSG_t* mess, USB_TM_t tm );
USB_ER_t    R_usb_cstd_PgetBlk( uint8_t id, USB_UTR_t** blk );
USB_ER_t    R_usb_cstd_RelBlk( uint8_t id,  USB_UTR_t* blk );
void        R_usb_cstd_Scheduler(void);
void        R_usb_cstd_SetTaskPri(uint8_t tasknum, uint8_t pri);

void        R_usb_cstd_debug_hook(uint16_t error_code);

/******************************************************************************
Macro definitions (Debug hook)
******************************************************************************/
/* Error discrimination */
#define USB_DEBUG_HOOK_HWR      0x0100
#define USB_DEBUG_HOOK_HOST     0x0200
#define USB_DEBUG_HOOK_PERI     0x0400
#define USB_DEBUG_HOOK_STD      0x0800
#define USB_DEBUG_HOOK_CLASS    0x1000
#define USB_DEBUG_HOOK_APL      0x2000

/* Error Code */
#define USB_DEBUG_HOOK_CODE1    0x0001
#define USB_DEBUG_HOOK_CODE2    0x0002
#define USB_DEBUG_HOOK_CODE3    0x0003
#define USB_DEBUG_HOOK_CODE4    0x0004
#define USB_DEBUG_HOOK_CODE5    0x0005
#define USB_DEBUG_HOOK_CODE6    0x0006
#define USB_DEBUG_HOOK_CODE7    0x0007
#define USB_DEBUG_HOOK_CODE8    0x0008
#define USB_DEBUG_HOOK_CODE9    0x0009
#define USB_DEBUG_HOOK_CODE10   0x000A
#define USB_DEBUG_HOOK_CODE11   0x000B
#define USB_DEBUG_HOOK_CODE12   0x000C
#define USB_DEBUG_HOOK_CODE13   0x000D
#define USB_DEBUG_HOOK_CODE14   0x000E
#define USB_DEBUG_HOOK_CODE15   0x000F

#ifdef USB_DEBUG_HOOK_USE
  #define USB_DEBUG_HOOK(x)     R_usb_cstd_debug_hook(x)
#else
  #define USB_DEBUG_HOOK(x)
#endif

#endif /* _USB_BASIC_H */
