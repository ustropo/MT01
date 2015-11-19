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
* File Name    : r_mtu_pwm_rx_private.h
* Version      : 1.00
* Device(s)    : Renesas RX Family
* Tool-Chain   : Renesas RX Standard Toolchain
* H/W Platform :
* Description  : Private definitions for the RX FIT IRQ support module.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY  Version Description
*         : 30.09.2014  1.00    First Release

***********************************************************************************************************************/
#ifndef R_MTU_PRIVATE_H_
#define R_MTU_PRIVATE_H_

#include "platform.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef void(*mtu_callback)(void *pargs);

#define  MTU_BIT0 (0x01)
#define  MTU_BIT1 (0x02)
#define  MTU_BIT2 (0x04)
#define  MTU_BIT3 (0x08)
#define  MTU_BIT4 (0x10)
#define  MTU_BIT5 (0x20)
#define  MTU_BIT6 (0x40)
#define  MTU_BIT7 (0x80)

#define MTU_NOT_SUPP (0xFF) // A value used to mark register settings table entries where setting not available.

typedef enum
{
    MTU_TGIEA = MTU_BIT0, // TGR Interrupt Enable A
    MTU_TGIEB = MTU_BIT1, // TGR Interrupt Enable B
    MTU_TGIEC = MTU_BIT2, // TGR Interrupt Enable C
    MTU_TGIED = MTU_BIT3, // TGR Interrupt Enable D
    MTU_TCIEV = MTU_BIT4, // Overflow Interrupt Enable
    MTU_TCIEU = MTU_BIT5, // Underflow Interrupt Enable
    MTU_TTGE2 = MTU_BIT6, // A/D Converter Start Request Enable
    MTU_TTGE  = MTU_BIT7  // A/D Converter Start Request Enable
} mtu_int_src_t;

typedef enum mtu_filter_en_e
{
    MTU_FILT_EN_A = MTU_BIT0,
    MTU_FILT_EN_B = MTU_BIT1,
    MTU_FILT_EN_C = MTU_BIT2,
    MTU_FILT_EN_D = MTU_BIT3
} mtu_filter_en_t;

/* Create a standardized structure for MTU register addresses used in this module to cover all channels. */
typedef struct mtu_timer_regs_s
{
    volatile __evenaccess unsigned char  *tcr;    // TCR
    volatile __evenaccess unsigned char  *tmdr;   // TMDR
    volatile __evenaccess unsigned char  *tiorh;  // TIORH or TIOR
    volatile __evenaccess unsigned char  *tiorl;  // TIORL
    volatile __evenaccess unsigned char  *tier;   // TIER
    volatile __evenaccess unsigned char  *tbtm;   // TBTM
    volatile __evenaccess unsigned char  *nfcr;   // NFCR
    volatile __evenaccess unsigned short *tcnt;   // TCNT
    volatile __evenaccess unsigned short *tgra;   // TGRA
    volatile __evenaccess unsigned short *tgrb;   // TGRB
    volatile __evenaccess unsigned short *tgrc;   // TGRC
    volatile __evenaccess unsigned short *tgrd;   // TGRD
    volatile __evenaccess unsigned short *tgre;   // TGRE
    volatile __evenaccess unsigned short *tgrf;   // TGRF
    volatile __evenaccess unsigned char  *ir;     // Interrupt Request register
    volatile __evenaccess unsigned char  *ien_a;  // Interrupt A Enable register
    volatile __evenaccess unsigned char  *ien_b;  // Interrupt B Enable register
    volatile __evenaccess unsigned char  *ien_c;  // Interrupt C Enable register
    volatile __evenaccess unsigned char  *ien_d;  // Interrupt D Enable register
    volatile __evenaccess unsigned char  *ipr;    // Channel shared Interrupt Priority register
} mtu_timer_regs_t;

/* configuration and control structure */
typedef struct mtu_config_block_s
{
    mtu_callback *const p_callback;                    // pointer to callback function pointer.
    mtu_timer_chnl_settings_t * p_mtu_chnl_tmr_settings; // location of variable timer settings struct.
    mtu_timer_regs_t   regs;
    uint8_t            channel;
    uint8_t            priority;        // interrupt priority
    uint8_t            filt_clk;        // input capture noise filter clock source
} mtu_config_block_t;

/* Abstraction of channel handle data structure. */
typedef struct mtu_config_block_s *mtu_handle_t;

typedef struct mtu_tcr_reg_s
{
    struct
    {
        uint8_t tpsc:3;
        uint8_t ckeg:2;
        uint8_t cclr:3;
    } bits;
} mtu_tcr_reg_t;

