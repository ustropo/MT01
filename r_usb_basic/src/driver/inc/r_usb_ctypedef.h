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
* File Name    : r_usb_ctypedef.h
* Description  : Type Definition Header File
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/


#ifndef __R_USB_CTYPEDEF_H__
#define __R_USB_CTYPEDEF_H__


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>

#include "r_usb_basic_config.h"


/*****************************************************************************
Typedef definitions
******************************************************************************/
    typedef void*           VP;             /* Pointer to variable      */
    typedef long            ER;             /* Error code               */
    typedef short           ID;             /* Object ID (xxxid)        */
    typedef long            TMO;            /* Time out                 */
    typedef long            VP_INT;         /* Integer data             */

    /*----------- msghead -----------*/
    typedef struct
    {
        VP              msghead;        /* Message header               */
    } T_MSG;

/*****************************************************************************
Typedef definitions
******************************************************************************/
    typedef T_MSG           USB_MSG_t;      /* ITRON message            */
    typedef ER              USB_ER_t;       /* ITRON system call err    */
    typedef ID              USB_ID_t;       /* ITRON system call define */
    typedef TMO             USB_TM_t;       /* ITRON time out           */
    typedef VP              USB_MH_t;       /* ITRON Message Header     */
    typedef VP_INT          USB_VP_INT_t;

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct st_usb       USB_STNBYINT_t;

typedef void (*USB_CB_CHECK_t)(struct USB_UTR *, uint16_t**);
typedef void (*USB_CB_t)(struct USB_UTR *, uint16_t, uint16_t);
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
typedef volatile struct st_usba __evenaccess*   USB_REGADR1_t;
#else
typedef volatile struct st_usb1 __evenaccess*   USB_REGADR1_t;
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
typedef volatile struct st_usb0 __evenaccess*   USB_REGADR_t;

typedef struct
{
    uint16_t        ReqType;            /* Request type */
    uint16_t        ReqTypeType;        /* Request type TYPE */
    uint16_t        ReqTypeRecip;       /* Request type RECIPIENT */
    uint16_t        ReqRequest;         /* Request */
    uint16_t        ReqValue;           /* Value */
    uint16_t        ReqIndex;           /* Index */
    uint16_t        ReqLength;          /* Length */
} USB_REQUEST_t;

typedef struct USB_HCDREG
{
    uint16_t        rootport;       /* Root port */
    uint16_t        devaddr;        /* Device address */
    uint16_t        devstate;       /* Device state */
    uint16_t        ifclass;        /* Interface Class */
    uint16_t        *tpl;           /* Target peripheral list 
                                        (Vendor ID, Product ID) */
    uint16_t        *pipetbl;       /* Pipe Define Table address */
    USB_CB_t        classinit;      /* Driver init */
    USB_CB_CHECK_t  classcheck;     /* Driver check */
    USB_CB_t        devconfig;      /* Device configured */
    USB_CB_t        devdetach;      /* Device detach */
    USB_CB_t        devsuspend;     /* Device suspend */
    USB_CB_t        devresume;      /* Device resume */
} USB_HCDREG_t;

typedef struct USB_UTR
{
    USB_MH_t        msghead;        /* Message header (for SH-solution) */
    uint16_t        msginfo;        /* Message Info for F/W */
    uint16_t        keyword;        /* Rootport / Device address / Pipe number */
    union {
        USB_REGADR_t    ipp;        /* IP Address(USB0orUSB1)*/
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
        USB_REGADR1_t   ipp1;       /* IP Address(USBHS) */
#endif /*#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)*/
    };
    uint16_t        ip;             /* IP number(0or1) */
    uint16_t        result;         /* Result */
    USB_CB_t        complete;       /* Call Back Function Info */
    void            *tranadr;       /* Transfer data Start address */
    uint32_t        tranlen;        /* Transfer data length */
    uint16_t        *setup;         /* Setup packet(for control only) */
    uint16_t        status;         /* Status */
    uint16_t        pipectr;        /* Pipe control register */
    uint8_t         errcnt;         /* Error count */
    uint8_t         segment;        /* Last flag */
    void            *usr_data;      
} USB_UTR_t;

/* Class request processing function type. */
typedef void (*USB_CB_TRN_t)(USB_UTR_t *, USB_REQUEST_t*, uint16_t ctsq);

typedef struct USB_PCDREG
{
    uint16_t        **pipetbl;      /* Pipe Define Table address */
    uint8_t         *devicetbl;     /* Device descriptor Table address */
    uint8_t         *qualitbl;      /* Qualifier descriptor Table address */
    uint8_t         **configtbl;    /* Configuration descriptor
                                        Table address */
    uint8_t         **othertbl;     /* Other configuration descriptor
                                        Table address */
    uint8_t         **stringtbl;    /* String descriptor Table address */
    USB_CB_t        classinit;      /* Driver init */
    USB_CB_t        devdefault;     /* Device default */
    USB_CB_t        devconfig;      /* Device configured */
    USB_CB_t        devdetach;      /* Device detach */
    USB_CB_t        devsuspend;     /* Device suspend */
    USB_CB_t        devresume;      /* Device resume */
    USB_CB_t        interface;      /* Interface changed */
    USB_CB_TRN_t    ctrltrans;      /* Control Transfer */
} USB_PCDREG_t;

