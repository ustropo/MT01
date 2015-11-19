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
* File Name    : r_usb_hHubsys.c
* Description  : USB Host Hub system code
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
#include "r_usb_api.h"


/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
/******************************************************************************
Constant macro definitions
******************************************************************************/
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_1PORT_PP
#define USB_MAXHUB              (uint16_t)1
#endif  /* USB_1PORT_PP */
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
#define USB_MAXHUB              (uint16_t)2
#endif  /* USB_2PORT_PP */

#define USB_HUB_CLSDATASIZE     (uint16_t)512
#define USB_HUB_QOVR            (uint16_t)0xFFFE

#define USB_BIT_PORT_CONNECTION     (0x00000001u)
#define USB_BIT_PORT_ENABLE         (0x00000002u)
#define USB_BIT_PORT_SUSPEND        (0x00000004u)
#define USB_BIT_PORT_OVER_CURRENT   (0x00000008u)
#define USB_BIT_PORT_RESET          (0x00000010u)
#define USB_BIT_PORT_POWER          (0x00000100u)
#define USB_BIT_PORT_LOW_SPEED      (0x00000200u)
#define USB_BIT_C_PORT_CONNECTION   (0x00010000u)
#define USB_BIT_C_PORT_ENABLE       (0x00020000u)
#define USB_BIT_C_PORT_SUSPEND      (0x00040000u)
#define USB_BIT_C_PORT_OVER_CURRENT (0x00080000u)
#define USB_BIT_C_PORT_RESET        (0x00100000u)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct USB_HUB_INFO
{
    uint16_t        up_addr;        /* Up-address  */
    uint16_t        up_port_num;    /* Up-port num */
    uint16_t        port_num;       /* Port number */
    uint16_t        pipe_num;       /* Pipe number */
} USB_HUB_INFO_t;


/******************************************************************************
External variables and functions
******************************************************************************/
extern void (*usb_ghstd_EnumarationProcess[])(USB_UTR_t *, uint16_t,uint16_t);
extern uint16_t    usb_hreg_read_devadd( USB_UTR_t *ptr, uint16_t devadr );


/* Enumeration request */
//extern uint16_t   usb_ghstd_EnumSeq[2u];
extern uint8_t  usb_ghstd_ClassData[][512];

/******************************************************************************
Static variables and functions
******************************************************************************/
static  uint16_t    usb_hhub_ChkConfig(uint16_t **table, uint16_t spec);
static  uint16_t    usb_hhub_ChkInterface(uint16_t **table, uint16_t spec);
static  uint16_t    usb_hhub_pipe_info(USB_UTR_t *ptr, uint8_t *table, uint16_t offset
                        , uint16_t speed, uint16_t length);
static  void        usb_hhub_port_detach(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum);
static  void        usb_hhub_selective_detach(USB_UTR_t *ptr, uint16_t devaddr);
static  void        usb_hhub_trans_start(USB_UTR_t *ptr, uint16_t hubaddr, uint32_t size
                        , uint8_t *table, USB_CB_t complete);
static  void        usb_hhub_trans_complete(USB_UTR_t *mess, uint16_t, uint16_t);
static  uint16_t    usb_hhub_GetNewDevaddr(USB_UTR_t *ptr);
static  uint16_t    usb_hhub_GetHubaddr(USB_UTR_t *ptr, uint16_t pipenum);
static  uint16_t    usb_hhub_GetCnnDevaddr(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum);
static  uint16_t    usb_hhub_ChkTblIndx1(USB_UTR_t *ptr, uint16_t hubaddr);
static  uint16_t    usb_hhub_ChkTblIndx2(USB_UTR_t *ptr, uint16_t hubaddr);
static  uint16_t    usb_hhub_PortSetFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port
                        , uint16_t command, USB_CB_t complete);
static  uint16_t    usb_hhub_PortClrFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port
                        , uint16_t command, USB_CB_t complete);
static  void        usb_hhub_InitDownPort(USB_UTR_t *ptr, uint16_t hubaddr, USB_CLSINFO_t *mess);
static  void        usb_hhub_new_connect(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  uint16_t    usb_hhub_PortAttach(uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  void        usb_hhub_port_reset(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess);
static  void        usb_hhub_enumeration(USB_CLSINFO_t *mess);
static  void        usb_hhub_event(USB_CLSINFO_t *mess);
static  void        usb_hhub_SpecifiedPath(USB_CLSINFO_t *mess);
static  void        usb_hhub_SpecifiedPathWait(USB_CLSINFO_t *mess, uint16_t times);
static  void        usb_hhub_class_request_complete(USB_UTR_t *mess, uint16_t, uint16_t);
static  void        usb_hhub_CheckRequest(USB_UTR_t *ptr, uint16_t result);
        void        usb_hhub_initial(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
static  uint16_t    usb_hhub_request_result(uint16_t checkerr);
static  void        usb_hstd_DeviceDescripInfo(USB_UTR_t *ptr);
static  void        usb_hstd_ConfigDescripInfo(USB_UTR_t *ptr);
        void        usb_hstd_EndpDescripInfo(uint8_t *tbl);
        void        usb_hhub_check_class(USB_UTR_t *ptr, uint16_t **table);
        void        usb_hhub_Task(USB_VP_INT stacd);

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Control transfer message */
USB_UTR_t       usb_shhub_ControlMess[USB_NUM_USBIP];
/* Data transfer message */
USB_UTR_t       usb_shhub_DataMess[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* HUB descriptor */
uint8_t         usb_ghhub_Descriptor[USB_NUM_USBIP][USB_CONFIGSIZE];
/* HUB status data */
uint8_t         usb_ghhub_Data[USB_NUM_USBIP][USB_MAXDEVADDR + 1u][8];
/* HUB downport status */
uint16_t        usb_shhub_DownPort[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Downport remotewakeup */
uint16_t        usb_shhub_Remote[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
/* Up-hubaddr, up-hubport, portnum, pipenum */
USB_HUB_INFO_t  usb_shhub_InfoData[USB_NUM_USBIP][USB_MAXDEVADDR + 1u];
uint16_t        usb_shhub_Number[USB_NUM_USBIP];
uint16_t        usb_shhub_ClassRequest[USB_NUM_USBIP][5];

uint16_t        usb_hstd_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t        usb_hstd_GetStringDescriptor1Check(uint16_t errcheck);
uint16_t        usb_hstd_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete);
uint16_t        usb_hstd_GetStringDescriptor2Check(uint16_t errcheck);

/* Condition compilation by the difference of USB function */
#if USB_NUM_USBIP == 2
uint16_t        usb_shhub_ClassSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_InitSeq[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_InitPort[USB_NUM_USBIP]       = { USB_HUB_P1, USB_HUB_P1 };
uint16_t        usb_shhub_EventSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_EventPort[USB_NUM_USBIP]      = { USB_HUB_P1, USB_HUB_P1 };
uint16_t        usb_shhub_AttachSeq[USB_NUM_USBIP]      = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_ResetSeq[USB_NUM_USBIP]       = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_State[USB_NUM_USBIP]          = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_Info[USB_NUM_USBIP]           = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_HubAddr[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
uint16_t        usb_shhub_Process[USB_NUM_USBIP]        = { USB_SEQ_0, USB_SEQ_0 };
#else   /* USB_NUM_USBIP == 2 */
uint16_t        usb_shhub_ClassSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_InitSeq[USB_NUM_USBIP]        = { USB_SEQ_0 };
uint16_t        usb_shhub_InitPort[USB_NUM_USBIP]       = { USB_HUB_P1 };
uint16_t        usb_shhub_EventSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_EventPort[USB_NUM_USBIP]      = { USB_HUB_P1 };
uint16_t        usb_shhub_AttachSeq[USB_NUM_USBIP]      = { USB_SEQ_0 };
uint16_t        usb_shhub_ResetSeq[USB_NUM_USBIP]       = { USB_SEQ_0 };
uint16_t        usb_shhub_State[USB_NUM_USBIP]          = { USB_SEQ_0 };
uint16_t        usb_shhub_Info[USB_NUM_USBIP]           = { USB_SEQ_0 };
uint16_t        usb_shhub_HubAddr[USB_NUM_USBIP]        = { USB_SEQ_0 };
uint16_t        usb_shhub_Process[USB_NUM_USBIP]        = { USB_SEQ_0 };
#endif  /* USB_NUM_USBIP == 2 */

uint8_t         *usb_shhub_DeviceTable[USB_NUM_USBIP];
uint8_t         *usb_shhub_ConfigTable[USB_NUM_USBIP];
uint8_t         *usb_shhub_InterfaceTable[USB_NUM_USBIP];
uint16_t        usb_shhub_Spec[USB_NUM_USBIP];
uint16_t        usb_shhub_Root[USB_NUM_USBIP];
uint16_t        usb_shhub_Speed[USB_NUM_USBIP];
uint16_t        usb_shhub_DevAddr[USB_NUM_USBIP];
uint16_t        usb_shhub_Index[USB_NUM_USBIP];

const uint16_t usb_ghhub_TPL[4] =
{
    1,                      /* Number of list */
    0,                      /* Reserved */
    USB_NOVENDOR,           /* Vendor ID  : no-check */
    USB_NOPRODUCT,          /* Product ID : no-check */
};

/* Host hub Pipe Information Table (endpoint table) definition */
uint16_t usb_ghhub_DefEPTbl[USB_NUM_USBIP][USB_EPL +1] =
{
  {
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
/* Pipe end */
    USB_PDTBLEND,
  },
#if USB_NUM_USBIP == 2
  {
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
/* Pipe end */
    USB_PDTBLEND,
  }
#endif  /* USB_NUM_USBIP == 2 */
};

/* Host hub temporary Pipe Information Table ("endpoint table") definition */
uint16_t usb_ghhub_TmpEPTbl[USB_NUM_USBIP][USB_EPL +1] =
{
  {
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
/* Pipe end */
    USB_PDTBLEND,
  },
#if USB_NUM_USBIP == 2
  {
    /* PIPE9 Definition */
    USB_PIPE9,
    USB_NONE | USB_BFREOFF | USB_DBLBOFF | USB_CNTMDOFF | USB_SHTNAKOFF
  | USB_NONE | USB_NONE,
    (uint16_t)USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u),
    USB_NONE,
    USB_NONE,
    USB_CUSE,
/* Pipe end */
    USB_PDTBLEND,
  }
#endif  /* USB_NUM_USBIP == 2 */
};


/******************************************************************************
Renesas Abstracted Hub Driver API functions
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_hhub_Task
Description     : Call HUB task
Arguments       : USB_VP_INT stacd          : Start Code of Hub Task
Return value    : none
******************************************************************************/
void R_usb_hhub_Task(USB_VP_INT stacd)
{
    usb_hhub_Task( stacd );
}   /* eof R_usb_hhub_Task() */

/******************************************************************************
Function Name   : R_usb_hhub_Open
Description     : HUB sys open
Arguments       : uint16_t devaddr          : device address
                : uint16_t data2            : Not use
Return value    : USB_ER_t                  : Error Info
******************************************************************************/
void R_usb_hhub_Open(USB_UTR_t *ptr, uint16_t devaddr, uint16_t data2)
{
    USB_ER_t        err, err2;
    USB_MH_t        p_blf;
    USB_MGRINFO_t   *mp;
    uint16_t        hubaddr, index;

    err = USB_ERROR;
    hubaddr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    index = usb_hhub_ChkTblIndx1(ptr, devaddr);

    if( USB_MAXHUB != usb_shhub_Number[ptr->ip] )
    {
        /* Wait 10ms */
        usb_cpu_DelayXms((uint16_t)10);
        err = USB_PGET_BLK(USB_HUB_MPL, &p_blf);
        if( USB_E_OK == err )
        {
            mp = (USB_MGRINFO_t*)p_blf;
            mp->msghead = (USB_MH_t)USB_NULL;
            mp->msginfo = USB_MSG_CLS_INIT;
            mp->keyword = devaddr;

            mp->ipp     = ptr->ipp;
            mp->ip      = ptr->ip;

            /* Send message */
            err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)p_blf);
            if( USB_E_OK != err )
            {
                /* Send Message failure */
                USB_PRINTF1("### hHubOpen snd_msg error (%ld)\n", err);
                err2 = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
                if( err2 != USB_E_OK )
                {
                    USB_PRINTF1("### hHubOpen rel_blk error (%ld)\n", err2);
                }
            }
        }
        else
        {
            /* Release memory block failure */
            USB_PRINTF1("### hHubOpen pget_blk error (%ld)\n", err);
            while( 1 );
        }
        /* Pipe number set */
        usb_shhub_InfoData[ptr->ip][devaddr].pipe_num = usb_ghhub_TmpEPTbl[ptr->ip][index];
        /* HUB downport status */
        usb_shhub_DownPort[ptr->ip][devaddr]    = 0;
        /* Downport remotewakeup */
        usb_shhub_Remote[ptr->ip][devaddr]  = 0;
        usb_ghhub_TmpEPTbl[ptr->ip][index+3]    |= hubaddr;
        R_usb_hstd_SetPipeInfo(&usb_ghhub_DefEPTbl[ptr->ip][index],
            &usb_ghhub_TmpEPTbl[ptr->ip][index], (uint16_t)USB_EPL);

        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_INIT;
        R_usb_hstd_SetPipeRegistration(ptr, (uint16_t*)&usb_ghhub_DefEPTbl[ptr->ip],
                usb_ghhub_DefEPTbl[ptr->ip][index]);

        usb_shhub_Number[ptr->ip]++;
    }
}   /* eof R_usb_hhub_Open() */


/******************************************************************************
Function Name   : R_usb_hhub_Close
Description     : HUB sys close
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t data2            : Not use
Return value    : USB_ER_t                  : Error Info
******************************************************************************/
void R_usb_hhub_Close(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t data2)
{
    uint16_t        md, i;
    USB_HCDREG_t    *driver;
    uint16_t        devaddr, index;

    for( i = 1; i <= usb_shhub_InfoData[ptr->ip][hubaddr].port_num; i++ )
    {
        /* Now downport device search */
        devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, i);
        if( 0 != devaddr )
        {
            /* HUB down port selective disconnect */
            usb_hhub_selective_detach(ptr, devaddr);
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( devaddr == driver->devaddr )
                {
                    (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);
                    /* Root port */
                    driver->rootport    = USB_NOPORT;
                    /* Device devaddress */
                    driver->devaddr     = USB_NODEVICE;
                    /* Device state */
                    driver->devstate    = USB_DETACHED;
                }
            }
        }
    }

    usb_shhub_Number[ptr->ip]--;
    index = usb_hhub_ChkTblIndx2(ptr, hubaddr);

    /* Set pipe information */
    for(i = 1; i <= USB_MAXDEVADDR ; i++)
    {
        usb_shhub_InfoData[ptr->ip][i].up_addr       = 0;    /* Up-address clear */
        usb_shhub_InfoData[ptr->ip][i].up_port_num   = 0;    /* Up-port num clear */
        usb_shhub_InfoData[ptr->ip][i].port_num      = 0;    /* Port number clear */
        usb_shhub_InfoData[ptr->ip][i].pipe_num      = 0;    /* Pipe number clear */
    }

    usb_shhub_DownPort[ptr->ip][hubaddr]    = 0;
    usb_shhub_Remote[ptr->ip][hubaddr]      = 0;
    usb_ghhub_DefEPTbl[ptr->ip][index + 1]  = USB_NONE;
    usb_ghhub_DefEPTbl[ptr->ip][index + 3]  = USB_NONE;
    usb_ghhub_DefEPTbl[ptr->ip][index + 4]  = USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 1]  = USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 3]  = USB_NONE;
    usb_ghhub_TmpEPTbl[ptr->ip][index + 4]  = USB_NONE;

}   /* eof R_usb_hhub_Close() */


