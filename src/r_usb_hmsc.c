/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2014 Renesas Electronics Corporation
* and Renesas Solutions Corp. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_usb_hmsc_sample.c
* Version      : 1.00
* Device(s)    : Renesas R5F564MxxDxx
* Tool-Chain   : Renesas e2 studio v3.0.1.7 or later
*              : C/C++ Compiler Package for RX Family V2.01.00 or later
* OS           : None
* H/W Platform : Renesas Starter Kit+ RX64M
* Description  : Sample code for driving the USB-BASIC-FW and HMSC
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.06.2014 1.00    First Release
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "./include/r_usb_hmsc.h"         /* USB HMSC Sample Code Header */
#include "platform.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_tfat_lib.h"

#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#endif
/*****************************************************************************
Macro definitions
******************************************************************************/
#define USB_HOST_USBIP_NUM  USB_USBIP_0

/******************************************************************************
Section    <Section Definition> , "Data Sections"
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t    usb_ghmsc_SmpAplProcess = USB_HMSC_WAIT;
FATFS       usb_gFatfs;
uint8_t     usb_gBuf[512];

const uint16_t usb_gapl_devicetpl[] =
{
    /* Number of tpl table */
    4,
    /* Reserved */
    0,
    /* Vendor ID  : no-check */
    USB_NOVENDOR,
    /* Product ID : no-check */
    USB_NOPRODUCT,
};

uint16_t usb_gvendor_smpl_eptbl[] =
{
        USB_PIPE1,
        USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_NONE | USB_NONE | USB_NONE,
        USB_NONE,
        USB_NONE,
        USB_NONE,
        USB_CUSE,
        USB_PIPE1,
        USB_NONE | USB_BFREOFF | USB_DBLBON | USB_CNTMDON | USB_NONE | USB_NONE | USB_NONE,
        USB_NONE,
        USB_NONE,
        USB_NONE,
        USB_CUSE,	/* Pipe end */
        USB_PDTBLEND,
};

void    usb_cstd_IdleTaskStart(void);
void    usb_cstd_IdleTask(USB_VP_INT stacd);
void    usb_hmsc_task_start(void);
void    usb_apl_task_switch(void);
void    usb_hapl_task_start(USB_UTR_t *ptr);
void    usb_hmsc_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2);
void    usb_hmsc_DriveOpen(USB_UTR_t *ptr, uint16_t addr, uint16_t data2);
void    usb_hapl_registration(USB_UTR_t *ptr);
void    usb_hmsc_apl_init(USB_UTR_t *ptr);
void    usb_hmsc_StrgCommandResult(USB_UTR_t *mess, uint16_t data1, uint16_t data2);
void    usb_hmsc_SampleAplTask(void);

/******************************************************************************
External variables and functions
******************************************************************************/
extern USB_UTR_t    tfat_ptr;

/*** File System Interface for HMSC ***/
extern void     R_usb_hmsc_DriveClose(USB_UTR_t *ptr, uint16_t addr, uint16_t data2);

/*****************************************************************************
Enumerated Types
******************************************************************************/

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_IdleTaskStart
Description     : Idle Task Start process
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_IdleTaskStart(void)
{
/* Condition compilation by the difference of the devices */
#if (USB_CPU_LPW_PP == USB_LPWR_USE_PP)
    R_usb_cstd_SetTaskPri(USB_IDL_TSK, USB_PRI_6);
    USB_SND_MSG(USB_IDL_MBX, 0);
#endif /* (USB_CPU_LPW_PP == USB_LPWR_USE_PP) */
}
/******************************************************************************
End of function usb_cstd_IdleTaskStart
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_IdleTask
Description     : Call Idle Task (sleep sample)
Arguments       : USB_VP_INT stacd      : task start code(not use)
Return value    : none
******************************************************************************/
void usb_cstd_IdleTask(USB_VP_INT stacd)
{
}
/******************************************************************************
End of function usb_cstd_IdleTask
******************************************************************************/

