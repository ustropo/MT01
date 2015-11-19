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
* File Name    : r_mtu_rx_common.c
* Device(s)    : RX Family
* Tool-Chain   : Renesas RX Standard Toolchain 1.02+
* OS           : None
* H/W Platform :
* Description  : Shared routines used by the the FIT MTU2a modules on RX devices.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.09.2014  1.00   First Release
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"
#include "r_mtu_rx_if.h"
/* Internal definitions. */
#include "r_mtu_rx_private.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/
/* Possible input clock divisors for internal clocking. Not all channels support all divisors. */
const uint32_t g_mtu_clock_divisors[MTU_NUM_CLK_DIVISORS] = { 1, 4, 16, 64, 256, 1024 };
volatile uint8_t g_num_channels_in_use = 0;  // Flag to tell whether any channel of the mtu is currently in use.
uint8_t g_mtu_channel_mode[MTU_CHANNEL_MAX] = {MTU_MODE_CLOSED}; // Channel mode or is available (0).
uint8_t g_mtu_channel_clr_src[MTU_CHANNEL_MAX] = {0}; // The selected timer clearing source.
uint8_t g_mtu_channel_repeats[MTU_CHANNEL_MAX] = {0}; // Flags for do repeat or just once.
uint8_t g_mtu_tgr_callbacks[MTU_CHANNEL_MAX][MTU_NUM_TIMERS]; // Flags for do callback or not
uint8_t g_mtu_tgi_icu_en_flags[MTU_CHANNEL_MAX][MTU_NUM_TGIS] = {0};
mtu_callback_data_t  g_mtu_cb_data[MTU_CHANNEL_MAX]; // Allocate callback data storage for all channels.

/* Create table of available internal clock divisors for each channel, and their setting bits. */
const uint8_t g_chnl_clk_divs[MTU_CHANNEL_MAX][MTU_NUM_CLK_DIVISORS] =
{
    MTU0_PCLK_DIVS,
    MTU1_PCLK_DIVS,
    MTU2_PCLK_DIVS,
#ifndef BSP_MCU_RX110
    MTU3_PCLK_DIVS,
    MTU4_PCLK_DIVS
#endif
};

/* Create table of available external clock source for each channel, and their setting bits. */
const uint8_t g_chnl_ext_clks[MTU_CHANNEL_MAX][MTU_NUM_EXT_CLK_SRCS] =
{
    MTU0_EXT_CLKS,
    MTU1_EXT_CLKS,
    MTU2_EXT_CLKS,
#ifndef BSP_MCU_RX110
    MTU3_EXT_CLKS,
    MTU4_EXT_CLKS
#endif
};

/* Create table of available counter clearing sources for each channel, and their setting bits. */
const uint8_t g_chnl_clear_src[MTU_CHANNEL_MAX][MTU_NUM_CLR_SRCS] =
{
    MTU0_CLR_SRC,
    MTU1_CLR_SRC,
    MTU2_CLR_SRC,
#ifndef BSP_MCU_RX110
    MTU3_CLR_SRC,
    MTU4_CLR_SRC
#endif
};

/* Create table of TGI ICU interrupt enable bit masks for each channel. */
const uint8_t g_mtu_tgi_icu_en_masks[MTU_CHANNEL_MAX][MTU_NUM_TGIS] =
{
    MTU0_TGI_EN,
    MTU1_TGI_EN,
    MTU2_TGI_EN,
#ifndef BSP_MCU_RX110
    MTU3_TGI_EN,
    MTU4_TGI_EN
#endif
};

/* Create table of channel start register bits for each channel. */
const uint8_t g_mtu_tstr_bits[] =
{
    (uint8_t)MTU_TSTR_CH0,
    (uint8_t)MTU_TSTR_CH1,
    (uint8_t)MTU_TSTR_CH2,
#ifndef BSP_MCU_RX110
    (uint8_t)MTU_TSTR_CH3,
    (uint8_t)MTU_TSTR_CH4
#endif
};

/* Create table of channel start register bits for each channel. */
const uint8_t g_mtu_group_bits[] =
{
     (uint8_t)MTU_GRP_CH0,
     (uint8_t)MTU_GRP_CH1,
     (uint8_t)MTU_GRP_CH2,
#ifndef BSP_MCU_RX110
     (uint8_t)MTU_GRP_CH3,
     (uint8_t)MTU_GRP_CH4,
#endif
};

/* Static control structures for each channel */
#if MTU_CFG_USE_CH0 == 1
mtu_callback p_callback_mtu0;
mtu_timer_chnl_settings_t mtu0_tmr_settings;
const mtu_config_block_t g_mtu0_handle =
{
    &p_callback_mtu0,       // pointer to callback function pointer.
    &mtu0_tmr_settings,     // location of variable timer settings struct.
    MTU0_REGS,              // Pointers to registers for this channel
    0,                      // MTU channel number.
    MTU_IR_PRIORITY_CHAN0,  // Interrupt priority level.
    MTU_CFG_FILT_CHAN0      // Input capture filter clock setting.
};
#endif

#if MTU_CFG_USE_CH1 == 1
mtu_callback p_callback_mtu1;
mtu_timer_chnl_settings_t mtu1_tmr_settings;
const mtu_config_block_t g_mtu1_handle =
{
    &p_callback_mtu1,       // pointer to callback function pointer.
    &mtu1_tmr_settings,     // location of variable timer settings struct.
    MTU1_REGS,              // Pointers to registers for this channel
    1,                      // MTU channel number.
    MTU_IR_PRIORITY_CHAN1,  // Interrupt priority level.
    MTU_CFG_FILT_CHAN1      // Input capture filter clock setting.
};
#endif

