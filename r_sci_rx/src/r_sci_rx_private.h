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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_sci_rx_private.h
* Description  : Functions for using SCI on RX devices. 
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           25.09.2013 1.00    Initial Release
*           17.04.2014 1.20    Bumped revision for RX110 support.
*           02.07.2014 1.30    Fixed bug that caused Group12 rx errors to only be enabled for channel 2.
*           25.11.2014 1.40    Added support for RX113
***********************************************************************************************************************/

#ifndef SCI_PRIVATE_H
#define SCI_PRIVATE_H

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"

#include "r_sci_rx_if.h"
#if (SCI_CFG_ASYNC_INCLUDED)
#include "r_byteq_if.h"
#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Bit position masks */
#define BIT0_MASK   (0x01U)
#define BIT1_MASK   (0x02U)
#define BIT2_MASK   (0x04U)
#define BIT3_MASK   (0x08U)
#define BIT4_MASK   (0x10U)
#define BIT5_MASK   (0x20U)
#define BIT6_MASK   (0x40U)
#define BIT7_MASK   (0x80U)
#define BIT8_MASK   (0x0100U)
#define BIT9_MASK   (0x0200U)
#define BIT10_MASK  (0x0400U)
#define BIT11_MASK  (0x0800U)
#define BIT12_MASK  (0x1000U)
#define BIT13_MASK  (0x2000U)
#define BIT14_MASK  (0x4000U)
#define BIT15_MASK  (0x8000U)
#define BIT16_MASK  (0x010000U)
#define BIT17_MASK  (0x020000U)
#define BIT18_MASK  (0x040000U)
#define BIT19_MASK  (0x080000U)
#define BIT20_MASK  (0x100000U)
#define BIT21_MASK  (0x200000U)
#define BIT22_MASK  (0x400000U)
#define BIT23_MASK  (0x800000U)
#define BIT24_MASK  (0x01000000U)
#define BIT25_MASK  (0x02000000U)
#define BIT26_MASK  (0x04000000U)
#define BIT27_MASK  (0x08000000U)
#define BIT28_MASK  (0x10000000U)
#define BIT29_MASK  (0x20000000U)
#define BIT30_MASK  (0x40000000U)
#define BIT31_MASK  (0x80000000U)

#ifndef NULL    // Resolves e2studio code analyzer false error message.
    #define NULL 0
#endif

/*****************************************************************************
Typedef definitions
******************************************************************************/

/* ROM INFO */

typedef struct st_sci_ch_rom    // SCI ROM info for channel control block
{
    volatile  __evenaccess struct st_sci12  *regs;  // base ptr to ch registers
    volatile  __evenaccess uint32_t *mstp;      // ptr to mstp register
    uint32_t                        stop_mask;  // mstp mask to disable ch
    volatile  __evenaccess uint8_t  *ipr;       // ptr to IPR register
    volatile  __evenaccess uint8_t  *ir_rxi;    // ptr to RXI IR register
    volatile  __evenaccess uint8_t  *ir_txi;    // ptr to TXI IR register
    volatile  __evenaccess uint8_t  *ir_tei;    // ptr to TEI IR register
#ifndef BSP_MCU_RX63_ALL
    volatile  __evenaccess uint8_t  *ir_eri;    // ptr to ERI IR register
#endif
    /* 
    * DO NOT use the enable/disable interrupt bits in the SCR 
    * register. Pending interrupts can be lost that way.
    */
    volatile  __evenaccess uint8_t  *icu_rxi;   // ptr to ICU register
    volatile  __evenaccess uint8_t  *icu_txi;   
    volatile  __evenaccess uint8_t  *icu_tei; 
#ifdef BSP_MCU_RX63_ALL
    uint16_t	group12_ch_mask;	// bit mask to enable/disable rx error int in ICU
#else
    volatile  __evenaccess uint8_t  *icu_eri;
    uint8_t     eri_en_mask;    // bit mask to enable/disable eri INT in ICU
#endif
    uint8_t     rxi_en_mask;    // bit mask to enable/disable rxi INT in ICU
    uint8_t     txi_en_mask;    // bit mask to enable/disable txi INT in ICU   
    uint8_t     tei_en_mask;    // bit mask to enable/disable tei INT in ICU
} sci_ch_rom_t;


