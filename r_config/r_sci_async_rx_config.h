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
* File Name     : r_sci_async_rx_config.h
* Description   : Configures the SCI drivers
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
***********************************************************************************************************************/
#ifndef SCI_ASYNC_CONFIG_H
#define SCI_ASYNC_CONFIG_H

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/

/* SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING */
// Setting to BSP_CFG_PARAM_CHECKING_ENABLE utilizes the system default setting
// Setting to 1 includes parameter checking; 0 compiles out parameter checking
#define SCI_CFG_PARAM_CHECKING_ENABLE   1


/* SPECIFY CHANNELS TO INCLUDE SOFTWARE SUPPORT FOR 1=included, 0=not */
// * = port connector YRDKRX63N, RSKRX111, RSKRX210
                                        // mcu supported channels
#define SCI_CFG_CH0_INCLUDED    0       // RX63N         RX210*
#define SCI_CFG_CH1_INCLUDED    0       // RX63N  RX111* RX210
#define SCI_CFG_CH2_INCLUDED    0       // RX63N*
#define SCI_CFG_CH3_INCLUDED    0       // RX63N
#define SCI_CFG_CH4_INCLUDED    0       // RX63N
#define SCI_CFG_CH5_INCLUDED    0       // RX63N  RX111  RX210
#define SCI_CFG_CH6_INCLUDED    0       // RX63N         RX210
#define SCI_CFG_CH7_INCLUDED    0       // RX63N
#define SCI_CFG_CH8_INCLUDED    0       // RX63N         RX210
#define SCI_CFG_CH9_INCLUDED    1       // RX63N         RX210
#define SCI_CFG_CH10_INCLUDED   0       // RX63N
#define SCI_CFG_CH11_INCLUDED   0       // RX63N
#define SCI_CFG_CH12_INCLUDED   0       // RX63N  RX111  RX210

/* SPECIFY TX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_TX_BUFSIZ   80
#define SCI_CFG_CH1_TX_BUFSIZ   80
#define SCI_CFG_CH2_TX_BUFSIZ   80
#define SCI_CFG_CH3_TX_BUFSIZ   80
#define SCI_CFG_CH4_TX_BUFSIZ   80
#define SCI_CFG_CH5_TX_BUFSIZ   80
#define SCI_CFG_CH6_TX_BUFSIZ   80
#define SCI_CFG_CH7_TX_BUFSIZ   80
#define SCI_CFG_CH8_TX_BUFSIZ   80
#define SCI_CFG_CH9_TX_BUFSIZ   80
#define SCI_CFG_CH10_TX_BUFSIZ  80
#define SCI_CFG_CH11_TX_BUFSIZ  80
#define SCI_CFG_CH12_TX_BUFSIZ  80

/* SPECIFY RX QUEUE BUFFER SIZES (will not allocate if chan not enabled */
#define SCI_CFG_CH0_RX_BUFSIZ   80
#define SCI_CFG_CH1_RX_BUFSIZ   80
#define SCI_CFG_CH2_RX_BUFSIZ   80
#define SCI_CFG_CH3_RX_BUFSIZ   80
#define SCI_CFG_CH4_RX_BUFSIZ   80
#define SCI_CFG_CH5_RX_BUFSIZ   80
#define SCI_CFG_CH6_RX_BUFSIZ   80
#define SCI_CFG_CH7_RX_BUFSIZ   80
#define SCI_CFG_CH8_RX_BUFSIZ   80
#define SCI_CFG_CH9_RX_BUFSIZ   80
#define SCI_CFG_CH10_RX_BUFSIZ  80
#define SCI_CFG_CH11_RX_BUFSIZ  80
#define SCI_CFG_CH12_RX_BUFSIZ  80

/* 
* ENABLE TRANSMIT END INTERRUPT 
* This interrupt only occurs when the last bit of the last byte of data 
* has been sent and the transmitter has become idle. The interrupt calls
* the user's callback function specified in R_SCI_Open() and passes it an
* SCI_EVT_TEI event. A typical use of this feature is to disable an external
* transceiver to save power. It would then be up to the user's code to 
* re-enable the transceiver before sending again. Not including this feature
* reduces code space used by the interrupt.
*/
#define SCI_CFG_TEI_INCLUDED    0       // 1=included, 0=not

/* 
* SET GROUP12 (RECEIVER ERROR) INTERRUPT PRIORITY; RX63N ONLY
* This #define sets the priority level for the interrupt that handles 
* receiver overrun, framing, and parity errors for all SCI channels
* on the RX63N. It is ignored for all other parts.
*/
#define SCI_CFG_RXERR_PRIORITY  3       // 1 lowest, 15 highest

#endif /* SCI_ASYNC_CONFIG_H */
