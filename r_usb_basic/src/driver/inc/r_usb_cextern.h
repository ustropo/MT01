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
* File Name    : r_usb_cextern.h
* Description  : USB common extern header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

/* $Id: r_usb_cextern.h 162 2012-05-21 10:20:32Z ssaek $ */

#ifndef __R_USB_CEXTERN_H__
#define __R_USB_CEXTERN_H__


/*****************************************************************************
Public Variables
******************************************************************************/
/* r_usb_cDataIO.c */
extern uint32_t         usb_gcstd_DataCnt[][USB_MAX_PIPE_NO + 1u];  /* PIPEn Buffer counter */
extern uint16_t         usb_gcstd_Dma0Dir[];                /* DMA0 direction */
extern uint32_t         usb_gcstd_Dma0Size[];               /* DMA0 buffer size */
extern uint16_t         usb_gcstd_Dma0Fifo[];               /* DMA0 FIFO buffer size */
extern uint16_t         usb_gcstd_Dma0Pipe[];               /* DMA0 pipe number */
extern uint8_t          *usb_gcstd_DataPtr[][USB_MAX_PIPE_NO + 1u]; /* PIPEn Buffer pointer(8bit) */
extern uint16_t         usb_ghstd_IgnoreCnt[][USB_MAX_PIPE_NO + 1u];    /* Ignore count */
extern USB_UTR_t        *usb_gcstd_Pipe[][USB_MAX_PIPE_NO + 1u];    /* Message pipe */
extern uint16_t         usb_gcstd_XckeMode;                 /* XCKE Mode Flag */
extern uint16_t         usb_gcstd_HsEnable[];               /* Hi-speed enable */


/* r_usb_hDriver.c */
extern USB_HCDREG_t     usb_ghstd_DeviceDrv[][USB_MAXDEVADDR + 1u]; /* Device driver (registration) */
extern uint16_t         usb_ghstd_DeviceInfo[][USB_MAXDEVADDR + 1u][8u];
                                                            /* port status, config num, interface class, speed, */
extern uint16_t         usb_ghstd_RemortPort[];
extern uint16_t         usb_ghstd_Ctsq[];                   /* Control transfer stage management */
extern uint16_t         usb_ghstd_MgrMode[][2u];            /* Manager mode */
extern uint16_t         usb_ghstd_DcpRegister[][USB_MAXDEVADDR + 1u];   /* DEVSEL & DCPMAXP (Multiple device) */
extern uint16_t         usb_ghstd_DeviceAddr[];         /* Device address */
extern uint16_t         usb_ghstd_DeviceSpeed[];            /* Reset handshake result */
extern uint16_t         usb_ghstd_DeviceNum[];          /* Device driver number */


/* r_usb_hManager.c */
extern uint16_t         usb_ghstd_EnumSeq[];                /* Enumeration request */
extern uint16_t         usb_ghstd_DeviceDescriptor[][USB_DEVICESIZE / 2u];
extern uint16_t         usb_ghstd_ConfigurationDescriptor[][USB_CONFIGSIZE / 2u];
extern uint16_t         usb_ghstd_SuspendPipe[][USB_MAX_PIPE_NO + 1u];
extern uint8_t          usb_ghstd_EnuWait[];                /* Class check TaskID */
extern uint16_t         usb_ghstd_CheckEnuResult[];     /* Enumeration result check */

/* r_usb_pDriver.c */
extern uint16_t         usb_gpstd_StallPipe[USB_MAX_PIPE_NO + 1u];  /* Stall Pipe info */
extern USB_CB_t         usb_gpstd_StallCB;                  /* Stall Callback function */
extern uint16_t         usb_gpstd_ConfigNum;                /* Configuration Number */
extern uint16_t         usb_gpstd_AltNum[];                 /* Alternate */
extern uint16_t         usb_gpstd_RemoteWakeup;             /* Remote Wakeup Enable Flag */
extern uint16_t         usb_gpstd_TestModeSelect;           /* Test Mode Selectors */
extern uint16_t         usb_gpstd_TestModeFlag;             /* Test Mode Flag */
extern uint16_t         usb_gpstd_EpTblIndex[2][USB_MAX_EP_NO + 1u];    /* Index of Endpoint Information table */
extern uint16_t         usb_gpstd_ReqType;                  /* Request type */
extern uint16_t         usb_gpstd_ReqTypeType;              /* Request type TYPE */
extern uint16_t         usb_gpstd_ReqTypeRecip;             /* Request type RECIPIENT */
extern uint16_t         usb_gpstd_ReqRequest;               /* Request */
extern uint16_t         usb_gpstd_ReqValue;                 /* Value */
extern uint16_t         usb_gpstd_ReqIndex;                 /* Index */
extern uint16_t         usb_gpstd_ReqLength;                /* Length */
extern uint16_t         usb_gpstd_intsts0;                  /* INTSTS0 */
extern USB_PCDREG_t     usb_gpstd_Driver;                   /* Driver registration */
extern USB_REQUEST_t    usb_gpstd_ReqReg;                   /* Request variable */


