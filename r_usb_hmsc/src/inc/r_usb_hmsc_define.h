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
* File Name    : r_usb_hmsc_define.h
* Description  : USB User definition Pre-pro
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

#ifndef __R_USB_HMSC_DEFINE_H__
#define __R_USB_HMSC_DEFINE_H__


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_usb_cTypedef.h"      /* type define */


/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_HMSC_DEV_DET    (uint16_t)0x00      /* detached device */
#define USB_HMSC_DEV_ATT    (uint16_t)0x01      /* attached device */
#define USB_HMSC_DEV_ENU    (uint16_t)0x02      /* Device enumeration */


#define USB_MAXDRIVE        (uint16_t)16        /* Max drive */
#define USB_MAXSTRAGE       (uint16_t)8         /* Max device */

#define USB_DRIVE           0
#define USB_MEDIADRIVE      USB_DRIVE


/******************************************************************************
Enum define
******************************************************************************/
/* USB Mass Storage Device Class Request code define. */
enum usb_gpmsc_Request
{
    USB_MASS_STORAGE_RESET  = 0xFF00, /* Mass dtorage reset request. */
    USB_GET_MAX_LUN         = 0xFE00, /* Get max logical unit number */
};

/* GET_MAX_LUN request check code. */
enum usb_gpmsc_LunReqChk
{
    USB_MSC_LUN_LENGTH      = 0x01, /* GET_MAX_LUN request wLength. */
};

/* USB Mass Storage Devie Class Lapper check. */
enum usb_gpmsc_Case13check
{
    /* Device No Data */
    USB_MSC_Dnxx    = 0x10,
    /* Device Send(IN) Data */
    USB_MSC_Dixx    = 0x20,
    /* Device Recieved(OUT) Data */
    USB_MSC_Doxx    = 0x30,
    /* Host No Data */
    USB_MSC_xxHn    = 0x01,
    /* Host Recieved(IN) Data */
    USB_MSC_xxHi    = 0x02,
    /* Host Send(OUT) Data */
    USB_MSC_xxHo    = 0x03,
    /* Device No Data & Host No Data */
    USB_MSC_DnHn    = 0x11,
    /* Device No Data & Host Recieved(IN) Data */
    USB_MSC_DnHi    = 0x12,
    /* Device No Data & Host Send(OUT) Data */
    USB_MSC_DnHo    = 0x13,
    /* Device Send(IN) Data & Host No Data */
    USB_MSC_DiHn    = 0x21,
    /* Device Send(IN) Data & Host Recieved(IN) Data */
    USB_MSC_DiHi    = 0x22,
    /* Device Send(IN) Data & Host Send(OUT) Data */
    USB_MSC_DiHo    = 0x23,
    /* Device Recieved(OUT) Data & Host No Data */
    USB_MSC_DoHn    = 0x31,
    /* Device Recieved(OUT) Data & Host Recieved(IN) Data */
    USB_MSC_DoHi    = 0x32,
    /* Device Recieved(OUT) Data & Host Send(OUT) Data */
    USB_MSC_DoHo    = 0x33,
};

/* USB Mass Storage Devie Class Lapper check. */
enum usb_gpmsc_Case13nun
{
    USB_MSC_CASE00  = 0x00, /* CBW check case00(Not Use) */
    USB_MSC_CASE01  = 0x01, /* CBW check case01 */
    USB_MSC_CASE02  = 0x02, /* CBW check case02 */
    USB_MSC_CASE03  = 0x03, /* CBW check case03 */
    USB_MSC_CASE04  = 0x04, /* CBW check case04 */
    USB_MSC_CASE05  = 0x05, /* CBW check case05 */
    USB_MSC_CASE06  = 0x06, /* CBW check case06 */
    USB_MSC_CASE07  = 0x07, /* CBW check case07 */
    USB_MSC_CASE08  = 0x08, /* CBW check case08 */
    USB_MSC_CASE09  = 0x09, /* CBW check case09 */
    USB_MSC_CASE10  = 0x10, /* CBW check case10 */
    USB_MSC_CASE11  = 0x11, /* CBW check case11 */
    USB_MSC_CASE12  = 0x12, /* CBW check case12 */
    USB_MSC_CASE13  = 0x13, /* CBW check case13 */
};


/* ERROR CODE */
enum usb_ghmsc_Error
{
    USB_HMSC_OK             = (uint16_t)0,
    USB_HMSC_STALL          = (uint16_t)-1,
    USB_HMSC_CBW_ERR        = (uint16_t)-2,     /* CBR error */
    USB_HMSC_DAT_RD_ERR     = (uint16_t)-3,     /* Data IN error */
    USB_HMSC_DAT_WR_ERR     = (uint16_t)-4,     /* Data OUT error */
    USB_HMSC_CSW_ERR        = (uint16_t)-5,     /* CSW error */
    USB_HMSC_CSW_PHASE_ERR  = (uint16_t)-6,     /* Phase error */
    USB_HMSC_SUBMIT_ERR     = (uint16_t)-7,     /* submit error */
};


typedef struct DRIVE_MANAGEMENT    /* Structure for DRIVE No. */
{
    uint16_t    use_flag;           /* USE flag */
    uint16_t    ip;                 /* IP No. */
    uint16_t    dev_adr;            /* Device address */
}
DRIVE_MANAGEMENT_t;


/*****************************************************************************
Macro definitions
******************************************************************************/
/* CBW Structure define.    */
#define     USB_MSC_CBWLENGTH       31
#define     USB_MSC_CBWCB_LENGTH    12
#define     USB_MSC_CSW_LENGTH      13

