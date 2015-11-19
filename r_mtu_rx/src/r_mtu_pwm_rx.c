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
* File Name    : r_mtu_pwm_rx.c
* Device(s)    : RX Family
* Tool-Chain   : Renesas RX Standard Toolchain 1.02+
* OS           : None
* H/W Platform :
* Description  : Functions for using MTU on RX devices.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 23.09.2014  1.00   First Release
***********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"
#include "r_mtu_rx_if.h"
/* Internal definitions. */
#include "r_mtu_rx_private.h"

#if MTU_CFG_USE_PWM == 1
/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
enum
{
    MTU_TMDR_NORMAL            = 0x00,
    MTU_TMDR_PWM_MODE_1        = 0x02,
    MTU_TMDR_PWM_MODE_2        = 0x03,
    MTU_TMDR_PHASE_COUNTING_1  = 0x04,
    MTU_TMDR_PHASE_COUNTING_2  = 0x05,
    MTU_TMDR_PHASE_COUNTING_3  = 0x06,
    MTU_TMDR_PHASE_COUNTING_4  = 0x07,
    MTU_TMDR_RESET_SYNC_PWM    = 0x08,
    MTU_TMDR_COMP_PWM_1        = 0x0D,
    MTU_TMDR_COMP_PWM_2        = 0x0E,
    MTU_TMDR_COMP_PWM_3        = 0x0F,
    MTU_TMDR_BFA               = 0x10,
    MTU_TMDR_BFB               = 0x20,
    MTU_TMDR_BFE               = 0x40
};

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/
static mtu_pwm_chnl_settings_t   * g_mtu_pwm_settings[MTU_CHANNEL_MAX] = {0}; // Pointers to timer configuration data

/***********************************************************************************************************************
Private local function declarations
***********************************************************************************************************************/
static mtu_err_t mtu_setup_pwm_mode1(uint8_t chan, mtu_timer_num_t pwm_num,
                                mtu_pwm_settings_t *p_pwm_settings, uint32_t cycle_freq, uint16_t pclk_div);
static mtu_err_t mtu_setup_pwm_mode2(uint8_t chan, mtu_pwm_chnl_settings_t * pconfig, uint16_t pclk_div);