/* r_usb_creg_abs.c */
extern uint16_t         usb_gcstd_RhstBit;
extern uint16_t         usb_gcstd_DvsqBit;
extern uint16_t         usb_gcstd_AddrBit;
extern uint16_t         usb_gcstd_SqmonBit;


//extern uint16_t           usb_gcstd_PcutMode[2u];             /* PCUT Mode Flag */



#ifdef USB_HOST_BC_ENABLE
extern usb_bc_status_t g_usb_hstd_bc[2u];
extern void            (*usb_hstd_bc_func[USB_BC_STATE_MAX][USB_BC_EVENT_MAX])(USB_UTR_t *ptr, uint16_t port);
#endif /* USB_HOST_BC_ENABLE */


#ifdef USB_PERI_BC_ENABLE
extern uint16_t        g_usb_bc_detect;
#endif /* USB_PERI_BC_ENABLE */



/*****************************************************************************
Public Functions
******************************************************************************/
/* main.c */

/* r_usb_cIntHandler.c */
void        usb_cstd_InitUsbMessage(USB_UTR_t *ptr, uint16_t function);
void        usb_cstd_DmaHandler(void);
void        usb_cstd_UsbHandler(void);


/* r_usb2_cIntHandler.c */
void        usb2_cstd_DmaHandler(void);
void        usb2_cstd_UsbHandler(void);


/* r_usb_cDataIO.c */
void        usb_cstd_SendStart(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_Buf2Fifo(USB_UTR_t *ptr, uint16_t Pipe, uint16_t useport);
uint16_t    usb_cstd_write_data(USB_UTR_t *, uint16_t, uint16_t);
void        usb_cstd_ReceiveStart(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_Fifo2Buf(USB_UTR_t *ptr, uint16_t Pipe, uint16_t useport);
uint16_t    usb_cstd_read_data(USB_UTR_t *, uint16_t, uint16_t);
void        usb_cstd_DataEnd(USB_UTR_t *ptr, uint16_t Pipe, uint16_t Status);
void        usb_cstd_ForcedTermination(USB_UTR_t *ptr, uint16_t Pipe, uint16_t Status);


/* r_usb_cIntFIFO.c */
void        usb_cstd_BrdyPipe(USB_UTR_t *ptr, uint16_t bitsts);
void        usb_cstd_NrdyPipe(USB_UTR_t *ptr, uint16_t bitsts);
void        usb_cstd_BempPipe(USB_UTR_t *ptr, uint16_t bitsts);


/* r_usb_cScheduler.c */
USB_ER_t    usb_cstd_RecMsg( uint8_t id, USB_MSG_t** mess, USB_TM_t tm );
USB_ER_t    usb_cstd_SndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    usb_cstd_iSndMsg( uint8_t id, USB_MSG_t* mess );
USB_ER_t    usb_cstd_WaiMsg( uint8_t id, USB_MSG_t* mess, USB_TM_t times );
void        usb_cstd_WaitScheduler(void);
USB_ER_t    usb_cstd_PgetBlk( uint8_t id, USB_UTR_t** blk );
USB_ER_t    usb_cstd_RelBlk( uint8_t id,  USB_UTR_t* blk );
void        usb_cstd_ScheInit(void);


/* r_usb_cStdApi.c */
void        usb_cstd_set_usbip_mode(USB_UTR_t *ptr, uint16_t function);


/* r_usb_hControlRW */
uint16_t    usb_hstd_ControlWriteStart(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table);
void        usb_hstd_ControlReadStart(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table);
void        usb_hstd_StatusStart(USB_UTR_t *ptr);
void        usb_hstd_ControlEnd(USB_UTR_t *ptr, uint16_t Status);
void        usb_hstd_SetupStart(USB_UTR_t *ptr);


/* r_usb_hDriver.c */
uint16_t    usb_hstd_get_device_state(USB_UTR_t *ptr, uint16_t devaddr);
uint8_t     *usb_hstd_DevDescriptor(USB_UTR_t *ptr);
uint8_t     *usb_hstd_ConDescriptor(USB_UTR_t *ptr);
uint16_t    usb_hstd_HsFsResult(USB_UTR_t *ptr);
void        usb_hstd_DeviceResume(USB_UTR_t *ptr, uint16_t devaddr);
USB_ER_t    usb_hstd_HcdSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t *adr, USB_CB_t callback);
void        usb_hstd_MgrSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t dat, uint16_t res);
void        usb_hstd_HcdTask(USB_VP_INT_t);
USB_ER_t    usb_hstd_ChangeDeviceState(USB_UTR_t *ptr, USB_CB_t complete, uint16_t msginfo, uint16_t member);
USB_ER_t    usb_hstd_TransferStart(USB_UTR_t *ptr);