#if MTU_CFG_USE_CH2 == 1
mtu_callback p_callback_mtu2;
mtu_timer_chnl_settings_t mtu2_tmr_settings;
const mtu_config_block_t g_mtu2_handle =
{
    &p_callback_mtu2,       // pointer to callback function pointer.
    &mtu2_tmr_settings,     // location of variable timer settings struct.
    MTU2_REGS,              // Pointers to registers for this channel
    2,                      // MTU channel number.
    MTU_IR_PRIORITY_CHAN2,  // Interrupt priority level.
    MTU_CFG_FILT_CHAN2      // Input capture filter clock setting.
};
#endif

#if MTU_CFG_USE_CH3 == 1
mtu_callback p_callback_mtu3;
mtu_timer_chnl_settings_t mtu3_tmr_settings;
const mtu_config_block_t g_mtu3_handle =
{
    &p_callback_mtu3,       // pointer to callback function pointer.
    &mtu3_tmr_settings,     // location of variable timer settings struct.
    MTU3_REGS,              // Pointers to registers for this channel
    3,                      // MTU channel number.
    MTU_IR_PRIORITY_CHAN3,  // Interrupt priority level.
    MTU_CFG_FILT_CHAN3      // Input capture filter clock setting.
};
#endif

#if MTU_CFG_USE_CH4 == 1
mtu_callback p_callback_mtu4;
mtu_timer_chnl_settings_t mtu4_tmr_settings;
const mtu_config_block_t g_mtu4_handle =
{
    &p_callback_mtu4,       // pointer to callback function pointer.
    &mtu4_tmr_settings,     // location of variable timer settings struct.
    MTU4_REGS,              // Pointers to registers for this channel
    4,                      // MTU channel number.
    MTU_IR_PRIORITY_CHAN4,  // Interrupt priority level.
    MTU_CFG_FILT_CHAN4      // Input capture filter clock setting.
};
#endif

const mtu_handle_t g_mtu_handles[] =
{
    #if MTU_CFG_USE_CH0 == 1
    (mtu_handle_t) &g_mtu0_handle,
    #else
    NULL,
    #endif
    #if MTU_CFG_USE_CH1 == 1
    (mtu_handle_t) &g_mtu1_handle,
    #else
    NULL,
    #endif
    #if MTU_CFG_USE_CH2 == 1
    (mtu_handle_t) &g_mtu2_handle,
    #else
    NULL,
    #endif
    #if MTU_CFG_USE_CH3 == 1
    (mtu_handle_t) &g_mtu3_handle,
    #else
    NULL,
    #endif
    #if MTU_CFG_USE_CH4 == 1
    (mtu_handle_t) &g_mtu4_handle,
    #else
    NULL,
    #endif

};

/***********************************************************************************************************************
Private local function declarations
***********************************************************************************************************************/
void mtu_interrupts_enable(uint8_t channel);
void mtu_interrupts_disable(uint8_t channel);
void mtu_interrupts_clear(uint8_t channel);
uint8_t mtu_interrupts_check(uint8_t channel);
void mtu_interrupts_group_enable(uint8_t group);
void mtu_interrupts_group_disable(uint8_t group);
bool mtu_check_group(uint8_t group);
void power_on_off (uint8_t on_or_off);
bool mtu_calc_clock_divisor(uint8_t chan, uint8_t *div_idx, uint32_t frq_target);
uint16_t mtu_calc_tgr_ticks(uint16_t pclk_div, uint32_t frq_target );
static void mtu_isr_common(mtu_channel_t channel, mtu_timer_num_t tgr);


/***********************************************************************************************************************
* Function Name: R_MTU_GetVersion
* Description  : Returns the version of this module.
*                The version number is encoded where the top 2 bytes are the major version number and the bottom 2 bytes
*                are the minor version number. For example, Rev 4.25 would be 0x00040019.
*                Version number is defined in "r_mtu_timer_rx_private.h"
* Arguments    : none
* Return Value : Version Number
*  NOTE: This function is inlined using #pragma inline directive.
***********************************************************************************************************************/
#pragma inline(R_MTU_GetVersion)
uint32_t R_MTU_GetVersion(void)
{
    uint32_t version_number = 0;
    /* Bring in major version number. */
    version_number = ((uint16_t)MTU_RX_VERSION_MAJOR) << 16;
    /* Bring in minor version number. */
    version_number |= (uint16_t)MTU_RX_VERSION_MINOR;
    return version_number;
}