/******************************************************************************
Function Name   : R_usb_hhub_Registration
Description     : HUB driver
Arguments       : USB_HCDREG_t *callback
Return value    : none
******************************************************************************/
void R_usb_hhub_Registration(USB_UTR_t *ptr, USB_HCDREG_t *callback)
{
    USB_HCDREG_t    driver;

    /* Driver registration */
    if( USB_NULL == callback )
    {
        /* Target peripheral list */
        driver.tpl          = (uint16_t*)&usb_ghhub_TPL[0];
        /* Pipe Information Table Define address */
        driver.pipetbl      = (uint16_t*)&usb_ghhub_DefEPTbl[ptr->ip];
    }
    else
    {
        /* Target peripheral list */
        driver.tpl          = callback->tpl;
        /* Pipe Define Table address */
        driver.pipetbl      = callback->pipetbl;
    }
    /* Interface Class */
    driver.ifclass      = (uint16_t)USB_IFCLS_HUB;
    /* Driver init */
    driver.classinit    = &usb_hhub_initial;
    /* Driver check */
    driver.classcheck   = &usb_hhub_check_class;
    /* Device configured */
    driver.devconfig    = (USB_CB_t)&R_usb_hhub_Open;
    /* Device detach */
    driver.devdetach    = (USB_CB_t)&R_usb_hhub_Close;
    /* Device suspend */
    driver.devsuspend   = &usb_cstd_DummyFunction;
    /* Device resume */
    driver.devresume    = &usb_cstd_DummyFunction;

    R_usb_hstd_DriverRegistration(ptr, (USB_HCDREG_t *)&driver);
}   /* eof R_usb_hhub_Registration() */


/******************************************************************************
Function Name   : R_usb_hhub_GetHubInformation
Description     : Read HUB-Descriptor
Arguments       : uint16_t hubaddr          : hub address
                : USB_CB_t complete         : callback function
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t R_usb_hhub_GetHubInformation(USB_UTR_t *ptr, uint16_t hubaddr, USB_CB_t complete)
{
    USB_ER_t    qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_CLASS | USB_DEVICE;
    usb_shhub_ClassRequest[ptr->ip][1]  = USB_HUB_DESCRIPTOR;
    usb_shhub_ClassRequest[ptr->ip][2]  = 0;
    usb_shhub_ClassRequest[ptr->ip][3]  = 0x0047;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

    /* HUB Descriptor */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Descriptor[ptr->ip][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    qerr = usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if( USB_E_QOVR == qerr )
    {
        return USB_HUB_QOVR;
    }

    return USB_DONE;
}   /* eof R_usb_hhub_GetHubInformation() */


/******************************************************************************
Function Name   : R_usb_hhub_GetPortInformation
Description     : GetStatus request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : USB_CB_t complete
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t R_usb_hhub_GetPortInformation(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, USB_CB_t complete)
{
    USB_ER_t        qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_GET_STATUS| USB_DEV_TO_HOST | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = 0;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;     /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 4;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;  /* Device address */

    /* Port GetStatus */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if( USB_E_QOVR == qerr )
    {
        return USB_HUB_QOVR;
    }

    return USB_DONE;
}   /* eof R_usb_hhub_GetPortInformation() */


/******************************************************************************
Function Name   : R_usb_hhub_get_hub_addr
Description     : Get connected Hub address from device address.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t devadr           : Device address
Return          : 1-10:Hub address / USB_ERROR:Device address error or Hub no connect.
******************************************************************************/
uint16_t R_usb_hhub_get_hub_addr(USB_UTR_t *ptr, uint16_t devadr)
{
    uint16_t ret;

    /* Device address check */
    if( ( USB_MAXDEVADDR < devadr ) || ( 0 == devadr ) )
    {
        ret = USB_ERROR;                /* Device address error */
    }
    else
    {
        /* Set Hub address */
        ret = usb_shhub_InfoData[ptr->ip][devadr].up_addr;
        /* Hub address check */
        if( ( USB_MAXDEVADDR < ret ) || ( 0 == ret ) )
        {
            ret = USB_ERROR;            /* Hub address error */
        }
    }

    return ret;
} /* eof R_usb_hhub_get_hub_addr */

/******************************************************************************
Function Name   : R_usb_hhub_get_hub_port_no
Description     : Get Hub connected port no. from device address.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t devadr           : Device address
Return          : 1-4:Hub port no. / USB_ERROR:Device address error or Hub no connect.
******************************************************************************/
uint16_t R_usb_hhub_get_hub_port_no(USB_UTR_t *ptr, uint16_t devadr)
{
    uint16_t ret;

    /* Device address check */
    if( ( USB_MAXDEVADDR < devadr ) || ( 0 == devadr ) )
    {
        ret = USB_ERROR;                /* Device address error */
    }
    else
    {
        /* Set Hub port no */
        ret = usb_shhub_InfoData[ptr->ip][devadr].up_port_num;
        /* Hub port no check */
        if( ( USB_HUBDOWNPORT < ret ) || ( 0 == ret ) )
        {
            ret = USB_ERROR;            /* Hub port no error */
        }
    }

    return ret;
} /* eof R_usb_hhub_get_hub_port_no */

/******************************************************************************
Function Name   : R_usb_hhub_chk_connect_status
Description     : Device connect check of after GET_STATUS(Get Port Status) complete.
Argument        : USB_UTR_t *ptr    : IP info (mode, IP no, reg. address).
                : uint16_t hub_adr          : Hub address
Return          : USB_DEV_NO_CONNECT:No device is present.
                : USB_DEV_CONNECTED:A device is present on this port.
                : USB_ERROR: Hub address error.
******************************************************************************/
uint16_t R_usb_hhub_chk_connect_status(USB_UTR_t *ptr, uint16_t hub_adr)
{
    uint16_t ret;

    /* Hub address check */
    if( ( USB_MAXDEVADDR < hub_adr ) || ( 0 == hub_adr ) )
    {
        ret = USB_ERROR;                /* Hub address error */
    }
    else
    {
        /* Port Status : Current Connect Status */
        if( usb_ghhub_Data[ptr->ip][hub_adr][0] & USB_BIT0 )
        {
            ret = USB_DEV_CONNECTED;    /* A device is present on this port. */
        }
        else
        {
            ret = USB_DEV_NO_CONNECT;   /* No device is present. */
        }
    }

    return ret;
} /* eof R_usb_hhub_chk_connect_status */


/******************************************************************************
Renesas Abstracted Hub Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hhub_Task
Description     : HUB task
Arguments       : USB_VP_INT stacd          : Start Code of Hub Task
Return value    : none
******************************************************************************/
void usb_hhub_Task(USB_VP_INT stacd)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t        elseport;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
#ifdef FREE_RTOS_PP
    for( ;; )
	{
#endif
    /* Receive message */
    err = USB_TRCV_MSG(USB_HUB_MBX, (USB_MSG_t**)&mess, (USB_TM_t)0);
    if( USB_OK != err )
    {
#ifdef FREE_RTOS_PP
		continue;
#else
        return;
#endif
    }

    switch( mess->msginfo )
    {
    case USB_MSG_CLS_CHECKREQUEST:
        /* USB HUB Class Enumeration */
        usb_hhub_enumeration((USB_CLSINFO_t *) mess);
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {
            /* Release Memoryblock failure */
            USB_PRINTF0("### USB HUB Task rel_blk error\n");
        }
        break;

    case USB_MSG_CLS_INIT:
        /* Down port initialize */
        usb_hhub_InitDownPort(mess, (uint16_t)0, (USB_CLSINFO_t *)mess);
        break;
    /* Enumeration waiting of other device */
    case USB_MSG_CLS_WAIT:
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
        elseport = 0;
        if( 0 == mess->keyword )
        {
            elseport = 1;
        }
        if( usb_ghstd_MgrMode[mess->ip][elseport] != USB_DEFAULT )
        {
            mess->msginfo = USB_MSG_MGR_AORDETACH;
            err = USB_SND_MSG( USB_MGR_MBX, (USB_MSG_t*)mess );
            if( USB_OK != err )
            {
                /* Send Message failure */
                USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
            }
        }
        else
        {
            err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)mess);
            if( USB_OK != err )
            {
                /* Send Message failure */
                USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
            }
        }
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
        mess->msginfo = USB_MSG_MGR_AORDETACH;
        err = USB_SND_MSG( USB_MGR_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            /* Send Message failure */
            USB_PRINTF0("### USB HUB enuwait snd_msg error\n");
        }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
        break;

    case USB_MSG_HUB_EVENT:
        usb_hhub_event((USB_CLSINFO_t *) mess);
        break;

    case USB_MSG_HUB_ATTACH:
        /* Hub Port attach */
        usb_hhub_PortAttach((uint16_t)0, (uint16_t)0, (USB_CLSINFO_t *) mess);
        break;

    case USB_MSG_HUB_RESET:
        /* Hub Reset */
        usb_hhub_port_reset(mess, (uint16_t)0, (uint16_t)0, (USB_CLSINFO_t *) mess);
        break;

    default:
        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {
            USB_PRINTF0("### USB HUB rel_blk error\n");
        }
        break;
    }