typedef enum
{
    MTU_TSTR_CH0 = MTU_BIT0,
    MTU_TSTR_CH1 = MTU_BIT1,
    MTU_TSTR_CH2 = MTU_BIT2,
    MTU_TSTR_CH3 = MTU_BIT6,
    MTU_TSTR_CH4 = MTU_BIT7
} mtu_tstr_bits_t;

/******************************************************************************
Macro definitions
******************************************************************************/
/* Version Number of API. */
#define MTU_RX_VERSION_MAJOR           (1)
#define MTU_RX_VERSION_MINOR           (00)

/* Define register addresses assigned to each MTU register. */
#define MTU0_REGS { &MTU0.TCR.BYTE,   \
                    &MTU0.TMDR.BYTE,  \
                    &MTU0.TIORH.BYTE, \
                    &MTU0.TIORL.BYTE, \
                    &MTU0.TIER.BYTE,  \
                    &MTU0.TBTM.BYTE,  \
                    &MTU0.NFCR.BYTE,  \
                    &MTU0.TCNT,       \
                    &MTU0.TGRA,       \
                    &MTU0.TGRB,       \
                    &MTU0.TGRC,       \
                    &MTU0.TGRD,       \
                    &MTU0.TGRE,       \
                    &MTU0.TGRF,       \
                    &(ICU.IR[IR_MTU0_TGIA0].BYTE), \
                    &(ICU.IER[IER_MTU0_TGIA0].BYTE), \
                    &(ICU.IER[IER_MTU0_TGIB0].BYTE), \
                    &(ICU.IER[IER_MTU0_TGIC0].BYTE), \
                    &(ICU.IER[IER_MTU0_TGID0].BYTE), \
                    &(ICU.IPR[IPR_MTU0_TGIA0].BYTE)}

#define MTU1_REGS { &MTU1.TCR.BYTE,   \
                    &MTU1.TMDR.BYTE,  \
                    &MTU1.TIOR.BYTE,  \
                    NULL,             \
                    &MTU1.TIER.BYTE,  \
                    NULL,             \
                    &MTU1.NFCR.BYTE,  \
                    &MTU1.TCNT,       \
                    &MTU1.TGRA,       \
                    &MTU1.TGRB,       \
                    NULL,             \
                    NULL,             \
                    NULL,             \
                    NULL,              \
                    &(ICU.IR[IR_MTU1_TGIA1].BYTE), \
                    &(ICU.IER[IER_MTU1_TGIA1].BYTE), \
                    &(ICU.IER[IER_MTU1_TGIB1].BYTE), \
                    NULL, \
                    NULL, \
                    &(ICU.IPR[IPR_MTU1_TGIA1].BYTE)}

#define MTU2_REGS { &MTU2.TCR.BYTE,   \
                    &MTU2.TMDR.BYTE,  \
                    &MTU2.TIOR.BYTE,  \
                    NULL,             \
                    &MTU2.TIER.BYTE,  \
                    NULL,             \
                    &MTU2.NFCR.BYTE,  \
                    &MTU2.TCNT,       \
                    &MTU2.TGRA,       \
                    &MTU2.TGRB,       \
                    NULL,             \
                    NULL,             \
                    NULL,             \
                    NULL,              \
                    &(ICU.IR[IR_MTU2_TGIA2].BYTE), \
                    &(ICU.IER[IER_MTU2_TGIA2].BYTE), \
                    &(ICU.IER[IER_MTU2_TGIB2].BYTE), \
                    NULL, \
                    NULL, \
                    &(ICU.IPR[IPR_MTU2_TGIA2].BYTE)}

#define MTU3_REGS { &MTU3.TCR.BYTE,   \
                    &MTU3.TMDR.BYTE,  \
                    &MTU3.TIORH.BYTE, \
                    &MTU3.TIORL.BYTE, \
                    &MTU3.TIER.BYTE,  \
                    &MTU3.TBTM.BYTE,  \
                    &MTU3.NFCR.BYTE,  \
                    &MTU3.TCNT,       \
                    &MTU3.TGRA,       \
                    &MTU3.TGRB,       \
                    &MTU3.TGRC,       \
                    &MTU3.TGRD,       \
                    NULL,             \
                    NULL,              \
                    &(ICU.IR[IR_MTU3_TGIA3].BYTE), \
                    &(ICU.IER[IER_MTU3_TGIA3].BYTE), \
                    &(ICU.IER[IER_MTU3_TGIB3].BYTE), \
                    &(ICU.IER[IER_MTU3_TGIC3].BYTE), \
                    &(ICU.IER[IER_MTU3_TGID3].BYTE), \
                    &(ICU.IPR[IPR_MTU3_TGIA3].BYTE)}