USB_ER_t    usb_hstd_ClearStall(USB_UTR_t *ptr, uint16_t pipe, USB_CB_t complete);
USB_ER_t    usb_hstd_ClearFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete);
uint16_t    usb_hstd_GetStringDescriptor1Check(uint16_t errcheck);
void        usb_hstd_Suspend(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_Interrupt(USB_UTR_t *p);
void        usb_hstd_BusIntDisable(USB_UTR_t *ptr, uint16_t port);


/* r_usb_hIntFIFO.c */
void        usb_hstd_BrdyPipe(USB_UTR_t *ptr);
void        usb_hstd_NrdyPipe(USB_UTR_t *ptr);
void        usb_hstd_BempPipe(USB_UTR_t *ptr);


/* r_usb_hManager.c */
void        usb_hstd_NotifAtorDetach(USB_UTR_t *ptr, uint16_t result, uint16_t port);
void        usb_hstd_OvcrNotifiation(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_StatusResult(USB_UTR_t *ptr, uint16_t port, uint16_t result);
void        usb_hstd_EnumGetDescriptorAnsi(USB_UTR_t *ptr, uint8_t addr, uint8_t cnt_value);
void        usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint16_t addr, uint16_t cnt_value);
void        usb_hstd_EnumSetAddress(USB_UTR_t *ptr, uint16_t addr, uint16_t setaddr);
void        usb_hstd_EnumSetConfiguration(USB_UTR_t *ptr, uint16_t addr, uint16_t confnum);
void        usb_hstd_EnumDummyRequest(USB_UTR_t *ptr, uint16_t addr, uint16_t CntValue);
void        usb_hstd_ElectricalTestMode(USB_UTR_t *ptr, uint16_t product_id, uint16_t port);
void        usb_hstd_MgrTask(USB_VP_INT_t);
uint16_t    usb_hstd_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete);


/* r_usb_hStdFunction.c */
void        usb_hstd_Bchg0Function(USB_UTR_t *ptr);
void        usb_hstd_LsConnectFunction(USB_UTR_t *ptr);
void        usb_hstd_AttachFunction(void);
uint16_t    usb_hstd_EnumFunction1(void);
uint16_t    usb_hstd_EnumFunction2(uint16_t* enummode);
void        usb_hstd_EnumFunction3(USB_UTR_t *ptr, uint16_t devaddr, uint16_t enum_seq);
void        usb_hstd_EnumFunction4(uint16_t* reqnum, uint16_t* enummode, uint16_t devaddr);
void        usb_hstd_EnumFunction5(void);


/* r_usb_pDriver.c */
USB_ER_t    usb_pstd_PcdSndMbx(USB_UTR_t *ptr, uint16_t msginfo, uint16_t keyword, USB_CB_t complete);
void        usb_pstd_PcdRelMpl(uint16_t);
void        usb_pstd_PcdTask(USB_VP_INT_t);
void        usb_pstd_Interrupt(USB_UTR_t *ptr);
void        usb_pstd_ClearMem(void);
void        usb_pstd_SetConfigNum(uint16_t Value);
void        usb_pstd_ClearEpTblIndex(void);
uint16_t    usb_pstd_GetConfigNum(void);
uint16_t    usb_pstd_GetInterfaceNum(uint16_t Con_Num);
uint16_t    usb_pstd_GetAlternateNum(uint16_t Con_Num, uint16_t Int_Num);
void        usb_pstd_SetEpTblIndex(uint16_t Con_Num, uint16_t Int_Num, uint16_t Alt_Num);
uint16_t    usb_pstd_ChkRemote(void);
uint8_t     usb_pstd_GetCurrentPower(void);
USB_ER_t    R_usb_pstd_SetStall(USB_UTR_t *ptr, USB_CB_t complete, uint16_t pipe);