#ifdef FREE_RTOS_PP
	}
#endif
}   /* eof usb_hhub_Task() */


/******************************************************************************
Function Name   : usb_hhub_enumeration
Description     : USB HUB Class Enumeration
Arguments       : USB_CLSINFO_t *ptr    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_enumeration(USB_CLSINFO_t *ptr)
{
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
    uint8_t     pdata[32];
    uint16_t    j;
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
    uint16_t    checkerr,retval;
    uint8_t     string;
    uint16_t    *table[8];

    checkerr = ptr->result;
    table[0] = (uint16_t*)usb_shhub_DeviceTable[ptr->ip];
    table[1] = (uint16_t*)usb_shhub_ConfigTable[ptr->ip];
    table[2] = (uint16_t*)usb_shhub_InterfaceTable[ptr->ip];

    /* Manager Mode Change */
    switch( usb_shhub_ClassSeq[ptr->ip] )
    {
    case USB_SEQ_0:
        checkerr = USB_DONE;
        /* Descriptor check */
        retval = usb_hhub_ChkConfig((uint16_t**)&table, (uint16_t)usb_shhub_Spec[ptr->ip]);
        if( USB_ERROR == retval )
        {
            USB_PRINTF0("### Configuration descriptor error !\n");
            checkerr = USB_ERROR;
            break;
        }
        /* Interface Descriptor check */
        retval = usb_hhub_ChkInterface((uint16_t**)&table, (uint16_t)usb_shhub_Spec[ptr->ip]);
        if( USB_ERROR == retval )
        {
            USB_PRINTF0("### Interface descriptor error !\n");
            checkerr = USB_ERROR;
            break;
        }

        /* String descriptor */
        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
        usb_hstd_GetStringDescriptor1( ptr, usb_shhub_DevAddr[ptr->ip], (uint16_t)15,
                                       (USB_CB_t)&usb_hhub_class_request_complete);
        break;

    case USB_SEQ_1:
        /* String descriptor check */
        retval = usb_hstd_GetStringDescriptor1Check(checkerr);
        if( USB_DONE == retval )
        {
            string = usb_shhub_DeviceTable[ptr->ip][15];
            /* String descriptor */
            usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
            usb_hstd_GetStringDescriptor2( ptr, usb_shhub_DevAddr[ptr->ip], (uint16_t)string,
                                           (USB_CB_t)&usb_hhub_class_request_complete);
        }
        else
        {
            /* If USB_ERROR, Go case 3 (checkerr==USB_ERROR) */
            usb_shhub_ClassSeq[ptr->ip] = USB_SEQ_2;
            /* Next sequence */
            usb_hhub_CheckRequest(ptr, USB_ERROR);
            checkerr = USB_DONE;
        }
        break;
        
    case USB_SEQ_2:
        /* String descriptor check */
        retval = usb_hstd_GetStringDescriptor2Check(checkerr);
        if( USB_DONE == retval )
        {
            /* Next sequence */
            usb_hhub_CheckRequest(ptr, checkerr);
        }
        break;
    
    case USB_SEQ_3:
        /* String descriptor check */
        if( checkerr == USB_DONE )
        {
            if( usb_ghstd_ClassData[ptr->ip][0] < (uint8_t)(32 * 2 + 2) )
            {
                usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)(usb_ghstd_ClassData[ptr->ip][0] / 2);
                usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)(usb_ghstd_ClassData[ptr->ip][0] - 1);
            }
            else
            {
                usb_ghstd_ClassData[ptr->ip][0] = (uint8_t)32;
            }
/* Condition compilation by the difference of useful function */
#if defined(USB_DEBUGUART_PP) || defined(USB_DEBUGPRINT_PP)
            for( j = (uint16_t)0; j < usb_ghstd_ClassData[ptr->ip][0]; j++ )
            {
                pdata[j] = usb_ghstd_ClassData[ptr->ip][j * (uint16_t)2 + (uint16_t)2];
            }
            pdata[usb_ghstd_ClassData[ptr->ip][0]] = 0;
            USB_PRINTF1("    Product name : %s\n", pdata);
#endif  /* USB_DEBUGUART_PP || USB_DEBUGPRINT_PP */
        }
        else
        {
            USB_PRINTF0("*** Product name error\n");
            checkerr = USB_DONE;
        }
        
        /* Get HUB descriptor */
        usb_shhub_Process[ptr->ip] = USB_MSG_CLS_CHECKREQUEST;
        checkerr = R_usb_hhub_GetHubInformation( ptr, usb_shhub_DevAddr[ptr->ip], usb_hhub_class_request_complete );
        /* Submit overlap error */
        if( USB_HUB_QOVR == checkerr )
        {
            usb_hhub_SpecifiedPathWait(ptr, (uint16_t)10);
        }
        break;
        
    case USB_SEQ_4:
        /* Get HUB descriptor Check */
        retval = usb_hhub_request_result(checkerr);
        if( USB_DONE == retval )
        {
            usb_hhub_CheckRequest(ptr, checkerr);   /* Next sequence */
        }
        break;
        
    case USB_SEQ_5:
        /* Get HUB descriptor Check */
        if( checkerr == USB_DONE )
        {
            retval = usb_hstd_CheckDescriptor( usb_ghhub_Descriptor[ptr->ip], (uint16_t)USB_DT_HUBDESCRIPTOR );
            if( USB_ERROR == retval )
            {
                USB_PRINTF0("### HUB descriptor error !\n");
                checkerr = USB_ERROR;
            }
            else if( usb_ghhub_Descriptor[ptr->ip][2] > USB_HUBDOWNPORT )
            {
                USB_PRINTF0("### HUB Port number over\n");
                checkerr = USB_ERROR;
            }
            else
            {
                USB_PRINTF1("    Attached %d port HUB\n", usb_ghhub_Descriptor[ptr->ip][2]);
            }
        }
        else
        {
            USB_PRINTF0("### HUB Descriptor over\n");
            checkerr = USB_ERROR;
        }

        /* Pipe Information table set */
        switch( usb_shhub_Spec[ptr->ip] )
        {
        case USB_FSHUB:     /* Full Speed Hub */
            if( USB_FSCONNECT == usb_shhub_Speed[ptr->ip] )
            {
                retval = usb_hhub_pipe_info( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( USB_ERROR == retval )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;

        case USB_HSHUBS:    /* Hi Speed Hub(Single) */
            if( USB_HSCONNECT == usb_shhub_Speed[ptr->ip] )
            {
                retval = usb_hhub_pipe_info( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( USB_ERROR == retval )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;

        case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
            if( USB_HSCONNECT == usb_shhub_Speed[ptr->ip] )
            {
                /* Set pipe information */
                retval = usb_hhub_pipe_info( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( USB_ERROR == retval )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr = USB_ERROR;
                }
                /* Set pipe information */
                retval = usb_hhub_pipe_info( ptr, (uint8_t *)usb_shhub_InterfaceTable[ptr->ip],
                                            usb_shhub_Index[ptr->ip], usb_shhub_Speed[ptr->ip],
                                            (uint16_t)usb_shhub_ConfigTable[ptr->ip][2]);
                if( USB_ERROR == retval )
                {
                    USB_PRINTF0("### Device information error(HUB) !\n");
                    checkerr= USB_ERROR;
                }
            }
            else
            {
                USB_PRINTF0("### HUB Descriptor speed error\n");
                checkerr = USB_ERROR;
            }
            break;
        default:
            checkerr = USB_ERROR;
            break;
        }
        /* Port number set */
        usb_shhub_InfoData[ptr->ip][usb_shhub_DevAddr[ptr->ip]].port_num = usb_ghhub_Descriptor[ptr->ip][2];
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, checkerr);
        break;
    }

    switch( checkerr )
    {
    /* Driver open */
    case USB_DONE:
        usb_shhub_ClassSeq[ptr->ip]++;
        break;

    /* Submit overlap error */
    case USB_HUB_QOVR:
        break;

    /* Descriptor error */
    case USB_ERROR:
        USB_PRINTF0("### Enumeration is stoped(ClassCode-ERROR)\n");
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, USB_ERROR);
        break;

    default:
        usb_shhub_Process[ptr->ip] = USB_NONE;
        /* Return to MGR */
        R_usb_hstd_ReturnEnuMGR((USB_UTR_t *)ptr, USB_ERROR);
        break;
    }
}   /* eof usb_hhub_enumeration() */


/******************************************************************************
Function Name   : usb_hhub_InitDownPort
Description     : Down port initialized
Arguments       : uint16_t hubaddr          : hub address
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_InitDownPort(USB_UTR_t *ptr, uint16_t hubaddr, USB_CLSINFO_t *mess)
{
    USB_ER_t        err;
    uint16_t        retval;

    hubaddr = usb_shhub_HubAddr[ptr->ip];
    retval = USB_DONE;

    if( USB_MSG_CLS_INIT != usb_shhub_Process[ptr->ip] )        /* Check Hub Process */
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            USB_PRINTF0("### HUB snd_msg error\n");             /* Send Message failure */
        }
    }
    else
    {
        switch( usb_shhub_InitSeq[ptr->ip] )
        {
        case USB_SEQ_0:                                         /* HUB port power */
            hubaddr = mess->keyword;
            usb_shhub_HubAddr[ptr->ip] = hubaddr;

            usb_hstd_DeviceDescripInfo(ptr);
            usb_hstd_ConfigDescripInfo(ptr);
            USB_PRINTF0("\nHHHHHHHHHHHHHHHHHHHHHHHHH\n");
            USB_PRINTF0("         USB HOST        \n");
            USB_PRINTF0("      HUB CLASS DEMO     \n");
            USB_PRINTF0("HHHHHHHHHHHHHHHHHHHHHHHHH\n\n");
            usb_shhub_InitSeq[ptr->ip] = USB_SEQ_1;             /* Next Sequence */
            usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;
            usb_hhub_SpecifiedPath(mess);                       /* Next Process Selector */
            break;

        case USB_SEQ_1:                                         /* Request */
            retval = usb_hhub_PortSetFeature( ptr, hubaddr, usb_shhub_InitPort[ptr->ip],
                (uint16_t)USB_HUB_PORT_POWER, usb_hhub_class_request_complete);    /* SetFeature request */
            if( USB_HUB_QOVR == retval )                        /* Submit overlap error */
            {
                usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
            }
            else
            {
                usb_shhub_InitPort[ptr->ip]++;                  /* Next Port */
                usb_shhub_InitSeq[ptr->ip] = USB_SEQ_2;         /* Next Sequence */
            }
            break;

        case USB_SEQ_2:                                         /* Request Result Check */
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                if( usb_shhub_InitPort[ptr->ip] > usb_shhub_InfoData[ptr->ip][hubaddr].port_num )
                {
                    usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;   /* Port Clear */
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_3;     /* Next Sequence */
                    usb_hhub_SpecifiedPath(mess);               /* Next Process Selector */
                }
                else
                {
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_1;     /* Loop Sequence */
                    usb_hhub_SpecifiedPath(mess);               /* Next Process Selector */
                }
            }
            break;

        case USB_SEQ_3:                                         /* HUB downport initialize */
            retval = usb_hhub_PortClrFeature( ptr,hubaddr, usb_shhub_InitPort[ptr->ip],
                (uint16_t)USB_HUB_C_PORT_CONNECTION, usb_hhub_class_request_complete);      /* Request */
            if( USB_HUB_QOVR == retval )                        /* Submit overlap error */
            {
                usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
            }
            else
            {
                usb_shhub_InitPort[ptr->ip]++;                  /* Next Port */
                usb_shhub_InitSeq[ptr->ip] = USB_SEQ_4;         /* Next Sequence */
            }
            break;

        /* Request Result Check */
        case USB_SEQ_4:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                if( usb_shhub_InitPort[ptr->ip] > usb_shhub_InfoData[ptr->ip][hubaddr].port_num )
                {
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_0;             /* Sequence Clear */
                    usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;           /* Port Clear */
                    usb_shhub_Info[ptr->ip] = USB_MSG_CLS_INIT;
                    usb_shhub_Process[ptr->ip] = USB_MSG_HUB_EVENT;     /* Next Attach Process */
                    usb_hhub_SpecifiedPath(mess);                       /* Next Process Selector */
                }
                else
                {
                    usb_shhub_InitSeq[ptr->ip] = USB_SEQ_3;             /* Loop Sequence */
                    usb_hhub_SpecifiedPath(mess);                       /* Next Process Selector */
                }
            }
            break;

        default:
            retval = USB_NG;
            break;
        }

        if( ( USB_DONE != retval ) && ( USB_HUB_QOVR != retval ) ) 
        {
            usb_shhub_InitPort[ptr->ip] = USB_HUB_P1;                   /* Port Clear */
            usb_shhub_InitSeq[ptr->ip] = USB_SEQ_0;                     /* Sequence Clear */
            usb_shhub_Info[ptr->ip] = USB_NONE;                         /* Clear */
            usb_shhub_Process[ptr->ip] = USB_NONE;                      /* Clear */
        }

        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {                                                       /* Release Memoryblock failure */
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
}   /* eof usb_hhub_InitDownPort() */


