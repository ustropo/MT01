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
/******************************************************************************
* File Name    : config_kernel.c
* Version      : 1.00
* Device(s)    : Renesas RX-Series
* Tool-Chain   : Renesas RX Standard Toolchain
* OS           : FreeRTOS V7.4.0
* H/W Platform :
* Description  : FreeRTOS configuration
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.06.2013 0.50    First Release
******************************************************************************/

#ifdef FREE_RTOS_PP
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "ut_state.h"

#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "ff.h"

#include "plasma.h"

extern TaskHandle_t xCncTaskHandle;

/******************************************************************************
Private global variables and functions
******************************************************************************/

#if SOCKET_IF_USE_SEMP
xSemaphoreHandle r_socket_semaphore;
#endif

xTaskHandle task_main_handle;
xTaskHandle task_handle0;
xTaskHandle task_handle1;
xTaskHandle task_handle2;
xTaskHandle task_handle3;
xTaskHandle task_handle4;
xTaskHandle task_handle5;

xTaskHandle* task_table[] = {
    &task_handle0,  /* USB_PCD_TASK   */
    &task_handle1,  /* USB_HCD_TASK   */
    &task_handle2,  /* USB_MGR_TASK   */
    &task_handle3,  /* USB_HUB_TASK   */
    &task_handle4,  /* USB_HMSC_TASK  */
    &task_handle5,  /* USB_HSTRG_TASK */
};

xQueueHandle mbox_handle0;
xQueueHandle mbox_handle1;
xQueueHandle mbox_handle2;
xQueueHandle mbox_handle3;
xQueueHandle mbox_handle4;
xQueueHandle mbox_handle5;
xQueueHandle mbox_handle6;
xQueueHandle mbox_handle7;

xQueueHandle* mbox_table[] = {
    &mbox_handle0,  /* USB_PCD_MBX     */
    &mbox_handle1,  /* USB_HCD_MBX     */
    &mbox_handle2,  /* USB_MGR_MBX     */
    &mbox_handle3,  /* USB_HUB_MBX     */
    &mbox_handle4,  /* USB_HMSC_MBX    */
    &mbox_handle5,  /* USB_HSTRG_MBX   */
    &mbox_handle6, /* USB_HMSCSMP_MBX  */
    &mbox_handle7, /* USB_TFAT_MBX    */
};

xQueueHandle mpl_handle0;
xQueueHandle mpl_handle1;
xQueueHandle mpl_handle2;
xQueueHandle mpl_handle3;
xQueueHandle mpl_handle4;
xQueueHandle mpl_handle5;
xQueueHandle mpl_handle6;
xQueueHandle mpl_handle7;

xQueueHandle* mpl_table[] = {
    &mpl_handle0,   /* USB_PCD_MPL     */
    &mpl_handle1,   /* USB_HCD_MPL     */
    &mpl_handle2,   /* USB_MGR_MPL     */
    &mpl_handle3,   /* USB_HUB_MPL     */
    &mpl_handle4,   /* USB_HMSC_MPL    */
    &mpl_handle5,   /* USB_HSTRG_MPL   */
    &mpl_handle6,   /* USB_HMSCSMP_MPL   */
    &mpl_handle7,   /* USB_TFAT_MPL   */
};

xQueueHandle qKeyboard;

/***********************************************************************************************************************
External function Prototypes
***********************************************************************************************************************/
extern void main_task( void * pvParameters);
extern void R_httpd(void);
extern void usb_hmsc_Task(void);
extern void usb_hmsc_StrgDriveTask(void);
extern void hmsc_cstd_task_start( void );
extern void keyboard_task(void);
extern void states_task(void);
extern void main_cnc_task(void);
void usb_hmsc_main_task(USB_VP_INT stacd);

/******************************************************************************
Function Name   : vApplicationMallocFailedHook
Description     : Hook function
Arguments       : none
Return value    : none
******************************************************************************/
void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/******************************************************************************
End of function vApplicationMallocFailedHook()
******************************************************************************/