/******************************************************************************
Function Name   : hmsc_cstd_task_start
Description     : Start task processing.
Arguments       : none
Return value    : none
******************************************************************************/
void hmsc_cstd_task_start( void )
{
    /* Start the Idle task. */
    usb_cstd_IdleTaskStart();

    /* Start host-related USB drivers. */
    usb_hmsc_task_start();

        
    /* Task switching. */
    usb_apl_task_switch ();
}   /* eof usb_cstd_task_start() */
/******************************************************************************
Function Name   : usb_hmsc_task_start
Description     : Start task processing.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_hmsc_task_start(void)
{
    /* The USB Communication Structure allocation for this MSC. */
    USB_UTR_t   utr;
    /* Pointer to the USB Communication Structure above. */
    USB_UTR_t*  ptr = &utr;

    /* Determine which port is host. */
    ptr->ip = USB_HOST_USBIP_NUM;

    /* If port is used, register the driver etc. */
    if (USB_NOUSE_PP != ptr->ip )
    {
        ptr->ipp = R_usb_cstd_GetUsbIpAdr( ptr->ip ); /* Get the USB IP base address. */

        tfat_ptr.ip     = ptr->ip;          /* Set up USB IP number for TFAT system. */
        tfat_ptr.ipp    = ptr->ipp;         /* Set up USB IP base address for TFAT. */

        /* Set-up tasks and drivers to use the allocated Comm. structure (for MSC) above. */
        R_usb_hstd_MgrOpen(ptr);            /* Manager open */
        R_usb_hstd_HcdOpen(ptr);            /* Hcd open */

        usb_hapl_registration( ptr );       /* Host application registration. */
        R_usb_hmsc_hub_registration(ptr);   /* Hub registration. */
        R_usb_hmsc_driver_start( ptr );     /* Host class driver. */
        usb_hapl_task_start( ptr );         /* Host application task. */

        /* Finally, init the HW with the Comm. struct. */
        R_usb_cstd_UsbIpInit( ptr, USB_HOST_PP );
    }
}
/******************************************************************************
End of function usb_hmsc_task_start
******************************************************************************/