/******************************************************************************
Function Name   : usb_hhub_PortAttach
Description     : port attach
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : uint16_t                  : Manager Mode
******************************************************************************/
uint16_t usb_hhub_PortAttach(uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
    uint16_t        rootport, devaddr, retval;
    uint16_t        hpphub, hubport, buffer;
    USB_ER_t        err;
    USB_UTR_t       *ptr;

    ptr     = (USB_UTR_t *)mess;
    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_EventPort[ptr->ip];

    if( USB_MSG_HUB_ATTACH != usb_shhub_Process[ptr->ip] )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            /* Send Message failure */
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch( usb_shhub_AttachSeq[ptr->ip] )
        {
        case USB_SEQ_0:
            if( USB_NULL == usb_gcstd_Pipe[ptr->ip][USB_PIPE0] )
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_1;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_RESET;
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
            }
            /* Next Process Selector */
            usb_hhub_SpecifiedPath(mess);
            break;

        case USB_SEQ_1:
            /* Device Enumeration */
            switch( (uint16_t)(usb_ghhub_Data[ptr->ip][hubaddr][1] & (uint8_t)0x06) )
            {
            case 0x00:
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_FSCONNECT;
                USB_PRINTF0(" Full-Speed Device\n");
                break;
            case 0x02:
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_LSCONNECT;
                USB_PRINTF0(" Low-Speed Device\n");
                break;
            case 0x04:
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_HSCONNECT;
                USB_PRINTF0(" Hi-Speed Device\n");
                break;
            default:
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_NOCONNECT;
                USB_PRINTF0(" Detach Detached\n");
                break;
            }
            rootport = usb_hstd_GetRootport(ptr, 
                (uint16_t)(hubaddr << USB_DEVADDRBIT));
            /* Now downport device search */
            devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, portnum);
            usb_ghstd_DeviceAddr[ptr->ip] = devaddr;
            devaddr = (uint16_t)(devaddr << USB_DEVADDRBIT);
            usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
            if( 0 != devaddr )
            {
                usb_hstd_SetHubPort(ptr, devaddr, (uint16_t)(hubaddr << 11), (uint16_t)(portnum << 8));
                /* Get DEVADDR register */
                buffer = usb_hreg_read_devadd( ptr, devaddr );
                do
                {
                    hpphub  = (uint16_t)(buffer & USB_UPPHUB);
                    hubport = (uint16_t)(buffer & USB_HUBPORT);
                    devaddr = (uint16_t)(hpphub << 1);
                    /* Get DEVADDR register */
                    buffer = usb_hreg_read_devadd( ptr, devaddr );
                }
                while( ( USB_HSCONNECT != ( buffer & USB_USBSPD ) ) && ( USB_DEVICE_0 != devaddr ) );

                usb_hstd_SetDevAddr(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
                /* Set up-port hub */
                usb_hstd_SetHubPort(ptr, (uint16_t)USB_DEVICE_0, hpphub, hubport);
                /* Set up-port hub */
                usb_hstd_SetHubPort(ptr, (uint16_t)(usb_ghstd_DeviceAddr[ptr->ip] << USB_DEVADDRBIT), hpphub, hubport);
                /* Clear Enumeration Sequence Number */
                usb_ghstd_EnumSeq[ptr->ip] = 0;
                if( USB_NOCONNECT != usb_ghstd_DeviceSpeed[ptr->ip] )
                {
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_2;
                    usb_hhub_SpecifiedPathWait(mess, 3u);
                }
                else
                {
                    usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_3;
                    /* Next Process Selector */
                    usb_hhub_SpecifiedPath(mess);
                }
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_3;
                /* Next Process Selector */
                usb_hhub_SpecifiedPath(mess);
            }
            break;

        case USB_SEQ_2:
            /* Get Port Number */
            rootport = usb_hstd_GetRootport(ptr, (uint16_t)(hubaddr << USB_DEVADDRBIT));
            if( ( USB_CONFIGURED == usb_ghstd_MgrMode[ptr->ip][rootport] )
             || ( USB_DEFAULT != usb_ghstd_MgrMode[ptr->ip][rootport] ) )
            {
                /* HUB downport status */
                usb_shhub_DownPort[ptr->ip][hubaddr] |= USB_BITSET(portnum);
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_EVENT;
                usb_hhub_SpecifiedPath(mess);
            }
            else
            {
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_2;
                usb_hhub_SpecifiedPathWait(mess, 3u);
            }
            break;

        case USB_SEQ_3:
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_4;
            usb_shhub_Process[ptr->ip] = USB_MSG_HUB_RESET;
            /* Next Process Selector */
            usb_hhub_SpecifiedPath(mess);
            break;

        case USB_SEQ_4:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                /* Hub Port Set Feature Request */
                retval = usb_hhub_PortSetFeature( ptr, hubaddr, portnum, USB_HUB_PORT_SUSPEND,
                                                  usb_hhub_class_request_complete);
                /* Submit overlap error */
                if( USB_HUB_QOVR == retval )
                {
                    usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
                }
                else
                {
                    usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_5;
                }
            }
            break;

        case USB_SEQ_5:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                usb_hhub_port_detach(ptr, hubaddr, portnum);
                usb_shhub_InfoData[ptr->ip][devaddr].up_addr     = 0;            /* Up-hubaddr clear */
                usb_shhub_InfoData[ptr->ip][devaddr].up_port_num = 0;            /* Up-hubport clear */
                usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_EVENT;
                usb_hhub_SpecifiedPath(mess);
            }
            break;

        default:
            usb_shhub_AttachSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_NONE;
            usb_shhub_Info[ptr->ip] = USB_NONE;
            break;
        }

        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
    return USB_DONE;
}   /* eof usb_hhub_PortAttach() */


/******************************************************************************
Function Name   : usb_hhub_event
Description     : USB Hub Event process.
Arguments       : USB_CLSINFO_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_event(USB_CLSINFO_t *mess)
{
    USB_ER_t        err;
    uint16_t        hubaddr, devaddr, retval;
    USB_UTR_t       *ptr;
    uint32_t        port_status;
    uint16_t        next_port_check = USB_NO;
    uint16_t        port_clr_feature_type;

    ptr = (USB_UTR_t *)mess;
    hubaddr = usb_shhub_HubAddr[ptr->ip];

    if( USB_MSG_HUB_EVENT != usb_shhub_Process[ptr->ip] )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch( usb_shhub_EventSeq[ptr->ip] )
        {
        case USB_SEQ_0:        /* Request */
            if( USB_MSG_HUB_SUBMITRESULT == usb_shhub_Info[ptr->ip] )        /* Event Check */
            {
                /* Hub and Port Status Change Bitmap(b0:Hub,b1:DownPort1change detected,b2:DownPort2,...) */
                if( 0 != ( usb_ghhub_Data[ptr->ip][hubaddr][0] & USB_BITSET(usb_shhub_EventPort[ptr->ip]) ) )
                {
                    USB_PRINTF1(" *** HUB port %d \t",usb_shhub_EventPort[ptr->ip]);
                    /* GetStatus request */
                    retval = R_usb_hhub_GetPortInformation( ptr, hubaddr, usb_shhub_EventPort[ptr->ip],
                                                            usb_hhub_class_request_complete );
                    if( USB_HUB_QOVR == retval )                            /* Submit overlap error */
                    {
                        usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );   /* Retry */
                    }
                    else
                    {
                        usb_shhub_EventSeq[ptr->ip] = USB_SEQ_1;
                    }
                }
                else
                {
                    /* Port check end */
                    next_port_check = USB_YES;
                }
            }
            else