/***********************************************************************************************************************
* Function Name: R_MTU_Control
* Description  : This function is responsible for handling special hardware or software operations for the MTU channel.
* Arguments    : channel-
*                   The channel number
*                cmd
*                   Enumerated command code
*                pcmd_data
*                   Pointer to the command-data structure parameter of type void that is used to reference the location
*                   of any data specific to the command that is needed for its completion.
* Return Value : MTU_SUCCESS-
*                   Command successfully completed.
*                MTU_TIMERS_ERR_CH_NOT_OPEN-
*                   The channel has not been opened.  Perform R_MTU_Open() first
*                MTU_TIMERS_ERR_BAD_CHAN-
*                   Channel number is invalid for part
*                MTU_TIMERS_ERR_UNKNOWN_CMD-
*                   Control command is not recognized.
*                MTU_TIMERS_ERR_NULL_PTR-
*                   pcmd_data  pointer or handle is NULL
*                MTU_TIMERS_ERR_INVALID_ARG-
*                   An element of the pcmd_data structure contains an invalid value.
*                MTU_TIMERS_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
mtu_err_t  R_MTU_Control (mtu_channel_t channel,
                          mtu_cmd_t     cmd,
                          void          *pcmd_data)
{
    mtu_capture_status_t * p_capture_data;
    mtu_timer_status_t   * p_timer_data;
    mtu_group_t    * p_group_data;
    mtu_capture_set_edge_t  * p_cap_edge_data;
    uint8_t temp_byte;

    /* Command function data structure definitions. One for each command in mtu_timer_cmd_t. */
    #if MTU_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    mtu_handle_t  my_handle;
    mtu_timer_chnl_settings_t *pconfig; // Store a pointer to the user's config structure.

    #if MTU_CFG_PARAM_CHECKING_ENABLE == 1
    if (MTU_CHANNEL_MAX <= channel)   // First check for channel number out of range
    {
        return MTU_ERR_BAD_CHAN;
    }

    switch(cmd) /* Check for valid command and data. */
    {
        case MTU_CMD_START:
        case MTU_CMD_STOP:
        case MTU_CMD_SAFE_STOP:
        case MTU_CMD_RESTART:
        case MTU_CMD_CLEAR_EVENTS:
        break;

        case MTU_CMD_GET_STATUS:
        case MTU_CMD_SYNCHRONIZE:
        case MTU_CMD_SET_CAPT_EDGE:
            if ((NULL == pcmd_data) || (FIT_NO_PTR == pcmd_data))
            {
                return MTU_ERR_NULL_PTR;
            }
        break;

        default:
        {
           /* Error, command not recognized. */
           return MTU_ERR_UNKNOWN_CMD;
        }
    }

    if (!g_mtu_channel_mode[channel])
    {
        return MTU_ERR_CH_NOT_OPENED;
    }
    #endif

    #if MTU_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this MTU channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));

    if(false == lock_result)
    {
        return MTU_ERR_LOCK; /* The control function is currently locked. */
    }
    #endif

    my_handle = g_mtu_handles[channel];

    pconfig = my_handle->p_mtu_chnl_tmr_settings;

    switch(cmd)
    {
        case MTU_CMD_START:       // Activate clocking
        {
            if ((NULL != pcmd_data) && (FIT_NO_PTR != pcmd_data)) // A channel group specifier was provided
            {
                p_group_data =  (mtu_group_t *)pcmd_data;

                temp_byte = (uint8_t) *p_group_data;

                if(!mtu_check_group(temp_byte) || (temp_byte & ~MTU_TSTR_MASK)) // Error in group parameter.
                {
                    #if MTU_CFG_REQUIRE_LOCK == 1
                    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                    #endif
                    return MTU_ERR_INVALID_ARG;
                }

                mtu_interrupts_group_enable(temp_byte);
                MTU.TSTR.BYTE = temp_byte;  //Set the start bits for the group.
            }
            else    // Just this channel.
            {
                mtu_interrupts_enable(channel);
                MTU.TSTR.BYTE |= g_mtu_tstr_bits[channel];
            }
        }
        break;

        case MTU_CMD_STOP:       // Pause clocking
        {
            if ((NULL != pcmd_data) && (FIT_NO_PTR != pcmd_data)) // A channel group specifier was provided
            {
                p_group_data =  (mtu_group_t *)pcmd_data;

                temp_byte = (uint8_t) *p_group_data;
                temp_byte &= MTU_TSTR_MASK; // Protect reserved TSTR bits.

                if(!mtu_check_group(temp_byte) || (temp_byte & ~MTU_TSTR_MASK)) // Error in group parameter.
                {
                    #if MTU_CFG_REQUIRE_LOCK == 1
                    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                    #endif
                    return MTU_ERR_INVALID_ARG;
                }

                mtu_interrupts_group_disable(temp_byte);
                MTU.TSTR.BYTE &= (~temp_byte);  // clear the start bits for this group.
            }
            else    // Just this channel.
            {
                mtu_interrupts_disable(channel);
                MTU.TSTR.BYTE &= (uint8_t)(~(g_mtu_tstr_bits[channel]));
            }
        }
        break;

        case MTU_CMD_SAFE_STOP:   // Stop clocking and set outputs to safe state
        {
            // First stop the clocking.
            MTU.TSTR.BYTE &= (uint8_t)(~(g_mtu_tstr_bits[channel]));
            // Now re-write the TIOR register to revert to 'initial' MTIOC output state.
            if((0 != pconfig->timer_a.freq) && (MTU_ACTION_OUTPUT & pconfig->timer_a.actions.do_action))
            {
                *my_handle->regs.tiorh |= pconfig->timer_a.actions.output;  // Set bits in lower nibble
            }
            if((0 != pconfig->timer_b.freq) && (MTU_ACTION_OUTPUT & pconfig->timer_b.actions.do_action))
            {
                *my_handle->regs.tiorh |=  (pconfig->timer_b.actions.output << 4); // Move bits to upper nibble
            }
            if((0 != pconfig->timer_c.freq) && (MTU_ACTION_OUTPUT & pconfig->timer_c.actions.do_action))
            {
                *my_handle->regs.tiorl |= pconfig->timer_c.actions.output;  // Set bits in lower nibble
            }
            if((0 != pconfig->timer_d.freq) && (MTU_ACTION_OUTPUT & pconfig->timer_d.actions.do_action))
            {
                *my_handle->regs.tiorl |=  (pconfig->timer_d.actions.output << 4); // Move bits to upper nibble
            }

        }
        break;

        case MTU_CMD_RESTART:      // Zero the counter then resume clocking
        {
            *my_handle->regs.tcnt = 0;                 // Clear the counter TCNT register.
            mtu_interrupts_enable(channel);
            MTU.TSTR.BYTE |= g_mtu_tstr_bits[channel]; // Start counting.
        }
        break;

        case MTU_CMD_GET_STATUS:   // Retrieve the current status of the channel
        {
            if (MTU_MODE_COMPARE_MATCH == g_mtu_channel_mode[channel])
            {
                /* Copy void pcmd_data pointer over to a concrete type. */
                p_timer_data =  (mtu_timer_status_t *)pcmd_data;
                /* Return timer status to application */
                p_timer_data->timer_running = (bool)(MTU.TSTR.BYTE & g_mtu_tstr_bits[channel]); // Running status
                p_timer_data->timer_count = *my_handle->regs.tcnt;    // The current timer count value.
            }
            else if  (MTU_MODE_INPUT_CAPTURE == g_mtu_channel_mode[channel])
            {
                /* Cast void pcmd_data pointer to a concrete type. */
                p_capture_data =  (mtu_capture_status_t *)pcmd_data;

                /* Return a snapshot of TGR capture interrupts that have fired. */
                p_capture_data->capture_flags = mtu_interrupts_check(channel);

                p_capture_data->timer_count = *my_handle->regs.tcnt;    // The current timer count value.

                /* Grab the TGR register values. */
                p_capture_data->capt_a_count = *my_handle->regs.tgra;
                p_capture_data->capt_b_count = *my_handle->regs.tgrb;
                /* Not all channels have TGRC and TGRD */
                if (NULL != my_handle->regs.tgrc)
                {
                    p_capture_data->capt_c_count = *my_handle->regs.tgrc;
                }
                else
                {
                    p_capture_data->capt_c_count = 0;
                }
                if (NULL != my_handle->regs.tgrd)
                {
                    p_capture_data->capt_d_count = *my_handle->regs.tgrd;
                }
                else
                {
                    p_capture_data->capt_d_count = 0;
                }
            }
        }
        break;

        case MTU_CMD_CLEAR_EVENTS:      // Clears the interrupt flags for the channel
        {
            mtu_interrupts_clear(channel);
        }
        break;

        case MTU_CMD_SET_CAPT_EDGE:      // Set the detection edge polarity for input capture.
        {
            if  (MTU_MODE_INPUT_CAPTURE == g_mtu_channel_mode[channel])
            {
                 /* Cast void pcmd_data pointer to a concrete type. */
                p_cap_edge_data =  (mtu_capture_set_edge_t *)pcmd_data;

                if ((MTU_CHANNEL_1 == channel) || (MTU_CHANNEL_2 == channel))
                {
                    if((MTU_CAP_SRC_C == p_cap_edge_data->capture_src) || (MTU_CAP_SRC_D == p_cap_edge_data->capture_src))
                    {
                        #if MTU_CFG_REQUIRE_LOCK == 1
                        R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                        #endif
                        return MTU_ERR_INVALID_ARG; // Resource not present on these channels.
                    }
                }
                switch (p_cap_edge_data->capture_src)
                {
                    case MTU_CAP_SRC_A:
                    {
                        *my_handle->regs.tiorl &= 0xF0;                           // First clear the lower nibble.
                        *my_handle->regs.tiorh |= p_cap_edge_data->capture_edge;  // Set bits in lower nibble
                    }
                    break;
                    case MTU_CAP_SRC_B:
                    {
                        *my_handle->regs.tiorl &= 0x0F;                                  // First clear the upper nibble.
                        *my_handle->regs.tiorh |=  (p_cap_edge_data->capture_edge << 4); // Move bits to upper nibble
                    }
                    break;
                    case MTU_CAP_SRC_C:
                    {
                        *my_handle->regs.tiorl &= 0xF0;                           // First clear the lower nibble.
                        *my_handle->regs.tiorl |= p_cap_edge_data->capture_edge;  // Set bits in lower nibble
                    }
                    break;
                    case MTU_CAP_SRC_D:
                    {
                        *my_handle->regs.tiorl &= 0x0F;                                   // First clear the upper nibble.
                        *my_handle->regs.tiorl |= (p_cap_edge_data->capture_edge << 4);   // Move bits to upper nibble
                    }
                    break;
                }
            }
            else    // Command not valid for this mode.
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_INVALID_ARG;
            }

        }
        break;

        case MTU_CMD_SYNCHRONIZE:
        {
            /* Copy void pcmd_data pointer over to a concrete type. */
            p_group_data =  (mtu_group_t *)pcmd_data;

            temp_byte = (uint8_t) *p_group_data;
            temp_byte &= MTU_TSYR_MASK; // Protect reserved TSYR bits.

            if(!mtu_check_group(temp_byte))
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_INVALID_ARG;
            }

            MTU.TSYR.BYTE = temp_byte;  //Set the SYNCn 0-4 bits.
        }
        break;

        default:
        {
            //Nothing -- unreachable.
        }
    }

    #if MTU_CFG_REQUIRE_LOCK == 1
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
    #endif

    return MTU_SUCCESS;
}
/* end of function R_MTU_Control(). */