/* r_usb_pIntFIFO.c */
void        usb_pstd_BrdyPipe(USB_UTR_t *ptr, uint16_t bitsts);
void        usb_pstd_NrdyPipe(USB_UTR_t *ptr, uint16_t bitsts);
void        usb_pstd_BempPipe(USB_UTR_t *ptr, uint16_t bitsts);


/* r_usb_pStdFunction.c */
void        usb_pstd_AttachFunction(USB_UTR_t *ptr);
void        usb_pstd_BusresetFunction(USB_UTR_t *ptr);
void        usb_pstd_SuspendFunction(USB_UTR_t *ptr);

/* r_usb_pStdRequest.c */
void        usb_pstd_StandReq0(USB_UTR_t *ptr);
void        usb_pstd_StandReq1(USB_UTR_t *ptr);
void        usb_pstd_StandReq2(USB_UTR_t *ptr);
void        usb_pstd_StandReq3(USB_UTR_t *ptr);
void        usb_pstd_StandReq4(USB_UTR_t *ptr);
void        usb_pstd_StandReq5(USB_UTR_t *ptr);
void        usb_pstd_SetFeatureFunction(USB_UTR_t *ptr);


/* r_usb_smp_cSub.c */
void        usb_cstd_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        usb_cstd_DummyTrn(USB_UTR_t *ptr, USB_REQUEST_t *data1, uint16_t data2);
void        usb_cstd_ClassProcessResult(USB_UTR_t *ptr, uint16_t data,uint16_t dummy);
void        usb_cstd_ClassTransResult(USB_UTR_t *mess, uint16_t, uint16_t);


/* r_usb_smp_hSub.c */
uint16_t    usb_hstd_CheckDescriptor(uint8_t *table, uint16_t spec);
uint16_t    usb_hstd_GetConfigDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t length, USB_CB_t complete);
uint16_t    usb_hstd_SetFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete);
uint16_t    usb_hstd_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t    usb_hstd_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t    usb_hstd_GetStringDescriptor2Check(uint16_t errcheck);
uint16_t    usb_hstd_StdReqCheck(uint16_t errcheck);

