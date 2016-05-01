/*
 * keyboard.h
 *
 *  Created on: 07/08/2015
 *      Author: LAfonso01
 */

#ifndef SRC_KEYBOARD_H_
#define SRC_KEYBOARD_H_

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "platform.h"

/*****************************************************************************
Macro definitions
******************************************************************************/
#ifndef MODULO
#define KEY_UP 		(0x0004U)
#define KEY_DOWN 	(0x0010U)
#define KEY_RIGHT	(0x2000U)
#define KEY_LEFT	(0x20000000U)
#define KEY_ESC		(0x40000U)
#define KEY_ENTER 	(0x100000U)
#define KEY_Z_UP 	(0x4000000U)
#define KEY_Z_DOWN 	(0x10000000U)
#define KEY_RELEASED (0)
#else
#define KEY_UP 		(0x910000)
#define KEY_DOWN 	(0xc10000)
#define KEY_RIGHT	(0xa10000)
#define KEY_LEFT	(0xa100)
#define KEY_ESC		(0x91)
#define KEY_ENTER 	(0xc1)
#define KEY_Z_UP 	(0x9100)
#define KEY_Z_DOWN 	(0xc100)
#define KEY_RELEASED (0)
#endif

#define USB_DISCONNECTED (0x55555555)
#define EMERGENCIA_SIGNAL (0xAAAAAAAA)
/******************************************************************************
Section    <Section Definition> , "Data Sections"
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/
extern xQueueHandle qKeyboard;
/*****************************************************************************
Enumerated Types
******************************************************************************/

/******************************************************************************
Section    <Section Definition> , "Project Sections"
******************************************************************************/

#endif /* SRC_KEYBOARD_H_ */