#define MTU4_REGS { &MTU4.TCR.BYTE,   \
                    &MTU4.TMDR.BYTE,  \
                    &MTU4.TIORH.BYTE, \
                    &MTU4.TIORL.BYTE, \
                    &MTU4.TIER.BYTE,  \
                    &MTU4.TBTM.BYTE,  \
                    &MTU4.NFCR.BYTE,  \
                    &MTU4.TCNT,       \
                    &MTU4.TGRA,       \
                    &MTU4.TGRB,       \
                    &MTU4.TGRC,       \
                    &MTU4.TGRD,       \
                    NULL,             \
                    NULL,              \
                    &(ICU.IR[IR_MTU4_TGIA4].BYTE), \
                    &(ICU.IER[IER_MTU4_TGIA4].BYTE), \
                    &(ICU.IER[IER_MTU4_TGIB4].BYTE), \
                    &(ICU.IER[IER_MTU4_TGIC4].BYTE), \
                    &(ICU.IER[IER_MTU4_TGID4].BYTE), \
                    &(ICU.IPR[IPR_MTU4_TGIA4].BYTE)}

#if defined BSP_MCU_RX63N
/* ICU.IER.IEN  bits for each TGI source.   0xFF = not available this channel.      */
/*                  { TGIA      TGIB      TGIC          TGID         }  */
#define MTU0_TGI_EN { MTU_BIT6, MTU_BIT7, MTU_BIT0,     MTU_BIT1     }
#define MTU1_TGI_EN { MTU_BIT4, MTU_BIT5, MTU_NOT_SUPP, MTU_NOT_SUPP }
#define MTU2_TGI_EN { MTU_BIT6, MTU_BIT7, MTU_NOT_SUPP, MTU_NOT_SUPP }
#define MTU3_TGI_EN { MTU_BIT0, MTU_BIT1, MTU_BIT2,     MTU_BIT3     }
#define MTU4_TGI_EN { MTU_BIT4, MTU_BIT5, MTU_BIT6,     MTU_BIT7     }

#elif defined BSP_MCU_RX111 || defined BSP_MCU_RX110 || defined BSP_MCU_RX210
/* ICU.IER.IEN  bits for each TGI source.   0xFF = not available this channel.      */
/*                  { TGIA      TGIB      TGIC          TGID         }  */
#define MTU0_TGI_EN { MTU_BIT2, MTU_BIT3, MTU_BIT4,     MTU_BIT5     }
#define MTU1_TGI_EN { MTU_BIT1, MTU_BIT2, MTU_NOT_SUPP, MTU_NOT_SUPP }
#define MTU2_TGI_EN { MTU_BIT5, MTU_BIT6, MTU_NOT_SUPP, MTU_NOT_SUPP }
#define MTU3_TGI_EN { MTU_BIT1, MTU_BIT2, MTU_BIT3,     MTU_BIT4     }
#define MTU4_TGI_EN { MTU_BIT6, MTU_BIT7, MTU_BIT0,     MTU_BIT1     }
#endif

/* TCR.TPSC[2:0] bits for internal clock selection. From Tables.  0xFF = not available this channel. */
/* PCLK divisor        { PCLK/1 | PCLK/4 | PCLK/16 | PCLK/64 | PCLK/256 | PCLK/1024 }                */
#define MTU0_PCLK_DIVS {  0x00,    0x01,     0x02,     0x03,     0xFF,      0xFF    }
#define MTU1_PCLK_DIVS {  0x00,    0x01,     0x02,     0x03,     0x06,      0xFF    }
#define MTU2_PCLK_DIVS {  0x00,    0x01,     0x02,     0x03,     0xFF,      0x07    }
#define MTU3_PCLK_DIVS {  0x00,    0x01,     0x02,     0x03,     0x04,      0x05    }
#define MTU4_PCLK_DIVS {  0x00,    0x01,     0x02,     0x03,     0x04,      0x05    }

/* TCR.TPSC[2:0]  bits for external clock selection. From Tables.   0xFF = not available this channel.               */
/*                    { MTCLKA MTCLKB MTCLKC MTCLKD OFL_UFL }  */
#define MTU0_EXT_CLKS { 0x04,  0x05,  0x06,  0x07,  0xFF    }
#define MTU1_EXT_CLKS { 0x04,  0x05,  0xFF,  0xFF,  0x07    }
#define MTU2_EXT_CLKS { 0x04,  0x05,  0x06,  0xFF,  0xFF    }
#define MTU3_EXT_CLKS { 0x06,  0x07,  0xFF,  0xFF,  0xFF    }
#define MTU4_EXT_CLKS { 0x06,  0x07,  0xFF,  0xFF,  0xFF    }