/***********************************************************************************************************************
* Function Name: R_MTU_Close
* Description  : Removes power to the MTU channel designated by the handle and disables the associated interrupts.
* Arguments    : : channel-
*                   The channel number
* Return Value : MTU_SUCCESS-
*                    Successful; channel closed
*                MTU_TIMERS_ERR_CH_NOT_OPEN-
*                    The channel has not been opened so closing has no effect.
*                MTU_TIMERS_ERR_BAD_CHAN-
*                    Channel number is invalid for part
***********************************************************************************************************************/
mtu_err_t  R_MTU_Close(mtu_channel_t  channel)
{
    #if MTU_CFG_PARAM_CHECKING_ENABLE == 1
    if (MTU_CHANNEL_MAX <= channel)           // First check for channel number out of range
    {
        return MTU_ERR_BAD_CHAN;
    }
    #endif

    /* Check to see if the channel is currently initialized. */
    if (!g_mtu_channel_mode[channel])
    {
        /* This channel is not open so need not be closed. */
        return MTU_ERR_CH_NOT_OPENED;
    }

    /* Stop any current operation. */
    R_MTU_Control((mtu_channel_t)channel, MTU_CMD_STOP, FIT_NO_PTR);

    mtu_interrupts_disable(channel);

    g_mtu_channel_mode[channel] = MTU_MODE_CLOSED;
    g_num_channels_in_use--;

    /* Shut down the MTU unit if this was the last channel in use. */
    if (0 == g_num_channels_in_use)
    {
            power_on_off(MTU_POWER_OFF);
    }

    /* Clear all control flags arrays for this channel. */
    g_mtu_channel_repeats[channel] = 0;
    g_mtu_channel_clr_src[channel] = 0;

    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_A] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_B] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_C] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_D] = 0;

    g_mtu_tgr_callbacks[channel][MTU_TIMER_A] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_B] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_C] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_D] = 0;

    return MTU_SUCCESS;
}
/* end of function R_MTU_Close(). */

