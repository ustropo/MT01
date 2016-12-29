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

#define KEY_UP 		(0x0100U)
#define KEY_DOWN 	(0x0400U)
#define KEY_RIGHT	(0x0200U)
#define KEY_LEFT	(0x0020U)
#define KEY_ESC		(0x0001U)
#define KEY_ENTER 	(0x0004U)
#define KEY_Z_UP 	(0x0010U)
#define KEY_Z_DOWN 	(0x0040U)
#define KEY_RELEASED (0)

#define USB_DISCONNECTED (0x55555555)
#define EMERGENCIA_SIGNAL (0xAAAA0000)
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