/* TCR.CCLR[2:0] bits for counter clearing selection. From Tables.   0xFF = not available this channel.     */
/* TCNT clear source { TGRA  TGRB  TGRC  TGRD  Sync  disabled}  */
#define MTU0_CLR_SRC { 0x20, 0x40, 0xA0, 0xC0, 0x60,  0x00 }
#define MTU1_CLR_SRC { 0x20, 0x40, 0xFF, 0xFF, 0x60,  0x00 }
#define MTU2_CLR_SRC { 0x20, 0x40, 0xFF, 0xFF, 0x60,  0x00 }
#define MTU3_CLR_SRC { 0x20, 0x40, 0xA0, 0xC0, 0x60,  0x00 }
#define MTU4_CLR_SRC { 0x20, 0x40, 0xA0, 0xC0, 0x60,  0x00 }

#define MTU_MAX_TIMER_TICKS (0xFFFF)
#define MTU_NUM_CLK_DIVISORS (6)
#define MTU_NUM_EXT_CLK_SRCS (5)
#define MTU_NUM_CLR_SRCS (8)
#define MTU_NOT_SUPP (0xFF)
#define MTU_NUM_TGIS (4)

#define MTU_ADC_TRIG (0x80)  // TIER.TTGE bit

#define MTU_TSYR_MASK (0xC7) // Protect reserved TSYR bits.
#define MTU_TSTR_MASK (0xC7) // Protect reserved TSTR bits.

#define    MTU_CLR_TGRA (0)
#define    MTU_CLR_TGRB (1)
#define    MTU_CLR_TGRC (2)
#define    MTU_CLR_TGRD (3)

#define MTU_POWER_ON  (0)
#define MTU_POWER_OFF (1)

#define MTU_MODE_CLOSED        (0)
#define MTU_MODE_COMPARE_MATCH (1)
#define MTU_MODE_INPUT_CAPTURE (2)
#define MTU_MODE_PWM_MODE_1 (3)
#define MTU_MODE_PWM_MODE_2 (2)

/***********************************************************************************************************************
Pre-declaration of private global variables
***********************************************************************************************************************/
/* Possible input clock divisors for internal clocking. Not all channels support all divisors. */
extern const uint32_t g_mtu_clock_divisors[MTU_NUM_CLK_DIVISORS];
extern volatile uint8_t g_num_channels_in_use;  // Flag to tell whether any channel of the mtu is currently in use.
extern uint8_t g_mtu_channel_mode[MTU_CHANNEL_MAX]; // Channel mode or is available (0).
extern uint8_t g_mtu_channel_clr_src[MTU_CHANNEL_MAX]; // The selected timer clearing source.
extern uint8_t g_mtu_channel_repeats[MTU_CHANNEL_MAX]; // Flags for do repeat or just once.
extern uint8_t g_mtu_tgr_callbacks[MTU_CHANNEL_MAX][MTU_NUM_TIMERS]; // Flags for do callback or not
extern uint8_t g_mtu_tgi_icu_en_flags[MTU_CHANNEL_MAX][MTU_NUM_TGIS];

/*  table of available internal clock divisors for each channel, and their setting bits. */
extern const uint8_t g_chnl_clk_divs[MTU_CHANNEL_MAX][MTU_NUM_CLK_DIVISORS];
/*  table of available external clock source for each channel, and their setting bits. */
extern const uint8_t g_chnl_ext_clks[MTU_CHANNEL_MAX][MTU_NUM_EXT_CLK_SRCS];
/*  table of available counter clearing sources for each channel, and their setting bits. */
extern const uint8_t g_chnl_clear_src[MTU_CHANNEL_MAX][MTU_NUM_CLR_SRCS];
/*  table of channel start register bits for each channel. */
extern const uint8_t g_mtu_tstr_bits[];

extern const mtu_handle_t g_mtu_handles[];

/***********************************************************************************************************************
Pre-declaration of private local functions
***********************************************************************************************************************/
extern void mtu_interrupts_enable(uint8_t channel);
extern void mtu_interrupts_disable(uint8_t channel);
extern void mtu_channel_clear(uint8_t channel);
extern void mtu_interrupts_clear(uint8_t channel);
extern void mtu_interrupts_group_enable(uint8_t group);
extern void mtu_interrupts_group_disable(uint8_t group);
extern bool mtu_check_group(uint8_t group);
extern void power_on_off (uint8_t on_or_off);
extern bool mtu_calc_clock_divisor(uint8_t chan, uint8_t *div_idx, uint32_t frq_target);
extern uint16_t mtu_calc_tgr_ticks(uint16_t pclk_div, uint32_t frq_target );

#endif /* R_MTU_PWM_PRIVATE_H_ */