/* CHANNEL CONTROL BLOCK */

typedef struct st_sci_ch_ctrl       // SCI channel control (for handle)
{
    sci_ch_rom_t const *rom;        // pointer to rom info
    sci_mode_t      mode;           // operational mode
    uint32_t        baud_rate;      // baud rate
    void          (*callback)(void *p_args); // function ptr for rcvr errs
    union
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        byteq_hdl_t     que;        // async transmit queue handle
#endif
        uint8_t         *buf;       // sspi/sync tx buffer ptr
    } u_tx_data;
    union
    {
#if (SCI_CFG_ASYNC_INCLUDED)
        byteq_hdl_t     que;        // async receive queue handle
#endif
        uint8_t         *buf;       // sspi/sync rx buffer ptr
    } u_rx_data;
    bool            tx_idle;        // TDR is empty (async); TSR is empty (sync/sspi)
#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
    bool            save_rx_data;   // save the data that is clocked in
    uint16_t        tx_cnt;         // number of bytes to transmit
    bsp_lock_t      xcvr_lock;
    bool            tx_dummy;       // transmit dummy byte, not buffer
#endif
} sci_ch_ctrl_t;


/* BAUD DIVISOR INFO */

// BRR = (PCLK/(divisor * baud)) - 1
// when abcs=1, divisor = 32*pow(2,2n-1)
// when abcs=0, divisor = 64*pow(2,2n-1)

typedef struct st_baud_divisor
{
    int16_t     divisor;    // clock divisor
    uint8_t     abcs;       // abcs value to get divisor
    uint8_t     cks;        // cks  value to get divisor (cks = n)
} baud_divisor_t;


/*****************************************************************************
Private global variables and functions
******************************************************************************/

/* BAUD DIVISOR INFO */

// Asynchronous
// BRR = (PCLK/(divisor * baud)) - 1
// when abcs=1, divisor = 32*pow(2,2n-1)
// when abcs=0, divisor = 64*pow(2,2n-1)

#if (SCI_CFG_ASYNC_INCLUDED)
#define NUM_DIVISORS_ASYNC  8
const baud_divisor_t async_baud[NUM_DIVISORS_ASYNC]=
{
    {16,   1, 0},   // divisor result, abcs, n
    {32,   0, 0},
    {64,   1, 1},
    {128,  0, 1},
    {256,  1, 2},
    {512,  0, 2},
    {1024, 1, 3},
    {2048, 0, 3}
};
#endif

// Synchronous and Simple SPI
// BRR = (PCLK/(divisor * baud)) - 1
// abcs=0, divisor = 8*pow(2,2n-1)

#if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
#define NUM_DIVISORS_SYNC   4
const baud_divisor_t sync_baud[NUM_DIVISORS_SYNC]=
{
    {4,   0, 0},    // divisor result, abcs, n
    {16,  0, 1},
    {64,  0, 2},
    {256, 0, 3}
};
#endif


/* CHANNEL MEMORY ALLOCATIONS */

