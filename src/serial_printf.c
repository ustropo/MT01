/******************************************************************************
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
*******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************
* File Name	   : serial_printf.c
* Version	   : 1.00
* Device 	   : RX600 Series
* Tool-Chain   : RX Family C Compiler
* OS           : None
* H/W Platform : RSKRX62N & RSKRX610
* Description  : Implements charget & charput for printf and scanf
*******************************************************************************
* History : MM.DD.YYYY Version Description
*         : 10.05.2010 1.00    First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*****************************************************************************/
#include "platform.h"
#include "r_sci_rx_if.h"
#include "serial_printf.h"

void SerialCallback(void *p_args);
/******************************************************************************
Global variables and functions
******************************************************************************/

void serial_init(void)
{
	sci_cfg_t config;
	sci_hdl_t Console;
	sci_err_t err;
	config.async.baud_rate = 115200;
	config.async.clk_src = SCI_CLK_INT;
	config.async.data_size = SCI_DATA_8BIT;
	config.async.parity_en = SCI_PARITY_OFF;
	config.async.parity_type = SCI_EVEN_PARITY; // ignored because parity is disabled
	config.async.stop_bits = SCI_STOPBITS_1;
	config.async.int_priority = 2; // 1=lowest, 15=highest
	err = R_SCI_Open(SCI_CH2, SCI_MODE_ASYNC, &config, SerialCallback, &Console);

}

/******************************************************************************
* Function Name: charput
* Description  : Outputs a character on a serial port
* Arguments    : character to output
* Return Value : none
******************************************************************************/
void charput(uint8_t output_char)
{
	static sci_hdl_t Console;
	sci_err_t err;
	err = R_SCI_Send(Console, &output_char, 1);
}
/******************************************************************************
End of function  charput
******************************************************************************/

/******************************************************************************
* Function Name: charget
* Description  : Gets a character on a serial port
* Arguments    : character to output
* Return Value : none
******************************************************************************/
uint8_t charget(void)
{
	uint8_t temp = 0;
//
//	/* Read any 'junk' out of receive buffer */
//	temp = SCI2.RDR;
//
//	/* Clear flag to receive next byte */
//    IR(SCI2, RXI2) = 0;
//
//	/* Wait for next receive data */
//	while(IR(SCI2, RXI2) == 0);
//
//	/* Read data */
//	temp = SCI2.RDR;
//
//	/* Echo data back out */
//	if(temp == 0x0d)
//	{
//		/* Enter was pressed, output newline */
//		charput('\r');
//		charput('\n');
//	}
//	else
//	{
//		/* Echo back character */
//		charput(temp);
//	}
//
//	/* Receive data acquired, send back up */
	return temp;		
}
/******************************************************************************
End of function  charget
******************************************************************************/

void SerialCallback(void *p_args)
{
	sci_cb_args_t *args;
	args = (sci_cb_args_t *)p_args;
	if (args->event == SCI_EVT_RX_CHAR)
	{
		// from RXI interrupt; character placed in queue is in args->byte
		nop();
	}
#if SCI_CFG_TEI_INCLUDED
	else if (args->event == SCI_EVT_TEI)
	{
		// from TEI interrupt; transmitter is idle
		// possibly disable external transceiver here
		nop();
	}
#endif
	else if (args->event == SCI_EVT_RXBUF_OVFL)
	{
		// from RXI interrupt; receive queue is full
		// unsaved char is in args->byte
		// will need to increase buffer size or reduce baud rate
		nop();
	}
	else if (args->event == SCI_EVT_OVFL_ERR)
	{
		// from ERI/Group12 interrupt; receiver overflow error occurred
		// error char is in args->byte
		// error condition is cleared in ERI routine
		nop();
	}
	else if (args->event == SCI_EVT_FRAMING_ERR)
	{
		// from ERI/Group12 interrupt; receiver framing error occurred
		// error char is in args->byte; if = 0, received BREAK condition
		// error condition is cleared in ERI routine
		nop();
	}
	else if (args->event == SCI_EVT_PARITY_ERR)
	{
		// from ERI/Group12 interrupt; receiver parity error occurred
		// error char is in args->byte
		// error condition is cleared in ERI routine
		nop();
	}
}