/******************************************************************************
Function Name   : vApplicationStackOverflowHook
Description     : Hook function
Arguments       : xTaskHandle pxTask, signed char *pcTaskName
Return value    : none
******************************************************************************/
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
/******************************************************************************
End of function vApplicationStackOverflowHook()
******************************************************************************/
void vAssertCalled( void )
{
volatile unsigned long ul = 0;

	taskENTER_CRITICAL();
	{
		/* Use the debugger to set ul to a non-zero value in order to step out
		of this function to determine why it was called. */
		while( ul == 0 )
		{
			portNOP();
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{

}

void vApplicationTickHook( void )
{
}

/******************************************************************************
Function Name   : vApplicationSetupTimerInterrupt
Description     : setup tick timer
Arguments       : none
Return value    : none
******************************************************************************/
void vApplicationSetupTimerInterrupt( void )
{
    /* protect off */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Enable compare match timer 0. */
    MSTP( CMT0 ) = 0;

    /* Interrupt on compare match. */
    //CMT0.CMCR.BIT.CMIE = 1;
    /* Divide the PCLK by 8. */
    //CMT0.CMCR.BIT.CKS = 0;
    CMT0.CMCR.WORD = 0x00C0; // CKS=00b,CMIE=1; PCLK/8,Compare match interrupt (CMIn) enabled @48MHz
    /* Set the compare match value. */
    CMT0.CMCOR = ( unsigned short ) ( ( ( configPERIPHERAL_CLOCK_HZ / configTICK_RATE_HZ )) / 8 - 1);

    /* Enable the interrupt... */
    _IEN( _CMT0_CMI0 ) = 1;
    
    /* ...and set its priority to the application defined kernel priority. */
    _IPR( _CMT0_CMI0 ) = configKERNEL_INTERRUPT_PRIORITY;
    
    /* Start the timer. */
    CMT.CMSTR0.BIT.STR0 = 1;

    /* protect on */
    SYSTEM.PRCR.WORD = 0xA500;

}
/******************************************************************************
End of function vApplicationSetupTimerInterrupt()
******************************************************************************/
#define NUM 10
/******************************************************************************
Function Name   : FreeRTOSConfig
Description     : create task
Arguments       : none
Return value    : none
******************************************************************************/
void FreeRTOSConfig( void )
{
    int8_t i;
    void *tmpPtr = NULL;

    /* Mail box */
#if 0
    mbox_handle0  = xQueueCreate( 10, sizeof(void *) ); /* USB_PCD_MBX     */
#else
    mbox_handle0 = NULL;
#endif
    mbox_handle1  = xQueueCreate( NUM, sizeof(void *) ); /* USB_HCD_MBX     */
    mbox_handle2  = xQueueCreate( NUM, sizeof(void *) ); /* USB_MGR_MBX     */
    mbox_handle3  = xQueueCreate( NUM, sizeof(void *) ); /* USB_HUB_MBX     */
    mbox_handle4  = xQueueCreate( NUM, sizeof(void *) ); /* USB_HMSC_MBX    */
    mbox_handle5  = xQueueCreate( NUM, sizeof(void *) ); /* USB_HSTRG_MBX   */
    mbox_handle6  = xQueueCreate( NUM, sizeof(void *) ); /* USB_HMSCSMP_MBX   */
    mbox_handle7  = xQueueCreate( 1, sizeof(void *) ); /* USB_TFAT_MBX   */

    /* Memory pool */
#if 0
    /* USB_PCD_MPL     */
    mpl_handle0 = xQueueCreate( NUM, sizeof(void *) );

    for (i = 0; i < 10; i++ )
    {
    	tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
    	if ( tmpPtr == NULL )
    	{
    		for( ;; );
    	}
    	else
    	{
    		xQueueSend( mpl_handle0, &tmpPtr, 0 );
    	}
    }
#endif
    /* USB_HCD_MPL     */
    mpl_handle1 = xQueueCreate( NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle1, &tmpPtr, 0 );
        }
    }

    /* USB_MGR_MPL     */
    mpl_handle2 = xQueueCreate( NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle2, &tmpPtr, 0 );
        }
    }

    /* USB_HUB_MPL     */
    mpl_handle3 = xQueueCreate( NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle3, &tmpPtr, 0 );
        }
    }

    /* USB_HMSC_MPL    */
    mpl_handle4 = xQueueCreate( NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle4, &tmpPtr, 0 );
        }
    }

    /* USB_HSTRG_MPL   */
    mpl_handle5 = xQueueCreate( NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle5, &tmpPtr, 0 );
        }
    }
    /* USB_HMSCSMP_MPL    */
    mpl_handle6 = xQueueCreate(  NUM, sizeof(void *) );

    for ( i = 0; i < NUM; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle6, &tmpPtr, 0 );
        }
    }
    /* USB_TFAT_MPL    */
    mpl_handle7 = xQueueCreate(  1, sizeof(void *) );

    for ( i = 0; i < 1; i++ )
    {
        tmpPtr = pvPortMalloc( sizeof( USB_UTR_t ) );
        if ( tmpPtr == NULL )
        {
            for( ;; );
        }
        else
        {
            xQueueSend( mpl_handle7, &tmpPtr, 0 );
        }
    }

	qKeyboard = xQueueCreate(  1, sizeof(void *) );
    /* Task */