typedef struct USB_UTR      USB_HCDINFO_t;
typedef struct USB_UTR      USB_MGRINFO_t;
typedef struct USB_UTR      USB_PCDINFO_t;
typedef struct USB_UTR      USB_CLSINFO_t;



struct usb_devinfo 
{
    uint8_t         devadr;
    uint8_t         speed;
    uint8_t         isTPL;
    uint8_t         interfaceClass;
    uint8_t         isActDev;
    USB_UTR_t       *ptr;
};



#ifdef USB_HOST_BC_ENABLE
typedef struct
{
    uint8_t         dcpmode;        /* DCP Mode Flag */
    uint8_t         state;          /* BC State */
    uint8_t         pd_detect;      /* PD Detect Flag */
} usb_bc_status_t;
#endif  /* USB_HOST_BC_ENABLE */


/******************************************************************************
Constant macro definitions
******************************************************************************/

/* Condition compilation by the difference of the operating system */
    #ifndef NULL
      #define   NULL    0u
    #endif  /* NULL */

    #define E_OK        0L                  /* Normal end               */
    #define E_TMOUT     (-50L)              /* Time out                 */
    #define E_QOVR      (-43L)              /* Queuing over flow        */


/******************************************************************************
Constant macro definitions
******************************************************************************/

    /*-------------------- Task/Handler attribute (***atr) -------------*/
    #define TA_HLNG     0x00000000u     /* High-level language program  */
    /*----------------------- Object attribute (***atr) ----------------*/
    #define TA_TFIFO    0x00000000u     /* FIFO wait queue              */
    #define TA_MFIFO    0x00000000u     /* FIFO message queue           */
    #define TA_ACT      0x00000002u     /* Create task with activation  */

    /*-------------------------- Object status -------------------------*/
    #define TTS_RUN     0x00000001UL    /* RUNNING                      */
    #define TTS_RDY     0x00000002UL    /* READY                        */
    #define TTS_WAI     0x00000004UL    /* WAITING                      */
    #define TTS_SUS     0x00000008UL    /* SUSPENDED                    */
    #define TTS_WAS     0x0000000cUL    /* WAITING-SUSPENDED            */
    #define TTS_DMT     0x00000010UL    /* DORMANT                      */
    #define TTS_STK     0x40000000UL    /* STACK WAITING                */

    /* <system call> */
    #define USB_NO_SYSTEM_PP

/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_NULL        NULL
#define USB_VP_INT      VP_INT
#define USB_TA_HLNG TA_HLNG
#define USB_TA_TFIFO    TA_TFIFO
#define USB_TA_MFIFO    TA_MFIFO

#define USB_E_TMOUT E_TMOUT     /* TRCV_MSG time out */
#define USB_E_QOVR      E_QOVR      /* Submit overlap error */
#define USB_E_ERROR (-1L)
#define USB_E_OK        E_OK
#define USB_TMPOL       TMO_POL     /* TRCV_MSG poling */
#define USB_TMFEVR      TMO_FEVR    /* TRCV_MSG no time */
#define USB_TAJLNG      TA_HLNG     /* High-level language program */
#define USB_TATFIFO TA_TFIFO    /* FIFO wait queue */
#define USB_TAMFIFO TA_MFIFO    /* FIFO message queue */
#define USB_TAACT       TA_ACT      /* Create task with activation */
#define USB_TTSRUN      TTS_RUN     /* RUNNING */

#define USB_TTS_RUN TTS_RUN
#define USB_TTS_RDY TTS_RDY
#define USB_TTS_WAI TTS_WAI
#define USB_TTS_SUS TTS_SUS
#define USB_TTS_WAS TTS_WAS
#define USB_TTS_DMT TTS_DMT
#define USB_TTS_STK TTS_STK

/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_NONE            (uint16_t)(0)
#define USB_YES             (uint16_t)(1)
#define USB_NO              (uint16_t)(0)
#define USB_DONE            (uint16_t)(0)
#define USB_ERROR           (uint16_t)(0xFFFF)
#define USB_OK              (uint16_t)(0)
#define USB_NG              (uint16_t)(0xFFFF)
#define USB_ON              (uint16_t)(1)
#define USB_OFF             (uint16_t)(0)
#define USB_OTG_DONE        (uint16_t)(2)
#define USB_NOT_USED        (uint16_t)(0)

#endif  /* __R_USB_CTYPEDEF_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
