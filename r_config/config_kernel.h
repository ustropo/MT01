/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************
* Copyright (C) 2013 Renesas Electronics Corpration
* and Renesas Solutions Corp. All rights reserved.
******************************************************************************
* File Name    : config_kernel.h
* Version      : 1.00
* Device(s)    : Renesas RX-Series
* Tool-Chain   : Renesas RX Standard Toolchain
* OS           : FreeRTOS V7.4.0
* H/W Platform :
* Description  : FreeRTOS configuration
******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 24.04.2013 0.50    First Release
******************************************************************************/

#include "task.h"
#include "semphr.h"
/***********************************************************************************************************************
External global variables and functions
***********************************************************************************************************************/
extern xTaskHandle* task_table[];
extern xQueueHandle* mbox_table[];
extern xQueueHandle* mpl_table[];

extern void FreeRTOSConfig(void);
extern xSemaphoreHandle semaphore_table[];
/******************************************************************************
End  Of File
******************************************************************************/