/***********************************************************************************************************************
Private MTU function definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
* Function Name: mtu_calc_input_clocks
* Description  : local helper function. Figures out the register settings for clocking divisor and TGR count.
* Arguments    : frq_target -
*                   The requested frequency.
* Return Value : none
***********************************************************************************************************************/
bool mtu_calc_clock_divisor(uint8_t chan, uint8_t *div_idx, uint32_t frq_target )
{
    uint32_t i;
    bool result = false;
    uint16_t pclk_div;
    uint16_t tgr_try;

    if ((uint32_t)BSP_PCLKB_HZ >= frq_target) /* Requested frequency must not be higher than PCLK. */
    {
        /* Check the available dividers to see if we can match the frequency requested by the user. */
        for (i = 0; i < MTU_NUM_CLK_DIVISORS; i++)
        {
            /* First check to see if this channel supports this divisor. */
            if (0xFF != g_chnl_clk_divs[chan][i]) // 0xFF means not available.
            {
                pclk_div = g_mtu_clock_divisors[i];

                /* Determine minimum frequency this divider can hit. For example, if a PCLK/16 is used and PCLK is 48MHz, then
                   the minimum frequency we can support is around 45.8Hz. This obtained by doing the following calculation:
                   (PCLK / divider) / max_counter_value
                   Example:
                   (48,000,000 / 16) / 65,535 = 45.8 */

                tgr_try = mtu_calc_tgr_ticks(pclk_div, frq_target);

                if (tgr_try != 0)
                {
                    /* We can use this divisor. Return clock divisor to be used. */
                    *div_idx = i;

                    /* A valid divisor was found. */
                    result = true;

                    /* No need to check other divisors. */
                    break;
                }
            } // 0xFF
        } // for
    }

    return result;
}
/* end of function mtu_calc_clock_divisor(). */

/***********************************************************************************************************************
* Function Name: mtu_calc_tgr_ticks
* Description  : local helper function. Calculate the TGR tick count based on frequency target and fixed PCLK divisor.
* Arguments    : frq_target -
*                   The requested freq.
* Return Value : the TGR value on success, 0 on failure.
***********************************************************************************************************************/
uint16_t mtu_calc_tgr_ticks(uint16_t pclk_div, uint32_t frq_target )
{
    uint16_t result = 0;
    uint32_t clock_freq;

    clock_freq = BSP_PCLKB_HZ / pclk_div; /* Pre-scale the clock */

    if ((frq_target > (clock_freq / MTU_MAX_TIMER_TICKS)) && (frq_target < clock_freq))
    {
        /* Figure out counter ticks needed for this frequency, and return it */
        result = (uint16_t)((clock_freq/frq_target) -1 );
    }

    return result;
}
/* end of function mtu_calc_tgr_ticks(). */