/***********************************************************************************************************************
API function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name: R_MTU_PWM_Open
* Description  : This function applies power to the MTU channel,
*                initializes the associated registers to prepare for compare/match timer operations,
*                and applies user-configurable options.
* Arguments    : channel -
*                   Number of the MTU channel to be initialized
*                pconfig -
*                   Pointer to MTU channel configuration data structure.
*                pcallback -
*                   Pointer to function called from interrupt
* Return Value : MTU_SUCCESS-
*                   Successful; channel initialized
*                MTU_ERR_BAD_CHAN-
*                   Channel number is invalid for part
*                MTU_ERR_CH_NOT_CLOSED-
*                   Channel currently in operation; Perform R_MTU_Close() first
*                MTU_ERR_NULL_PTR-
*                   pconfig pointer is NULL
*                MTU_ERR_ARG_RANGE-
*                   The pconfig structure contains a value that exceeds limits.
*                MTU_ERR_INVALID_ARG-
*                   An element of the pconfig structure contains an invalid value.
*                MTU_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
mtu_err_t  R_MTU_PWM_Open (mtu_channel_t            channel,
                           mtu_pwm_chnl_settings_t *pconfig,
                           void                   (*pcallback)(void *pdata))
{
    bool     result;
    uint8_t  pclk_divisor_index;
    uint16_t pclk_divisor;
    uint8_t  tcr_bits = 0;
    mtu_handle_t  my_handle;

    #if MTU_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    #if MTU_CFG_PARAM_CHECKING_ENABLE == 1
    if (MTU_CHANNEL_MAX <= channel)           // First check for channel number out of range
    {
        return MTU_ERR_BAD_CHAN;
    }

    if (NULL == g_mtu_handles[channel])  // Now check that channel has been configured for build
    {
        return MTU_ERR_BAD_CHAN;
    }

    if (NULL == pconfig)
    {
        return MTU_ERR_NULL_PTR;
    }

    /* Check to see if the peripheral has already been initialized. */
    if (g_mtu_channel_mode[channel])
    {
        return MTU_ERR_CH_NOT_CLOSED;     // This channel has already been initialized.
    }

    if ((MTU_CHANNEL_1 == channel) || (MTU_CHANNEL_2 == channel))
    {
        if((MTU_ACTION_NONE != pconfig->pwm_c.actions)
           && (MTU_ACTION_NONE != pconfig->pwm_d.actions))
        {
            return MTU_ERR_INVALID_ARG;    // Resource not present on these channels.
        }
    }

    if ((MTU_PWM_MODE_1 != pconfig->pwm_mode) && (MTU_PWM_MODE_2 != pconfig->pwm_mode))
    {
        return MTU_ERR_INVALID_ARG;
    }

    if ((MTU_CHANNEL_3 == channel) || (MTU_CHANNEL_4 == channel))
    {
        if (MTU_PWM_MODE_2 == pconfig->pwm_mode)
        {
            return MTU_ERR_INVALID_ARG;      // PWM mode 2 not available on these channels.
        }
    }

    if ((MTU_ACTION_NONE == pconfig->pwm_a.actions)
        && (MTU_ACTION_NONE == pconfig->pwm_b.actions)
        && (MTU_ACTION_NONE == pconfig->pwm_c.actions)
        && (MTU_ACTION_NONE == pconfig->pwm_d.actions))
    {
        return MTU_ERR_INVALID_ARG; // Must define something to do!
    }

    switch (pconfig->clock_src.source)    // Check counter clock source
    {
        case MTU_CLK_SRC_INTERNAL:
        break;

        /* Other than internal clocking source: */
        case MTU_CLK_SRC_EXT_MTCLKA:
        case MTU_CLK_SRC_EXT_MTCLKB:
        case MTU_CLK_SRC_EXT_MTCLKC:
        case MTU_CLK_SRC_EXT_MTCLKD:
        {
            /* Not all channels have this setting. */
            if(MTU_NOT_SUPP == g_chnl_ext_clks[channel][pconfig->clock_src.source])
            {
                return MTU_ERR_INVALID_ARG;    // Not supported by this channel
            }
        }
        break;

        default:
        {
            return MTU_ERR_INVALID_ARG;
        }
    }

    if (0 == pconfig->cycle_freq) // don't allow 0 frequency.
    {
        return MTU_ERR_INVALID_ARG;
    }

    /* Check clear source selection. */
    if (MTU_PWM_MODE_2 == pconfig->pwm_mode)
    {
        switch (pconfig->clear_src)
        {
            case MTU_CLR_DISABLED:
            case MTU_CLR_TIMER_A:
            case MTU_CLR_TIMER_B:
            case MTU_CLR_TIMER_C:
            case MTU_CLR_TIMER_D:
            case MTU_CLR_SYNC:
            {   /* Find the bits to set this in the table. Not all channels have this setting. */
                if(MTU_NOT_SUPP == g_chnl_clear_src[channel][pconfig->clear_src])
                {
                    return MTU_ERR_INVALID_ARG; // Not supported by this channel
                }
            }
            break;
            default:
            {
                return MTU_ERR_INVALID_ARG;
            }
        }
    }
    #endif // MTU_CFG_PARAM_CHECKING_ENABLE

    #if MTU_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this MTU channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));

    if(false == lock_result)
    {
        return MTU_ERR_LOCK; /* The R_MTU_Timer_Create function is currently locked. */
    }
    #endif

    my_handle = g_mtu_handles[channel];

    g_mtu_pwm_settings[channel] = pconfig;  // Save a copy of the pointer to the user's config structure.

    /* ICU settings. */
    mtu_interrupts_disable(channel);
    mtu_interrupts_clear(channel);
    *my_handle->regs.ipr = my_handle->priority; // Set the priority register from config.h value.

    power_on_off(MTU_POWER_ON);    // Make sure MTU channel is powered on.

    mtu_channel_clear(channel);             // Clear the registers and state variables for this channel.

    /* Select counter clearing source  */
    /* If PWM Mode1 then clearing source must be either A or C. */
    if (MTU_PWM_MODE_1 == pconfig->pwm_mode)
    {
        /* Default. */
        if (MTU_ACTION_NONE != pconfig->pwm_a.actions)
        {
            tcr_bits = g_chnl_clear_src[channel][MTU_CLR_TIMER_A];
            g_mtu_channel_clr_src[channel] = MTU_CLR_TIMER_A; // Keep a global copy for ISRs.
        }
        /* Must be using TGRC then. Update if needed. */
        else
        {
            tcr_bits = g_chnl_clear_src[channel][MTU_CLR_TIMER_C];
            g_mtu_channel_clr_src[channel] = MTU_CLR_TIMER_C; // Keep a global copy for ISRs.
        }
    }
    else // mode 2 then
    {

        tcr_bits = g_chnl_clear_src[channel][pconfig->clear_src];     // Select the clear source bits.
        g_mtu_channel_clr_src[channel] = pconfig->clear_src;          // Keep a global copy for ISRs.
    }   // mode2

    /* Select either internal clock divisor or external counter clock source. */
    switch (pconfig->clock_src.source)
    {
        case MTU_CLK_SRC_INTERNAL:
        {
            /* calculate clock divisor based on target frequency or period. */
            result = mtu_calc_clock_divisor(channel, &pclk_divisor_index, pconfig->cycle_freq);

            if(true == result)
            {
                tcr_bits |= g_chnl_clk_divs[channel][pclk_divisor_index]; // Save divisor bits for later.
                pclk_divisor = g_mtu_clock_divisors[pclk_divisor_index];
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_ARG_RANGE; // Could not obtain requested frequency.
            }
        }
        break;
        /* Other than internal clocking source: */
        case MTU_CLK_SRC_EXT_MTCLKA:
        case MTU_CLK_SRC_EXT_MTCLKB:
        case MTU_CLK_SRC_EXT_MTCLKC:
        case MTU_CLK_SRC_EXT_MTCLKD:
        {   /* Find the bits to set this in the table.  */
            tcr_bits |= g_chnl_ext_clks[channel][pconfig->clock_src.source];
        }
        break;

        default:
        break;
    }

    tcr_bits |= pconfig->clock_src.clock_edge;      // Set clock active edge.
    *my_handle->regs.tcr = tcr_bits;                // Copy the completed setting to the TCR register.

    /* Set up PWM mode. */
    if (MTU_PWM_MODE_1 == pconfig->pwm_mode)
    {
        *my_handle->regs.tmdr = MTU_TMDR_PWM_MODE_1;       // Set the channel mode register.

        /* Select waveform output levels. */
        if (MTU_ACTION_NONE != pconfig->pwm_a.actions)
        {
            mtu_setup_pwm_mode1(channel, MTU_TIMER_A, &(pconfig->pwm_a), pconfig->cycle_freq, pclk_divisor);
            mtu_setup_pwm_mode1(channel, MTU_TIMER_B, &(pconfig->pwm_b), pconfig->cycle_freq, pclk_divisor);
        }
        if (MTU_ACTION_NONE != pconfig->pwm_c.actions)
        {
            mtu_setup_pwm_mode1(channel, MTU_TIMER_C, &(pconfig->pwm_c), pconfig->cycle_freq, pclk_divisor);
            mtu_setup_pwm_mode1(channel, MTU_TIMER_D, &(pconfig->pwm_d), pconfig->cycle_freq, pclk_divisor);
        }
    }
    else  // Must be mode 2
    {
        *my_handle->regs.tmdr = MTU_TMDR_PWM_MODE_2;       // Set the channel mode register.
        /* Set up waveform output levels in TIORs and set TGR counts. */
        mtu_setup_pwm_mode2(channel, pconfig, pclk_divisor);
    }

    g_mtu_channel_mode[channel] = MTU_MODE_COMPARE_MATCH;   // Tag the channel is in use for compare match.
    g_num_channels_in_use++;                                // Add this channel to the count.
    *g_mtu_handles[channel]->p_callback = pcallback;

    #if MTU_CFG_REQUIRE_LOCK == 1
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
    #endif

    return MTU_SUCCESS;    // Ready to start PWM operation now with a start command.
}
/* end of function R_MTU_Timer_Create(). */