/******************************************************************************
Function Name   : usb_apl_task_switch
Description     : Non-OS task switch loop.
Argument        : none
Return value    : none
******************************************************************************/
void usb_apl_task_switch(void)
{
#ifdef FREE_RTOS_PP
    usb_hmsc_SampleAplTask();                       /* HMSC Sample Task  */
#else
    /* Scheduler */
    R_usb_cstd_Scheduler();

    /* Check for any task processing requests flags. */
    if( USB_FLGSET == R_usb_cstd_CheckSchedule() )
    {
        R_usb_hstd_HcdTask((USB_VP_INT)0);          /* HCD Task */
        R_usb_hstd_MgrTask((USB_VP_INT)0);          /* MGR Task */
        R_usb_hhub_Task((USB_VP_INT)0);             /* HUB Task */

        R_usb_hmsc_Task();                      /* HMSC Task */
        R_usb_hmsc_StrgDriveTask();             /* HSTRG Task */
        usb_hmsc_SampleAplTask();               /* HMSC Sample Task  */
    }
    else
    {
        /* Idle task - background "sleep". */
        usb_cstd_IdleTask(0);
    }
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_apl_task_switch
******************************************************************************/

/******************************************************************************
Function Name   : usb_hapl_task_start
Description     : Start up host USB application task.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void usb_hapl_task_start(USB_UTR_t *ptr)
{
    /* Set task priority of HMSC sample application. */
    R_usb_cstd_SetTaskPri(USB_HMSCSMP_TSK, USB_PRI_4);

    /* Clear application using fresh USB Comm. Structure. */
    usb_hmsc_apl_init(ptr);
}
/******************************************************************************
End of function usb_hapl_task_start
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_DummyFunction
Description     : dummy function
Arguments       : USB_UTR_t *ptr		: The app's USB Comm. Structure.
                : uint16_t data1		: not use
                : uint16_t data2		: not use
Return value    : none
******************************************************************************/
void usb_hmsc_DummyFunction(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
}
/******************************************************************************
End of function usb_hmsc_DummyFunction
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_DriveOpen
Description     : HMSC drive open
Arguments       : uint16_t addr :
                : uint16_t data2 :
Return value    : none
******************************************************************************/
void usb_hmsc_DriveOpen(USB_UTR_t *ptr, uint16_t addr, uint16_t data2)
{
/* Condition compilation by the difference of the File system */
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSCSMP_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_HMSC_DRIVE_OPEN;
        cp->keyword = addr;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HMSCSMP_MBX, (USB_MSG_t*)p_blf );

        if(err != USB_E_OK)
        {
            err = R_USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### DriveOpen snd_msg error\n");
            while( 1 );
        }
    }
    else
    {
        USB_PRINTF0("### DriveOpen pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hmsc_DriveOpen
******************************************************************************/

/******************************************************************************
Function Name   : usb_hapl_registration
Description     : Callback registration.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void usb_hapl_registration(USB_UTR_t *ptr)
{
    USB_HCDREG_t    driver;

    /* Driver registration */
    driver.ifclass    = (uint16_t)USB_IFCLS_MAS;            /* Use Interface class for MSC. */
    driver.tpl        = (uint16_t*)&usb_gapl_devicetpl;     /* Target peripheral list. */
    driver.pipetbl    = (uint16_t*)&usb_gvendor_smpl_eptbl; /* Pipe def. table address. */
    driver.classinit  = &R_usb_hmsc_Initialized;            /* Driver init. */
    driver.classcheck = &R_usb_hmsc_ClassCheck;             /* Driver check. */
    driver.devconfig  = &usb_hmsc_DriveOpen;                /* Callback when device is configured. */
    driver.devdetach  = &R_usb_hmsc_DriveClose;             /* Callback when device is detached. */
    driver.devsuspend = &usb_hmsc_DummyFunction;            /* Callback when device is suspended. */
    driver.devresume  = &usb_hmsc_DummyFunction;            /* Callback when device is resumed. */

    /* Host MSC class driver registration. */
    R_usb_hstd_DriverRegistration(ptr, &driver);
}
/******************************************************************************
End of function usb_hapl_registration
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_apl_init
Description     : Clear application.
Arguments       : USB_UTR_t *ptr        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void usb_hmsc_apl_init(USB_UTR_t *ptr)
{
    /* Init this application's drive open sequence state. */
    usb_ghmsc_SmpAplProcess = USB_HMSC_WAIT;
}
/******************************************************************************
End of function usb_hmsc_apl_init
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_StrgCommandResult
Description     : Callback function of storage drive search
Arguments       : USB_UTR_t *mess        : The app's USB Comm. Structure.
Return value    : none
******************************************************************************/
void usb_hmsc_StrgCommandResult( USB_UTR_t *mess, uint16_t data1, uint16_t data2 )
{
    USB_ER_t        err;
    USB_MH_t        p_blf;
    USB_CLSINFO_t   *mes;
    USB_CLSINFO_t   *cp;

    /* Switch action depending on message. */
    mes = (USB_CLSINFO_t *)mess;

    /* Device status setting = attached. */
    R_usb_hmsc_SetDevSts(0,(uint16_t)USB_HMSC_DEV_ATT);

    /* Application initialization sequence start. */

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HMSCSMP_MPL, &p_blf) == USB_E_OK )
    {
        /* Send message to myself: Drive mount. */
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo    = USB_HMSC_DRIVEMOUNT;
        cp->keyword    = mes->keyword;
        cp->result     = mes->result;

        /* Send message */
        err = R_USB_SND_MSG(USB_HMSCSMP_MBX, (USB_MSG_t*)p_blf);
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HMSCSMP_MPL, (USB_MH_t)p_blf);
            USB_PRINTF0("### CommandResult snd_msg error\n");
            while( 1 );
        }
    }
    else
    {
        USB_PRINTF0("### CommandResult pget_blk error\n");
        while( 1 );
    }
}
/******************************************************************************
End of function usb_hmsc_StrgCommandResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SampleAplTask
Description     : Sample application task for driving the USB-BASIC-FW and HMSC.
                : Received the detection message of the USB device, do sequence
                : for mount to the file system.
Argument        : none
Return value    : none
******************************************************************************/
void usb_hmsc_SampleAplTask(void)
{
    USB_UTR_t       *mess;
    USB_ER_t        err;
    FRESULT         res;
    USB_CLSINFO_t   *mes;
    uint16_t        addr;
    uint8_t 		usbEnter;

#ifdef FREE_RTOS_PP
    for( ;; )
    {
#endif
    /* Check for incoming application messages. */
    err = R_USB_TRCV_MSG(USB_HMSCSMP_MBX, (USB_MSG_t**)&mess, (USB_TM_t)5000);
    if( err != USB_OK )
    {
#ifdef FREE_RTOS_PP
        continue;
#else
        return;
#endif
    }

    /* Switch action depending on message. */
    mes = (USB_CLSINFO_t *)mess;
    switch( mes->msginfo )
    {
    	/* Device detection */
        case USB_HMSC_DRIVE_OPEN:
            usb_ghmsc_SmpAplProcess = USB_HMSC_DRIVE_OPEN;

            /* Set device address. */
            addr = mes->keyword;

            /* Storage drive search. */
            R_usb_hmsc_StrgDriveSearch(mess, addr, (USB_CB_t)&usb_hmsc_StrgCommandResult);
            break;
        
            /* Mount to the file system */
        case USB_HMSC_DRIVEMOUNT:
        	/* File system media work area memory mount. */
        	res = R_tfat_f_mount(0, &usb_gFatfs);
        	if( res != TFAT_FR_OK )
        	{
        		USB_PRINTF1("R_tfat_f_mount error: %d\n", res);
        	}
        	LED10 = LED_ON;
        	usbEnter = USB_ENTER;
            xQueueSend( qKeyboard, &usbEnter, 0 );
        	/* Send message to myself: Wait. */
        	usb_ghmsc_SmpAplProcess = USB_HMSC_WAIT;
        	break;

        case USB_HMSC_WAIT:
        	break;

        default:
        	break;
    }

    /* Release message memory from pool. */
    err = R_USB_REL_BLK(USB_HMSCSMP_MPL,(USB_MH_t)mess);
    if( err != USB_E_OK )
    {
        USB_PRINTF0("### USB Hsmp Task rel_blk error\n");
    }
#ifdef FREE_RTOS_PP
    vTaskDelay(10);
    }
#endif
}
/******************************************************************************
End of function usb_hmsc_SampleAplTask
******************************************************************************/

/******************************************************************************
End of file
******************************************************************************/
