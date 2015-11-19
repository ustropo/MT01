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
* File Name    : r_usb_fixed_config.h
* Description  : USB Fixed Configuration
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/


#ifndef __R_USB_FIXEDCFG_RX_H__
#define __R_USB_FIXEDCFG_RX_H__

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* !!!!! WARNING--You can not edit this file. !!!!!*/
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/*****************************************************************************
Macro definitions (COMMON)
******************************************************************************/


#if defined(BSP_MCU_RX64M)

    /* Select PORT */
    #define USB_PORTSEL_PP          USB_1PORT_PP    /* 1port in 1IP */

    /* The number of USBIP */
    #define USB_NUM_USBIP           2

    /* Select Transfer Speed */
    #define USB0_IPTYPE_PP          USB_FS_PP
    #define USB1_IPTYPE_PP          USB_HS_PP

    /* Select IP mode DEVADD MAX */
    #define USB_IP_DEVADD_PP        USB_IP_DEVADD_A_PP

    /* FIFO port register default access size */
    #define USB0_CFIFO_MBW          USB_MBW_16
    #define USB0_D0FIFO_MBW         USB_MBW_16
    #define USB0_D1FIFO_MBW         USB_MBW_16
    #define USB1_CFIFO_MBW          USB_MBW_32
    #define USB1_D0FIFO_MBW         USB_MBW_32
    #define USB1_D1FIFO_MBW         USB_MBW_32

#endif  /* defined(BSP_MCU_RX64M) */

#if defined(BSP_MCU_RX71M)

    /* Select PORT */
    #define USB_PORTSEL_PP          USB_1PORT_PP    /* 1port in 1IP */

    /* The number of USBIP */
    #define USB_NUM_USBIP           2

    /* Select Transfer Speed */
    #define USB0_IPTYPE_PP          USB_FS_PP
    #define USB1_IPTYPE_PP          USB_HS_PP

    /* Select IP mode DEVADD MAX */
    #define USB_IP_DEVADD_PP        USB_IP_DEVADD_A_PP

    /* FIFO port register default access size */
    #define USB0_CFIFO_MBW          USB_MBW_16
    #define USB0_D0FIFO_MBW         USB_MBW_16
    #define USB0_D1FIFO_MBW         USB_MBW_16
    #define USB1_CFIFO_MBW          USB_MBW_32
    #define USB1_D0FIFO_MBW         USB_MBW_32
    #define USB1_D1FIFO_MBW         USB_MBW_32

#endif  /* defined(BSP_MCU_RX71M) */

#if defined(BSP_MCU_RX63N)

    /* Select PORT */
    #define USB_PORTSEL_PP          USB_1PORT_PP    /* 1port in 1IP */

    /* The number of USBIP */
    #define USB_NUM_USBIP           2

    /* Select Transfer Speed */
    #define USB0_IPTYPE_PP          USB_FS_PP
    #define USB1_IPTYPE_PP          USB_HS_PP

    /* Select IP mode DEVADD MAX */
    #define USB_IP_DEVADD_PP        USB_IP_DEVADD_A_PP

    /* FIFO port register default access size */
    #define USB0_CFIFO_MBW          USB_MBW_16
    #define USB0_D0FIFO_MBW         USB_MBW_16
    #define USB0_D1FIFO_MBW         USB_MBW_16
    #define USB1_CFIFO_MBW          USB_MBW_32
    #define USB1_D0FIFO_MBW         USB_MBW_32
    #define USB1_D1FIFO_MBW         USB_MBW_32

#endif  /* defined(BSP_MCU_RX64M) */

    /* Start Pipe No */
    #define USB_MIN_PIPE_NO         USB_PIPE1

    /* USB Device address define */
    #define USB_DEVICEADDR          1u      /* PORT0 USB Address (1 to 10) */

    /* Clock mode */
    #define USB_USBIP_LPW_PP        USB_CLK_NOT_STOP_PP

    /* Sleep mode */
    #define USB_LPWRSEL_PP          USB_LPSM_DISABLE_PP

    /* Data Trans mode */
    #define USB_TRANS_MODE_PP       USB_TRANS_DTC_PP

    /* HUB Address */
    #define USB_HUBDPADDR           (uint16_t)(USB_DEVICEADDR + 1u)

    #define USB_MAX_FILENUMBER      16
    #define USB_DEVICENUM           10

    #define USB_BC_SUPPORT_IP       USB_USBIP_1


/*****************************************************************************
Macro definitions (Host Mode)
******************************************************************************/
/* Number of software retries when a no-response condition occurs during a transfer */
#define USB_PIPEERROR                   (1u)

/* Descriptor size */
#define USB_DEVICESIZE                  (20u)   /* Device Descriptor size */
#define USB_CONFIGSIZE                  (256u)  /* Configuration Descriptor size */


/* HUB down port */
#define USB_HUBDOWNPORT                 (4u)    /* HUB downport (MAX15) */

/* Total number of Interface */
#define USB_IFNUM                       (1u)

/* Total number of Configuration */
#define USB_CFGNUM                      (1u)
#define USB_MAX_DEVICENUM               (USB_DEVICENUM * USB_IFNUM * USB_CFGNUM)


#endif /* __R_USB_FIXEDCFG_RX_H__ */
/******************************************************************************
End  Of File
******************************************************************************/

