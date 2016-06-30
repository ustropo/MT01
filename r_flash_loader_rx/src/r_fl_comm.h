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
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_fl_comm.h
* Version      : 3.00
* Description  : Has functions for communicating with Host.  This file will
*                change depending on what communications medium is used to 
*                communicate with the host.
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 23.02.2012 3.00    Moved 'FL_COM_CHANNEL' macro into C source file
*                              for UART COM layer. This was done because this
*                              only applied to the UART implementation.
******************************************************************************/

#ifndef FL_COM_H
#define FL_COM_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Fixed width types support. */
#include <stdint.h>
/* Used for bool. */
#include <stdbool.h>

/******************************************************************************
Macro definitions
******************************************************************************/
/* DEFINEs associated with communications protocol */
/* These first two are for initializing communications */
#define FL_COM_INIT_1               (0xBC)
#define FL_COM_INIT_2               (0xCD)
/* Acknowledge command */
#define FL_COM_ACK                  (0xB1)
/* Error has occurred */
#define FL_COM_ERROR                (0xB2)
/* Tells host that transmitted block was corrupted, send again */
#define FL_COM_RESEND               (0xB3)
/* Tells MCU that host has sent all records */
#define FL_COM_DONE                 (0xB4)

/* Command from host for new image to be downloaded */
#define FL_COM_OP_NEW_IMAGE         (0xD1)
/* Command from host for info on current images */
#define FL_COM_OP_INFO_REQ          (0xD2)
/* Command from host to erase a load block */
#define FL_COM_OP_ERASE_BLOCK       (0xD3)

/* Reply that image is stored, just needs to be flashed */
#define FL_COM_REP_NEEDS_FLASH      (0xC1)
/* Reply that the requested image is already running */
#define FL_COM_REP_ALREADY_RUNNING  (0xC2)
/* Reply that there was not enough room for the new image */
#define FL_COM_REP_NO_ROOM          (0xC3)
/* Reply that max_block_size is too large */
#define FL_COM_REP_BLOCK_TOO_LARGE  (0xC4)
/* Reply that image is to large to store */
#define FL_COM_REP_IMAGE_TOO_LARGE  (0xC5)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void     fl_com_init(void);
void     fl_com_receive(uint8_t *rx_buffer, uint32_t rx_index);
uint32_t fl_com_bytes_received(void);
void     fl_com_transmit(uint8_t *tx_buffer, uint32_t tx_bytes);
bool     fl_com_send_status(void);

#endif /* FL_COM_H */