#if 0
    xTaskCreate( (pdTASK_CODE)R_usb_pstd_PcdTask,     "USB_PCD_TSK     ",  512, NULL, 5, &task_handle0 ); /* USB_PCD_TASK   */
#endif

    /* USB tasks */
//    xTaskCreate( (pdTASK_CODE)R_usb_hstd_HcdTask,     "USB_HCD_TSK     ",  128, NULL, 5, &task_handle1 ); /* USB_HCD_TASK   */
//    xTaskCreate( (pdTASK_CODE)R_usb_hstd_MgrTask,     "USB_MGR_TSK     ",  128, NULL, 4, &task_handle2 ); /* USB_MGR_TASK   */
//    xTaskCreate( (pdTASK_CODE)R_usb_hhub_Task,        "USB_HUB_TSK     ",  128, NULL, 3, &task_handle3 ); /* USB_HUB_TASK   */
//
//    xTaskCreate( (pdTASK_CODE)R_usb_hmsc_Task,          "USB_HMSC_TSK    ",  128, NULL, 3, &task_handle4 ); /* USB_HMSC_TASK  */
//    xTaskCreate( (pdTASK_CODE)R_usb_hmsc_StrgDriveTask, "USB_HSTRG_TSK   ",  128, NULL, 3, &task_handle5 ); /* USB_HSTRG_TASK */
//    xTaskCreate( (pdTASK_CODE)hmsc_cstd_task_start,     "HMSC_MAIN_TSK   ",  128, NULL, 2, NULL); /* HMSC_MAIN_TASK      */

    xTaskCreate( (pdTASK_CODE)R_usb_hstd_HcdTask,     "USB_HCD_TSK     ",  128, NULL, 6, &task_handle1 ); /* USB_HCD_TASK   */
    xTaskCreate( (pdTASK_CODE)R_usb_hstd_MgrTask,     "USB_MGR_TSK     ",  128, NULL, 5, &task_handle2 ); /* USB_MGR_TASK   */
    xTaskCreate( (pdTASK_CODE)R_usb_hhub_Task,        "USB_HUB_TSK     ",  128, NULL, 4, &task_handle3 ); /* USB_HUB_TASK   */

    xTaskCreate( (pdTASK_CODE)R_usb_hmsc_Task,          "USB_HMSC_TSK    ",  128, NULL, 4, &task_handle4 ); /* USB_HMSC_TASK  */
    xTaskCreate( (pdTASK_CODE)R_usb_hmsc_StrgDriveTask, "USB_HSTRG_TSK   ",  128, NULL, 4, &task_handle5 ); /* USB_HSTRG_TASK */
    xTaskCreate( (pdTASK_CODE)hmsc_cstd_task_start,     "HMSC_MAIN_TSK   ",  128, NULL, 3, NULL); /* HMSC_MAIN_TASK      */
    /*User interface task*/
    xTaskCreate( (pdTASK_CODE)keyboard_task,     "keyboard_task    ",  512, NULL, 2, NULL); /* keyboard_task      */
    xTaskCreate( (pdTASK_CODE)states_task,     "states_task    ",  2048, NULL, 1, &task_main_handle); /* states_task      */
    xTaskCreate( (pdTASK_CODE)main_cnc_task,     "CNC_task   ",  2048, NULL, 1, &xCncTaskHandle); /* CNC_task      */

}

/******************************************************************************/
#endif /* FREE_RTOS_PP */
/******************************************************************************
End  Of File
******************************************************************************/
