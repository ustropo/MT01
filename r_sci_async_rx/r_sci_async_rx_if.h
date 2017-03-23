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
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_sci_async_rx_if.h
* Description  : Functions for using SCI on RX devices. 
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           08.05.2013 2.00    Initial multi-channel release.
*           20.05.2013 2.10    Added e_sci_cmd commands SCI_CMD_TX_Q_BYTES_FREE and SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ.
*           11.06.2013 2.11    Change in r_sci_async_rx.c
*           12.06.2013 2.12    Change in r_sci_async_rx.c
***********************************************************************************************************************/

#ifndef SCI_ASYNC_IF_H
#define SCI_ASYNC_IF_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
/* Used for configuring the SCI code */
#include "r_sci_async_rx_config.h"


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define SCI_ASYNC_VERSION_MAJOR  (2)
#define SCI_ASYNC_VERSION_MINOR  (12)

//#define NULL    0

/*****************************************************************************
Typedef definitions
******************************************************************************/
typedef enum e_sci_ch       // SCI channel numbers
{
    SCI_CH0=0,
    SCI_CH1,
    SCI_CH2,
    SCI_CH3,
    SCI_CH4,
    SCI_CH5,
    SCI_CH6,
    SCI_CH7,
    SCI_CH8,
    SCI_CH9,
    SCI_CH10,
    SCI_CH11,
    SCI_CH12,
    SCI_NUM_CH
} sci_ch_t;


typedef enum e_sci_mode     // SCI operational modes
{
    SCI_MODE_OFF=0,         // channel not in use
    SCI_MODE_ASYNC,         // Asyncronous
    SCI_MODE_SYNC,          // Synchronous
    SCI_MODE_SSPI,          // Simple SPI
    SCI_MODE_SI2C,          // Simple I2C
    SCI_MODE_SMART          // Smart card interface
} sci_mode_t;


typedef enum e_sci_err      // SCI API error codes
{
    SCI_SUCCESS=0,
    SCI_ERR_BAD_CHAN,       // non-existent channel number
    SCI_ERR_OMITTED_CHAN,   // SCI_CHx_INCLUDED is 0 in config.h
    SCI_ERR_CH_NOT_CLOSED,  // chan still running in another mode
    SCI_ERR_BAD_MODE,       // unsupported or incorrect mode for channel
    SCI_ERR_INVALID_ARG,    // argument is not one of the predefined values
    SCI_ERR_QUEUE_UNAVAILABLE,  // can't open tx or rx queue or both
    SCI_ERR_INSUFFICIENT_SPACE, // not enough space in tx queue
    SCI_ERR_INSUFFICIENT_DATA,  // not enough data in receive queue
    SCI_ERR_NULL_PTR        // received null ptr; missing required argument
} sci_err_t;


/* CHANNEL CONTROL BLOCK HANDLE */

typedef struct st_sci_ch_ctrl * sci_hdl_t;


/* SCI_OPEN() ASYNC ARGUMENT DEFINITIONS (do NOT change values) */

#define SCI_CLK_INT         (0x00U) // use internal clock for baud generation
#define SCI_CLK_EXT8X       (0x03U) // use external clock 8x baud rate
#define SCI_CLK_EXT16X      (0x02U) // use external clock 16x baud rate
#define SCI_DATA_7BIT       (0x40U)
#define SCI_DATA_8BIT       (0x00U)
#define SCI_PARITY_ON       (0x20U)
#define SCI_PARITY_OFF      (0x00U)
#define SCI_ODD_PARITY      (0x10U)
#define SCI_EVEN_PARITY     (0x00U)
#define SCI_STOPBITS_2      (0x08U)
#define SCI_STOPBITS_1      (0x00U)


typedef struct st_sci_uart
{
    uint32_t    baud_rate;      // ie 9600, 19200, 115200
    uint8_t     clk_src;        // use SCI_CLK_INT/EXT8X/EXT16X
    uint8_t     data_size;      // use SCI_DATA_nBIT
    uint8_t     parity_en;      // use SCI_PARITY_ON/OFF
    uint8_t     parity_type;    // use SCI_ODD/EVEN_PARITY
    uint8_t     stop_bits;      // use SCI_STOPBITS_1/2
    uint8_t     int_priority;   // txi, tei, rxi INT priority; 1=low, 15=high
} sci_uart_t;


/* CALLBACK FUNCTION ARGUMENT DEFINITIONS */

typedef enum e_sci_cb_evt   // callback function events
{
    SCI_EVT_TEI,            // TEI interrupt occurred; transmitter is idle
    SCI_EVT_RX_CHAR,        // received a character; already placed in queue
    SCI_EVT_RXBUF_OVFL,     // rx queue is full; can't save anymore data
    SCI_EVT_OVFL_ERR,       // receiver hardware overrun error
    SCI_EVT_FRAMING_ERR,    // receiver hardware framing error
    SCI_EVT_PARITY_ERR      // receiver hardware parity error
} sci_cb_evt_t;

typedef struct st_sci_cb_args // callback arguments
{
    sci_hdl_t       hdl;   
    sci_cb_evt_t    event;  
    uint8_t         byte;   // byte read when error occurred (unused for TEI)
} sci_cb_args_t;


/* SCI_CONTROL() ARGUMENT DEFINITIONS */

// commands
typedef enum e_sci_cmd
{
    SCI_CMD_EN_NOISE_CANCEL,    // enable noise cancellation
    SCI_CMD_EN_CTS_IN,          // enable CTS input (default RTS output)
    SCI_CMD_EN_TEI,             // enable TEI interrupts
    SCI_CMD_OUTPUT_BAUD_CLK,    // output baud clock on the SCK pin
    SCI_CMD_START_BIT_EDGE,     // detect start bit as falling edge of RXDn pin
                                // (default detect as low level on RXDn pin)
    SCI_CMD_GENERATE_BREAK,     // generate break condition
    SCI_CMD_TX_Q_FLUSH,         // flush transmit queue
    SCI_CMD_RX_Q_FLUSH,         // flush receive queue
    // the following use *p_args
    SCI_CMD_TX_Q_BYTES_FREE,    // get count of unused transmit queue bytes
    SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ, // get num bytes ready for reading
    SCI_CMD_CHANGE_BAUD         // change baud rate
} sci_cmd_t;

// SCI_CMD_CHANGE_BAUD takes a pointer to this structure for *p_args
typedef struct st_sci_baud
{
    uint32_t    pclk;       // peripheral clock speed; e.g. 24000000 is 24MHz
    uint32_t    rate;       // e.g. 9600, 19200, 115200
} sci_baud_t;

// SCI_CMD_TX_Q_BYTES_FREE and SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ take a pointer
// to a uint16_t for p_args


/*****************************************************************************
Public Functions
******************************************************************************/
sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     void * const       p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl);

sci_err_t R_SCI_Send(sci_hdl_t const    hdl,
                     uint8_t            *p_src,
                     uint16_t const     length);
                    
sci_err_t R_SCI_Receive(sci_hdl_t const hdl,
                        uint8_t         *p_dst,
                        uint16_t const  length);

sci_err_t R_SCI_Control(sci_hdl_t const     hdl,
                        sci_cmd_t const     cmd,
                        void                *p_args);

sci_err_t R_SCI_Close(sci_hdl_t const hdl);

uint32_t  R_SCI_GetVersion(void);

                                    
#endif /* SCI_ASYNC_IF_H */