/***********************************************************************************************************************
Private local function definitions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name: mtu_setup_pwm_mode1
* Description  :
* Arguments    :
* Return Value : none
***********************************************************************************************************************/
static mtu_err_t mtu_setup_pwm_mode1(uint8_t chan, mtu_timer_num_t pwm_num,
                                mtu_pwm_settings_t *p_pwm_settings, uint32_t cycle_freq, uint16_t pclk_div)
{
    uint16_t tgr_value;
    uint32_t cycle_tgr;
    uint32_t duty;
    mtu_handle_t handle = g_mtu_handles[chan];
    mtu_err_t result = MTU_SUCCESS;
    volatile __evenaccess unsigned short *my_tgr = handle->regs.tgra + pwm_num; // Create a Base TGR address pointer

    if ((MTU_TIMER_A == pwm_num) || (MTU_TIMER_C == pwm_num)) // TGRA and TGRC have cycle setting
    {
        if(MTU_CLK_SRC_INTERNAL ==  g_mtu_pwm_settings[chan]->clock_src.source)
        {
            /* Set compare match register with the value calculated from requested frequency. */
            tgr_value = mtu_calc_tgr_ticks(pclk_div, cycle_freq);

            if(0 != tgr_value)
            {
                *my_tgr = tgr_value;
            }
            else
            {
                return MTU_ERR_ARG_RANGE; // Could not obtain requested frequency.
            }
        }
        else
        {
            /* External clock source. Use cycle_freq as direct TGR count. */
            *my_tgr = cycle_freq;
        }
    }
    else   // must be TGRB or TGRD, so this is duty setting.
    {
        /* If this is TGRB then cycle is in TGRA. If this is TGRD then cycle is in TGRC */
        cycle_tgr = *(my_tgr - 1);
        duty = p_pwm_settings->duty;

        /* Calculate TGR value from duty percentage. Percentage is expressed in 10ths for resolution. */
        *my_tgr =  (uint16_t)((cycle_tgr * duty)/1000);
    }

    /* Set up actions to perform on compare match. */
    if(MTU_ACTION_OUTPUT & p_pwm_settings->actions)          // Output to a pin
    {
        if (MTU_TIMER_A == pwm_num)
        {
            *handle->regs.tiorh |= p_pwm_settings->outputs;  // Set bits in lower nibble
             #ifndef BSP_MCU_RX110
            if (MTU_CHANNEL_4 == chan)
            {
                MTU.TOER.BIT.OE4A = 1;  // Must also turn on Timer Output Master Enable Register for this channel.
            }
            #endif
        }
        else if (MTU_TIMER_B == pwm_num)
        {
            *handle->regs.tiorh |=  (p_pwm_settings->outputs << 4); // Move bits to upper nibble
            /* Must also turn on Timer Output Master Enable Register for these channels. */
            #ifndef BSP_MCU_RX110
            if (MTU_CHANNEL_3 == chan)
            {
                MTU.TOER.BIT.OE3B = 1;
            }
            if (MTU_CHANNEL_4 == chan)
            {
                MTU.TOER.BIT.OE4B = 1;
            }
            #endif
        }
        else if (MTU_TIMER_C == pwm_num)
        {
            *handle->regs.tiorl |= p_pwm_settings->outputs;  // Set bits in lower nibble
            #ifndef BSP_MCU_RX110
            if (MTU_CHANNEL_4 == chan)
            {
                MTU.TOER.BIT.OE4C = 1; // Must also turn on Timer Output Master Enable Register for this channel.
            }
            #endif
        }
        else
        {
            *handle->regs.tiorl |=  (p_pwm_settings->outputs << 4); // Move bits to upper nibble

            #ifndef BSP_MCU_RX110
            /* Must also turn on Timer Output Master Enable Register for these channels. */
            if (MTU_CHANNEL_3 == chan)
            {
                MTU.TOER.BIT.OE3D = 1;
            }
            if (MTU_CHANNEL_4 == chan)
            {
                MTU.TOER.BIT.OE4D = 1;
            }
            #endif
        }
    }

    /* Set up more actions to perform on compare match. */
    if((MTU_ACTION_INTERRUPT & p_pwm_settings->actions)
      || (MTU_ACTION_CALLBACK & p_pwm_settings->actions)) // Request an interrupt
    {
        g_mtu_tgi_icu_en_flags[chan][pwm_num] = 1;        // Set a software control flag

        if (MTU_ACTION_CALLBACK & p_pwm_settings->actions)
        {
             g_mtu_tgr_callbacks[chan][pwm_num] = 1;      // Do the callback for this interrupt.
        }
    }

    if (MTU_ACTION_TRIGGER_ADC & p_pwm_settings->actions)
    {
        *handle->regs.tier |= MTU_ADC_TRIG;   // Set ADC TTGE trigger bit in MTU register.
    }

    *handle->regs.tier |= (uint8_t)(1 << pwm_num);   // Always set interrupt enable bit in MTU register.

    /* Set repeat mode if option selected and this TGR is clear source.*/
    if (MTU_ACTION_REPEAT & p_pwm_settings->actions)
    {
        g_mtu_channel_repeats[chan] = 1;             // Continuous running
    }

    return result;
}