/* CPU bit endian select (BIT_LITTLE:little, BIT_BIG:big) */
#if USB_CPUBYTE_PP == USB_BYTE_BIG_PP
 #define    USB_MSC_CBW_SIGNATURE   (uint32_t)0x55534243
 #define    USB_MSC_CSW_SIGNATURE   (uint32_t)0x55534253
#else   /* USB_CPUBYTE_PP == USB_BYTE_BIG_PP */
 #define    USB_MSC_CBW_SIGNATURE   (uint32_t)0x43425355
 #define    USB_MSC_CSW_SIGNATURE   (uint32_t)0x53425355
#endif  /* USB_CPUBYTE_PP == USB_BYTE_BIG_PP */

/* subClass code */
#define USB_ATAPI_MMC5                      (uint8_t)0x02
#define USB_ATAPI                           (uint8_t)0x05
#define USB_SCSI                            (uint8_t)0x06
#define USB_BOTP                            (uint8_t)0x50
#define USB_TOTALEP                         (uint8_t)0x02

#define USB_MSG_HMSC_NO_DATA                (uint16_t)0x501
#define USB_MSG_HMSC_DATA_IN                (uint16_t)0x502
#define USB_MSG_HMSC_DATA_OUT               (uint16_t)0x503
#define USB_MSG_HMSC_CSW_ERR                (uint16_t)0x504
#define USB_MSG_HMSC_CSW_PHASE_ERR          (uint16_t)0x505
#define USB_MSG_HMSC_CBW_ERR                (uint16_t)0x506
#define USB_MSG_HMSC_DATA_STALL             (uint16_t)0x507

#define USB_MSG_HMSC_STRG_DRIVE_SEARCH      (uint16_t)0x601
#define USB_MSG_HMSC_DEV_READ_SECTOR_SIZE   (uint16_t)0x602
#define USB_MSG_HMSC_DEV_READ_PARTITION     (uint16_t)0x603
#define USB_MSG_HMSC_STRG_DRIVE_OPEN        (uint16_t)0x604
#define USB_MSG_HMSC_STRG_USER_COMMAND      (uint16_t)0x605

#define USB_HMSC_DRIVEMOUNT                 (uint16_t)0x1000
#define USB_HMSC_FILEREAD                   (uint16_t)0x1001
#define USB_HMSC_FILEWRITE                  (uint16_t)0x1002
#define USB_HMSC_DRIVE_OPEN                 (uint16_t)0x1003
#define USB_HMSC_DRIVE_OPEN2                (uint16_t)0x1004
#define USB_HMSC_WAIT                       (uint16_t)0x1005

#define TFAT_SECT_SIZE                      (512)     /* TFAT Sector Size */
#define USB_HMSC_CLSDATASIZE                (512)

#define USB_SQ_DATA1                        (1)
#define USB_SQ_DATA0                        (0)

/******************************************************************************
Bit Order Definition "LEFT"
******************************************************************************/
#pragma bit_order left

/*****************************************************************************
Typedef definitions
******************************************************************************/
/* CBW Structure define.    */
typedef struct
{
    uint32_t    dCBWSignature;
    uint32_t    dCBWTag;
    uint8_t     dCBWDTL_Lo;
    uint8_t     dCBWDTL_ML;
    uint8_t     dCBWDTL_MH;
    uint8_t     dCBWDTL_Hi;
    struct
    {
        uint8_t CBWdir:1;
        uint8_t reserved7:7;
    }
    bmCBWFlags;
    struct
    {
        uint8_t reserved4:4;
        uint8_t bCBWLUN:4;
    }
    bCBWLUN;
    struct
    {
        uint8_t reserved3:3;
        uint8_t bCBWCBLength:5;
    }
    bCBWCBLength;
#if  (((USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
    ||((USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
    uint8_t CBWCB[(16 + (512-31))];
#else  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
    uint8_t CBWCB[(16 + (64-31))];
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
}
USB_MSC_CBW_t;

/* CSW Structure define define. */
typedef struct
{
    uint32_t    dCSWSignature;
    uint32_t    dCSWTag;
    uint8_t     dCSWDataResidue_Lo;
    uint8_t     dCSWDataResidue_ML;
    uint8_t     dCSWDataResidue_MH;
    uint8_t     dCSWDataResidue_Hi;
    uint8_t     bCSWStatus;
#if  (((USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
    ||((USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
    uint8_t     dummy[(512-13)];      /* DTC Receive Size dummy (MAX Packet size = 512Byte) */
#else  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
    uint8_t     dummy[(64-13)];      /* DTC Receive Size dummy (MAX Packet size = 64Byte) */
#endif  /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP */
}
USB_MSC_CSW_t;

/* CSW STATUS */
enum usb_gcmsc_CswSts
{
    USB_MSC_CSW_OK          = (uint16_t)0x00,
    USB_MSC_CSW_NG          = (uint16_t)0x01,
    USB_MSC_CSW_PHASE_ERR   = (uint16_t)0x02
};

/******************************************************************************
Bit Order Definition default
******************************************************************************/
#pragma bit_order

/* Host Sample Task */
#define USB_HMSC_TSK        USB_TID_4           /* Task ID */
#define USB_HMSC_MBX        USB_HMSC_TSK        /* Mailbox ID */
#define USB_HMSC_MPL        USB_HMSC_TSK        /* Memorypool ID */

/* Host Sample Task */
#define USB_HSTRG_TSK       USB_TID_5           /* Task ID */
#define USB_HSTRG_MBX       USB_HSTRG_TSK       /* Mailbox ID */
#define USB_HSTRG_MPL       USB_HSTRG_TSK       /* Memorypool ID */

#endif  /* __R_USB_HMSC_DEFINE_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