/***********************************************************************************************************************
* Function Name: mtu_channel_clear
* Description  : Clears the registers and state variables of the given channel.
* Arguments    : channel -
*                 Which channel to clear.
* Return Value : none
***********************************************************************************************************************/
void mtu_channel_clear(uint8_t channel)
{
    mtu_handle_t  my_handle = g_mtu_handles[channel];

    /* Clear the software control flags */
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_A] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_B] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_C] = 0;
    g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_D] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_A] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_B] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_C] = 0;
    g_mtu_tgr_callbacks[channel][MTU_TIMER_D] = 0;

    /* Clear all relevant MTU registers. */
    *my_handle->regs.tmdr = 0x00;       // Clear the mode register.
    *my_handle->regs.tiorh = 0x00;      // Clear TIORs to disable outputs.

    if (NULL != my_handle->regs.tiorl)
    {
        *my_handle->regs.tiorl = 0x00;  // Some channels do not have this.
    }

    *my_handle->regs.tier = 0x00;
    *my_handle->regs.tcnt = 0x00;       // Clear the count in the TCNT register.
    *my_handle->regs.tgra = 0x00;       // Clear TGRs.
    *my_handle->regs.tgrb = 0x00;

    if (NULL != my_handle->regs.tgrc)
    {
        *my_handle->regs.tgrc = 0x00;   // Some channels do not have this.
    }

    if (NULL != my_handle->regs.tgrd)
    {
        *my_handle->regs.tgrd = 0x00;   // Some channels do not have this.
    }
}
/* End of function mtu_channel_clear(). */

/***********************************************************************************************************************
* Function Name: power_on_off
* Description  : Switches power to an MTU channel.  Required by FIT spec.
* Arguments    : channel -
*                   Which channel to use.
*                on_or_off -
*                   What it says.
* Return Value : none
***********************************************************************************************************************/
void power_on_off (uint8_t on_or_off)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

    MSTP(MTU) = on_or_off; // All channels are on the same module stop register.

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
}
/* End of function power_on(). */

/***********************************************************************************************************************
* Function Name: mtu_interrupts_clear
* Description  : Clears all pending MTU interrupts for the given channel.
* Arguments    : channel -
*                 Which channel to clear.
* Return Value : none
***********************************************************************************************************************/
void mtu_interrupts_clear(uint8_t channel)
{
    mtu_handle_t       my_handle = g_mtu_handles[channel];
    uint8_t i;

    for (i = 0; i < MTU_NUM_TGIS; i++)
    {
        if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][i])
        {
            *(my_handle->regs.ir + i) = 0x00; // Clear the interrupt request bit.
        }
    }
}
/* End of function mtu_interrupts_clear(). */

/***********************************************************************************************************************
* Function Name: MTU_interrupts_check
* Description  : Checks all MTU TGR interrupts for the given channel. Used for polling type operation.
* Arguments    : channel -
*                 Which channel to check.
* Return Value : Zero if all interrupts clear, Bit pattern of any interrupts set.
***********************************************************************************************************************/
uint8_t mtu_interrupts_check(uint8_t channel)
{
    mtu_handle_t       my_handle = g_mtu_handles[channel];
    uint8_t i;
    uint8_t int_flags = 0;

    for (i = 0; i < MTU_NUM_TGIS; i++)
    {
        if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][i])
        {
            if (*(my_handle->regs.ir + i)) // IR flag is set.
            {
                int_flags |= (1 << i); // Set a flag in the bit position corresponding to the TGR number.
            }
        }
    }

    return int_flags;
}
/* End of function mtu_interrupts_clear(). */

/***********************************************************************************************************************
* Function Name: mtu_interrupts_enable
* Description  : Enable all MTU interrupts in ICU for the selected channel that are configured to be used.
* Arguments    : channel -
*                 Which channel to use.
* Return Value : none
***********************************************************************************************************************/
void mtu_interrupts_enable(uint8_t channel)
{
    mtu_handle_t       my_handle = g_mtu_handles[channel];

    if ((MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_A]) && (g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_A]))
    {
        *my_handle->regs.ien_a |= g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_A];     // Set interrupt enable bit in ICU
    }
    if ((MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_B]) && (g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_B]))
    {
        *my_handle->regs.ien_b |= g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_B];
    }
    if ((MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_C]) && (g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_C]))
    {
        *my_handle->regs.ien_c |= g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_C];
    }
    if ((MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_D]) && (g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_D]))
    {
        *my_handle->regs.ien_d |= g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_D];
    }
}
/* End of function mtu_interrupts_enable(). */


/***********************************************************************************************************************
* Function Name: mtu_interrupts_disable
* Description  : Disable all MTU interrupts in ICU for the selected channel.
* Arguments    : channel -
*                 Which channel to use.
* Return Value : none
***********************************************************************************************************************/
void mtu_interrupts_disable(uint8_t channel)
{
    mtu_handle_t       my_handle = g_mtu_handles[channel];

    if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_A])
    {
        *my_handle->regs.ien_a &= ~(g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_A]);  // Clear interrupt enable bit in ICU
    }
    if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_B])
    {
        *my_handle->regs.ien_b &= ~(g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_B]);
    }
    if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_C])
    {
        *my_handle->regs.ien_c &= ~(g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_C]);
    }
    if (MTU_NOT_SUPP != g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_D])
    {
        *my_handle->regs.ien_d &= ~(g_mtu_tgi_icu_en_masks[channel][MTU_TIMER_D]);  // Clear interrupt enable bit in ICU
    }
}
/* End of function mtu_interrupts_disable(). */