/***********************************************************************************************************************
* Function Name: mtu_setup_pwm_mode2
* Description  :
* Arguments    :
* Return Value : none
***********************************************************************************************************************/
static mtu_err_t mtu_setup_pwm_mode2(uint8_t chan, mtu_pwm_chnl_settings_t * pconfig, uint16_t pclk_div)
{
    uint32_t cycle_tgr = 0;
    uint32_t duty;
    uint8_t i;
    mtu_handle_t handle = g_mtu_handles[chan];
    mtu_err_t result = MTU_SUCCESS;
    volatile __evenaccess unsigned short *my_tgr; // Create a Base TGR address pointer
    mtu_pwm_settings_t * pwm_settings[] =
    {
        &(pconfig->pwm_a),
        &(pconfig->pwm_b),
        &(pconfig->pwm_c),
        &(pconfig->pwm_d)
    };

    if(MTU_CLK_SRC_INTERNAL ==  g_mtu_pwm_settings[chan]->clock_src.source)
    {
        cycle_tgr = mtu_calc_tgr_ticks(pclk_div, pconfig->cycle_freq);
    }
    else
    {
        /* External clock source. Use cycle_freq value as actual TGR (tick) count. */
        cycle_tgr = pconfig->cycle_freq;
    }

    for (i = MTU_TIMER_A; i < MTU_NUM_TIMERS; i++)
    {
        my_tgr = handle->regs.tgra + i;

        if(i == g_mtu_pwm_settings[chan]->clear_src)  /* This is the cycle TGR. */
        {
            if(0 != cycle_tgr)
            {
                *my_tgr = cycle_tgr;
            }
            else
            {
                return MTU_ERR_ARG_RANGE; // Could not obtain requested frequency.
            }
        }
        else // This is a duty TGR
        {
            duty = pwm_settings[i]->duty;

            /* Calculate TGR value from duty percentage. Percentage is expressed in 10ths for resolution. */
            *my_tgr =  (uint16_t)((cycle_tgr * duty)/1000);
        }
    }

    /* Now set up actions to perform on compare match. */
    for (i = MTU_TIMER_A; i < MTU_NUM_TIMERS; i++)
    {
        if(MTU_ACTION_OUTPUT & pwm_settings[i]->actions) // Output to a pin
        {
            if (MTU_TIMER_A == i)
            {
                *handle->regs.tiorh |= pwm_settings[i]->outputs;  // Set bits in lower nibble
                #ifndef BSP_MCU_RX110
                if (MTU_CHANNEL_4 == chan)
                {
                    MTU.TOER.BIT.OE4A = 1; // Must also turn on Timer Output Master Enable Register for this channel.
                }
                #endif
            }
            else if (MTU_TIMER_B == i)
            {
                *handle->regs.tiorh |=  (pwm_settings[i]->outputs << 4); // Move bits to upper nibble
                /* Must also turn on Timer Output Master Enable Register for these channels. */
                #ifndef BSP_MCU_RX110
                if (MTU_CHANNEL_3 == chan)
                {
                    MTU.TOER.BIT.OE3B = 1;
                }
                if (MTU_CHANNEL_4 == chan)
                {
                    MTU.TOER.BIT.OE4B = 1;
                }
                #endif
            }
            else if (MTU_TIMER_C == i)
            {
                *handle->regs.tiorl |= pwm_settings[i]->outputs;  // Set bits in lower nibble
                #ifndef BSP_MCU_RX110
                if (MTU_CHANNEL_4 == chan)
                {
                    MTU.TOER.BIT.OE4C = 1; // Must also turn on Timer Output Master Enable Register for this channel.
                }
                #endif
            }
            else
            {
                *handle->regs.tiorl |=  (pwm_settings[i]->outputs << 4); // Move bits to upper nibble

                #ifndef BSP_MCU_RX110
                /* Must also turn on Timer Output Master Enable Register for these channels. */
                if (MTU_CHANNEL_3 == chan)
                {
                    MTU.TOER.BIT.OE3D = 1;
                }
                if (MTU_CHANNEL_4 == chan)
                {
                    MTU.TOER.BIT.OE4D = 1;
                }
                #endif
            }
        }
         /* Set up more actions to perform on compare match. */
        if((MTU_ACTION_INTERRUPT & pwm_settings[i]->actions)
          || (MTU_ACTION_CALLBACK & pwm_settings[i]->actions)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[chan][i] = 1;   // Set a software control flag

            if (MTU_ACTION_CALLBACK & pwm_settings[i]->actions)
            {
                 g_mtu_tgr_callbacks[chan][i] = 1; // Do the callback for this interrupt.
            }
        }

        if (MTU_ACTION_TRIGGER_ADC & pwm_settings[i]->actions)
        {
            *handle->regs.tier |= MTU_ADC_TRIG;   // Set ADC TTGE trigger bit in MTU register.
        }

        *handle->regs.tier |= (uint8_t)(1 << i);   // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if (MTU_ACTION_REPEAT & pwm_settings[i]->actions)
        {
            g_mtu_channel_repeats[chan] = 1;       // Continuous running
        }
    }

    return result;
}
#endif