//            if( USB_MSG_CLS_INIT == usb_shhub_Info[ptr->ip] )           /* Event Check */
            {                                                           /* USB_MSG_HUB_INIT */
                USB_PRINTF2(" *** address %d downport %d \t", hubaddr, usb_shhub_EventPort[ptr->ip]);

                /* GetStatus request */
                retval = R_usb_hhub_GetPortInformation(ptr, hubaddr, usb_shhub_EventPort[ptr->ip],
                                                        usb_hhub_class_request_complete);
                if( USB_HUB_QOVR == retval )                            /* Submit overlap error */
                {
                    usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );   /* Retry */
                }
                else
                {
                    usb_shhub_EventSeq[ptr->ip] = USB_SEQ_3;
                }
            }
            break;

        case USB_SEQ_1:        /* Request Result Check & Request */
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                port_status = (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][0];
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][1] << 8;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][2] << 16;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][3] << 24;
                USB_PRINTF2(" [port/status] : %d, 0x%08x\n", usb_shhub_EventPort[ptr->ip], port_status);

                if( 0 != ( port_status & USB_BIT_C_PORT_CONNECTION ) )      /* C_PORT_CONNECTION */
                {
                    retval = usb_hhub_PortClrFeature( ptr, hubaddr, usb_shhub_EventPort[ptr->ip],
                                    (uint16_t)USB_HUB_C_PORT_CONNECTION, usb_hhub_class_request_complete );
                    if( USB_HUB_QOVR == retval )                            /* Submit overlap error */
                    {
                        usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );   /* Retry */
                    }
                    else
                    {
                        usb_shhub_EventSeq[ptr->ip] = USB_SEQ_3;            /* Attach Sequence */
                    }
                }
                else
                {
                    /* Now downport device search */
                    devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, usb_shhub_EventPort[ptr->ip]);
                    if( 0 != ( port_status & USB_BIT_PORT_ENABLE ) )            /* PORT_ENABLE */
                    {
                        USB_PRINTF1(" Hubport error address%d\n",devaddr);
                        port_clr_feature_type = USB_HUB_C_PORT_ENABLE;          /* C_PORT_ENABLE */
                    }
                    else if( 0 != ( port_status & USB_BIT_PORT_SUSPEND ) )      /* PORT_SUSPEND */
                    {
                        USB_PRINTF1(" Hubport suspend(resume complete) address%d\n", devaddr);
                        port_clr_feature_type = USB_HUB_C_PORT_SUSPEND;         /* C_PORT_SUSPEND */
                    }
                    else if( 0 != (  port_status & USB_BIT_C_PORT_OVER_CURRENT ) )    /* PORT_OVER_CURRENT */
                    {
                        USB_PRINTF1(" Hubport over current address%d\n", devaddr);
                        port_clr_feature_type = USB_HUB_C_PORT_OVER_CURRENT;    /* C_PORT_OVER_CURRENT */
                    }
                    else if( 0 != ( port_status & USB_BIT_PORT_RESET ) )        /* PORT_RESET */
                    {
                        USB_PRINTF1(" Hubport reset(reset complete) address%d\n", devaddr);
                        port_clr_feature_type = USB_HUB_C_PORT_RESET;           /* C_PORT_RESET */
                    }
                    else
                    {
                        next_port_check = USB_YES;
                    }

                    if( USB_NO == next_port_check )
                    {
                        /* ClearFeature request */
                        retval = usb_hhub_PortClrFeature( ptr, hubaddr, usb_shhub_EventPort[ptr->ip],
                                                          port_clr_feature_type, usb_hhub_class_request_complete );
                        if( USB_HUB_QOVR == retval )                            /* Submit overlap error */
                        {
                            usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );   /* Retry */
                        }
                        else
                        {
                            usb_shhub_EventSeq[ptr->ip] = USB_SEQ_2;            /* Next Sequence */
                        }
                    }
                }
            }
            break;

        case USB_SEQ_2:        /* Request Result Check */
            retval = usb_hhub_request_result( mess->result );
            if( USB_DONE == retval )
            {
                if( 0 != ( port_status & USB_BIT_PORT_SUSPEND ) )       /* PORT_SUSPEND */
                {                                                       /* C_PORT_SUSPEND */
                    /* HUB downport status */
                    usb_shhub_Remote[ptr->ip][hubaddr] |= USB_BITSET(usb_shhub_EventPort[ptr->ip]);
                }
                next_port_check = USB_YES;
            }
            break;

        case USB_SEQ_3:        /* Attach Sequence */
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                port_status = (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][0];
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][1] << 8;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][2] << 16;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][3] << 24;
                USB_PRINTF2(" [port/status] : %d, 0x%08x\n", usb_shhub_EventPort[ptr->ip], port_status);

                if( 0 != ( port_status & USB_BIT_PORT_CONNECTION ) )                    /* PORT_CONNECTION */
                {
                    if( USB_MSG_HUB_SUBMITRESULT == usb_shhub_Info[ptr->ip] )
                    {
                        if( 0 == ( usb_shhub_DownPort[ptr->ip][hubaddr] & USB_BITSET(usb_shhub_EventPort[ptr->ip] ) ) )
                        {
                            usb_shhub_EventSeq[ptr->ip] = USB_SEQ_4;                    /* Next Attach sequence */
                            usb_hhub_new_connect( mess, (uint16_t)0, (uint16_t)0, mess );
                        }
                        else
                        {
                            next_port_check = USB_YES;                                  /* Not PORT_CONNECTION */
                        }
                    }
                    else
//                    if( USB_MSG_CLS_INIT == usb_shhub_Info[ptr->ip] )
                    {
                        usb_shhub_EventSeq[ptr->ip] = USB_SEQ_4;                        /* Next Attach sequence */
                        usb_hhub_new_connect( mess, (uint16_t)0, (uint16_t)0, mess );
                    }
                }
                else
                {                                                                       /* non connect */
                    if( USB_MSG_HUB_SUBMITRESULT == usb_shhub_Info[ptr->ip] )           /*  */
                    {
                        /* Now downport device search */
                        devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, usb_shhub_EventPort[ptr->ip]);
                        if( 0 != devaddr )
                        {
                            usb_hhub_port_detach(ptr, hubaddr, usb_shhub_EventPort[ptr->ip]);
                            USB_PRINTF1(" Hubport disconnect address%d\n", devaddr);
                            usb_shhub_InfoData[ptr->ip][devaddr].up_addr = 0;            /* Up-address clear */
                            usb_shhub_InfoData[ptr->ip][devaddr].up_port_num = 0;        /* Up-port num clear */
                            usb_shhub_InfoData[ptr->ip][devaddr].port_num = 0;           /* Port number clear */
                            usb_shhub_InfoData[ptr->ip][devaddr].pipe_num = 0;           /* Pipe number clear */
                        }
                    }
                    next_port_check = USB_YES;
                }
            }
            break;

        case USB_SEQ_4:        /* Attach */
            next_port_check = USB_YES;
            break;

        default:
            /* error */
            break;
        }

        if( USB_YES == next_port_check )
        {
            if( usb_shhub_EventPort[ptr->ip] >= usb_shhub_InfoData[ptr->ip][hubaddr].port_num )
            {   /* Port check end */
                usb_hhub_trans_start(ptr, hubaddr, (uint32_t)1, &usb_ghhub_Data[ptr->ip][hubaddr][0],
                                    &usb_hhub_trans_complete);     /* Get Hub and Port Status Change Bitmap */

                usb_shhub_EventPort[ptr->ip] = USB_HUB_P1;      /* Port Clear */
                usb_shhub_EventSeq[ptr->ip] = USB_SEQ_0;        /* Sequence Clear */
                usb_shhub_Process[ptr->ip] = USB_NONE;
                usb_shhub_Info[ptr->ip] = USB_NONE;
            }
            else
            {
                usb_shhub_EventPort[ptr->ip]++;                 /* Next port check */
                usb_shhub_EventSeq[ptr->ip] = USB_SEQ_0;        /* Sequence Clear */
                usb_hhub_SpecifiedPath(mess);
            }
        }

        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
}   /* eof usb_hhub_event() */


/******************************************************************************
Function Name   : usb_hhub_port_reset
Description     : HUB down port USB-reset request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_port_reset(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
    USB_ER_t        err;
    uint16_t        retval;
    uint32_t        port_status;

    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_EventPort[ptr->ip];

    if( usb_shhub_Process[ptr->ip] != USB_MSG_HUB_RESET )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        switch( usb_shhub_ResetSeq[ptr->ip] )
        {
        case USB_SEQ_0:
            /* Hub port SetFeature */
            usb_cpu_DelayXms((uint16_t)100);
            retval = usb_hhub_PortSetFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_PORT_RESET,
                                              usb_hhub_class_request_complete);
            /* Submit overlap error */
            if( USB_HUB_QOVR == retval )
            {
                usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
            }
            else
            {
                usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_1;
            }
            break;

        case USB_SEQ_1:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                usb_cpu_DelayXms((uint16_t)60);

                /* Get Status */
                retval = R_usb_hhub_GetPortInformation( ptr, hubaddr, portnum, usb_hhub_class_request_complete );
                /* Submit overlap error */
                if( USB_HUB_QOVR == retval )
                {
                    usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
                }
                else
                {
                    usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_2;
                }
            }
            break;

        case USB_SEQ_2:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                port_status = (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][0];
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][1] << 8;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][2] << 16;
                port_status |= (uint32_t)usb_ghhub_Data[ptr->ip][hubaddr][3] << 24;
                /* Check Reset Change(C_PORT_RESET) */
                if( USB_BIT_C_PORT_RESET != ( port_status & USB_BIT_C_PORT_RESET ) )
                {
                    usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_0;
                    usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );                }
                else
                {
                    /* Hub port ClearFeature */
                    usb_cpu_DelayXms((uint16_t)20);

                    retval = usb_hhub_PortClrFeature( ptr, hubaddr, portnum, (uint16_t)USB_HUB_C_PORT_RESET,
                                                      usb_hhub_class_request_complete );
                    /* Submit overlap error */
                    if( USB_HUB_QOVR == retval )
                    {
                        usb_hhub_SpecifiedPathWait( mess, (uint16_t)10 );
                    }
                    else
                    {
                        usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_3;
                    }
                }
            }
            break;

        case USB_SEQ_3:
            retval = usb_hhub_request_result(mess->result);
            if( USB_DONE == retval )
            {
                usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_0;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_ATTACH;
                usb_hhub_SpecifiedPath(mess);
            }
            break;

        default:
            usb_shhub_ResetSeq[ptr->ip] = USB_SEQ_0;
            usb_shhub_Process[ptr->ip] = USB_NONE;
            break;
        }

        err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)mess);
        if( USB_OK != err )
        {
            USB_PRINTF0("### USB HostHubClass rel_blk error\n");
        }
    }
}   /* eof usb_hhub_port_reset() */


/******************************************************************************
Function Name   : usb_hhub_release
Description     : HUB driver release
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hhub_release(USB_UTR_t *ptr)
{
    uint16_t    i;

    for( i = 0; i < USB_MAXHUB; i++ )
    {
        /* Hub driver release */
        R_usb_hstd_DriverRelease(ptr, (uint8_t)USB_IFCLS_HUB);
    }
}   /* eof usb_hhub_release() */


/******************************************************************************
Function Name   : usb_hhub_check_class
Description     : HUB Class driver check
Arguments       : uint16_t **table          : Descriptor, etc
Return value    : none
******************************************************************************/
void usb_hhub_check_class(USB_UTR_t *ptr, uint16_t **table)
{
    USB_CLSINFO_t   *p_blf;
    USB_CLSINFO_t   *cp;
    USB_ER_t err;
    
    usb_shhub_DeviceTable[ptr->ip]      = (uint8_t*)(table[0]);
    usb_shhub_ConfigTable[ptr->ip]      = (uint8_t*)(table[1]);
    usb_shhub_InterfaceTable[ptr->ip]   = (uint8_t*)(table[2]);
    *table[3]                           = USB_DONE;
    usb_shhub_Spec[ptr->ip]             = *table[4];
    usb_shhub_Root[ptr->ip]             = *table[5];
    usb_shhub_Speed[ptr->ip]            = *table[6];
    usb_shhub_DevAddr[ptr->ip]          = *table[7];
    usb_shhub_Index[ptr->ip]            = usb_hhub_ChkTblIndx1(ptr, usb_shhub_DevAddr[ptr->ip]);
    
    usb_shhub_ClassSeq[ptr->ip]=0;

    /* Get mem pool blk */
    if( USB_OK == USB_PGET_BLK( USB_HUB_MPL, &p_blf ) )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_CHECKREQUEST;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;
        
        /* Enumeration wait TaskID change */
        R_usb_hstd_EnuWait(ptr, (uint8_t)USB_HUB_TSK);
        
        /* Class check of enumeration sequence move to class function */
        err = USB_SND_MSG(USB_HUB_MBX, (USB_MSG_t*)cp);
        if( USB_OK != err )
        {
            /* Send Message failure */
            USB_PRINTF1("Host HUB snd_msg error %x\n", err);
        }
    }
    else
    {
        /* Send Message failure */
        while( 1 );
    }
}   /* eof usb_hhub_check_class() */


