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
/**********************************************************************************************************************
* File Name    : r_sci_async_rx.c
* Device(s)    : RX63N, RX631, RX210, RX111
* Tool-Chain   : Renesas RX Standard Toolchain 1.02
* OS           : None
* H/W Platform : YRDKRX63N
* Description  : Functions for using SCI on RX devices. 
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*           08.05.2013 2.00    Initial multi-channel release.
*           20.05.2013 2.10    Added e_sci_cmd commands SCI_CMD_TX_Q_BYTES_FREE and SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ.
*           11.06.2013 2.11    Optimized code: Merged sci_open_uart() into R_SCI_Open() & broke out sci_init_queues().
*           12.06.2013 2.12    Modified sci_init_bit_rate() for future multi-protocol tables.
***********************************************************************************************************************/
/* PRQA S 3116, 2889, 3219 ++ */
/* GRI 1/28/2013
 * 3116 "inline" pragma not recognized
 * 2889 more than one return statement: ok for parameter checking
 * 3219 isr static functions are not called within file; leave as static
 */
/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stddef.h>
/* Fixed width integers */
#include <stdint.h>
/* Boolean defines */
#include <stdbool.h>
/* Used for xchg() intrinsic */
#include <machine.h>
/* Access to peripherals and board defines. */
#include "platform.h"
#include "r_byteq_if.h"
/* Defines for SCI support */
#include "r_sci_async_rx_private.h"
#include "r_sci_async_rx_if.h"
#include "r_sci_async_rx_config.h"

/*****************************************************************************
Typedef definitions
******************************************************************************/

/*****************************************************************************
Macro definitions
******************************************************************************/

/* Mask of all active channels */
#define SCI_CFG_CH_INCLUDED_MASK ((SCI_CFG_CH0_INCLUDED << 0)   |   \
                                  (SCI_CFG_CH1_INCLUDED << 1)   |   \
                                  (SCI_CFG_CH2_INCLUDED << 2)   |   \
                                  (SCI_CFG_CH3_INCLUDED << 3)   |   \
                                  (SCI_CFG_CH4_INCLUDED << 4)   |   \
                                  (SCI_CFG_CH5_INCLUDED << 5)   |   \
                                  (SCI_CFG_CH6_INCLUDED << 6)   |   \
                                  (SCI_CFG_CH7_INCLUDED << 7)   |   \
                                  (SCI_CFG_CH8_INCLUDED << 8)   |   \
                                  (SCI_CFG_CH9_INCLUDED << 9)   |   \
                                  (SCI_CFG_CH10_INCLUDED << 10) |   \
                                  (SCI_CFG_CH11_INCLUDED << 11) |   \
                                  (SCI_CFG_CH12_INCLUDED << 12))

/* SCI SCR register masks */
#define SCI_SCR_RE_MASK     (0x10U)     // receiver enable
#define SCI_SCR_TE_MASK     (0x20U)     // tranmitter enable
#define SCI_EN_XCVR_MASK    (SCI_SCR_RE_MASK | SCI_SCR_TE_MASK)

/* SCI SSR register receiver error masks */
#define SCI_SSR_ORER_MASK   (0x20U)     // overflow error
#define SCI_SSR_FER_MASK    (0x10U)     // framing error
#define SCI_SSR_PER_MASK    (0x08U)     // parity err
#define SCI_RCVR_ERR_MASK   (SCI_SSR_ORER_MASK | SCI_SSR_FER_MASK | SCI_SSR_PER_MASK)

/* Macros to enable and disable ICU interrupts */
#define ENABLE_ERI_INT      (*hdl->rom->icu_eri |= hdl->rom->eri_en_mask)
#define DISABLE_ERI_INT     (*hdl->rom->icu_eri &= (uint8_t)~hdl->rom->eri_en_mask)
#define ENABLE_RXI_INT      (*hdl->rom->icu_rxi |= hdl->rom->rxi_en_mask)
#define DISABLE_RXI_INT     (*hdl->rom->icu_rxi &= (uint8_t)~hdl->rom->rxi_en_mask)
#define ENABLE_TXI_INT      (*hdl->rom->icu_txi |= hdl->rom->txi_en_mask)
#define DISABLE_TXI_INT     (*hdl->rom->icu_txi &= (uint8_t)~hdl->rom->txi_en_mask)
#define ENABLE_TEI_INT      (*hdl->rom->icu_tei |= hdl->rom->tei_en_mask)
#define DISABLE_TEI_INT     (*hdl->rom->icu_tei &= (uint8_t)~hdl->rom->tei_en_mask)

/*****************************************************************************
Private global variables and functions
******************************************************************************/
static void power_on(sci_hdl_t const hdl);
static void power_off(sci_hdl_t const hdl);

static sci_err_t sci_init_mode(sci_hdl_t const  hdl,
                               sci_mode_t const mode,
                               void * const     p_cfg,
                               uint8_t * const  p_priority);

static sci_err_t sci_init_uart(sci_hdl_t const      hdl,
                               sci_uart_t * const   p_cfg,
                               uint8_t * const      p_priority);

static int32_t sci_init_bit_rate(sci_hdl_t const    hdl,
                                 uint32_t const     pclk,
                                 uint32_t const     baud);

static sci_err_t sci_init_queues(uint8_t const  chan);

static void sci_enable_ints(sci_hdl_t const hdl,
                            uint8_t const   priority);

static sci_err_t sci_put_byte(sci_hdl_t const   hdl,
                              uint8_t const     byte);

static sci_err_t sci_get_byte(sci_hdl_t const   hdl,
                              uint8_t * const   p_byte);
                   
static void txi_handler(sci_hdl_t const hdl);
static void tei_handler(sci_hdl_t const hdl);
static void rxi_handler(sci_hdl_t const hdl);
#ifndef BSP_MCU_RX63_ALL
static void eri_handler(sci_hdl_t const hdl);
#endif


/*****************************************************************************
* Function Name: R_SCI_Open
* Description  : Initializes an SCI channel for a particular mode.
*
* NOTE: The associated port must be configured/initialized prior to
*       calling this function.
*
* Arguments    : chan -
*                    channel to initialize
*                mode -
*                    operational mode (UART, SPI, I2C, ...)
*                p_cfg -
*                    ptr to configuration structure specific to mode, 
*                    casted to void *
*                p_callback -
*                    ptr to function called from interrupt when a receiver 
*                    error is detected or for transmit end (TEI) condition
*                p_hdl -
*                     pointer to a handle for channel (value set here)
* Return Value : SCI_SUCCESS -
*                    channel opened successfully
*                SCI_ERR_BAD_CHAN -
*                    channel number invalid for part
*                SCI_ERR_OMITTED_CHAN -
*                    channel not included in config.h
*                SCI_ERR_CH_NOT_CLOSED -
*                    channel already in use
*                SCI_ERR_BAD_MODE -
*                    unsupported mode
*                SCI_ERR_NULL_PTR -
*                    missing required p_cfg argument
*                SCI_ERR_INVALID_ARG -
*                    element of casted mode config structure (p_cfg) is invalid
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    cannot open transmit or receive queue or both
******************************************************************************/
sci_err_t R_SCI_Open(uint8_t const      chan,
                     sci_mode_t const   mode,
                     void * const       p_cfg,
                     void               (* const p_callback)(void *p_args),
                     sci_hdl_t * const  p_hdl)
{
sci_err_t   err;
uint8_t     priority;


    /* CHECK ARGUMENTS */

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)

#if defined(BSP_MCU_RX63_ALL)
    if (chan >= SCI_NUM_CH)
    {
        return SCI_ERR_BAD_CHAN;
    }
