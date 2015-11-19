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
* File Name    : r_usb_cscheduler.c
* Description  : USB Host and Peripheral scheduler code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"

#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "config_kernel.h"
#endif
#ifdef FREE_RTOS_PP
    /* empty */
#else
/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_IDMAX           11u                 /* Maximum Task ID +1 */
#define USB_PRIMAX          8u                  /* Maximum Priority number +1 */
#define USB_BLKMAX          20u                 /* Maximum block */
#define USB_TABLEMAX        USB_BLKMAX          /* Maximum priority table */
#define USB_WAIT_EVENT_MAX  (5u)

/******************************************************************************
Private global variables and functions
******************************************************************************/

/* Priority Table */
static USB_MSG_t*   usb_scstd_TableAdd[USB_PRIMAX][USB_TABLEMAX];
static uint8_t      usb_scstd_TableID[USB_PRIMAX][USB_TABLEMAX];
static uint8_t      usb_scstd_PriR[USB_PRIMAX];
static uint8_t      usb_scstd_PriW[USB_PRIMAX];
static uint8_t      usb_scstd_Pri[USB_IDMAX];

/* Schedule Set Flag  */
static uint8_t      usb_scstd_ScheduleFlag;

/* Fixed-sized memory pools */
static USB_UTR_t    usb_scstd_Block[USB_BLKMAX];
static uint8_t      usb_scstd_BlkFlg[USB_BLKMAX];

static USB_MSG_t*   usb_scstd_Add_use;
static uint8_t      usb_scstd_ID_use;

/* Wait MSG */
static USB_MSG_t*   usb_scstd_WaitAdd[USB_IDMAX][USB_WAIT_EVENT_MAX];
static uint16_t     usb_scstd_WaitCounter[USB_IDMAX][USB_WAIT_EVENT_MAX];
#endif /* FREE_RTOS_PP */

/******************************************************************************
External variables and functions
******************************************************************************/
extern void     usb_cpu_int_enable(USB_UTR_t *ptr);
extern void     usb_cpu_int_disable(USB_UTR_t *ptr);

/******************************************************************************
Static variables and functions
******************************************************************************/

/******************************************************************************
Renesas Scheduler API functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_RecMsg
Description     : Receive a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t** mess  : Message pointer
                : USB_TM_t tm       : Timeout Value
Return          : uint16_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t usb_cstd_RecMsg( uint8_t id, USB_MSG_t** mess, USB_TM_t tm )
{
#ifdef FREE_RTOS_PP
    signed portBASE_TYPE err;
    xQueueHandle handle;

    handle = *( mbox_table[ id ] );
    *mess = NULL;
    tm; /* unused */

    /* xQueueReceive never time out */
    err = xQueueReceive( handle, (void *)mess, portMAX_DELAY );

    if (( err == pdTRUE ) && ( *mess != NULL ))
    {
        return USB_E_OK;
    }
    else
    {
        return USB_E_ERROR;
    }
#else
    if(( id < USB_IDMAX ) && ( usb_scstd_ID_use < USB_IDMAX ))
    {
        if( id == usb_scstd_ID_use )
        {
            *mess = usb_scstd_Add_use;
            return  USB_E_OK;
        }
    }   
    return  USB_E_ERROR;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_RecMsg
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_RecMsg
Description     : Receive a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t** mess  : Message pointer
                : USB_TM_t tm       : Timeout Value