/* r_usb_hHubsys.c, r_usb_hHubsys_uitron.c */
void        usb_hhub_Initial(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void        usb_hhub_ChkClass(USB_UTR_t *ptr, uint16_t **table);
USB_ER_t    usb_hhub_ChangeState(USB_UTR_t *ptr, uint16_t devaddr, uint16_t msginfo, USB_CB_t callback);

/* r_usb_creg_abs.c */
USB_REGADR_t    usb_cstd_GetUsbIpAdr( uint16_t ipno );

uint32_t    usb_cstd_GetD0fifo32Adr( USB_UTR_t *ptr );
uint32_t    usb_cstd_GetD0fifo16Adr( USB_UTR_t *ptr );
uint32_t    usb_cstd_GetD0fifo8Adr( USB_UTR_t *ptr );
void        usb_cstd_AsspConfig(USB_UTR_t *ptr);
void        usb_cstd_Pinconfig(USB_UTR_t *ptr);
void        usb_cstd_InitialClock(USB_UTR_t *ptr);
void        usb_cstd_InterruptClock(USB_UTR_t *ptr);
void        usb_cstd_SelfClock(USB_UTR_t *ptr);
void        usb_cstd_StopClock(USB_UTR_t *ptr);
void        usb_cstd_NrdyEnable(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_BerneEnable(USB_UTR_t *ptr);
void        usb_cstd_SwReset(USB_UTR_t *ptr);
uint16_t    usb_cstd_GetPid(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_ClearIntEnb( USB_UTR_t *ptr );
void        usb_cstd_ClearIntSts( USB_UTR_t *ptr );
void        usb_cstd_ClearDline( USB_UTR_t *ptr );
uint16_t    usb_cstd_PortSpeed(USB_UTR_t *ptr, uint16_t port);
uint16_t    usb_cstd_HiSpeedEnable(USB_UTR_t *ptr, uint16_t port);
void        usb_cstd_SetHse(USB_UTR_t *ptr, uint16_t port, uint16_t speed);
void        usb_cstd_DoSqtgl(USB_UTR_t *ptr, uint16_t Pipe, uint16_t toggle);
uint16_t    usb_cstd_GetBufSize(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_GetMaxPacketSize(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_GetDevsel(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_GetPipeDir(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_GetPipeType(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_pipe_init(USB_UTR_t *ptr, uint16_t pipe, uint16_t *tbl, uint16_t ofs);
void        usb_cstd_ClrPipeCnfg(USB_UTR_t *ptr, uint16_t PipeNo);
uint16_t    usb_cstd_is_host_mode(USB_UTR_t *ptr);
void        usb_cstd_set_usbip_mode_sub(USB_UTR_t *ptr, uint16_t function);
void        usb_cstd_WaitUsbip(USB_UTR_t *ptr);
void        usb_cstd_DoAclrm(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_SetBuf(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_SetNak(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_cstd_ClrStall(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_Epadr2Pipe(USB_UTR_t *ptr, uint16_t Dir_Ep);
uint8_t     usb_cstd_Pipe2Epadr(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_Pipe2Fport(USB_UTR_t *ptr, uint16_t Pipe);
uint16_t    usb_cstd_GetDeviceAddress(USB_UTR_t *ptr, uint16_t Pipe);
uint8_t     *usb_cstd_write_fifo( USB_UTR_t *, uint16_t, uint16_t, uint8_t * );
uint8_t     *usb_cstd_read_fifo( USB_UTR_t *, uint16_t, uint16_t, uint8_t * );
uint16_t    usb_cstd_is_set_frdy(USB_UTR_t *ptr, uint16_t Pipe, uint16_t fifosel, uint16_t isel);
void        usb_cstd_chg_curpipe(USB_UTR_t *ptr, uint16_t Pipe, uint16_t fifosel, uint16_t isel);
void        usb_cstd_SetTransactionCounter(USB_UTR_t *ptr, uint16_t trnreg, uint16_t trncnt);
void        usb_cstd_ClrTransactionCounter(USB_UTR_t *ptr, uint16_t trnreg);
void        usb_cstd_set_sofcfg_intl( USB_UTR_t *ptr );



/* r_usb_creg_dmadtc.c */
void        usb_cstd_D0fifoStopUsb(USB_UTR_t *ptr);
void        usb_cstd_D0fifoInt(USB_UTR_t *ptr);
#ifdef USB_DTC_ENABLE
void        usb_cstd_Buf2fifoStartDma( USB_UTR_t *, uint16_t, uint16_t );
void        usb_cstd_Fifo2BufStartDma( USB_UTR_t *, uint16_t, uint16_t, uint32_t );
#endif    /* USB_DTC_ENABLE */



/* r_usb_hostelectrical.c */
void        usb_hstd_TestStop(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_TestSignal(USB_UTR_t *ptr, uint16_t port, uint16_t command);
void        usb_hstd_TestUactControl(USB_UTR_t *ptr, uint16_t port, uint16_t command);
void        usb_hstd_TestVbusControl(USB_UTR_t *ptr, uint16_t port, uint16_t command);
void        usb_hstd_TestBusReset(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_TestSuspend(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_TestResume(USB_UTR_t *ptr, uint16_t port);


/* r_usb_hreg_abs */
void        usb_hstd_SetDevAddr(USB_UTR_t *ptr, uint16_t addr, uint16_t speed, uint16_t port);
void        usb_hstd_SetHubPort(USB_UTR_t *ptr, uint16_t addr, uint16_t upphub, uint16_t hubport);
void        usb_hstd_BchgEnable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_BchgDisable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_SetUact(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_OvrcrEnable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_OvrcrDisable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_AttchEnable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_AttchDisable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_DtchEnable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_DtchDisable(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_InterruptHandler(USB_UTR_t *ptr);
void        usb_hstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t PipeNo, uint16_t *tbl);
uint16_t    usb_hstd_GetRootport(USB_UTR_t *ptr, uint16_t addr);
uint16_t    usb_hstd_ChkDevAddr(USB_UTR_t *ptr, uint16_t addr, uint16_t rootport);
uint16_t    usb_hstd_GetDevSpeed(USB_UTR_t *ptr, uint16_t addr);
void        usb_hstd_VbusControl(USB_UTR_t *ptr, uint16_t port, uint16_t command);
void        usb_hstd_SuspendProcess(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_Attach(USB_UTR_t *ptr, uint16_t result, uint16_t port);
void        usb_hstd_Detach(USB_UTR_t *ptr, uint16_t port);
uint16_t    usb_hstd_InitConnect(USB_UTR_t *ptr, uint16_t port, uint16_t else_connect_inf );

uint16_t    usb_hstd_ChkAttach(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_ChkClk(USB_UTR_t *ptr, uint16_t port, uint16_t event);
void        usb_hstd_ChkClk2(USB_UTR_t *ptr, uint16_t else_connect_inf );
void        usb_hstd_DetachProcess(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_ReadLnst(USB_UTR_t *ptr, uint16_t port, uint16_t *buf);
void        usb_hstd_AttachProcess(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_ChkSof(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_BusReset(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_ResumeProcess(USB_UTR_t *ptr, uint16_t port);
void        usb_hstd_Ovrcr0Function(USB_UTR_t *ptr);
uint16_t    usb_hstd_support_speed_check( USB_UTR_t *ptr, uint16_t port );


/* r_usb_preg_abs.c */
void        usb_pstd_InterruptHandler(USB_UTR_t *ptr);
void        usb_pstd_SaveRequest(USB_UTR_t *ptr);
uint16_t    usb_pstd_ChkConfigured(USB_UTR_t *ptr);
void        usb_pstd_InterruptEnable(USB_UTR_t *ptr);
void        usb_pstd_DpEnable(USB_UTR_t *ptr);
void        usb_pstd_DpDisable(USB_UTR_t *ptr);
void        usb_pstd_BusReset(USB_UTR_t *ptr);
void        usb_pstd_RemoteWakeup(USB_UTR_t *ptr);
void        usb_pstd_InitConnect(USB_UTR_t *ptr);
void        usb_pstd_TestMode(USB_UTR_t *ptr);
void        usb_pstd_AttachProcess(USB_UTR_t *ptr);
void        usb_pstd_DetachProcess(USB_UTR_t *ptr);
void        usb_pstd_SuspendProcess(USB_UTR_t *ptr);
void        usb_pstd_ResumeProcess(USB_UTR_t *ptr);
uint16_t    usb_pstd_ChkVbsts(USB_UTR_t *ptr);
void        usb_pstd_SetStall(USB_UTR_t *ptr, uint16_t Pipe);
void        usb_pstd_SetStallPipe0(USB_UTR_t *ptr);


/* RX_RSK.c */
void        usb_cpu_DelayXms(uint16_t time);
void        usb_cpu_soft_wait_ms(uint16_t time);
void        usb_cpu_Delay1us(uint16_t time);

#ifdef USB_DTC_ENABLE
void        usb_cpu_d0fifo2buf_start_dma(USB_UTR_t *ptr, uint32_t SourceAddr);
void        usb_cpu_buf2d0fifo_start_dma(USB_UTR_t *ptr, uint32_t DistAdr);
void        usb_cpu_d0fifo_stop_dma(USB_UTR_t *ptr);
#endif   /* USB_DTC_ENABLE */

void        usb_cpu_target_init(void);
void        usb_cpu_TargetLcdClear(void);
void        usb_cpu_McuInitialize(void);
void        usb_cpu_DisableDma(USB_UTR_t *ptr);
void        usb_cpu_EnableDma(USB_UTR_t *ptr);
void        usb_cpu_d0fifo_restart_dma( USB_UTR_t *ptr );


/* resetprg.c */

uint16_t    usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        usb_pstd_ControlWrite(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table);
void        usb_pstd_ControlEnd(USB_UTR_t *ptr, uint16_t status);

/* r_usb_pDriverAPI.c */
void        usb_pstd_SetPipeRegister(USB_UTR_t *ptr, uint16_t PipeNo, uint16_t *tbl);

#ifdef USB_HOST_BC_ENABLE
void        usb_hstd_pddetint_process(USB_UTR_t *ptr, uint16_t port);
#endif /* USB_HOST_BC_ENABLE */


#ifdef USB_PERI_BC_ENABLE
void        usb_pstd_bc_detect_process(USB_UTR_t *ptr);
#endif /* USB_PERI_BC_ENABLE */


#endif  /* __R_USB_CEXTERN_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