#if SCI_CFG_CH0_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch0_tx_buf[SCI_CFG_CH0_TX_BUFSIZ];
uint8_t         ch0_rx_buf[SCI_CFG_CH0_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch0_rom = {(volatile __evenaccess struct st_sci12 *)&SCI0,
                                &SYSTEM.MSTPCRB.LONG, BIT31_MASK,
                                &ICU.IPR[IPR_SCI0_RXI0].BYTE,
                                &ICU.IR[IR_SCI0_RXI0].BYTE,
                                &ICU.IR[IR_SCI0_TXI0].BYTE,
                                &ICU.IR[IR_SCI0_TEI0].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI0_ERI0].BYTE,
                                #endif
                                &ICU.IER[IER_SCI0_RXI0].BYTE,
                                &ICU.IER[IER_SCI0_TXI0].BYTE,
                                &ICU.IER[IER_SCI0_TEI0].BYTE,
                                #if defined(BSP_MCU_RX63_ALL) 
                                (1<<0),
                                BIT6_MASK, BIT7_MASK, BIT0_MASK
								#elif (defined(BSP_MCU_RX113) || defined(BSP_MCU_RX21_ALL))
                                &ICU.IER[IER_SCI0_ERI0].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch0_ctrl = {&ch0_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH1_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch1_tx_buf[SCI_CFG_CH1_TX_BUFSIZ];
uint8_t         ch1_rx_buf[SCI_CFG_CH1_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch1_rom = {(volatile __evenaccess struct st_sci12 *)&SCI1,
                                &SYSTEM.MSTPCRB.LONG, BIT30_MASK,
                                &ICU.IPR[IPR_SCI1_RXI1].BYTE,
                                &ICU.IR[IR_SCI1_RXI1].BYTE,
                                &ICU.IR[IR_SCI1_TXI1].BYTE,
                                &ICU.IR[IR_SCI1_TEI1].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI1_ERI1].BYTE,
                                #endif
                                &ICU.IER[IER_SCI1_RXI1].BYTE,
                                &ICU.IER[IER_SCI1_TXI1].BYTE,
                                &ICU.IER[IER_SCI1_TEI1].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<1),
                                BIT1_MASK, BIT2_MASK, BIT3_MASK
                                #elif defined(BSP_MCU_RX11_ALL) || defined(BSP_MCU_RX21_ALL)
                                &ICU.IER[IER_SCI1_ERI1].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch1_ctrl = {&ch1_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH2_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch2_tx_buf[SCI_CFG_CH2_TX_BUFSIZ];
uint8_t         ch2_rx_buf[SCI_CFG_CH2_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch2_rom = {(volatile __evenaccess struct st_sci12 *)&SCI2,
                                &SYSTEM.MSTPCRB.LONG, BIT29_MASK,
                                &ICU.IPR[IPR_SCI2_RXI2].BYTE,
                                &ICU.IR[IR_SCI2_RXI2].BYTE,
                                &ICU.IR[IR_SCI2_TXI2].BYTE,
                                &ICU.IR[IR_SCI2_TEI2].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI2_ERI2].BYTE,
                                #endif
                                &ICU.IER[IER_SCI2_RXI2].BYTE,
                                &ICU.IER[IER_SCI2_TXI2].BYTE,
                                &ICU.IER[IER_SCI2_TEI2].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<2),
                                BIT4_MASK, BIT5_MASK, BIT6_MASK
								#elif defined(BSP_MCU_RX113)
								&ICU.IER[IER_SCI2_ERI2].BYTE,
								BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch2_ctrl = {&ch2_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH3_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch3_tx_buf[SCI_CFG_CH3_TX_BUFSIZ];
uint8_t         ch3_rx_buf[SCI_CFG_CH3_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch3_rom = {(volatile __evenaccess struct st_sci12 *)&SCI3,
                                &SYSTEM.MSTPCRB.LONG, BIT28_MASK,
                                &ICU.IPR[IPR_SCI3_RXI3].BYTE,
                                &ICU.IR[IR_SCI3_RXI3].BYTE,
                                &ICU.IR[IR_SCI3_TXI3].BYTE,
                                &ICU.IR[IR_SCI3_TEI3].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI3_ERI3].BYTE,
                                #endif
                                &ICU.IER[IER_SCI3_RXI3].BYTE,
                                &ICU.IER[IER_SCI3_TXI3].BYTE,
                                &ICU.IER[IER_SCI3_TEI3].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<3),
                                BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch3_ctrl = {&ch3_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH4_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch4_tx_buf[SCI_CFG_CH4_TX_BUFSIZ];
uint8_t         ch4_rx_buf[SCI_CFG_CH4_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch4_rom = {(volatile __evenaccess struct st_sci12 *)&SCI4,
                                &SYSTEM.MSTPCRB.LONG, BIT27_MASK,
                                &ICU.IPR[IPR_SCI4_RXI4].BYTE,
                                &ICU.IR[IR_SCI4_RXI4].BYTE,
                                &ICU.IR[IR_SCI4_TXI4].BYTE,
                                &ICU.IR[IR_SCI4_TEI4].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI4_ERI4].BYTE,
                                #endif
                                &ICU.IER[IER_SCI4_RXI4].BYTE,
                                &ICU.IER[IER_SCI4_TXI4].BYTE,
                                &ICU.IER[IER_SCI4_TEI4].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<4),
                                BIT2_MASK, BIT3_MASK, BIT4_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch4_ctrl = {&ch4_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH5_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch5_tx_buf[SCI_CFG_CH5_TX_BUFSIZ];
uint8_t         ch5_rx_buf[SCI_CFG_CH5_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch5_rom = {(volatile __evenaccess struct st_sci12 *)&SCI5,
                                &SYSTEM.MSTPCRB.LONG, BIT26_MASK,
                                &ICU.IPR[IPR_SCI5_RXI5].BYTE,
                                &ICU.IR[IR_SCI5_RXI5].BYTE,
                                &ICU.IR[IR_SCI5_TXI5].BYTE,
                                &ICU.IR[IR_SCI5_TEI5].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI5_ERI5].BYTE,
                                #endif
                                &ICU.IER[IER_SCI5_RXI5].BYTE,
                                &ICU.IER[IER_SCI5_TXI5].BYTE,
                                &ICU.IER[IER_SCI5_TEI5].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<5),
                                BIT5_MASK, BIT6_MASK, BIT7_MASK
                                #elif defined(BSP_MCU_RX11_ALL) || defined(BSP_MCU_RX21_ALL)
                                &ICU.IER[IER_SCI5_ERI5].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch5_ctrl = {&ch5_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH6_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch6_tx_buf[SCI_CFG_CH6_TX_BUFSIZ];
uint8_t         ch6_rx_buf[SCI_CFG_CH6_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch6_rom = {(volatile __evenaccess struct st_sci12 *)&SCI6,
                                &SYSTEM.MSTPCRB.LONG, BIT25_MASK,
                                &ICU.IPR[IPR_SCI6_RXI6].BYTE,
                                &ICU.IR[IR_SCI6_RXI6].BYTE,
                                &ICU.IR[IR_SCI6_TXI6].BYTE,
                                &ICU.IR[IR_SCI6_TEI6].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI6_ERI6].BYTE,
                                #endif
                                &ICU.IER[IER_SCI6_RXI6].BYTE,
                                &ICU.IER[IER_SCI6_TXI6].BYTE,
                                &ICU.IER[IER_SCI6_TEI6].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<6),
                                BIT0_MASK, BIT1_MASK, BIT2_MASK
                                #elif (defined(BSP_MCU_RX113) || defined(BSP_MCU_RX21_ALL))
                                &ICU.IER[IER_SCI6_ERI6].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch6_ctrl = {&ch6_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH7_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch7_tx_buf[SCI_CFG_CH7_TX_BUFSIZ];
uint8_t         ch7_rx_buf[SCI_CFG_CH7_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch7_rom = {(volatile __evenaccess struct st_sci12 *)&SCI7,
                                &SYSTEM.MSTPCRB.LONG, BIT24_MASK,
                                &ICU.IPR[IPR_SCI7_RXI7].BYTE,
                                &ICU.IR[IR_SCI7_RXI7].BYTE,
                                &ICU.IR[IR_SCI7_TXI7].BYTE,
                                &ICU.IR[IR_SCI7_TEI7].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI7_ERI7].BYTE,
                                #endif
                                &ICU.IER[IER_SCI7_RXI7].BYTE,
                                &ICU.IER[IER_SCI7_TXI7].BYTE,
                                &ICU.IER[IER_SCI7_TEI7].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<7),
                                BIT3_MASK, BIT4_MASK, BIT5_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch7_ctrl = {&ch7_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH8_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch8_tx_buf[SCI_CFG_CH8_TX_BUFSIZ];
uint8_t         ch8_rx_buf[SCI_CFG_CH8_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch8_rom = {(volatile __evenaccess struct st_sci12 *)&SCI8,
                                &SYSTEM.MSTPCRC.LONG, BIT27_MASK,
                                &ICU.IPR[IPR_SCI8_RXI8].BYTE,
                                &ICU.IR[IR_SCI8_RXI8].BYTE,
                                &ICU.IR[IR_SCI8_TXI8].BYTE,
                                &ICU.IR[IR_SCI8_TEI8].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI8_ERI8].BYTE,
                                #endif
                                &ICU.IER[IER_SCI8_RXI8].BYTE,
                                &ICU.IER[IER_SCI8_TXI8].BYTE,
                                &ICU.IER[IER_SCI8_TEI8].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<8),
                                BIT6_MASK, BIT7_MASK, BIT0_MASK
                                #elif (defined(BSP_MCU_RX113) || defined(BSP_MCU_RX21_ALL))
                                &ICU.IER[IER_SCI8_ERI8].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch8_ctrl = {&ch8_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH9_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch9_tx_buf[SCI_CFG_CH9_TX_BUFSIZ];
uint8_t         ch9_rx_buf[SCI_CFG_CH9_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch9_rom = {(volatile __evenaccess struct st_sci12 *)&SCI9,
                                &SYSTEM.MSTPCRC.LONG, BIT26_MASK,
                                &ICU.IPR[IPR_SCI9_RXI9].BYTE,
                                &ICU.IR[IR_SCI9_RXI9].BYTE,
                                &ICU.IR[IR_SCI9_TXI9].BYTE,
                                &ICU.IR[IR_SCI9_TEI9].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI9_ERI9].BYTE,
                                #endif
                                &ICU.IER[IER_SCI9_RXI9].BYTE,
                                &ICU.IER[IER_SCI9_TXI9].BYTE,
                                &ICU.IER[IER_SCI9_TEI9].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<9),
                                BIT1_MASK, BIT2_MASK, BIT3_MASK
                                #elif (defined(BSP_MCU_RX113) || defined(BSP_MCU_RX21_ALL))
                                &ICU.IER[IER_SCI9_ERI9].BYTE,
                                BIT2_MASK, BIT3_MASK, BIT4_MASK, BIT5_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch9_ctrl = {&ch9_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                            #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                            , true, 0, 0, false
                            #endif
                           };
#endif


#if SCI_CFG_CH10_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch10_tx_buf[SCI_CFG_CH10_TX_BUFSIZ];
uint8_t         ch10_rx_buf[SCI_CFG_CH10_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch10_rom = {(volatile __evenaccess struct st_sci12 *)&SCI10,
                                &SYSTEM.MSTPCRC.LONG, BIT25_MASK,
                                &ICU.IPR[IPR_SCI10_RXI10].BYTE,
                                &ICU.IR[IR_SCI10_RXI10].BYTE,
                                &ICU.IR[IR_SCI10_TXI10].BYTE,
                                &ICU.IR[IR_SCI10_TEI10].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI10_ERI10].BYTE,
                                #endif
                                &ICU.IER[IER_SCI10_RXI10].BYTE,
                                &ICU.IER[IER_SCI10_TXI10].BYTE,
                                &ICU.IER[IER_SCI10_TEI10].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<10),
                                BIT4_MASK, BIT5_MASK, BIT6_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch10_ctrl = {&ch10_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                             #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                             , true, 0, 0, false
                             #endif
                            };
#endif


#if SCI_CFG_CH11_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch11_tx_buf[SCI_CFG_CH11_TX_BUFSIZ];
uint8_t         ch11_rx_buf[SCI_CFG_CH11_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch11_rom = {(volatile __evenaccess struct st_sci12 *)&SCI11,
                                &SYSTEM.MSTPCRC.LONG, BIT24_MASK,
                                &ICU.IPR[IPR_SCI11_RXI11].BYTE,
                                &ICU.IR[IR_SCI11_RXI11].BYTE,
                                &ICU.IR[IR_SCI11_TXI11].BYTE,
                                &ICU.IR[IR_SCI11_TEI11].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI11_ERI11].BYTE,
                                #endif
                                &ICU.IER[IER_SCI11_RXI11].BYTE,
                                &ICU.IER[IER_SCI11_TXI11].BYTE,
                                &ICU.IER[IER_SCI11_TEI11].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<11),
                                BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch11_ctrl = {&ch11_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                             #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                             , true, 0, 0, false
                             #endif
                            };
#endif


#if SCI_CFG_CH12_INCLUDED

/* queue buffers */
#if (SCI_CFG_ASYNC_INCLUDED)
uint8_t         ch12_tx_buf[SCI_CFG_CH12_TX_BUFSIZ];
uint8_t         ch12_rx_buf[SCI_CFG_CH12_RX_BUFSIZ];
#endif

/* rom info */
const sci_ch_rom_t  ch12_rom = {(volatile __evenaccess struct st_sci12 *)&SCI12,
                                &SYSTEM.MSTPCRB.LONG, BIT4_MASK,
                                &ICU.IPR[IPR_SCI12_RXI12].BYTE,
                                &ICU.IR[IR_SCI12_RXI12].BYTE,
                                &ICU.IR[IR_SCI12_TXI12].BYTE,
                                &ICU.IR[IR_SCI12_TEI12].BYTE,
                                #ifndef BSP_MCU_RX63_ALL
                                &ICU.IR[IR_SCI12_ERI12].BYTE,
                                #endif
                                &ICU.IER[IER_SCI12_RXI12].BYTE,
                                &ICU.IER[IER_SCI12_TXI12].BYTE,
                                &ICU.IER[IER_SCI12_TEI12].BYTE,
                                #if defined(BSP_MCU_RX63_ALL)
                                (1<<12),
                                BIT2_MASK, BIT3_MASK, BIT4_MASK
                                #elif defined(BSP_MCU_RX11_ALL) || defined(BSP_MCU_RX21_ALL)
                                &ICU.IER[IER_SCI12_ERI12].BYTE,
                                BIT6_MASK, BIT7_MASK, BIT0_MASK, BIT1_MASK
                                #endif
                                };
/* channel control block */
sci_ch_ctrl_t   ch12_ctrl = {&ch12_rom, SCI_MODE_OFF, 0, NULL, NULL, NULL, true
                             #if (SCI_CFG_SSPI_INCLUDED || SCI_CFG_SYNC_INCLUDED)
                             , true, 0, 0, false
                             #endif
                            };
#endif


/* SCI HANDLE-ARRAY DECLARATION */

//static const sci_hdl_t g_handles[SCI_NUM_CH] =
const sci_hdl_t g_handles[SCI_NUM_CH] =
{
#if SCI_CFG_CH0_INCLUDED
            &ch0_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH1_INCLUDED
            &ch1_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH2_INCLUDED
            &ch2_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH3_INCLUDED
            &ch3_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH4_INCLUDED
            &ch4_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH5_INCLUDED
            &ch5_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH6_INCLUDED
            &ch6_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH7_INCLUDED
            &ch7_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH8_INCLUDED
            &ch8_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH9_INCLUDED
            &ch9_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH10_INCLUDED
            &ch10_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH11_INCLUDED
            &ch11_ctrl,
#else
            NULL,
#endif
#if SCI_CFG_CH12_INCLUDED
            &ch12_ctrl
#else
            NULL
#endif
};


#endif /* SCI_ASYNC_PRIVATE_H */