Return          : uint16_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t R_usb_cstd_RecMsg( uint8_t id, USB_MSG_t** mess, USB_TM_t tm )
{
    USB_ER_t err;

    err = usb_cstd_RecMsg( id,  mess, tm );

    return err;
}
/******************************************************************************
End of function R_usb_cstd_RecMsg
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_SndMsg
Description     : Send a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t* mess   : Message pointer
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t usb_cstd_SndMsg( uint8_t id, USB_MSG_t* mess )
{
#ifdef FREE_RTOS_PP
    signed portBASE_TYPE err;
    xQueueHandle handle;

    handle = *( mbox_table[ id ] );

    err = xQueueSend( handle, (const void *)&mess, 0 );

    if ( err == pdTRUE )
    {
        return USB_E_OK;
    }
    else
    {
        return USB_E_ERROR;
    }
#else
    USB_ER_t    status;

    /* USB interrupt disable */
    usb_cpu_int_disable((USB_UTR_t*)mess);
    status = usb_cstd_iSndMsg(id,mess);
    /* USB interrupt enable */
    usb_cpu_int_enable((USB_UTR_t*)mess);
    return status;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_SndMsg
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_SndMsg
Description     : Send a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t* mess   : Message pointer
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t R_usb_cstd_SndMsg( uint8_t id, USB_MSG_t* mess )
{
    USB_ER_t    status;

    status = usb_cstd_SndMsg( id, mess );

    return status;
}
/******************************************************************************
End of function R_usb_cstd_SndMsg
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_iSndMsg
Description     : Send a message to the specified id (mailbox) while executing 
                : an interrupt.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t* mess   : Message pointer
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t usb_cstd_iSndMsg( uint8_t id, USB_MSG_t* mess )
{
#ifdef FREE_RTOS_PP
    signed portBASE_TYPE err;
    xQueueHandle handle;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    handle = *( mbox_table[ id ] );

    err = xQueueSendFromISR( handle, (const void *)&mess, &xHigherPriorityTaskWoken );

    if ( err == pdTRUE )
    {
        portYIELD_FROM_ISR ( xHigherPriorityTaskWoken );
        return USB_E_OK;
    }
    else
    {
        return USB_E_ERROR;
    }
#else
    uint8_t usb_pri;        /* Task Priority */
    uint8_t usb_write;      /* Priority Table Writing pointer */

    if( id < USB_IDMAX )
    {
        /* Read priority and table pointer */
        usb_pri     = usb_scstd_Pri[id];
        usb_write   = usb_scstd_PriW[usb_pri];
        if( usb_pri < USB_PRIMAX )
        {
            /* Renewal write pointer */
            usb_write++;
            if( usb_write >= USB_TABLEMAX )
            {
                usb_write = USB_TBLCLR;
            }
            /* Check pointer */
            if( usb_write == usb_scstd_PriR[usb_pri])
            {
                return  USB_E_ERROR;
            }
            /* Save message */
            /* Set priority table */
            usb_scstd_TableID[usb_pri][usb_write]   = id;
            usb_scstd_TableAdd[usb_pri][usb_write]  = mess;
            usb_scstd_PriW[usb_pri]                 = usb_write;
            return  USB_E_OK;
        }
    }
    USB_PRINTF0("SND_MSG ERROR !!\n");
    return  USB_E_ERROR;
#endif /* FREE_RTOS_PP */

}
/******************************************************************************
End of function usb_cstd_iSndMsg
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_iSndMsg
Description     : Send a message to the specified id (mailbox) while executing 
                : an interrupt.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_MSG_t* mess   : Message pointer
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t R_usb_cstd_iSndMsg( uint8_t id, USB_MSG_t* mess )
{
    USB_ER_t err;

    err = usb_cstd_iSndMsg( id, mess );

    return err;
}
/******************************************************************************
End of function R_usb_cstd_iSndMsg
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_PgetBlk
Description     : Get a memory block for the caller.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_UTR_t** blk   : Memory block pointer.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t usb_cstd_PgetBlk( uint8_t id, USB_UTR_t** blk )
{
#ifdef FREE_RTOS_PP
    signed portBASE_TYPE err;
    xQueueHandle handle;
    handle = *( mpl_table[ id ] );
    err = xQueueReceive( handle, (void *)blk, (portTickType)0 );
    if ( err == pdTRUE )
    {
        return USB_E_OK;
    }
    else
    {
        return USB_E_ERROR;
    }
#else
    uint8_t usb_s_pblk_c;

    if( id < USB_IDMAX )
    {
        usb_s_pblk_c = USB_CNTCLR;
        while(usb_s_pblk_c != USB_BLKMAX)
        {
            if(usb_scstd_BlkFlg[usb_s_pblk_c] == USB_FLGCLR)
            {
                /* Acquire fixed-size memory block */
                *blk    = &usb_scstd_Block[usb_s_pblk_c];
                usb_scstd_BlkFlg[usb_s_pblk_c]  = USB_FLGSET;
                return  USB_E_OK;
            }
            usb_s_pblk_c++;
        }
        /* Error of BLK Table Full !!  */
        USB_PRINTF1("usb_scBlkFlg[%d][] Full !!\n",id);
    }
    return  USB_E_ERROR;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_PgetBlk
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_PgetBlk
Description     : Call the get memory block function to get a memory block for 
                : the caller.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_UTR_t** blk   : Memory block pointer.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t R_usb_cstd_PgetBlk( uint8_t id, USB_UTR_t** blk )
{
    USB_ER_t err;

    err = usb_cstd_PgetBlk( id, blk );

    return err;
}
/******************************************************************************
End of function R_usb_cstd_PgetBlk
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_RelBlk
Description     : Release a memory block.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_UTR_t* blk    : Memory block pointer.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t usb_cstd_RelBlk( uint8_t id,  USB_UTR_t* blk )
{
#ifdef FREE_RTOS_PP
    signed portBASE_TYPE err;
    xQueueHandle handle;

    handle = *( mpl_table[ id ] );
    err = xQueueSend( handle, (void *)&blk, (portTickType)0 );

    if ( err == pdTRUE )
    {
        return USB_E_OK;
    }
    else
    {
        return USB_E_ERROR;
    }
#else
    uint16_t    usb_s_rblk_c;

    if( id < USB_IDMAX )
    {
        usb_s_rblk_c = USB_CNTCLR;
        while(usb_s_rblk_c != USB_BLKMAX)
        {
            if(blk == &usb_scstd_Block[usb_s_rblk_c])
            {
                /* Release fixed-size memory block */
                usb_scstd_BlkFlg[usb_s_rblk_c] = USB_FLGCLR;
                return  USB_E_OK;
            }
            usb_s_rblk_c++;
        }
        /* Error of BLK Flag is not CLR !!  */
        USB_PRINTF0("TskBlk NO CLR !!\n");
    }
    return  USB_E_ERROR;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_RelBlk
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_RelBlk
Description     : Call the function to release a memory block.
Argument        : uint8_t id        : ID number (mailbox).
                : USB_UTR_t* blk    : Memory block pointer.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR
******************************************************************************/
USB_ER_t R_usb_cstd_RelBlk( uint8_t id,  USB_UTR_t* blk )
{
    USB_ER_t err;

    err = usb_cstd_RelBlk( id, blk );

    return  err;
}
/******************************************************************************
End of function R_usb_cstd_RelBlk
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_WaiMsg
Description     : Runs USB_SND_MSG after running the scheduler the specified 
                : number of times.
Argument        : uint8_t  id       : ID number (mailbox).
                : USB_MSG_t *mess   : Message pointer.
                : uint16_t  times   : Timeout value.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR.
******************************************************************************/
USB_ER_t usb_cstd_WaiMsg( uint8_t id, USB_MSG_t* mess, USB_TM_t times )
{
#ifdef FREE_RTOS_PP
    USB_ER_t err;

    vTaskDelay( times );

    err = usb_cstd_SndMsg( id, mess );

    return err;
#else
    uint8_t i;

    if( id < USB_IDMAX )
    {
        for( i =0; i <USB_WAIT_EVENT_MAX ; i++ )
        {
            if( usb_scstd_WaitCounter[id][i] == 0 )
            {
                usb_scstd_WaitAdd[id][i] = mess;
                usb_scstd_WaitCounter[id][i] = times;
                return  USB_E_OK;
            }
        }
    }
    /* Error !!  */
    USB_PRINTF0("WAI_MSG ERROR !!\n");
    return  USB_E_ERROR;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_WaiMsg
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_WaiMsg
Description     : Will run USB_SND_MSG after running the scheduler the specified 
                : number of times.
Argument        : uint8_t  id       : ID number (mailbox).
                : USB_MSG_t *mess   : Message pointer.
                : uint16_t  times   : Timeout value.
Return          : USB_ER_t          : USB_E_OK / USB_E_ERROR.
******************************************************************************/
USB_ER_t R_usb_cstd_WaiMsg( uint8_t id, USB_MSG_t* mess, USB_TM_t times )
{
    USB_ER_t err;

    err = usb_cstd_WaiMsg( id, mess, times );

    return  err;
}
/******************************************************************************
End of function R_usb_cstd_WaiMsg
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_WaitScheduler
Description     : Schedules a wait request.
Argument        : none
Return          : none
******************************************************************************/
void usb_cstd_WaitScheduler(void)
{
#ifdef FREE_RTOS_PP
    /* empty */
    /* This function is not used.
       Nothing is processed in this function. */
#else
    USB_ER_t    err;
    uint8_t     id;
    uint8_t i;

    for( id=0; id < USB_IDMAX; id++ )
    {
        for( i =0; i <USB_WAIT_EVENT_MAX ; i++ )
        {
            if(usb_scstd_WaitCounter[id][i] != 0)
            {
                usb_scstd_WaitCounter[id][i]--;
                if(usb_scstd_WaitCounter[id][i] == 0)
                {
                    err = usb_cstd_SndMsg(id, usb_scstd_WaitAdd[id][i]);
                    if( err != USB_E_OK )
                    {
                        usb_scstd_WaitCounter[id][i]++;
                    }
                }
            }
        }
    }
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_WaitScheduler
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_ScheInit
Description     : Scheduler initialization.
Argument        : none
Return          : none
******************************************************************************/
void usb_cstd_ScheInit(void)
{
#ifdef FREE_RTOS_PP
    /* empty */
    /* This function is not used.
       Nothing is processed in this function. */
#else
    uint8_t i;
    uint8_t j;

    /* Initial Scheduler */
    usb_scstd_ID_use        = USB_NONE;
    usb_scstd_ScheduleFlag  = USB_NONE;

    /* Initialize  priority table pointer and priority table */
    for(i=0;i!=USB_PRIMAX;i++)
    {
        usb_scstd_PriR[i]   = USB_NONE;
        usb_scstd_PriW[i]   = USB_NONE;
        for(j=0;j!=USB_TABLEMAX;j++)
        {
            usb_scstd_TableID[i][j] = USB_IDMAX;
        }
    }

    /* Initialize block table */
    for(i=0;i!=USB_BLKMAX;i++)
    {
        usb_scstd_BlkFlg[i]         = USB_NONE;
    }

    /* Initialize priority */
    for(i=0;i!=USB_IDMAX;i++)
    {
        usb_scstd_Pri[i]            = (uint8_t)USB_IDCLR;
        for( j =0; j <USB_WAIT_EVENT_MAX ; j++ )
        {
            usb_scstd_WaitAdd[i][j] = (USB_MSG_t*)USB_NONE;
            usb_scstd_WaitCounter[i][j] = USB_NONE;
        }
    }
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function usb_cstd_ScheInit
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_Scheduler
Description     : The scheduler.
Argument        : none
Return          : none
******************************************************************************/
void R_usb_cstd_Scheduler(void)
{
#ifdef FREE_RTOS_PP
    /* empty */
    /* This function is not used.
       Nothing is processed in this function. */
#else
    uint8_t    usb_pri;         /* Priority Counter */
    uint8_t    usb_read;        /* Priority Table read pointer */

    /* wait msg */
    usb_cstd_WaitScheduler();

    /* Priority Table reading */
    usb_pri = USB_CNTCLR;
    while( usb_pri < USB_PRIMAX )
    {
        usb_read    = usb_scstd_PriR[usb_pri];
        if( usb_read != usb_scstd_PriW[usb_pri] )
        {
            /* Priority Table read pointer increment */
            usb_read++;     
            if( usb_read >= USB_TABLEMAX )
            {
                usb_read = USB_TBLCLR;
            }
            /* Set practise message */
            usb_scstd_ID_use    = usb_scstd_TableID[usb_pri][usb_read];
            usb_scstd_Add_use   = usb_scstd_TableAdd[usb_pri][usb_read];
            usb_scstd_TableID[usb_pri][usb_read]    = USB_IDMAX;
            usb_scstd_PriR[usb_pri]                 = usb_read;
            usb_scstd_ScheduleFlag                  = USB_FLGSET;
            usb_pri = USB_PRIMAX;
        }
        else
        {
            usb_pri++;
        }
    }
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function R_usb_cstd_Scheduler
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_SetTaskPri
Description     : Set a task's priority.
Argument        : uint8_t tasknum   : Task id.
                : uint8_t pri       : The task priority to be set.
Return          : none
******************************************************************************/
void R_usb_cstd_SetTaskPri(uint8_t tasknum, uint8_t pri)
{
#ifdef FREE_RTOS_PP
    /* empty */
    /* This function is not used.
       Nothing is processed in this function. */
#else
    if(tasknum < USB_IDMAX)
    {
        if(pri < USB_PRIMAX)
        {
            usb_scstd_Pri[tasknum]=pri;
        }
        else if(pri == (uint8_t)USB_IDCLR)
        {
            usb_scstd_Pri[tasknum]=(uint8_t)USB_IDCLR;
        }
        else
        {
        }
    }
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function R_usb_cstd_SetTaskPri
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_CheckSchedule
Description     : Check schedule flag to see if caller's "time has come", then 
                : clear it.
Argument        : none
Return          : flg   : usb_scstd_ScheduleFlag
******************************************************************************/
uint8_t R_usb_cstd_CheckSchedule(void)
{
#ifdef FREE_RTOS_PP
    /* This function is not used. */
    return USB_FLGCLR; /* In order not to take out an error with compile. */
#else
    uint8_t flg;

    flg = usb_scstd_ScheduleFlag;
    usb_scstd_ScheduleFlag = USB_FLGCLR;
    return flg;
#endif /* FREE_RTOS_PP */
}
/******************************************************************************
End of function R_usb_cstd_CheckSchedule
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