#elif defined(BSP_MCU_RX11_ALL)
    if ((chan != SCI_CH1) && (chan != SCI_CH5) && (chan != SCI_CH12))
    {
        return SCI_ERR_BAD_CHAN;
    }
#elif defined(BSP_MCU_RX21_ALL)
    if ((chan != SCI_CH0) && (chan != SCI_CH1) 
     && (chan != SCI_CH5) && (chan != SCI_CH6) 
     && (chan != SCI_CH8) && (chan != SCI_CH9) && (chan != SCI_CH12))
    {
        return SCI_ERR_BAD_CHAN;
    }    
#endif
    if (g_handles[chan] == NULL)
    {
        return SCI_ERR_OMITTED_CHAN;
    }
    if (g_handles[chan]->mode != SCI_MODE_OFF)
    {
        return SCI_ERR_CH_NOT_CLOSED;
    }
    if (mode != SCI_MODE_ASYNC)
    {
        return SCI_ERR_BAD_MODE;
    }
    if ((p_cfg == NULL) || (p_hdl == NULL))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif
    
    
     /* APPLY POWER TO CHANNEL */

    power_on(g_handles[chan]);


    /* INITIALIZE MODE SPECIFIC FEATURES */

    err = sci_init_mode(g_handles[chan], mode, p_cfg, &priority);
    if (err != SCI_SUCCESS)
    {
        g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }
    g_handles[chan]->callback = p_callback;


    /* INITIALIZE TX AND RX QUEUES */

    err = sci_init_queues(chan);
    if (err != SCI_SUCCESS)
    {
        g_handles[chan]->mode = SCI_MODE_OFF;
        return err;
    }


    /* ENABLE INTERRUPTS */

    sci_enable_ints(g_handles[chan], priority);


    /* FINISH */

    *p_hdl = g_handles[chan];

    return SCI_SUCCESS;
}


/*****************************************************************************
* Function Name: power_on
* Description  : This function provides power to the channel referenced by
*                the handle by taking it out of the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_on(sci_hdl_t const hdl)
{

    SYSTEM.PRCR.WORD = 0xA502;      // unlock MSTP regs
    *hdl->rom->mstp &= ~hdl->rom->stop_mask;
    SYSTEM.PRCR.WORD = 0xA500;      // lock MSTP regs

    hdl->rom->regs->SCR.BYTE = 0;   // disable xcvr & interrupts (dflt)
    return;
}


/*****************************************************************************
* Function Name: power_off
* Description  : This function removes power to the channel referenced by
*                handle by putting it into the module stop state.
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
static void power_off(sci_hdl_t const hdl)
{

    SYSTEM.PRCR.WORD = 0xA502;      // unlock MSTP regs
    *hdl->rom->mstp |= hdl->rom->stop_mask;
    SYSTEM.PRCR.WORD = 0xA500;      // lock MSTP regs
    return;
}


/*****************************************************************************
* Function Name: sci_init_queues
* Description  : This function attaches transmit and receive queues to the
*                channel.
*
* Arguments    : chan -
*                    channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_QUEUE_UNAVAILABLE -
*                    no queue control blocks available
******************************************************************************/
static sci_err_t sci_init_queues(uint8_t const  chan)
{
byteq_err_t q_err1,q_err2;
sci_err_t   err=SCI_SUCCESS;


    // channel number verified as legal prior to calling this function

    switch (chan)
    {
#if SCI_CFG_CH0_INCLUDED
    case (SCI_CH0):
        q_err1 = R_BYTEQ_Open(ch0_tx_buf, SCI_CFG_CH0_TX_BUFSIZ, &g_handles[SCI_CH0]->tx_que);
        q_err2 = R_BYTEQ_Open(ch0_rx_buf, SCI_CFG_CH0_RX_BUFSIZ, &g_handles[SCI_CH0]->rx_que);
        break;
#endif

#if SCI_CFG_CH1_INCLUDED
    case (SCI_CH1):
        q_err1 = R_BYTEQ_Open(ch1_tx_buf, SCI_CFG_CH1_TX_BUFSIZ, &g_handles[SCI_CH1]->tx_que);
        q_err2 = R_BYTEQ_Open(ch1_rx_buf, SCI_CFG_CH1_RX_BUFSIZ, &g_handles[SCI_CH1]->rx_que);
        break;
#endif

#if SCI_CFG_CH2_INCLUDED
    case (SCI_CH2):
        q_err1 = R_BYTEQ_Open(ch2_tx_buf, SCI_CFG_CH2_TX_BUFSIZ, &g_handles[SCI_CH2]->tx_que);
        q_err2 = R_BYTEQ_Open(ch2_rx_buf, SCI_CFG_CH2_RX_BUFSIZ, &g_handles[SCI_CH2]->rx_que);
        break;
#endif

#if SCI_CFG_CH3_INCLUDED
    case (SCI_CH3):
        q_err1 = R_BYTEQ_Open(ch3_tx_buf, SCI_CFG_CH3_TX_BUFSIZ, &g_handles[SCI_CH3]->tx_que);
        q_err2 = R_BYTEQ_Open(ch3_rx_buf, SCI_CFG_CH3_RX_BUFSIZ, &g_handles[SCI_CH3]->rx_que);
        break;
#endif

#if SCI_CFG_CH4_INCLUDED
    case (SCI_CH4):
        q_err1 = R_BYTEQ_Open(ch4_tx_buf, SCI_CFG_CH4_TX_BUFSIZ, &g_handles[SCI_CH4]->tx_que);
        q_err2 = R_BYTEQ_Open(ch4_rx_buf, SCI_CFG_CH4_RX_BUFSIZ, &g_handles[SCI_CH4]->rx_que);
    break;
#endif

#if SCI_CFG_CH5_INCLUDED
    case (SCI_CH5):
        q_err1 = R_BYTEQ_Open(ch5_tx_buf, SCI_CFG_CH5_TX_BUFSIZ, &g_handles[SCI_CH5]->tx_que);
        q_err2 = R_BYTEQ_Open(ch5_rx_buf, SCI_CFG_CH5_RX_BUFSIZ, &g_handles[SCI_CH5]->rx_que);
        break;
#endif

#if SCI_CFG_CH6_INCLUDED
    case (SCI_CH6):
        q_err1 = R_BYTEQ_Open(ch6_tx_buf, SCI_CFG_CH6_TX_BUFSIZ, &g_handles[SCI_CH6]->tx_que);
        q_err2 = R_BYTEQ_Open(ch6_rx_buf, SCI_CFG_CH6_RX_BUFSIZ, &g_handles[SCI_CH6]->rx_que);
        break;
#endif

#if SCI_CFG_CH7_INCLUDED
    case (SCI_CH7):
        q_err1 = R_BYTEQ_Open(ch7_tx_buf, SCI_CFG_CH7_TX_BUFSIZ, &g_handles[SCI_CH7]->tx_que);
        q_err2 = R_BYTEQ_Open(ch7_rx_buf, SCI_CFG_CH7_RX_BUFSIZ, &g_handles[SCI_CH7]->rx_que);
        break;
#endif

#if SCI_CFG_CH8_INCLUDED
    case (SCI_CH8):
        q_err1 = R_BYTEQ_Open(ch8_tx_buf, SCI_CFG_CH8_TX_BUFSIZ, &g_handles[SCI_CH8]->tx_que);
        q_err2 = R_BYTEQ_Open(ch8_rx_buf, SCI_CFG_CH8_RX_BUFSIZ, &g_handles[SCI_CH8]->rx_que);
        break;
#endif

#if SCI_CFG_CH9_INCLUDED
    case (SCI_CH9):
        q_err1 = R_BYTEQ_Open(ch9_tx_buf, SCI_CFG_CH9_TX_BUFSIZ, &g_handles[SCI_CH9]->tx_que);
        q_err2 = R_BYTEQ_Open(ch9_rx_buf, SCI_CFG_CH9_RX_BUFSIZ, &g_handles[SCI_CH9]->rx_que);
        break;
#endif

#if SCI_CFG_CH10_INCLUDED
    case (SCI_CH10):
        q_err1 = R_BYTEQ_Open(ch10_tx_buf, SCI_CFG_CH10_TX_BUFSIZ, &g_handles[SCI_CH10]->tx_que);
        q_err2 = R_BYTEQ_Open(ch10_rx_buf, SCI_CFG_CH10_RX_BUFSIZ, &g_handles[SCI_CH10]->rx_que);
        break;
#endif

#if SCI_CFG_CH11_INCLUDED
case (SCI_CH11):
        q_err1 = R_BYTEQ_Open(ch11_tx_buf, SCI_CFG_CH11_TX_BUFSIZ, &g_handles[SCI_CH11]->tx_que);
        q_err2 = R_BYTEQ_Open(ch11_rx_buf, SCI_CFG_CH11_RX_BUFSIZ, &g_handles[SCI_CH11]->rx_que);
        break;
#endif

#if SCI_CFG_CH12_INCLUDED
    case (SCI_CH12):
        q_err1 = R_BYTEQ_Open(ch12_tx_buf, SCI_CFG_CH12_TX_BUFSIZ, &g_handles[SCI_CH12]->tx_que);
        q_err2 = R_BYTEQ_Open(ch12_rx_buf, SCI_CFG_CH12_RX_BUFSIZ, &g_handles[SCI_CH12]->rx_que);
        break;
#endif
    }

    if ((q_err1 != BYTEQ_SUCCESS) || (q_err2 != BYTEQ_SUCCESS))
    {
        err = SCI_ERR_QUEUE_UNAVAILABLE;
    }
    return err;
}