/***********************************************************************************************************************
* Function Name: mtu_interrupts_group_enable
* Description  : Enable all MTU interrupts in ICU for the selected channels..
* Arguments    : channel -
*                 Which channel to use.
* Return Value : none
***********************************************************************************************************************/
void mtu_interrupts_group_enable(uint8_t group)
{
    uint8_t i;

    for (i = 0; i < MTU_CHANNEL_MAX; i++)
    {
        if(g_mtu_group_bits[i] & group)
        {
            mtu_interrupts_enable(i);
        }
    }
}
/* End of function mtu_interrupts_group_enable(). */

/***********************************************************************************************************************
* Function Name: mtu_interrupts_group_disable
* Description  : Disable all MTU interrupts in ICU for the selected channels.
* Arguments    : group -
*                 Which channels to disable.
* Return Value : none
***********************************************************************************************************************/
void mtu_interrupts_group_disable(uint8_t group)
{
    uint8_t i;

    for (i = 0; i < MTU_CHANNEL_MAX; i++)
    {
        if(g_mtu_group_bits[i] & group)
        {
            mtu_interrupts_disable(i);
        }
    }
}
/* End of function mtu_interrupts_group_disable(). */

/***********************************************************************************************************************
* Function Name: mtu_check_group
* Description  : Checks all MTU channels in the group for open status.
* Arguments    : group -
*                 Which channels to check.
* Return Value : true if all members of group are open, false if any member closed.
***********************************************************************************************************************/
bool mtu_check_group(uint8_t group)
{
    uint8_t i;

    if (0 == group)
    {
        return false; // Group cannot be empty.
    }

    for (i = 0; i < MTU_CHANNEL_MAX; i++)
    {
        if(g_mtu_group_bits[i] & group)
        {
            if(!g_mtu_channel_mode[i])
            {
                return false; // Error, channel is closed.
            }
        }
    }

    return true;
}
/* End of function mtu_check_group(). */

/***********************************************************************************************************************
* Function Name: mtu_isr_common
* Description  :  the common handler for all TGR interrupts.
*                 Checks whether the mtu repeats the cycle or needs to stop the timer now.
*                 Calls the user defined callback if enabled.
* Arguments : channel -
*                 Which channel to use.
*             tgr -
*                 Which TGR had the interrupt.
* Return Value : none
***********************************************************************************************************************/
void mtu_isr_common(mtu_channel_t channel, mtu_timer_num_t tgr_num)
{
    /* See if we need to stop the timer now. Only the clear source TGR is allowed to control the oneshot mode. */
    if ((0 == g_mtu_channel_repeats[channel]) && (tgr_num == g_mtu_channel_clr_src[channel]))
    {
        R_MTU_Control(channel, MTU_CMD_STOP, FIT_NO_PTR);
    }

    /* Do the callback for this interrupt if it is enabled. */
    if (1 == g_mtu_tgr_callbacks[channel][tgr_num])
    {
        /* Store the event data. */
        g_mtu_cb_data[channel].channel = channel;
        g_mtu_cb_data[channel].timer_num = tgr_num;
        /* Get the TGR value.
        * Contains the counter value at time of event when doing input capture operation.
        * Contains the TGR preset (compare value) when doing compare/match operation. */
        g_mtu_cb_data[channel].count = (uint32_t)(*(g_mtu_handles[channel]->regs.tgra + tgr_num));

        /* Call the User defined callback function. */
        (*(g_mtu_handles[channel]->p_callback))((void*)&(g_mtu_cb_data[channel])); // Pass the event data.
    }
}
/* End of function mtu_isr_common(). */

/***********************************************************************************************************************
* Description  : MTU interrupt handler routines.
***********************************************************************************************************************/
#if MTU_CFG_USE_CH0 == 1
    /* MTU0.TGRA input capture/compare match. */
    #pragma interrupt (mtu_tgia0_isr(vect = VECT(MTU0, TGIA0)))
    void mtu_tgia0_isr(void)
    {
        /* Call the common handler for all TGR interrupts. */
        mtu_isr_common(MTU_CHANNEL_0, MTU_TIMER_A);
    }

    /* MTU0.TGRB input capture/compare match. */
    #pragma interrupt (mtu_tgib0_isr(vect = VECT(MTU0, TGIB0)))
    void mtu_tgib0_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_0, MTU_TIMER_B);
    }

    /* MTU0.TGRC input capture/compare match. */
    #pragma interrupt (mtu_tgic0_isr(vect = VECT(MTU0, TGIC0)))
    void mtu_tgic0_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_0, MTU_TIMER_C);
    }

    /* MTU0.TGRD input capture/compare match. */
    #pragma interrupt (mtu_tgid0_isr(vect = VECT(MTU0, TGID0)))
    void mtu_tgid0_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_0, MTU_TIMER_D);
    }

    #ifndef BSP_MCU_RX63_ALL
        #if(0)
        /* MTU0.TCNT overflow. */
        #pragma interrupt (mtu_tciv0_isr(vect = VECT(MTU0, TCIV0)))
        void mtu_tciv0_isr(void)
        {
            //FUTURE: implement this handler.
        }
        #endif
    #endif
#endif