/******************************************************************************
Function Name   : usb_hhub_trans_start
Description     : HUB sys data transfer / control transfer
Arguments       : uint16_t hubaddr          : hub address
                : uint32_t size             : Data Transfer size
                : uint8_t  *table           : Receive Data area
                : USB_CB_t complete         : Callback function
Return value    : none
******************************************************************************/
void usb_hhub_trans_start(USB_UTR_t *ptr, uint16_t hubaddr, uint32_t size, uint8_t *table, USB_CB_t complete)
{
    USB_ER_t        err;

    /* Transfer structure setting */
    usb_shhub_DataMess[ptr->ip][hubaddr].keyword    = usb_shhub_InfoData[ptr->ip][hubaddr].pipe_num;
    usb_shhub_DataMess[ptr->ip][hubaddr].tranadr    = table;
    usb_shhub_DataMess[ptr->ip][hubaddr].tranlen    = size;
    usb_shhub_DataMess[ptr->ip][hubaddr].setup      = 0;
    usb_shhub_DataMess[ptr->ip][hubaddr].status     = USB_DATA_WAIT;
    usb_shhub_DataMess[ptr->ip][hubaddr].complete   = complete;
    usb_shhub_DataMess[ptr->ip][hubaddr].segment    = USB_TRAN_END;

    usb_shhub_DataMess[ptr->ip][hubaddr].ipp        = ptr->ipp;
    usb_shhub_DataMess[ptr->ip][hubaddr].ip         = ptr->ip;

    /* Transfer start */
    err = R_usb_hstd_TransferStart(&usb_shhub_DataMess[ptr->ip][hubaddr]);
    if( USB_E_OK != err )
    {
        /* Send Message failure */
        USB_PRINTF1("### usb_hhub_trans_start error (%ld)\n", err);
    }
}   /* eof usb_hhub_trans_start() */


/******************************************************************************
Function Name   : usb_hhub_PortSetFeature
Description     : SetFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_PortSetFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, uint16_t command, USB_CB_t complete)
{
    USB_ER_t        qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = command;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;         /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 0;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

    /* Port SetFeature */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if( USB_E_QOVR == qerr )
    {
        return USB_HUB_QOVR;
    }

    return USB_DONE;
}   /* eof usb_hhub_PortSetFeature() */


/******************************************************************************
Function Name   : usb_hhub_PortClrFeature
Description     : ClearFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_PortClrFeature(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t port, uint16_t command, USB_CB_t complete)
{
    USB_ER_t        qerr;

    /* Request */
    usb_shhub_ClassRequest[ptr->ip][0]  = USB_CLEAR_FEATURE | USB_HOST_TO_DEV | USB_CLASS | USB_OTHER;
    usb_shhub_ClassRequest[ptr->ip][1]  = command;
    usb_shhub_ClassRequest[ptr->ip][2]  = port;         /* Port number */
    usb_shhub_ClassRequest[ptr->ip][3]  = 0;
    usb_shhub_ClassRequest[ptr->ip][4]  = hubaddr;      /* Device address */

    /* Port ClearFeature */
    usb_shhub_ControlMess[ptr->ip].keyword  = USB_PIPE0;
    usb_shhub_ControlMess[ptr->ip].tranadr  = (void*)&usb_ghhub_Data[ptr->ip][hubaddr][0];
    usb_shhub_ControlMess[ptr->ip].tranlen  = (uint32_t)usb_shhub_ClassRequest[ptr->ip][3];
    usb_shhub_ControlMess[ptr->ip].setup    = usb_shhub_ClassRequest[ptr->ip];
    usb_shhub_ControlMess[ptr->ip].segment  = USB_TRAN_END;
    usb_shhub_ControlMess[ptr->ip].complete = complete;

    usb_shhub_ControlMess[ptr->ip].ipp      = ptr->ipp;
    usb_shhub_ControlMess[ptr->ip].ip       = ptr->ip;

    /* Transfer start */
    qerr = R_usb_hstd_TransferStart(&usb_shhub_ControlMess[ptr->ip]);
    if( USB_E_QOVR == qerr )
    {
        return USB_HUB_QOVR;
    }

    return USB_DONE;
}   /* eof usb_hhub_PortClrFeature() */


/******************************************************************************
Function Name   : usb_hhub_request_result
Description     : Hub Request Result Check
Arguments       : uint16_t errcheck         : hub result
Return value    : uint16_t                  : USB_DONE/USB_ERROR
******************************************************************************/
uint16_t usb_hhub_request_result(uint16_t errcheck)
{
    if( errcheck == USB_DATA_TMO )
    {
        USB_PRINTF0("*** HUB Request Timeout error !\n");
        return USB_ERROR;
    }
    else if( errcheck == USB_DATA_STALL )
    {
        USB_PRINTF0("*** HUB Request STALL !\n");
        return USB_ERROR;
    }
    else if( errcheck != USB_CTRL_END )
    {
        USB_PRINTF0("*** HUB Request error !\n");
        return USB_ERROR;
    }
    else
    {
    }
    return USB_DONE;
}   /* eof usb_hhub_request_result() */

/******************************************************************************
Function Name   : usb_hhub_trans_complete
Description     : Recieve complete Hub and Port Status Change Bitmap
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_trans_complete(USB_UTR_t *mess, uint16_t data1, uint16_t data2)
{
    USB_ER_t        err;
    uint16_t        pipenum, hubaddr;
    USB_UTR_t       *ptr;
    
    ptr = (USB_UTR_t *)mess;
    pipenum = mess->keyword;
    hubaddr = usb_hhub_GetHubaddr(ptr, pipenum);
    usb_shhub_HubAddr[ptr->ip] = hubaddr;
    
    if( ( USB_MSG_HUB_SUBMITRESULT != usb_shhub_Process[ptr->ip] ) && ( USB_NONE != usb_shhub_Process[ptr->ip] ) )
    {
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
        if( USB_OK != err )
        {
            /* Send Message failure */
            USB_PRINTF0("### HUB snd_msg error\n");
        }
    }
    else
    {
        usb_shhub_Process[ptr->ip] = USB_NONE;
    
        switch( mess->status )
        {
        case USB_DATA_SHT:
                USB_PRINTF1("*** receive short(pipe %d : HUB) !\n", pipenum);
        /* Continue */
        /* Direction is in then data receive end */
        case USB_DATA_OK:
            if( ( USB_DEFAULT == usb_ghstd_MgrMode[ptr->ip][0] ) || ( USB_DEFAULT == usb_ghstd_MgrMode[ptr->ip][1] ) )
            {
                err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)mess );
                if( USB_OK != err )
                {
                    USB_PRINTF0("### HUB task snd_msg error\n");
                }
            }
            else
            {
                /* HUB port connection */
                usb_shhub_Info[ptr->ip] = USB_MSG_HUB_SUBMITRESULT;
                usb_shhub_Process[ptr->ip] = USB_MSG_HUB_EVENT;
                usb_hhub_SpecifiedPath(mess);
            }
            break;
        case USB_DATA_STALL:
            USB_PRINTF0("*** Data Read error. ( STALL ) !\n");
            /* CLEAR_FEATURE */
            usb_hstd_ClearStall(ptr, pipenum, (USB_CB_t)&usb_cstd_DummyFunction);
            break;
        case USB_DATA_OVR:
            USB_PRINTF0("### receiver over. !\n");
            break;
        case USB_DATA_STOP:
            USB_PRINTF0("### receiver stop. !\n");
            break;
        default:
            USB_PRINTF0("### HUB Class Data Read error !\n");
            break;
        }
    }
}   /* eof usb_hhub_trans_complete() */


/******************************************************************************
Function Name   : usb_hhub_class_request_complete
Description     : Hub class check result
Arguments       : USB_UTR_t *mess    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_class_request_complete(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( USB_OK == USB_PGET_BLK( USB_HUB_MPL, &p_blf) )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shhub_Process[ptr->ip];
        cp->keyword = ptr->keyword;
        cp->result  = ptr->status;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;

        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( USB_OK != err )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
        while( 1 );
    }
}   /* eof usb_hhub_class_request_complete() */


/******************************************************************************
Function Name   : usb_hhub_initial
Description     : Global memory initialized
Arguments       : uint16_t data1 : necessary for regist the callback
                : uint16_t data2 : necessary for regist the callback
Return value    : none
******************************************************************************/
void usb_hhub_initial(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    uint16_t    i;

    for( i = 0u; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        usb_shhub_InfoData[ptr->ip][i].up_addr = 0;          /* Up-address clear */
        usb_shhub_InfoData[ptr->ip][i].up_port_num = 0;      /* Up-port num clear */
        usb_shhub_InfoData[ptr->ip][i].port_num = 0;         /* Port number clear */
        usb_shhub_InfoData[ptr->ip][i].pipe_num = 0;         /* Pipe number clear */
    }
    usb_shhub_Number[ptr->ip] = 0;
}   /* eof usb_hhub_initial() */


/******************************************************************************
Function Name   : usb_hhub_pipe_info
Description     : Set pipe information
Arguments       : uint8_t  *table           : Interface Descriptor
                : uint16_t offset           : DEF_EP table offset
                : uint16_t speed            : device speed
                : uint16_t length           : Interface Descriptor length
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_pipe_info(USB_UTR_t *ptr, uint8_t *table, uint16_t offset, uint16_t speed, uint16_t length)
{
    uint16_t        ofdsc;
    uint16_t        retval;

    /* Check Descriptor */
    if( USB_DT_INTERFACE != table[1] )
    {
        /* Configuration Descriptor */
        USB_PRINTF0("### Interface descriptor error (HUB).\n");
        return USB_ERROR;
    }

    /* Check Endpoint Descriptor */
    ofdsc = table[0];
    while( ofdsc < (length - table[0]) )
    {
        /* Search within Interface */
        switch( table[ofdsc + 1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
        /* String Descriptor */
        case USB_DT_STRING:
        /* Interface Descriptor */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
            return USB_ERROR;
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            /* Interrupt Endpoint */
            if( USB_EP_INT == ( table[ofdsc + 3] & USB_EP_TRNSMASK ) )
            {
                retval = R_usb_hstd_ChkPipeInfo( speed, &usb_ghhub_TmpEPTbl[ptr->ip][offset], &table[ofdsc] );
                if( USB_DIR_H_IN == retval )
                {
                    return USB_DONE;
                }
                else
                {
                    USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
                }
            }
            ofdsc += table[ofdsc];
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
        /* Other Speed Configuration */
        case USB_DT_OTHER_SPEED_CONF:
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Endpoint Descriptor error(HUB).\n");
            return USB_ERROR;
            break;
        /* Another Descriptor */
        default:
            ofdsc += table[ofdsc];
            break;
        }
    }
    return USB_ERROR;
}   /* eof usb_hhub_pipe_info() */


/******************************************************************************
Function Name   : usb_hhub_CheckRequest
Description     : Class check request
Arguments       : uint16_t result
Return value    : none
******************************************************************************/
void usb_hhub_CheckRequest(USB_UTR_t *ptr, uint16_t result)
{
    USB_MH_t        p_blf;
    USB_ER_t        err;
    USB_CLSINFO_t   *cp;
    
    /* Get mem pool blk */
    if( USB_OK == USB_PGET_BLK( USB_HUB_MPL, &p_blf) )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_CLS_CHECKREQUEST;
        cp->result  = result;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;

        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( USB_OK != err )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckRequest function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USB_PRINTF0("### CheckRequest function pget_blk error\n");
        while( 1 );
    }
    
}   /* eof usb_hhub_CheckRequest() */