/*****************************************************************************
* Function Name: sci_init_mode
* Description  : This function calls the appropriate initialization function
*                based upon the mode.
*
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
*                mode -
*                    operational mode to initialize for (UART, SPI, I2C, ...)
*                p_cfg -
*                    ptr to mode configuration argument structure
*                p_priority -
*                    pointer to location to load interrupt priority into
* Return Value : SCI_SUCCESS -
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
static sci_err_t sci_init_mode(sci_hdl_t const  hdl,
                               sci_mode_t const mode,
                               void * const     p_cfg,
                               uint8_t * const  p_priority)
{
sci_err_t   err=SCI_ERR_BAD_MODE;


    // mode verified to be legal prior to calling this function

    hdl->mode = mode;
    //if (mode == SCI_MODE_ASYNC)
    {
        err = sci_init_uart(hdl, (sci_uart_t *)p_cfg, p_priority);
    }

    return err;
}


/*****************************************************************************
* Function Name: sci_init_uart
* Description  : This function initializes the control block and UART 
*                registers for an SCI channel.
*
* NOTE: p_cfg is checked to be non-NULL prior to this function.
*       The TE and RE bits in SCR must be 0 prior to calling this function.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_cfg -
*                    ptr to Uart configuration argument structure
*                p_priority -
*                    pointer to location to load interrupt priority into
* Return Value : SCI_SUCCESS - 
*                    channel initialized successfully
*                SCI_ERR_INVALID_ARG -
*                    element of p_cfg contains illegal value
******************************************************************************/
/* PRQA S 3227 ++ */
/* 3227- p_cfg could be const: eventually passed to sci_init_bit_rate() which
 *       is also called by R_SCI_Control(), only there a void* is cast and passed,
 *       and complications arise with const declarations there.
 */
