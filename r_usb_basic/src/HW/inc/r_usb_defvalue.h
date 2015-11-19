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
* File Name    : r_usb_defvalue.h
* Description  : USB value definition
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
***********************************************************************************************************************/


#ifndef __R_USB_DEFVAL_H__
#define __R_USB_DEFVAL_H__

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* !!!!! WARNING--You can not edit this file. !!!!!*/
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*****************************************************************************
Macro definitions (for Preprocessor)
******************************************************************************/

#define USB_FW_OS_PP                1
#define USB_FW_NONOS_PP             2

#define USB_OS_CRE_USE_PP           1   /* cre_* system call USE */
#define USB_OS_CRE_NOTUSE_PP        2   /* cre_* system call Not USE */

#define USB_HS_PP                   1   /* Hi-Speed */
#define USB_FS_PP                   2   /* Full-Speed */

#define USB_1PORT_PP                1   /* 1 port mode */
#define USB_2PORT_PP                2   /* 2 port mode */

#define USB_HOST_PP                 1
#define USB_PERI_PP                 2
#define USB_HOST_PERI_PP            3
#define USB_OTG_PP                  4
#define USB_NOUSE_PP                10
#define USB_USE_PP                  11
#define USB_PERI0_PERI1_PP          17
#define USB_PERI0_HOST1_PP          18
#define USB_HOST0_PERI1_PP          19
#define USB_HOST0_HOST1_PP          20

/* Clock mode */
#define USB_CLK_NOT_STOP_PP         0
#define USB_CLK_XCKE_USE_PP         1
#define USB_CLK_PCUT_USE_PP         2

/* ATCKM mode  */
#define USB_ATCKM_NOT_USE_PP        0
#define USB_ATCKM_USE_PP            1

/* Sleep mode */
#define USB_LPSM_DISABLE_PP         0   /* Low-power sleep disable (SOC) */
#define USB_LPSM_ENABLE_PP          1   /* Low-power sleep enable (ASSP) */

/* IP mode DEVADD MAX */
#define USB_IP_DEVADD_A_PP          10  /* DEVADD_MAX=10 */
#define USB_IP_DEVADD_5_PP          5   /* DEVADD_MAX=5 */

/* Select PIPEBUF fix or variable */
#define USB_PIPEBUF_FIX_PP          1
#define USB_PIPEBUF_CHANGE_PP       2

/* Select target chip define */
#define USB_ASSP_PP                 1
#define USB_RX600_PP                2

/* Data Trans mode */
#define USB_TRANS_DMA_PP            1
#define USB_TRANS_DTC_PP            2

/* Default Bus size */
#define USB_BUSSIZE_16_PP           16
#define USB_BUSSIZE_32_PP           32

/* Low Power Mode */
#define USB_LPWR_NOT_USE_PP         0
#define USB_LPWR_USE_PP             1

/* Debug Console on/off */
#define USB_DEBUG_ON_PP             1
#define USB_DEBUG_OFF_PP            0

/* BYTE ENDIAN */
#define USB_BYTE_LITTLE_PP          0
#define USB_BYTE_BIG_PP             1

/* SPEED mode */
#define USB_HS_DISABLE          (uint16_t)0
#define USB_HS_ENABLE           (uint16_t)1

/* H/W function type */
#define USB_HOST                (uint16_t)1     /* Host mode */
#define USB_PERI                (uint16_t)2     /* Peripheral mode */
#define USB_HOST_PERI           (uint16_t)3     /* Host/Peri mode */
#define USB_OTG                 (uint16_t)4     /* Otg mode */

/* H/W function type */
#define USB_BIT0                (uint16_t)0x0001
#define USB_BIT1                (uint16_t)0x0002
#define USB_BIT2                (uint16_t)0x0004
#define USB_BIT3                (uint16_t)0x0008
#define USB_BIT4                (uint16_t)0x0010
#define USB_BIT5                (uint16_t)0x0020
#define USB_BIT6                (uint16_t)0x0040
#define USB_BIT7                (uint16_t)0x0080
#define USB_BIT8                (uint16_t)0x0100
#define USB_BIT9                (uint16_t)0x0200
#define USB_BIT10               (uint16_t)0x0400
#define USB_BIT11               (uint16_t)0x0800
#define USB_BIT12               (uint16_t)0x1000
#define USB_BIT13               (uint16_t)0x2000
#define USB_BIT14               (uint16_t)0x4000
#define USB_BIT15               (uint16_t)0x8000
#define USB_BITSET(x)           (uint16_t)((uint16_t)1 << (x))

/* nonOS Use */
#define USB_SEQ_0               (uint16_t)0x0000
#define USB_SEQ_1               (uint16_t)0x0001
#define USB_SEQ_2               (uint16_t)0x0002
#define USB_SEQ_3               (uint16_t)0x0003
#define USB_SEQ_4               (uint16_t)0x0004
#define USB_SEQ_5               (uint16_t)0x0005
#define USB_SEQ_6               (uint16_t)0x0006
#define USB_SEQ_7               (uint16_t)0x0007
#define USB_SEQ_8               (uint16_t)0x0008
#define USB_SEQ_9               (uint16_t)0x0009
#define USB_SEQ_10              (uint16_t)0x000a

#define USB_HUB_P1              (uint16_t)0x0001
#define USB_HUB_P2              (uint16_t)0x0002
#define USB_HUB_P3              (uint16_t)0x0003
#define USB_HUB_P4              (uint16_t)0x0004

/* Interrupt message num */
#define USB_INTMSGMAX           (uint16_t)15
#define USB_DMAMSGMAX           (uint16_t)15

/* USB IP Number */
#define USB_USBIP_0             (uint16_t)0
#define USB_USBIP_1             (uint16_t)1
#define USB_NOTUSE              (uint16_t)2

/* USB Device Connect */
#define USB_DEV_NO_CONNECT      (uint16_t)0
#define USB_DEV_CONNECTED       (uint16_t)1

#endif
/******************************************************************************
End  Of File
******************************************************************************/