/******************************************************************************
Function Name   : usb_hstd_CheckDescriptor
Description     : check descriptor
Arguments       : uint8_t *table            : table (indata).
                : uint16_t spec             : spec.
Return value    : none
******************************************************************************/
uint16_t usb_hstd_CheckDescriptor(uint8_t *table, uint16_t spec)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    /* Check Descriptor */
    if( table[1] == spec )
    {
        switch( table[1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
            USB_PRINTF0("  Device Descriptor.\n");
            break;
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
            USB_PRINTF0("  Configuration Descriptor.\n");
            break;
        /* String Descriptor */
        case USB_DT_STRING:
            USB_PRINTF0("  String Descriptor.\n");
            break;
        /* Interface Descriptor */
        case USB_DT_INTERFACE:
            USB_PRINTF0("  Interface Descriptor.\n");
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            USB_PRINTF0("  Endpoint Descriptor.\n");
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
            USB_PRINTF0("  Device Qualifier Descriptor.\n");
            break;
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
            USB_PRINTF0("  Other Speed Configuration Descriptor.\n");
            break;
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("  Interface Power Descriptor.\n");
            break;
        /* HUB Descriptor */
        case USB_DT_HUBDESCRIPTOR:
            USB_PRINTF0("  HUB Descriptor.\n");
            break;
        /* Not Descriptor */
        default:
            USB_PRINTF0("### Descriptor error (Not Standard Descriptor).\n");
            return USB_ERROR;
            break;
        }
        return USB_DONE;
    }
    else
    {
        switch( table[1] )
        {
        /* Device Descriptor */
        case USB_DT_DEVICE:
            USB_PRINTF0("### Descriptor error ( Device Descriptor ).\n");
            break;
        /* Configuration Descriptor */
        case USB_DT_CONFIGURATION:
            USB_PRINTF0("### Descriptor error ( Configuration Descriptor ).\n");
            break;
        /* String Descriptor */
        case USB_DT_STRING:
            USB_PRINTF0("### Descriptor error ( String Descriptor ).\n");
            break;
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            USB_PRINTF0("### Descriptor error ( Interface Descriptor ).\n");
            break;
        /* Endpoint Descriptor */
        case USB_DT_ENDPOINT:
            USB_PRINTF0("### Descriptor error ( Endpoint Descriptor ).\n");
            break;
        /* Device Qualifier Descriptor */
        case USB_DT_DEVICE_QUALIFIER:
            USB_PRINTF0("### Descriptor error ( Device Qualifier Descriptor ).\n");
            break;
        /* Other Speed Configuration Descriptor */
        case USB_DT_OTHER_SPEED_CONF:
            USB_PRINTF0("### Descriptor error ( Other Speed Configuration Descriptor ).\n");
            break;
        /* Interface Power Descriptor */
        case USB_DT_INTERFACE_POWER:
            USB_PRINTF0("### Descriptor error ( Interface Power Descriptor ).\n");
            break;
        /* Not Descriptor */
        default:
            USB_PRINTF0("### Descriptor error ( Not Standard Descriptor ).\n");
            break;
        }
        return USB_ERROR;
    }
#else   /* Not USB_DEBUGPRINT_PP */
        return USB_DONE;
#endif  /* USB_DEBUGPRINT_PP */
}   /* eof usb_hstd_CheckDescriptor() */

/******************************************************************************
Function Name   : usb_hhub_ChkConfig
Description     : Configuration Descriptor check
Arguments       : uint16_t **table          : Configuration Descriptor
                : uint16_t spec             : HUB specification
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_ChkConfig(uint16_t **table, uint16_t spec)
{
    uint8_t     *descriptor_table;
    uint16_t    ofset;

    descriptor_table = (uint8_t*)(table[1]);

    /* Descriptor check */
    ofset = usb_hstd_CheckDescriptor(descriptor_table, (uint16_t)USB_DT_CONFIGURATION);
    if( USB_ERROR == ofset )
    {
        USB_PRINTF0("### Configuration descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface number */
    switch( spec )
    {
    case USB_FSHUB:     /* Full Speed Hub */
        if( descriptor_table[4] != USB_HUB_INTNUMFS )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBS:    /* Hi Speed Hub(Multi) */
        if( descriptor_table[4] != USB_HUB_INTNUMHSS )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Single) */
        if( descriptor_table[4] != USB_HUB_INTNUMHSM )
        {
            USB_PRINTF0("### HUB configuration descriptor error !\n");
            return USB_ERROR;
        }
        break;
    default:
        return USB_ERROR;
        break;
    }
    return USB_DONE;
}   /* eof usb_hhub_ChkConfig() */


/******************************************************************************
Function Name   : usb_hhub_ChkInterface
Description     : Interface Descriptor check
Arguments       : uint16_t **table          : Interface Descriptor
                : uint16_t spec             : HUB specification
Return value    : uint16_t                  : DONE/ERROR
******************************************************************************/
uint16_t usb_hhub_ChkInterface(uint16_t **table, uint16_t spec)
{
    uint8_t     *descriptor_Table;
    uint16_t    ofset;

    descriptor_Table = (uint8_t*)(table[2]);

    /* Descriptor check */
    ofset = usb_hstd_CheckDescriptor(descriptor_Table, (uint16_t)USB_DT_INTERFACE);
    if( USB_ERROR == ofset )
    {
        USB_PRINTF0("### Interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface class */
    if( descriptor_Table[5] != USB_IFCLS_HUB )
    {
        USB_PRINTF0("### HUB interface descriptor error !\n");
        return USB_ERROR;
    }

    /* Check interface number */
    switch( spec )
    {
    case USB_FSHUB:     /* Full Speed Hub */
        if( descriptor_Table[2] != (USB_HUB_INTNUMFS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBS:    /* Hi Speed Hub(Single) */
        if( descriptor_Table[2] != (USB_HUB_INTNUMHSS - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    case USB_HSHUBM:    /* Hi Speed Hub(Multi) */
        if( descriptor_Table[2] != (USB_HUB_INTNUMHSM - 1u) )
        {
            USB_PRINTF0("### HUB interface descriptor error !\n");
            return USB_ERROR;
        }
        break;
    default:
        return USB_ERROR;
        break;
    }
    return USB_DONE;
}   /* eof usb_hhub_ChkInterface() */


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx1
Description     : table index search
Arguments       : uint16_t hubaddr          : hub address
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx1(USB_UTR_t *ptr, uint16_t hubaddr)
{
    uint16_t    pipecheck[USB_MAX_PIPE_NO];
    uint16_t    i;

    for( i = 0; i < USB_MAX_PIPE_NO; i++ )
    {
        /* Check table clear */
        pipecheck[i] = 0;
    }

    for( i = 0; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        /* Used pipe number set */
        if( 0 != usb_shhub_InfoData[ptr->ip][i].pipe_num )
        {
            pipecheck[ usb_shhub_InfoData[ptr->ip][i].pipe_num - 1 ] = 1;
        }
    }

    for( i = USB_MAX_PIPE_NO; i != 0; i-- )
    {
        if( 0 == pipecheck[i - 1] )
        {
            return (uint16_t)((USB_MAX_PIPE_NO - i) * USB_EPL);
        }
    }
    return (USB_ERROR);
}   /* eof usb_hhub_ChkTblIndx1() */


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx2
Description     : table index search
Arguments       : uint16_t hubaddr          : hub address
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx2(USB_UTR_t *ptr, uint16_t hubaddr)
{
/* Search table index */
    switch( usb_shhub_InfoData[ptr->ip][hubaddr].pipe_num )
    {
    case USB_PIPE9: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE8: return (uint16_t)(1u * USB_EPL);    break;
    case USB_PIPE7: return (uint16_t)(2u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(3u * USB_EPL);    break;
    default:        break;
    }

    return (USB_ERROR);
}   /* eof usb_hhub_ChkTblIndx2() */


/******************************************************************************
Function Name   : usb_hhub_ChkTblIndx3
Description     : table index search
Arguments       : uint16_t pipenum          : pipe number
Return value    : uint16_t                  : table index
******************************************************************************/
uint16_t usb_hhub_ChkTblIndx3(uint16_t pipenum)
{
/* Search table index */
    switch( pipenum )
    {
    case USB_PIPE9: return (uint16_t)(0u * USB_EPL);    break;
    case USB_PIPE8: return (uint16_t)(1u * USB_EPL);    break;
    case USB_PIPE7: return (uint16_t)(2u * USB_EPL);    break;
    case USB_PIPE6: return (uint16_t)(3u * USB_EPL);    break;
    default:        break;
    }

    return (USB_ERROR);
}   /* eof usb_hhub_ChkTblIndx3() */


/******************************************************************************
Function Name   : usb_hstd_DeviceDescripInfo
Description     : device descriptor info
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_DeviceDescripInfo(USB_UTR_t *ptr)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint8_t     *p;

    p = usb_hstd_DevDescriptor(ptr);

    /* For DEMO */
    USB_PRINTF0("Device descriptor fields\n");
    USB_PRINTF2("  bcdUSB         : %02x.%02x\n",  p[0x03], p[0x02]);
    USB_PRINTF1("  bDeviceClass   : 0x%02x\n",     p[0x04]);
    USB_PRINTF1("  bDeviceSubClass: 0x%02x\n",     p[0x05]);
    USB_PRINTF1("  bProtocolCode  : 0x%02x\n",     p[0x06]);
    USB_PRINTF1("  bMaxPacletSize : 0x%02x\n",     p[0x07]);
    USB_PRINTF2("  idVendor       : 0x%02x%02x\n", p[0x09], p[0x08]);
    USB_PRINTF2("  idProduct      : 0x%02x%02x\n", p[0x0b], p[0x0a]);
    USB_PRINTF2("  bcdDevice      : 0x%02x%02x\n", p[0x0d], p[0x0c]);
    USB_PRINTF1("  iSerialNumber  : 0x%02x\n",     p[0x10]);
    USB_PRINTF1("  bNumConfig     : 0x%02x\n",     p[0x11]);
    USB_PRINTF0("\n");
#endif  /* USB_DEBUGPRINT_PP */
}   /* eof usb_hstd_DeviceDescripInfo() */


/******************************************************************************
Function Name   : usb_hstd_ConfigDescripInfo
Description     : configuration descriptor info
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hstd_ConfigDescripInfo(USB_UTR_t *ptr)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint8_t     *p;
    uint16_t    len;

    p = usb_hstd_ConDescriptor(ptr);

    len = 0;
    while( len < (p[2]) )
    {
        /* Search within Configuration descriptor */
        switch( p[len + 1] )
        {
        /* Configuration Descriptor ? */
        case 0x02:
            USB_PRINTF0("Configuration descriptor fields\n");
            USB_PRINTF1("  Configuration Value  : 0x%02x\n", p[0x05]);
            USB_PRINTF1("  Number of Interface  : 0x%02x\n", p[0x04]);
            break;
        /* Interface Descriptor ? */
        case 0x04:
            USB_PRINTF0("\nInterface descriptor fields\n");
            switch( p[len + 5] )
            {
            /* Class check */
            /* Audio Class */
            case 1: 
                USB_PRINTF0("  This device has Audio Class.\n");
                break;
            /* CDC-Control Class */
            case 2:
                USB_PRINTF0("  This device has CDC-Control Class.\n");
                break;
            /* HID Class */
            case 3:
                USB_PRINTF0("  This device has HID Class.\n");
                break;
            /* Physical Class */
            case 5:
                USB_PRINTF0("  This device has Physical Class.\n");
                break;
            /* Image Class */
            case 6:
                USB_PRINTF0("  This device has Image Class.\n");
                break;
            /* Printer Class */
            case 7:
                USB_PRINTF0("  This device has Printer Class.\n");
                break;
            /* Mass Storage Class */
            case 8:
                USB_PRINTF0("  I/F class    : Mass Storage\n");
                switch( p[len + 6] )
                {
                case 0x05:
                    USB_PRINTF0("  I/F subclass : SFF-8070i\n");
                    break;
                case 0x06:
                    USB_PRINTF0("  I/F subclass : SCSI command\n");
                    break;
                default:
                    USB_PRINTF0("### I/F subclass not support.\n");
                }
                if( 0x50 == p[ len + 7 ] )
                {
                    /* Check Interface Descriptor (protocol) */
                    USB_PRINTF0("  I/F protocol : BOT\n");
                }
                else
                {
                    USB_PRINTF0("### I/F protocol not support.\n");
                }
                break;
            /* HUB Class */
            case 9:
                USB_PRINTF0("  This device has HUB Class.\n");
                break;
            /* CDC-Data Class */
            case 10:
                USB_PRINTF0("  This device has CDC-Data Class.\n");
                break;
            /* Chip/Smart Card Class */
            case 11:
                USB_PRINTF0("  This device has Chip/Smart Class.\n");
                break;
            /* Content-Security Class */
            case 13:
                USB_PRINTF0("  This device has Content-Security Class.\n");
                break;
            /* Video Class */
            case 14:
                USB_PRINTF0("  This device has Video Class.\n");
                break;
            /* Vendor Specific Class */
            case 255:
                USB_PRINTF0("  I/F class : Vendor Specific\n");
                break;
            /* Reserved */
            case 0:
                USB_PRINTF0("  I/F class : class error\n");
                break;
            default:
                USB_PRINTF0("  This device has not USB Device Class.\n");
                break;
            }
            break;
        /* Endpoint Descriptor */
        case 0x05:
            usb_hstd_EndpDescripInfo(&p[len]);
            break;
        default:
            break;
        }
        len += p[len];
    }
#endif  /* USB_DEBUGPRINT_PP */
}   /* eof usb_hstd_ConfigDescripInfo() */


/******************************************************************************
Function Name   : usb_hstd_EndpDescripInfo
Description     : endpoint descriptor info
Arguments       : uint8_t *tbl              : table (indata).
Return value    : none
******************************************************************************/
void usb_hstd_EndpDescripInfo(uint8_t *tbl)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    uint16_t        epnum, pipe_mxp;

    switch( (uint8_t)(tbl[3] & USB_EP_TRNSMASK) )
    {
    /* Isochronous Endpoint */
    case USB_EP_ISO:
        USB_PRINTF0("  ISOCHRONOUS");
        break;
    /* Bulk Endpoint */
    case USB_EP_BULK:
        USB_PRINTF0("  BULK");
        break;
    /* Interrupt Endpoint */
    case USB_EP_INT:
        USB_PRINTF0("  INTERRUPT");
        break;
    /* Control Endpoint */
    default:
        USB_PRINTF0("### Control pipe is not support.\n");
        break;
    }

    if( USB_EP_IN == (uint8_t)(tbl[2] & USB_EP_DIRMASK) )
    {
        /* Endpoint address set */
        USB_PRINTF0(" IN  endpoint\n");
    }
    else
    {
        USB_PRINTF0(" OUT endpoint\n");
    }
    /* Endpoint number set */
    epnum       = (uint16_t)((uint16_t)tbl[2] & USB_EP_NUMMASK);
    /* Max packet size set */
    pipe_mxp = (uint16_t)(tbl[4] | (uint16_t)((uint16_t)(tbl[5]) << 8));
    USB_PRINTF2("   Number is %d. MaxPacket is %d. \n", epnum, pipe_mxp);
    switch( (uint16_t)(tbl[3] & USB_EP_TRNSMASK) )
    {
    /* Isochronous Endpoint */
    case 0x01u:
    /* Interrupt Endpoint */
    case 0x03u:
        USB_PRINTF1("    interval time is %d\n", tbl[6]);
        break;
    default:
        break;
    }
#endif  /* USB_DEBUGPRINT_PP */
}   /* eof usb_hstd_EndpDescripInfo() */


/******************************************************************************
Function Name   : usb_hhub_new_connect
Description     : new connect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
                : USB_CLSINFO_t *mess
Return value    : none
******************************************************************************/
void usb_hhub_new_connect(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum, USB_CLSINFO_t *mess)
{
        uint16_t        devaddr;
    
    hubaddr = usb_shhub_HubAddr[ptr->ip];
    portnum = usb_shhub_EventPort[ptr->ip];

    /* New downport device search */
    devaddr = usb_hhub_GetNewDevaddr(ptr);
    if( 0 != devaddr )
    {
        USB_PRINTF1(" Hubport connect address%d\n", devaddr);
        usb_shhub_InfoData[ptr->ip][devaddr].up_addr     = hubaddr;        /* Up-hubaddr set */
        usb_shhub_InfoData[ptr->ip][devaddr].up_port_num = portnum;        /* Up-hubport set */
        usb_shhub_Process[ptr->ip] = USB_MSG_HUB_ATTACH;
        /* Next Process Selector */
        usb_hhub_SpecifiedPath(mess);
        
    }
    else
    {
        USB_PRINTF0("### device count over !\n");
    }
}   /* eof usb_hhub_new_connect() */


/******************************************************************************
Function Name   : usb_hhub_port_detach
Description     : HUB down port disconnect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : none
******************************************************************************/
void usb_hhub_port_detach(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum)
{
    uint16_t        md;
    USB_HCDREG_t    *driver;
    /* Device number --> DeviceAddress */
    uint16_t        devaddr;

    /* HUB downport status */
    usb_shhub_DownPort[ptr->ip][hubaddr]    &= (uint16_t)(~USB_BITSET(portnum));
    /* HUB downport RemoteWakeup */
    usb_shhub_Remote[ptr->ip][hubaddr]  &= (uint16_t)(~USB_BITSET(portnum));
    /* Now downport device search */
    devaddr = usb_hhub_GetCnnDevaddr(ptr, hubaddr, portnum);
    for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
    {
        driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
        if( devaddr == driver->devaddr )
        {
            (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);

            /* Root port */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
            /* Device state */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;
            /* Not configured */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;
            /* Interface Class : NO class */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
            /* No connect */
            usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;

            /* Root port */
            driver->rootport    = USB_NOPORT;
            /* Device address */
            driver->devaddr     = USB_NODEVICE;
            /* Device state */
            driver->devstate    = USB_DETACHED;
        }
    }
    /* Selective detach */
    usb_hhub_selective_detach(ptr, devaddr);
}   /* eof usb_hhub_port_detach() */


/******************************************************************************
Function Name   : usb_hhub_selective_detach
Description     : HUB down port Selective disconnect
Arguments       : uint16_t devaddr          : device address
Return value    : none
******************************************************************************/
void usb_hhub_selective_detach(USB_UTR_t *ptr, uint16_t devaddr)
{
    uint16_t    addr, i;

    addr = (uint16_t)(devaddr << USB_DEVADDRBIT);
    /* Check Connection */
    if( USB_NOCONNECT != usb_hstd_GetDevSpeed(ptr, addr) )
    {
        for( i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++ )
        {
            /* Not control transfer */
            if( usb_cstd_GetDeviceAddress(ptr, i) == addr )
            {
                /* Agreement device address */
                if( USB_PID_BUF == usb_cstd_GetPid(ptr, i) )
                {
                    /* PID=BUF ? */
                    R_usb_hstd_TransferEnd(ptr, i, (uint16_t)USB_DATA_STOP);
                }
                usb_cstd_ClrPipeCnfg(ptr, i);
            }
        }
        usb_hstd_SetDevAddr(ptr, addr, USB_DONE, USB_DONE);
        usb_hstd_SetHubPort(ptr, addr, USB_DONE, USB_DONE);
        USB_PRINTF1("*** Device address %d clear.\n", devaddr);
    }

    /* Root port */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][0] = USB_NOPORT;
    /* Device state */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][1] = USB_DETACHED;
    /* Not configured */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][2] = (uint16_t)0;
    /* Interface Class : NO class */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][3] = (uint16_t)USB_IFCLS_NOT;
    /* No connect */
    usb_ghstd_DeviceInfo[ptr->ip][devaddr][4] = (uint16_t)USB_NOCONNECT;

}   /* eof usb_hhub_selective_detach() */


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor1
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
                : USB_CB_t complete         : callback
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hstd_GetStringDesc(ptr, devaddr, (uint16_t)0, complete);

    return  USB_DONE;
}   /* eof usb_hstd_GetStringDescriptor1() */


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor2
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
                : USB_CB_t complete         : callback
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hstd_GetStringDesc(ptr, devaddr, index, complete);

    return  USB_DONE;
}   /* eof usb_hstd_GetStringDescriptor2() */


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor1Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor1Check(uint16_t errcheck)
{
    if( (USB_ER_t)USB_DATA_STALL == errcheck )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else if( (USB_ER_t)USB_CTRL_END != errcheck )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return  USB_DONE;
}   /* eof usb_hstd_GetStringDescriptor1Check() */