static sci_err_t sci_init_uart(sci_hdl_t const      hdl,
                               sci_uart_t * const   p_cfg,
                               uint8_t * const      p_priority)
{
sci_err_t   err=SCI_SUCCESS;
int32_t     bit_err;


    /* Check arguments */    

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if (((p_cfg->data_size != SCI_DATA_8BIT) && (p_cfg->data_size != SCI_DATA_7BIT))
     || ((p_cfg->stop_bits != SCI_STOPBITS_1) && (p_cfg->stop_bits != SCI_STOPBITS_2))
     || ((p_cfg->int_priority < (BSP_MCU_IPL_MIN+1)) || (p_cfg->int_priority > BSP_MCU_IPL_MAX)))
    {
        return SCI_ERR_INVALID_ARG;
    }

    if (p_cfg->parity_en == SCI_PARITY_ON)
    {
        if ((p_cfg->parity_type != SCI_EVEN_PARITY) && (p_cfg->parity_type != SCI_ODD_PARITY))
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
    else if (p_cfg->parity_en != SCI_PARITY_OFF)
    {
        return SCI_ERR_INVALID_ARG;
    }
    else
    {
    }
    if (p_cfg->clk_src == SCI_CLK_INT)
    {
        if (p_cfg->baud_rate == 0)
        {
            return SCI_ERR_INVALID_ARG;
        }
    }
    else if ((p_cfg->clk_src != SCI_CLK_EXT8X) && (p_cfg->clk_src != SCI_CLK_EXT16X))
    {
        return SCI_ERR_INVALID_ARG;
    }
    else
    {
    }
#endif


    /* Initialize channel control block flags */

    hdl->tx_idle = true;
        
    /* Configure SMR for asynchronous mode, single processor, and user settings */
    if (p_cfg->parity_en == SCI_PARITY_OFF)
    {
    	p_cfg->parity_type = 0;         // ensure random value is not ORed into SMR
    }
    hdl->rom->regs->SMR.BYTE = p_cfg->data_size | p_cfg->stop_bits
                                        | p_cfg->parity_en | p_cfg->parity_type;
    
    /* SETUP CLOCK FOR BAUD RATE */

    if (p_cfg->clk_src == SCI_CLK_INT)
    {
        /* Use internal clock for baud rate */
        //TODO: Include peripheral clock speed in p_cfg? Only need if do Open() after
        // clock switch (BSP_PCLKB_HZ then not applicable)
        bit_err = sci_init_bit_rate(hdl, BSP_PCLKB_HZ, p_cfg->baud_rate);
        if (bit_err == 1000)
        {
            err = SCI_ERR_INVALID_ARG;          // impossible baud rate; 100% error
        }
        else
        {
            hdl->baud_rate = p_cfg->baud_rate;   // save baud rate for break generation
        }
    }
    else // Use external clock for baud rate
    {
        hdl->rom->regs->SCR.BIT.CKE = 0x02;
        if (p_cfg->clk_src == SCI_CLK_EXT8X)
        {
            hdl->rom->regs->SEMR.BIT.ABCS = 1;
        }
    }

    *p_priority = p_cfg->int_priority;
    return err;
}
/* PRQA S 3227 -- */

/*****************************************************************************
* Function Name: sci_init_bit_rate
* Description  : This function determines the best possible settings for the
*                baud rate registers for the specified peripheral clock speed
*                and baud rate. Note that this does not guarantee a low bit 
*                error rate, just the best possible one. The bit rate error is
*                returned in .1% increments. If the hardware cannot support
*                the specified combination, a value of 1000 (100% error) is
*                returned.
*
* NOTE: The transmitter and receiver (TE and RE bits in SCR) must be disabled 
*       prior to calling this function.
*
*       The application must pause for 1 bit time after the BRR register
*       is loaded before transmitting/receiving to allow time for the clock
*       to settle. 
*
* Arguments    : hdl -
*                    Handle for channel (ptr to chan control block)
*                    NOTE: mode element must be already set
*                pclk -
*                    Peripheral clock speed; e.g. 24000000 for 24MHz
*                baud -
*                    Baud rate; 19200, 57600, 115200, etc.
* Return Value : bit error in .1% increments; e.g. 16 = 1.6% bit rate error
*                a value of 1000 denotes 100% error; no registers set
******************************************************************************/
static int32_t sci_init_bit_rate(sci_hdl_t const  hdl,
                                 uint32_t const   pclk,
                                 uint32_t const   baud)
{
uint32_t i,num_divisors;
uint32_t ratio;
uint32_t tmp,pclk_calc;
int32_t  bit_err;
baud_divisor_t  *p_baud_info;


#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((pclk == 0) || (baud == 0))
    {
        return 1000;
    }
#endif

    /* SELECT PROPER TABLE BASED UPON MODE */

    //if (hdl->mode == SCI_MODE_ASYNC)
    {
        p_baud_info = async_baud;
        num_divisors = NUM_DIVISORS_ASYNC;
    }


    /* FIND DIVISOR; table has associated ABCS and CKS values */
    // BRR must be 255 or less
    // the "- 1" is ignored in some steps for approximations
    // BRR = (PCLK/(divisor * baud)) - 1
    // BRR = (ratio / divisor) - 1

    ratio = pclk/baud;
    for(i=0; i < num_divisors; i++)
    {
        if (ratio < (uint32_t)(p_baud_info[i].divisor * 256))
        {
            break;  /* PRQA S 0769 */
        }           /* avoid using multiple breaks in iteration */
    }
    if (i == num_divisors)
    {
        return(1000);
    }
    

    /* ROUND UP/DOWN BRR AND SET BAUD RATE RELATED REGISTERS */

    /* divide ratio by only half the divisor and see if odd number */
    tmp = ratio/((uint32_t)p_baud_info[i].divisor/2);
    /* if odd, "round up" by ignoring -1; divide by 2 again for rest of divisor */
    hdl->rom->regs->BRR = (tmp & 0x01) ? (tmp/2) : ((tmp/2)-1); /* PRQA S 3790 */
                                                                /* long to byte ok */
    hdl->rom->regs->SEMR.BIT.ABCS = p_baud_info[i].abcs;
    hdl->rom->regs->SMR.BIT.CKS = p_baud_info[i].cks;


    /* CALCULATE AND RETURN BIT RATE ERROR */

    // back calculate what PCLK should be:
    // BRR = (PCLK/(divisor * baud)) - 1
    // (BRR+1) * divisor * baud = PCLK
    pclk_calc = (uint32_t)(hdl->rom->regs->BRR +1) * (uint32_t)p_baud_info[i].divisor * baud;

    /* error represented in 0.1% increments */
    bit_err = (int32_t)(((pclk - pclk_calc) * 1000L) / pclk_calc);  /* PRQA S 2834 */
    return bit_err;                                      /* pclk_calc will not = 0 */
}


/*****************************************************************************
* Function Name: sci_enable_ints
* Description  : This function sets priority, clears flags, and enables 
*                interrupts in both the ICU and SCI peripheral. These include 
*                RXI, TXI, TEI, and ERI/GROUP12.
*                interrupts.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                priority -
*                    priority for interrupts
* Return Value : none
******************************************************************************/
static void sci_enable_ints(sci_hdl_t const hdl,
                            uint8_t const   priority)
{
    
    /* SET PRIORITY FOR RXI, TXI, and TEI INTERRUPTS */
    *hdl->rom->ipr = priority;              // includes ERI non-63x
#ifdef BSP_MCU_RX63_ALL
    IPR(ICU,GROUP12) = SCI_CFG_RXERR_PRIORITY;  // RX63x ERI equivalent
#endif

    /* CLEAR INTERRUPT FLAGS */
    *hdl->rom->ir_rxi &= ~0x01U;
    *hdl->rom->ir_txi &= ~0x01U;
#if SCI_CFG_TEI_INCLUDED
    *hdl->rom->ir_tei &= ~0x01U;
#endif
    /* Do not clear group12 flag on RX63x. May be set for another channel */
#ifndef BSP_MCU_RX63_ALL
    *hdl->rom->ir_eri &= ~0x01U;
#endif
    
    /* ENABLE INTERRUPTS IN ICU */

    ENABLE_RXI_INT;
    ENABLE_TXI_INT;
    // ENABLE_TEI_INT occurs in sci_put_byte()
#ifdef BSP_MCU_RX63_ALL
    IEN(ICU,GROUP12) = 1;               // enable RX63x rx err interrupts
#else
    ENABLE_ERI_INT;
#endif

    /* ENABLE INTERRUPTS IN SCI PERIPHERAL */

    /* Note: Enable interrupts after xcvr or will get "extra" interrupt */
    hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;    // TE | RE
#ifdef BSP_MCU_RX63_ALL
    ICU.GEN[12].BIT.EN2 = 1;            // enable RX err GROUP12 for 63x
#endif
    hdl->rom->regs->SCR.BYTE |= 0xC0U;  // enable RXI, [ERI], and TXI
                                        // TEI is enabled via R_SCI_Control()
    return;    
}


/*****************************************************************************
* Function Name: R_SCI_Send
* Description  : Puts data in tx queue for sending on an SCI channel. 
*                Transmit starts immediately if another transmission is 
*                not already in progress.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_src -
*                    ptr to data to transmit
*                length - 
*                    number of bytes to send
* Return Value : SCI_SUCCESS -
*                    requested number of bytes sent/loaded into tx queue
*                SCI_ERR_NULL_PTR -
*                    hdl or p_src is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_SPACE - 
*                    not enough space in tx queue to store data
******************************************************************************/
/* PRQA S 2814 -- */
sci_err_t R_SCI_Send(sci_hdl_t const    hdl,
                     uint8_t            *p_src,
                     uint16_t const     length)
{
uint16_t    cnt;
sci_err_t   err;


    /* Check arguments */

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((hdl == NULL) || (p_src == NULL))
    {
        return SCI_ERR_NULL_PTR;
    }
    if (hdl->mode != SCI_MODE_ASYNC)
    {
        return SCI_ERR_BAD_MODE;
    }
#endif


    /* Determine amount of space left in tx queue */

    DISABLE_TXI_INT;
    R_BYTEQ_Unused(hdl->tx_que, &cnt);
    ENABLE_TXI_INT;

    if (cnt < length)
    {
        /* If can't fit, return */
        err = SCI_ERR_INSUFFICIENT_SPACE;
    }
    else
    {
        /* Else load bytes into tx queue for transmission */    
        for (cnt=0; cnt < length; cnt++)
        {
            sci_put_byte(hdl, *p_src++);
        }
        err = SCI_SUCCESS;
    }
    return err;
}


/*****************************************************************************
* Function Name: sci_put_byte
* Description  : Transmits byte if channel is not busy. Otherwise, byte is
*                stored in tx queue until can transmit. If buffer is full
*                and cannot store it, an error code is returned.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                byte - 
*                    byte to transmit
* Return Value : SCI_SUCCESS - 
*                    byte is sent or loaded into ring buffer
*                SCI_ERR_INSUFFICIENT_SPACE -
*                    tx queue is full; cannot queue data                
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013 hdl checked for null in calling faunction */
static sci_err_t sci_put_byte(sci_hdl_t const   hdl,
                              uint8_t const     byte)
{
uint16_t    cnt;
    
    /* check if tx queue full */
    DISABLE_TXI_INT;
    R_BYTEQ_Unused(hdl->tx_que, &cnt);
    if (cnt == 0)
    {
        ENABLE_TXI_INT;
        return SCI_ERR_INSUFFICIENT_SPACE;
    }
 

    if (hdl->tx_idle == true)
    {
        ENABLE_TXI_INT;

        /* if xmtr is idle, kick off transmit */
        hdl->tx_idle = false;
        hdl->rom->regs->TDR = byte;
#if SCI_CFG_TEI_INCLUDED
        /* Enable transmit end interrupt */
        ENABLE_TEI_INT;
#endif
    }
    else
    {
        /* else load next byte into tx queue */
        R_BYTEQ_Put(hdl->tx_que, byte);
        ENABLE_TXI_INT;
    }
    
    return SCI_SUCCESS;
}
/* PRQA S 2814 -- */

/*****************************************************************************
* Function Name: sciN_txiN_isr
* Description  : TXI interrupt routines for every SCI channel
* Arguments    : none
* Return Value : none
******************************************************************************/
#if SCI_CFG_CH0_INCLUDED
#pragma interrupt sci0_txi0_isr(vect=VECT(SCI0,TXI0))
static void sci0_txi0_isr(void)
{
    txi_handler(&ch0_ctrl);
}
#endif

#if SCI_CFG_CH1_INCLUDED
#pragma interrupt sci1_txi1_isr(vect=VECT(SCI1,TXI1))
static void sci1_txi1_isr(void)
{
    txi_handler(&ch1_ctrl);
}
#endif

#if SCI_CFG_CH2_INCLUDED
#pragma interrupt sci2_txi2_isr(vect=VECT(SCI2,TXI2))
static void sci2_txi2_isr(void)
{
    txi_handler(&ch2_ctrl);
}
#endif

#if SCI_CFG_CH3_INCLUDED
#pragma interrupt sci3_txi3_isr(vect=VECT(SCI3,TXI3))
static void sci3_txi3_isr(void)
{
    txi_handler(&ch3_ctrl);
}
#endif

#if SCI_CFG_CH4_INCLUDED
#pragma interrupt sci4_txi4_isr(vect=VECT(SCI4,TXI4))
static void sci4_txi4_isr(void)
{
    txi_handler(&ch4_ctrl);
}
#endif

#if SCI_CFG_CH5_INCLUDED
#pragma interrupt sci5_txi5_isr(vect=VECT(SCI5,TXI5))
static void sci5_txi5_isr(void)
{
    txi_handler(&ch5_ctrl);
}
#endif

#if SCI_CFG_CH6_INCLUDED
#pragma interrupt sci6_txi6_isr(vect=VECT(SCI6,TXI6))
static void sci6_txi6_isr(void)
{
    txi_handler(&ch6_ctrl);
}
#endif

#if SCI_CFG_CH7_INCLUDED
#pragma interrupt sci7_txi7_isr(vect=VECT(SCI7,TXI7))
static void sci7_txi7_isr(void)
{
    txi_handler(&ch7_ctrl);
}
#endif

#if SCI_CFG_CH8_INCLUDED
#pragma interrupt sci8_txi8_isr(vect=VECT(SCI8,TXI8))
static void sci8_txi8_isr(void)
{
    txi_handler(&ch8_ctrl);
}
#endif

#if SCI_CFG_CH9_INCLUDED
#pragma interrupt sci9_txi9_isr(vect=VECT(SCI9,TXI9))
static void sci9_txi9_isr(void)
{
    txi_handler(&ch9_ctrl);
}
#endif

#if SCI_CFG_CH10_INCLUDED
#pragma interrupt sci10_txi10_isr(vect=VECT(SCI10,TXI10))
static void sci10_txi10_isr(void)
{
    txi_handler(&ch10_ctrl);
}
#endif

#if SCI_CFG_CH11_INCLUDED
#pragma interrupt sci11_txi11_isr(vect=VECT(SCI11,TXI11))
static void sci11_txi11_isr(void)
{
    txi_handler(&ch11_ctrl);
}
#endif

#if SCI_CFG_CH12_INCLUDED
#pragma interrupt sci12_txi12_isr(vect=VECT(SCI12,TXI12))
static void sci12_txi12_isr(void)
{
    txi_handler(&ch12_ctrl);
}
#endif


/*****************************************************************************
* Function Name: txi_handler
* Description  : TXI interrupt handler for SCI UART mode
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013: hdl is hard-coded non-null value in calling interrupts */
static void txi_handler(sci_hdl_t const hdl)
{

    /* Get byte from que and place in TDR for transmit */
    if (R_BYTEQ_Get(hdl->tx_que, (uint8_t * const)&hdl->rom->regs->TDR) != BYTEQ_SUCCESS) /* PRQA S 0312 */
    {                                                                                     /* volatile to const */
        hdl->tx_idle = true;    // set flag if queue empty
    }
}
/* PRQA S 2814 -- */


#if SCI_CFG_TEI_INCLUDED
/*****************************************************************************
* Function Name: sciN_teiN_isr
* Description  : TEI interrupt routines for every SCI channel
* Arguments    : none
* Return Value : none
******************************************************************************/

#if SCI_CFG_CH0_INCLUDED
#pragma interrupt sci0_tei0_isr(vect=VECT(SCI0,TEI0))
static void sci0_tei0_isr(void)
{
    tei_handler(&ch0_ctrl);
}
#endif

#if SCI_CFG_CH1_INCLUDED
#pragma interrupt sci1_tei1_isr(vect=VECT(SCI1,TEI1))
static void sci1_tei1_isr(void)
{
    tei_handler(&ch1_ctrl);
}
#endif

#if SCI_CFG_CH2_INCLUDED
#pragma interrupt sci2_tei2_isr(vect=VECT(SCI2,TEI2))
static void sci2_tei2_isr(void)
{
    tei_handler(&ch2_ctrl);
}
#endif

#if SCI_CFG_CH3_INCLUDED
#pragma interrupt sci3_tei3_isr(vect=VECT(SCI3,TEI3))
static void sci3_tei3_isr(void)
{
    tei_handler(&ch3_ctrl);
}
#endif

#if SCI_CFG_CH4_INCLUDED
#pragma interrupt sci4_tei4_isr(vect=VECT(SCI4,TEI4))
static void sci4_tei4_isr(void)
{
    tei_handler(&ch4_ctrl);
}
#endif

#if SCI_CFG_CH5_INCLUDED
#pragma interrupt sci5_tei5_isr(vect=VECT(SCI5,TEI5))
static void sci5_tei5_isr(void)
{
    tei_handler(&ch5_ctrl);
}
#endif

#if SCI_CFG_CH6_INCLUDED
#pragma interrupt sci6_tei6_isr(vect=VECT(SCI6,TEI6))
static void sci6_tei6_isr(void)
{
    tei_handler(&ch6_ctrl);
}
#endif

#if SCI_CFG_CH7_INCLUDED
#pragma interrupt sci7_tei7_isr(vect=VECT(SCI7,TEI7))
static void sci7_tei7_isr(void)
{
    tei_handler(&ch7_ctrl);
}
#endif

#if SCI_CFG_CH8_INCLUDED
#pragma interrupt sci8_tei8_isr(vect=VECT(SCI8,TEI8))
static void sci8_tei8_isr(void)
{
    tei_handler(&ch8_ctrl);
}
#endif

#if SCI_CFG_CH9_INCLUDED
#pragma interrupt sci9_tei9_isr(vect=VECT(SCI9,TEI9))
static void sci9_tei9_isr(void)
{
    tei_handler(&ch9_ctrl);
}
#endif

#if SCI_CFG_CH10_INCLUDED
#pragma interrupt sci10_tei10_isr(vect=VECT(SCI10,TEI10))
static void sci10_tei10_isr(void)
{
    tei_handler(&ch10_ctrl);
}
#endif

#if SCI_CFG_CH11_INCLUDED
#pragma interrupt sci11_tei11_isr(vect=VECT(SCI11,TEI11))
static void sci11_tei11_isr(void)
{
    tei_handler(&ch11_ctrl);
}
#endif

#if SCI_CFG_CH12_INCLUDED
#pragma interrupt sci12_tei12_isr(vect=VECT(SCI12,TEI12))
static void sci12_tei12_isr(void)
{
    tei_handler(&ch12_ctrl);
}
#endif


/*****************************************************************************
* Function Name: tei_handler
* Description  : TEI interrupt handler for SCI UART mode
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013: hdl is hard-coded non-null value in calling interrupts */
static void tei_handler(sci_hdl_t const hdl)
{
sci_cb_args_t    args;


    DISABLE_TEI_INT;
    
    if (hdl->callback != NULL)
    {
        args.hdl = hdl;
        args.event = SCI_EVT_TEI;
        hdl->callback((void *)&args);
    }
}
/* PRQA S 2814 -- */
#endif //SCI_CFG_TEI_INCLUDED


/*****************************************************************************
* Function Name: R_SCI_Receive
* Description  : Gets data received on an SCI channel referenced by the handle 
*                from rx queue. Function does not block if the requested 
*                number of bytes is not available. If any errors occurred 
*                during reception by hardware, they are handled by the callback 
*                function specified in R_SCI_Open() and no corresponding error 
*                code is provided here.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_dst -
*                    ptr to buffer to load data into
*                length - 
*                    number of bytes to read
* Return Value : SCI_SUCCESS -
*                    requested number of byte loaded into p_dst
*                SCI_ERR_NULL_PTR -
*                    hdl or p_dst is NULL
*                SCI_ERR_BAD_MODE -
*                    channel mode not currently supported
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue does not contain requested amount of data
******************************************************************************/
sci_err_t R_SCI_Receive(sci_hdl_t const hdl,
                        uint8_t         *p_dst,
                        uint16_t const  length)
{
sci_err_t   err;
uint16_t    cnt;


    /* Check arguments */

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((hdl == NULL) || (p_dst == NULL))
    {
        return SCI_ERR_NULL_PTR;
    }
    if (hdl->mode != SCI_MODE_ASYNC)
    {
        return SCI_ERR_BAD_MODE;
    }
#endif

    DISABLE_RXI_INT;
    R_BYTEQ_Used(hdl->rx_que, &cnt);
    ENABLE_RXI_INT;

    if (cnt < length)
    {
        err = SCI_ERR_INSUFFICIENT_DATA;
    }
    else
    {
        /* Get bytes from rx queue */
        for (cnt=0; cnt < length; cnt++)
        {    
            sci_get_byte(hdl, p_dst++);
        }

        err = SCI_SUCCESS;
    }

    return err;
}


/*****************************************************************************
* Function Name: sci_get_byte
* Description  : Gets byte from rz queue if one is available.
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                p_byte -
*                    ptr to load byte into
* Return Value : SCI_SUCCESS -
*                    byte was loaded
*                SCI_ERR_INSUFFICIENT_DATA -
*                    rx queue is empty; no byte loaded
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013 hdl checked for null in calling function */
static sci_err_t sci_get_byte(sci_hdl_t const   hdl,
                              uint8_t * const   p_byte)
{
sci_err_t   err;    /* PRQA S 3204 */ /* can't make const though assigned once */
byteq_err_t q_err;
    

    DISABLE_RXI_INT;
    q_err = R_BYTEQ_Get(hdl->rx_que, p_byte);
    ENABLE_RXI_INT;
    
    err = (q_err == BYTEQ_SUCCESS) ? SCI_SUCCESS : SCI_ERR_INSUFFICIENT_DATA;
    return err;
}        
/* PRQA S 2814 -- */

/*****************************************************************************
* Function Name: sciN_rxiN_isr
* Description  : RXI interrupt routines for every SCI channel
* Arguments    : none
* Return Value : none
******************************************************************************/
#if SCI_CFG_CH0_INCLUDED
#pragma interrupt sci0_rxi0_isr(vect=VECT(SCI0,RXI0))
static void sci0_rxi0_isr(void)
{
    rxi_handler(&ch0_ctrl);
}
#endif

#if SCI_CFG_CH1_INCLUDED
#pragma interrupt sci1_rxi1_isr(vect=VECT(SCI1,RXI1))
static void sci1_rxi1_isr(void)
{
    rxi_handler(&ch1_ctrl);
}
#endif

#if SCI_CFG_CH2_INCLUDED
#pragma interrupt sci2_rxi2_isr(vect=VECT(SCI2,RXI2))
static void sci2_rxi2_isr(void)
{
    rxi_handler(&ch2_ctrl);
}
#endif

#if SCI_CFG_CH3_INCLUDED
#pragma interrupt sci3_rxi3_isr(vect=VECT(SCI3,RXI3))
static void sci3_rxi3_isr(void)
{
    rxi_handler(&ch3_ctrl);
}
#endif

#if SCI_CFG_CH4_INCLUDED
#pragma interrupt sci4_rxi4_isr(vect=VECT(SCI4,RXI4))
static void sci4_rxi4_isr(void)
{
    rxi_handler(&ch4_ctrl);
}
#endif

#if SCI_CFG_CH5_INCLUDED
#pragma interrupt sci5_rxi5_isr(vect=VECT(SCI5,RXI5))
static void sci5_rxi5_isr(void)
{
    rxi_handler(&ch5_ctrl);
}
#endif

#if SCI_CFG_CH6_INCLUDED
#pragma interrupt sci6_rxi6_isr(vect=VECT(SCI6,RXI6))
static void sci6_rxi6_isr(void)
{
    rxi_handler(&ch6_ctrl);
}
#endif

#if SCI_CFG_CH7_INCLUDED
#pragma interrupt sci7_rxi7_isr(vect=VECT(SCI7,RXI7))
static void sci7_rxi7_isr(void)
{
    rxi_handler(&ch7_ctrl);
}
#endif

#if SCI_CFG_CH8_INCLUDED
#pragma interrupt sci8_rxi8_isr(vect=VECT(SCI8,RXI8))
static void sci8_rxi8_isr(void)
{
    rxi_handler(&ch8_ctrl);
}
#endif

#if SCI_CFG_CH9_INCLUDED
#pragma interrupt sci9_rxi9_isr(vect=VECT(SCI9,RXI9))
static void sci9_rxi9_isr(void)
{
    rxi_handler(&ch9_ctrl);
}
#endif

#if SCI_CFG_CH10_INCLUDED
#pragma interrupt sci10_rxi10_isr(vect=VECT(SCI10,RXI10))
static void sci10_rxi10_isr(void)
{
    rxi_handler(&ch10_ctrl);
}
#endif

#if SCI_CFG_CH11_INCLUDED
#pragma interrupt sci11_rxi11_isr(vect=VECT(SCI11,RXI11))
static void sci11_rxi11_isr(void)
{
    rxi_handler(&ch11_ctrl);
}
#endif

#if SCI_CFG_CH12_INCLUDED
#pragma interrupt sci12_rxi12_isr(vect=VECT(SCI12,RXI12))
static void sci12_rxi12_isr(void)
{
    rxi_handler(&ch12_ctrl);
}
#endif


/*****************************************************************************
* Function Name: rxi_handler
* Description  : RXI interrupt handler for SCI UART mode
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013: hdl is hard-coded non-null value in calling interrupts */
static void rxi_handler(sci_hdl_t const hdl)
{
sci_cb_args_t   args;
uint8_t         byte;


    /* Read byte */
    byte = hdl->rom->regs->RDR;
    
    /* Place in queue */
    if (R_BYTEQ_Put(hdl->rx_que, byte) == BYTEQ_SUCCESS)
    {
        args.event = SCI_EVT_RX_CHAR;
    }
    else
    {
        args.event = SCI_EVT_RXBUF_OVFL;
    }

    /* Do callback if available */
    if (hdl->callback != NULL)
    {
        args.hdl = hdl;
        args.byte = byte;
        hdl->callback((void *)&args);
    }
}
/* PRQA S 2814 -- */


#ifdef BSP_MCU_RX63_ALL
/*****************************************************************************
* Function Name: icu_group12_isr
* Description  : SCI receiver error interrupt handler for all channels
* Arguments    : none
* Return Value : none
******************************************************************************/
#pragma interrupt icu_group12_isr(vect=VECT(ICU,GROUP12))
static void icu_group12_isr(void)
{
sci_cb_args_t   args;
uint32_t        err,mask;
uint8_t         i,byte;

    /*
    * Get group12 error value. Bit 0 corresponds to ch0, bit 1 to ch1, etc.
    * If a bit is set, an error occurred on that corresponding channel.
    * Loop through each bit (channel), and if an error occurred on that
    * channel and it is enabled by this driver, process the error.
    */
    err = ICU.GRP[12].LONG;
    for (i=0,mask=1; i < SCI_NUM_CH; i++, mask<<=1)
    {
        if ((err & mask & SCI_CFG_CH_INCLUDED_MASK) != 0)
        {
            /* Flush register */
            byte = g_handles[i]->rom->regs->RDR;

            /* Do callback for error */
            if (g_handles[i]->callback != NULL)
            {
                args.hdl = g_handles[i];
                args.byte = byte;
                if (g_handles[i]->rom->regs->SSR.BIT.ORER == 1)
                {
                    args.event = SCI_EVT_OVFL_ERR;
                }
                else if (g_handles[i]->rom->regs->SSR.BIT.PER == 1)
                {
                    args.event = SCI_EVT_PARITY_ERR;
                }
                else if (g_handles[i]->rom->regs->SSR.BIT.FER == 1)
                {
                    args.event = SCI_EVT_FRAMING_ERR;
                }
                g_handles[i]->callback((void *)&args);
            }
            
            /* Clear error condition */
            g_handles[i]->rom->regs->SSR.BYTE &= ~SCI_RCVR_ERR_MASK;
            while ((g_handles[i]->rom->regs->SSR.BYTE & SCI_RCVR_ERR_MASK) != 0)
            {
                byte = g_handles[i]->rom->regs->RDR;
                g_handles[i]->rom->regs->SSR.BYTE &= ~SCI_RCVR_ERR_MASK;
            }
        }
    }   

}
#else // not BSP_MCU_RX63_ALL

/*****************************************************************************
* Function Name: sciN_eriN_isr
* Description  : ERI interrupt routines for every SCI channel
* Arguments    : none
* Return Value : none
******************************************************************************/
#if SCI_CFG_CH0_INCLUDED
#pragma interrupt sci0_eri0_isr(vect=VECT(SCI0,ERI0))
static void sci0_eri0_isr(void)
{
    eri_handler(&ch0_ctrl);
}
#endif

#if SCI_CFG_CH1_INCLUDED
#pragma interrupt sci1_eri1_isr(vect=VECT(SCI1,ERI1))
static void sci1_eri1_isr(void)
{
    eri_handler(&ch1_ctrl);
}
#endif

#if SCI_CFG_CH2_INCLUDED
#pragma interrupt sci2_eri2_isr(vect=VECT(SCI2,ERI2))
static void sci2_eri2_isr(void)
{
    eri_handler(&ch2_ctrl);
}
#endif

#if SCI_CFG_CH3_INCLUDED
#pragma interrupt sci3_eri3_isr(vect=VECT(SCI3,ERI3))
static void sci3_eri3_isr(void)
{
    eri_handler(&ch3_ctrl);
}
#endif

#if SCI_CFG_CH4_INCLUDED
#pragma interrupt sci4_eri4_isr(vect=VECT(SCI4,ERI4))
static void sci4_eri4_isr(void)
{
    eri_handler(&ch4_ctrl);
}
#endif

#if SCI_CFG_CH5_INCLUDED
#pragma interrupt sci5_eri5_isr(vect=VECT(SCI5,ERI5))
static void sci5_eri5_isr(void)
{
    eri_handler(&ch5_ctrl);
}
#endif

#if SCI_CFG_CH6_INCLUDED
#pragma interrupt sci6_eri6_isr(vect=VECT(SCI6,ERI6))
static void sci6_eri6_isr(void)
{
    eri_handler(&ch6_ctrl);
}
#endif

#if SCI_CFG_CH7_INCLUDED
#pragma interrupt sci7_eri7_isr(vect=VECT(SCI7,ERI7))
static void sci7_eri7_isr(void)
{
    eri_handler(&ch7_ctrl);
}
#endif

#if SCI_CFG_CH8_INCLUDED
#pragma interrupt sci8_eri8_isr(vect=VECT(SCI8,ERI8))
static void sci8_eri8_isr(void)
{
    eri_handler(&ch8_ctrl);
}
#endif

#if SCI_CFG_CH9_INCLUDED
#pragma interrupt sci9_eri9_isr(vect=VECT(SCI9,ERI9))
static void sci9_eri9_isr(void)
{
    eri_handler(&ch9_ctrl);
}
#endif

#if SCI_CFG_CH10_INCLUDED
#pragma interrupt sci10_eri10_isr(vect=VECT(SCI10,ERI10))
static void sci10_eri10_isr(void)
{
    eri_handler(&ch10_ctrl);
}
#endif

#if SCI_CFG_CH11_INCLUDED
#pragma interrupt sci11_eri11_isr(vect=VECT(SCI11,ERI11))
static void sci11_eri11_isr(void)
{
    eri_handler(&ch11_ctrl);
}
#endif

#if SCI_CFG_CH12_INCLUDED
#pragma interrupt sci12_eri12_isr(vect=VECT(SCI12,ERI12))
static void sci12_eri12_isr(void)
{
    eri_handler(&ch12_ctrl);
}
#endif


/*****************************************************************************
* Function Name: eri_handler
* Description  : ERI interrupt handler for SCI UART mode
* Arguments    : hdl -
*                    handle for channel (ptr to chan control block)
* Return Value : none
******************************************************************************/
/* PRQA S 2814 ++ */
/* GRI 1/28/2013: hdl is hard-coded non-null value in calling interrupts */
static void eri_handler(sci_hdl_t const hdl)
{
sci_cb_args_t   args;
uint8_t         byte;

    /* Flush register */
    byte = hdl->rom->regs->RDR;

    /* Do callback for error */
    if (hdl->callback != NULL)
    {
        args.hdl = hdl;
        args.byte = byte;
        if (hdl->rom->regs->SSR.BIT.ORER == 1)
        {
            args.event = SCI_EVT_OVFL_ERR;
        }
        else if (hdl->rom->regs->SSR.BIT.PER == 1)
        {
            args.event = SCI_EVT_PARITY_ERR;
        }
        else if (hdl->rom->regs->SSR.BIT.FER == 1)
        {
            args.event = SCI_EVT_FRAMING_ERR;
        }
        else
        {   // PQRA 2004
        }
        hdl->callback((void *)&args);
    }

    /* Clear error condition */
    hdl->rom->regs->SSR.BYTE &= ~SCI_RCVR_ERR_MASK;
    while ((hdl->rom->regs->SSR.BYTE & SCI_RCVR_ERR_MASK) != 0)
    {
        byte = hdl->rom->regs->RDR; /* PRQA S 2983 */ /* 'byte' not used */
        hdl->rom->regs->SSR.BYTE &= ~SCI_RCVR_ERR_MASK;
    }

}
/* PRQA S 2814 -- */

#endif // not BSP_MCU_RX63_ALL


/*****************************************************************************
* Function Name: R_SCI_Control
* Description  : This function configures non-standard UART hardware and
*                performs special software operations.
*
* WARNING: Some commands require the transmitter and receiver to be temporarily
*          disabled for the command to execute!
*          PFS and port pins must be configured prior to calling with an
*          SCI_EN_CTS_IN command.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
*                cmd -
*                    command to run
*                p_args -
*                    pointer argument(s) specific to command
* Return Value : SCI_SUCCESS -
*                    Command completed successfully.
*                SCI_ERR_NULL_PTR -
*                    hdl or p_args is NULL
*                SCI_ERR_INVALID_ARG -
*                    The cmd value or p_args contains an invalid value.
******************************************************************************/
/* PRQA S 3227 ++ */
/* 3227- p_cfg could be const: if made constant, must assign to const local variable
 *       at declaration. Compiler then gives warning "type qualifier meaningless
 *       on cast type" for the p_args argument.
 */
sci_err_t R_SCI_Control(sci_hdl_t const     hdl,
                        sci_cmd_t const     cmd,
                        void                *p_args)
{
static uint32_t pclkb_speed=BSP_PCLKB_HZ;  // peripheral clock speed
sci_err_t   err=SCI_SUCCESS;
sci_baud_t  *baud;
int32_t     bit_err;
uint32_t    slow_baud;


#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if ((hdl == NULL)
     || ((p_args == NULL) && (cmd == SCI_CMD_CHANGE_BAUD)))
    {
        return SCI_ERR_NULL_PTR;
    }
#endif
    
    switch (cmd)
    {
    case (SCI_CMD_EN_NOISE_CANCEL):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        hdl->rom->regs->SEMR.BIT.NFEN = 1;      // enable noise filter
        hdl->rom->regs->SNFR.BYTE = 0;          // clock divided by 1 (default)
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        break;
        
    case (SCI_CMD_EN_CTS_IN):
    	/* PFS & port pins must be configured for CTS prior to calling this */
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        hdl->rom->regs->SPMR.BIT.CTSE = 1;      // enable CTS input
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        break;
        
    case (SCI_CMD_EN_TEI):
        hdl->rom->regs->SCR.BIT.TEIE = 1;       // enable TEI interrupts
        break;
        
    case (SCI_CMD_OUTPUT_BAUD_CLK):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        hdl->rom->regs->SCR.BIT.CKE = 0x01;     // output baud clock on SCK pin
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        break;

#if defined(BSP_MCU_RX11_ALL)
    case (SCI_CMD_START_BIT_EDGE):
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        hdl->rom->regs->SEMR.BIT.RXDESEL = 1;   // detect start bit on falling edge
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        break;
#endif
    
    case (SCI_CMD_CHANGE_BAUD):
        baud = (sci_baud_t *)p_args;
        pclkb_speed = baud->pclk;        // save for break generation
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        bit_err = sci_init_bit_rate(hdl, baud->pclk, baud->rate);
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        if (bit_err == 1000)
        {
            err = SCI_ERR_INVALID_ARG;    // impossible baud rate; 100% error
        }
        else
        {
            hdl->baud_rate = baud->rate;  // save for break generation
        }
        break;
        
    case (SCI_CMD_GENERATE_BREAK):
        /* flush transmit queue */
        DISABLE_TXI_INT;
        R_BYTEQ_Flush(hdl->tx_que);
        ENABLE_TXI_INT;
        
        /* NOTE: the following steps will abort anything being sent */
        
        /* set baud rate 1.5x slower */
        slow_baud = (hdl->baud_rate << 1) / 3;
        hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
        bit_err = sci_init_bit_rate(hdl, pclkb_speed, slow_baud);
        hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        if (bit_err == 1000)
        {
            err = SCI_ERR_INVALID_ARG;      // TODO: make diff error code?
        }
        else
        {   
            /* transmit "0" and wait for completion */
            hdl->rom->regs->TDR = 0;
            while (hdl->rom->regs->SSR.BIT.TEND == 0)
            {
                nop();
            }
        
            /* restore original baud rate */
            hdl->rom->regs->SCR.BYTE &= ~SCI_EN_XCVR_MASK;
            sci_init_bit_rate(hdl, pclkb_speed, hdl->baud_rate);
            hdl->rom->regs->SCR.BYTE |= SCI_EN_XCVR_MASK;
        }
        break;
        
    case (SCI_CMD_TX_Q_FLUSH):
        DISABLE_TXI_INT;
        R_BYTEQ_Flush(hdl->tx_que);
        ENABLE_TXI_INT;
        break;
        
    case (SCI_CMD_RX_Q_FLUSH):
        DISABLE_RXI_INT;
        R_BYTEQ_Flush(hdl->rx_que);
        ENABLE_RXI_INT;
        break;

    case (SCI_CMD_TX_Q_BYTES_FREE):
        DISABLE_TXI_INT;
        R_BYTEQ_Unused(hdl->tx_que, (uint16_t *) p_args);
        ENABLE_TXI_INT;
        break;

    case (SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ):
        DISABLE_RXI_INT;
        R_BYTEQ_Used(hdl->rx_que, (uint16_t *) p_args);
        ENABLE_RXI_INT;
        break;

#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    default:
        err = SCI_ERR_INVALID_ARG;
        break;
#endif
    }

    return err;
}
/* PRQA S 3227 -- */