#if MTU_CFG_USE_CH1 == 1
    /* MTU1.TGRA input capture/compare match. */
    #pragma interrupt (mtu_tgia1_isr(vect = VECT(MTU1, TGIA1)))
    void mtu_tgia1_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_1, MTU_TIMER_A);
    }

    /* MTU1.TGRB input capture/compare match. */
    #pragma interrupt (mtu_tgib1_isr(vect = VECT(MTU1, TGIB1)))
    void mtu_tgib1_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_1, MTU_TIMER_B);
    }

    #ifndef BSP_MCU_RX63_ALL
        #if(0)
        /* MTU1.TCNT overflow. */
        #pragma interrupt (mtu_tciv1_isr(vect = VECT(MTU1, TCIV1)))
        void mtu_tciv1_isr(void)
        {
            //FUTURE: implement this handler.
        }

        /* MTU1.TCNT underflow. */
        #pragma interrupt (mtu_tciu1_isr(vect = VECT(MTU1, TCIU1)))
        void mtu_tciu1_isr(void)
        {
            //FUTURE: implement this handler.
        }
        #endif
    #endif
#endif


#ifdef BSP_MCU_RX63_ALL
    #if(0)
        #if (MTU_CFG_USE_CH0 == 1) || (MTU_CFG_USE_CH1 == 1)
        /* Shared group interrupt channels 0 and 1 overflow/underflow. */
        #pragma interrupt (mtu_tciv_0_1_isr(vect=VECT(ICU, GROUP1)))
        void mtu_tciv_0_1_isr(void)
        {
            //FUTURE: implement this handler.
        }
        #endif
    #endif
#endif


#if MTU_CFG_USE_CH2 == 1
    /* MTU2.TGRA input capture/compare match. */
    #pragma interrupt (mtu_tgia2_isr(vect = VECT(MTU2, TGIA2)))
    void mtu_tgia2_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_2, MTU_TIMER_A);
    }

    /* MTU2.TGRB input capture/compare match. */
    #pragma interrupt (mtu_tgib2_isr(vect = VECT(MTU2, TGIB2)))
    void mtu_tgib2_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_2, MTU_TIMER_B);
    }

    #ifndef BSP_MCU_RX63_ALL
        #if(0)
        /* MTU2.TCNT overflow. */
        #pragma interrupt (mtu_tciv2_isr(vect = VECT(MTU2, TCIV2)))
        void mtu_tciv2_isr(void)
        {
            //FUTURE: implement this handler.
        }

        /* MTU2.TCNT underflow. */
        #pragma interrupt (mtu_tciu2_isr(vect = VECT(MTU2, TCIU2)))
        void mtu_tciu2_isr(void)
        {
            //FUTURE: implement this handler.
        }
        #endif
    #endif
#endif

#if MTU_CFG_USE_CH3 == 1
    /* MTU3.TGRA input capture/compare match. */
    #pragma interrupt (mtu_tgia3_isr(vect = VECT(MTU3, TGIA3)))
    void mtu_tgia3_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_3, MTU_TIMER_A);
    }

    /* MTU3.TGRB input capture/compare match. */
    #pragma interrupt (mtu_tgib3_isr(vect = VECT(MTU3, TGIB3)))
    void mtu_tgib3_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_3, MTU_TIMER_B);
    }

    /* MTU3.TGRC input capture/compare match. */
    #pragma interrupt (mtu_tgic3_isr(vect = VECT(MTU3, TGIC3)))
    void mtu_tgic3_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_3, MTU_TIMER_C);
    }

    /* MTU3.TGRD input capture/compare match. */
    #pragma interrupt (mtu_tgid3_isr(vect = VECT(MTU3, TGID3)))
    void mtu_tgid3_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_3, MTU_TIMER_D);
    }

    #ifndef BSP_MCU_RX63_ALL
        #if(0)
            /* MTU3.TCNT overflow. */
            #pragma interrupt (mtu_tciv3_isr(vect = VECT(MTU3, TCIV3)))
            void mtu_tciv3_isr(void)
            {
                //FUTURE: implement this handler.
            }
        #endif
    #endif
#endif

#ifdef BSP_MCU_RX63_ALL
    #if (MTU_CFG_USE_CH2 == 1) || (MTU_CFG_USE_CH3 == 1)
        #if(0)
            /* Shared group interrupt channels 2 and 3 overflow/underflow. */
            #pragma interrupt (mtu_tciv_2_3_isr(vect=VECT(ICU, GROUP2)))
            void mtu_tciv_2_3_isr(void)
            {
                //FUTURE: implement this handler.
            }
        #endif
    #endif
#endif

#if MTU_CFG_USE_CH4 == 1
    /* MTU4.TGRA input capture/compare match. */
    #pragma interrupt (mtu_tgia4_isr(vect = VECT(MTU4, TGIA4)))
    void mtu_tgia4_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_4, MTU_TIMER_A);
    }

    /* MTU4.TGRB input capture/compare match. */
    #pragma interrupt (mtu_tgib4_isr(vect = VECT(MTU4, TGIB4)))
    void mtu_tgib4_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_4, MTU_TIMER_B);
    }

    /* MTU4.TGRC input capture/compare match. */
    #pragma interrupt (mtu_tgic4_isr(vect = VECT(MTU4, TGIC4)))
    void mtu_tgic4_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_4, MTU_TIMER_C);
    }

    /* MTU4.TGRD input capture/compare match. */
    #pragma interrupt (mtu_tgid4_isr(vect = VECT(MTU4, TGID4)))
    void mtu_tgid4_isr(void)
    {
        mtu_isr_common(MTU_CHANNEL_4, MTU_TIMER_D);
    }
    #if(0)
        /* MTU4.TCNT overflow/underflow. */
        #pragma interrupt (mtu_tciv4_isr(vect = VECT(MTU4, TCIV4)))
        void mtu_tciv4_isr(void)
        {
            //FUTURE: implement this handler.
        }
    #endif
#endif
