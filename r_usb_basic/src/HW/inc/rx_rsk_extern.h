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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : rx_rsk_extern.h
* Description  : RX63N RSK Extern
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
***********************************************************************************************************************/


#ifndef __RX_RSK_EXTERN_H__
#define __RX_RSK_EXTERN_H__

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_ctypedef.h"     /* Type define */


/******************************************************************************
External variables and functions
******************************************************************************/
/* SW input driver functions */
extern uint16_t     usb_cpu_GetKeyNo(void);
extern void         usb_cpu_AcquisitionKeyNo(void);
extern uint8_t      usb_cpu_KeepKeyNo(void);
extern uint8_t      usb_cpu_SingleKeyNo(void);

/* AD driver functions */
extern void         usb_cpu_AdInit(void);
extern uint32_t     usb_cpu_AdData(void);

/* LED driver functions */
extern void         usb_cpu_LedInit(void);
extern void         usb_cpu_LedSetBit(uint8_t bit, uint8_t data);

/* Serial Port driver functions */
extern uint16_t     usb_cpu_Sci_Set1(uint8_t *data_rate_top, uint8_t stop_bit, uint8_t parity, uint8_t data_bit);
extern void         usb_cpu_Sci_DataSend(uint16_t mode, void *tranadr, uint16_t length);
extern uint16_t     usb_cpu_Sci_DataReceive(uint8_t *tranadr, uint16_t receive_length);
extern uint16_t     usb_cpu_Sci_StxBuffStatus(void);
extern void         usb_cpu_Sci_Buffer_init(void);
extern void         usb_cpu_Sci_HW_init(void);
extern void         usb_cpu_Sci_enable(void);
extern void         usb_cpu_Sci_disable(void);
extern uint16_t     usb_cpu_Sci_CopyData_for_Echo(void);
extern uint16_t     usb_cpu_Sci_GetSerialState(uint16_t *serial_state);
extern uint16_t     usb_cpu_Sci_EnableStatus(void);

#endif  /* __RX_RSK_EXTERN_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