/*****************************************************************************
* Function Name: R_SCI_Close
* Description  : Disables the SCI channel designated by the handle.
*
* WARNING: This will abort any xmt or rcv messages in progress.
* NOTE:    This does not disable the GROUP12 (rcvr err) interrupts.
*
* Arguments    : hdl - 
*                    handle for channel (ptr to chan control block)
* Return Value : SCI_SUCCESS -
*                    channel closed
*                SCI_ERR_NULL_PTR -
*                    hdl was NULL
******************************************************************************/
sci_err_t R_SCI_Close(sci_hdl_t const hdl)
{


#if (SCI_CFG_PARAM_CHECKING_ENABLE == 1)
    if (hdl == NULL)
    {
        return SCI_ERR_NULL_PTR;
    }
#endif

    /* disable ICU interrupts */
    DISABLE_RXI_INT;
    DISABLE_TXI_INT;
    DISABLE_TEI_INT;
    // do not disable GROUP12 interrupts for 63N; may be active for another chan
#ifndef BSP_MCU_RX63_ALL
    DISABLE_ERI_INT;
#endif
    
    /* disable peripheral interrupts and xcvr (TE and RE) */
    hdl->rom->regs->SCR.BYTE = 0;
        
    /* free tx and rx queues */
    R_BYTEQ_Close(hdl->tx_que);
    R_BYTEQ_Close(hdl->rx_que);
        
    /* mark the channel as not in use and power down */
    hdl->mode = SCI_MODE_OFF;
    power_off(hdl);

    return SCI_SUCCESS;
}


/*****************************************************************************
* Function Name: R_SCI_GetVersion
* Description  : Returns the version of this module. The version number is 
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
#pragma inline(R_SCI_GetVersion)
uint32_t  R_SCI_GetVersion(void)
{
uint32_t const version = (SCI_ASYNC_VERSION_MAJOR << 16) | SCI_ASYNC_VERSION_MINOR;

    return version;
}