/******************************************************************************
Function Name   : usb_hstd_GetStringDescriptor2Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDescriptor2Check(uint16_t errcheck)
{
    if( (USB_ER_t)USB_DATA_STALL == errcheck )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else if( (USB_ER_t)USB_CTRL_END != errcheck )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return USB_DONE;
}   /* eof usb_hstd_GetStringDescriptor2Check() */


/******************************************************************************
Function Name   : usb_hhub_GetNewDevaddr
Description     : Get the new device address
                : when connection of a device detected in the down port of HUB
Arguments       : none
Return value    : uint16_t                  : New device address
******************************************************************************/
uint16_t usb_hhub_GetNewDevaddr(USB_UTR_t *ptr)
{
    uint16_t i;

    /* Search new device */
    for( i = (USB_HUBDPADDR); i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( 0 == usb_shhub_InfoData[ptr->ip][i].up_addr )
        {
            /* New device address */
            return i;
        }
    }
    return 0;
}   /* eof usb_hhub_GetNewDevaddr() */


/******************************************************************************
Function Name   : usb_hhub_GetHubaddr
Description     : Get the HUB address
                : from the pipe number for HUB notification
Arguments       : uint16_t pipenum          : pipe number
Return value    : uint16_t                  : HUB address
******************************************************************************/
uint16_t usb_hhub_GetHubaddr(USB_UTR_t *ptr, uint16_t pipenum)
{
    uint16_t    i;

    for( i = 1; i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( usb_shhub_InfoData[ptr->ip][i].pipe_num == pipenum )
        {
            /* HUB address */
            return i;
        }
    }
    return 0;
}   /* eof usb_hhub_GetHubaddr() */


/******************************************************************************
Function Name   : usb_hhub_GetCnnDevaddr
Description     : Get the connected device address
                : from the HUB address and the down port number of a HUB
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : uint16_t                  : Connected device address
******************************************************************************/
uint16_t usb_hhub_GetCnnDevaddr(USB_UTR_t *ptr, uint16_t hubaddr, uint16_t portnum)
{
    uint16_t    i;

    for( i = (USB_HUBDPADDR); i < (USB_MAXDEVADDR + 1u); i++ )
    {
        if( ( usb_shhub_InfoData[ptr->ip][i].up_addr == hubaddr ) &&
            ( usb_shhub_InfoData[ptr->ip][i].up_port_num == portnum ) )
        {
            /* Device address */
            return i;
        }
    }
    return 0;
}   /* eof usb_hhub_GetCnnDevaddr() */


/******************************************************************************
Function Name   : usb_hhub_SpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *ptr    : USB system internal message.
Return value    : none
******************************************************************************/
void usb_hhub_SpecifiedPath(USB_CLSINFO_t *ptr)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( USB_OK == USB_PGET_BLK( USB_HUB_MPL, &p_blf) )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shhub_Process[ptr->ip];
        cp->keyword = ptr->keyword;
        cp->result  = ptr->result;

        cp->ipp = ptr->ipp;
        cp->ip  = ptr->ip;
        
        /* Send message */
        err = USB_SND_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf );
        if( USB_OK != err )
        {
            /* Send message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
        while( 1 );
    }
}   /* eof usb_hhub_SpecifiedPath() */


/******************************************************************************
Function Name   : usb_hhub_SpecifiedPathWait
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess    : USB system internal message.
                : uint16_t times         : Timeout value.
Return value    : none
******************************************************************************/
void usb_hhub_SpecifiedPathWait(USB_CLSINFO_t *ptr, uint16_t times)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *hp;

    /* Get mem pool blk */
    if( USB_OK == USB_PGET_BLK( USB_HUB_MPL, &p_blf) )
    {
        hp = (USB_CLSINFO_t*)p_blf;
        hp->msginfo = usb_shhub_Process[ptr->ip];
        hp->keyword = ptr->keyword;
        hp->result  = ptr->result;

        hp->ipp = ptr->ipp;
        hp->ip  = ptr->ip;

        /* Wait message */
        err = USB_WAI_MSG( USB_HUB_MBX, (USB_MSG_t*)p_blf, times );
        if( USB_OK != err )
        {
            /* Wait message failure */
            err = USB_REL_BLK(USB_HUB_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPassWait function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPassWait function pget_blk error\n");
        while( 1 );
    }
}   /* eof usb_hhub_SpecifiedPathWait() */

#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

/******************************************************************************
End  Of File
******************************************************************************/
