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
* File Name    : r_mtu_timers_rx.c
* Device(s)    : RX Family
* Tool-Chain   : Renesas RX Standard Toolchain 1.02+
* OS           : None
* H/W Platform :
* Description  : Functions for using MTU on RX devices.
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
Private local function declarations
***********************************************************************************************************************/

/***********************************************************************************************************************
API function definitions
***********************************************************************************************************************/
#if MTU_CFG_USE_TIMER == 1
/***********************************************************************************************************************
* Function Name: R_MTU_Timer_Open
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
*                MTU_TIMERS_ERR_BAD_CHAN-
*                   Channel number is invalid for part
*                MTU_TIMERS_ERR_CH_NOT_CLOSED-
*                   Channel currently in operation; Perform R_MTU_Close() first
*                MTU_TIMERS_ERR_NULL_PTR-
*                   pconfig pointer is NULL
*                MTU_ERR_ARG_RANGE-
*                   The pconfig structure contains a value that exceeds limits.
*                MTU_TIMERS_ERR_INVALID_ARG-
*                   An element of the pconfig structure contains an invalid value.
*                MTU_TIMERS_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
mtu_err_t   R_MTU_Timer_Open (mtu_channel_t              channel,
                              mtu_timer_chnl_settings_t *pconfig,
                              void                     (*pcallback)(void *pdata))
{
    mtu_handle_t  my_handle;
    bool result;
    uint8_t pclk_divisor_index;
    uint16_t pclk_divisor;
    uint8_t tcr_bits = 0;
    uint16_t tgr_value;
    uint32_t cycle_freq;
    uint32_t i;
    uint8_t * p_byte1;
    uint8_t * p_byte2;

    #if MTU_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    #if MTU_CFG_PARAM_CHECKING_ENABLE == 1
    if (MTU_CHANNEL_MAX <= channel)     // First check for channel number out of range
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
        return MTU_ERR_CH_NOT_CLOSED;   // This channel has already been initialized.
    }

    if ((MTU_CHANNEL_1 == channel) || (MTU_CHANNEL_2 == channel))
    {
        if((MTU_ACTION_NONE != pconfig->timer_c.actions.do_action)
           && (MTU_ACTION_NONE != pconfig->timer_d.actions.do_action))
        {
            return MTU_ERR_INVALID_ARG; // Resource not present on these channels.
        }
    }

    /* Check counter clearing source  */
    switch (pconfig->clear_src)
    {
        case MTU_CLR_DISABLED:
        case MTU_CLR_TGRA:
        case MTU_CLR_TGRB:
        case MTU_CLR_TGRC:
        case MTU_CLR_TGRD:
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
    #endif // MTU_CFG_PARAM_CHECKING_ENABLE

    #if MTU_CFG_REQUIRE_LOCK == 1
    /* Attempt to acquire lock for this MTU channel. Prevents reentrancy conflict. */
    lock_result = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));

    if(false == lock_result)
    {
        return MTU_ERR_LOCK;   // The R_MTU_Timer_Create function is currently locked.
    }
    #endif

    my_handle = g_mtu_handles[channel];

    /* Save a copy of the user's config structure into local channel settings. */
    p_byte1 = (uint8_t *)pconfig;
    p_byte2 = (uint8_t *)my_handle->p_mtu_chnl_tmr_settings;

    for (i = 0; i < sizeof(mtu_timer_chnl_settings_t); i++)
    {
    	p_byte2[i] = p_byte1[i];
    }

    tcr_bits = g_chnl_clear_src[channel][pconfig->clear_src];   // Select counter clearing source.
    g_mtu_channel_clr_src[channel] = pconfig->clear_src;        // Keep a global copy for ISRs.

    /* ICU settings. */
    mtu_interrupts_disable(channel);
    mtu_interrupts_clear(channel);
    *my_handle->regs.ipr = my_handle->priority; // Set the priority register from config.h value.

    power_on_off(MTU_POWER_ON);                 // Make sure MTU channel is powered on.

    mtu_channel_clear(channel);                 // Clear the registers and state variables for channel.

    switch (pconfig->clock_src.source)          // Select counter clock source
    {
        case MTU_CLK_SRC_INTERNAL:
        {
            /* Calculate the clock pre-scaler based on timer that has the longest period. */
            cycle_freq = 0xFFFFFFFF; // Seed

            if (MTU_ACTION_NONE != pconfig->timer_a.actions.do_action)
            {
                cycle_freq = pconfig->timer_a.freq;
            }
            if ((MTU_ACTION_NONE != pconfig->timer_b.actions.do_action) && (cycle_freq > pconfig->timer_b.freq))
            {
                    cycle_freq = pconfig->timer_b.freq;
            }
            if ((MTU_ACTION_NONE != pconfig->timer_c.actions.do_action) && (cycle_freq > pconfig->timer_c.freq))
            {
                    cycle_freq = pconfig->timer_c.freq;
            }
            if ((MTU_ACTION_NONE != pconfig->timer_d.actions.do_action) && (cycle_freq > pconfig->timer_d.freq))
            {
                    cycle_freq = pconfig->timer_d.freq;
            }

            if (0 == cycle_freq) // don't allow 0 frequency.
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_INVALID_ARG;
            }

            /* calculate clock divisor based on target frequency or period. */
            result = mtu_calc_clock_divisor(channel, &pclk_divisor_index, cycle_freq);

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
        case MTU_CLK_SRC_CASCADE:
        {   /* Find the bits to set this in the table. Not all channels have this setting. */
            if(MTU_NOT_SUPP != g_chnl_ext_clks[channel][pconfig->clock_src.source])
            {
                tcr_bits |= g_chnl_ext_clks[channel][pconfig->clock_src.source];
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_INVALID_ARG; // Not supported by this channel
            }
        }
        break;

        default:
        {
            #if MTU_CFG_REQUIRE_LOCK == 1
            R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
            #endif
            return MTU_ERR_INVALID_ARG;
        }
    }

    tcr_bits |= pconfig->clock_src.clock_edge;  // Set clock active edge.
    *my_handle->regs.tcr = tcr_bits;            // Copy the completed setting to the TCR register.

    /* Set the compare/match operation register values for each TGR being used for this channel. */
    if(MTU_ACTION_NONE != pconfig->timer_a.actions.do_action)    // MTU_ACTION_NONE means this timer event not used.
    {
        if(MTU_CLK_SRC_INTERNAL ==  pconfig->clock_src.source)
        {
            /* Set compare match register with the value calculated from requested frequency. */
            tgr_value = mtu_calc_tgr_ticks(pclk_divisor, pconfig->timer_a.freq);

            if(0 != tgr_value)
            {
                *my_handle->regs.tgra = tgr_value;
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_ARG_RANGE; // Could not obtain requested frequency.
            }
        }
        else
        {
            *(my_handle->regs.tgra) = pconfig->timer_a.freq;  // External clock source. Use freq as direct TGR count.
        }

        /* Set up actions to perform on compare match. */
        if(MTU_ACTION_OUTPUT & pconfig->timer_a.actions.do_action)      // Output to a pin
        {
            *my_handle->regs.tiorh |= pconfig->timer_a.actions.output;  // Set bits in lower nibble

            #ifndef BSP_MCU_RX110
            if (MTU_CHANNEL_4 == channel)
            {
                MTU.TOER.BIT.OE4A = 1; // Must also turn on Timer Output Master Enable Register for this channel.
            }
            #endif
        }

        /* Set up actions to perform on compare match. */
        if((MTU_ACTION_INTERRUPT & pconfig->timer_a.actions.do_action)
          || (MTU_ACTION_CALLBACK & pconfig->timer_a.actions.do_action)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_A] = 1;            // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->timer_a.actions.do_action)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_A] = 1;          // Do the callback for this interrupt.
            }
        }

        if (MTU_ACTION_TRIGGER_ADC & pconfig->timer_a.actions.do_action)
        {
        	*my_handle->regs.tier |= MTU_ADC_TRIG;   // Set ADC TTGE trigger bit in MTU register.
        }

        *my_handle->regs.tier |= MTU_TGIEA;                 // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->timer_a.actions.do_action) && (MTU_CLR_TGRA == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;                  // Continuous running
        }
    }

    if(MTU_ACTION_NONE != pconfig->timer_b.actions.do_action)    // MTU_ACTION_NONE means this timer event not used.
    {
        if(MTU_CLK_SRC_INTERNAL ==  pconfig->clock_src.source)
        {
            /* Set compare match register with the value calculated from requested frequency. */
            tgr_value = mtu_calc_tgr_ticks(pclk_divisor, pconfig->timer_b.freq);

            if(0 != tgr_value)
            {
                *my_handle->regs.tgrb  = tgr_value;
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_ARG_RANGE;                    // Could not obtain requested frequency.
            }
        }
        else
        {
            *my_handle->regs.tgrb  = pconfig->timer_b.freq;  // External clock source. Use freq as direct TGR count.
        }

        /* Set up actions to perform on compare match. */
        if(MTU_ACTION_OUTPUT & pconfig->timer_b.actions.do_action) // Output to a pin
        {
            *my_handle->regs.tiorh |=  (pconfig->timer_b.actions.output << 4); // Move bits to upper nibble

            /* Must also turn on Timer Output Master Enable Register for these channels. */
            #ifndef BSP_MCU_RX110

            if (MTU_CHANNEL_3 == channel)
            {
                MTU.TOER.BIT.OE3B = 1;
            }

            if (MTU_CHANNEL_4 == channel)
            {
                MTU.TOER.BIT.OE4B = 1;
            }
            #endif
        }

        /* Set up actions to perform on compare match. */
        if((MTU_ACTION_INTERRUPT & pconfig->timer_b.actions.do_action)
          || (MTU_ACTION_CALLBACK & pconfig->timer_b.actions.do_action)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_B] = 1;            // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->timer_b.actions.do_action)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_B] = 1;          // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIEB;                 // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->timer_b.actions.do_action) && (MTU_CLR_TGRB == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;                  // Continuous running
        }
    }

    if(MTU_ACTION_NONE != pconfig->timer_c.actions.do_action)    // MTU_ACTION_NONE means this timer event not used..
    {
        if(MTU_CLK_SRC_INTERNAL ==  pconfig->clock_src.source)
        {
            /* Set compare match register with the value calculated from requested frequency. */
            tgr_value = mtu_calc_tgr_ticks(pclk_divisor, pconfig->timer_c.freq);

            if(0 != tgr_value)
            {
                *my_handle->regs.tgrc  = tgr_value;
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_ARG_RANGE;         // Could not obtain requested frequency.
            }
        }
        else
        {
            *my_handle->regs.tgrc  = pconfig->timer_c.freq;  // External clock source. Use freq as direct TGR count.
        }

        /* Set up actions to perform on compare match. */
        if(MTU_ACTION_OUTPUT & pconfig->timer_c.actions.do_action)      // Output to a pin
        {
            *my_handle->regs.tiorl |= pconfig->timer_c.actions.output;  // Set bits in lower nibble

            #ifndef BSP_MCU_RX110
            if (MTU_CHANNEL_4 == channel)
            {
                MTU.TOER.BIT.OE4C = 1; // Must also turn on Timer Output Master Enable Register for this channel.
            }
            #endif
        }

        if((MTU_ACTION_INTERRUPT & pconfig->timer_c.actions.do_action)
          || (MTU_ACTION_CALLBACK & pconfig->timer_c.actions.do_action)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_C] = 1;            // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->timer_c.actions.do_action)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_C] = 1; // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIEC;     // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->timer_c.actions.do_action) && (MTU_CLR_TGRC == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1; // Continuous running
        }
    }

    if(MTU_ACTION_NONE != pconfig->timer_d.actions.do_action)    // MTU_ACTION_NONE means this timer event not used.
    {
        if(MTU_CLK_SRC_INTERNAL ==  pconfig->clock_src.source)
        {
            /* Set compare match register with the value calculated from requested frequency. */
            tgr_value = mtu_calc_tgr_ticks(pclk_divisor, pconfig->timer_d.freq);

            if(0 != tgr_value)
            {
                *my_handle->regs.tgrd  = tgr_value;
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_ARG_RANGE;     // Could not obtain requested frequency.
            }
        }
        else
        {
            *my_handle->regs.tgrd  = pconfig->timer_d.freq; // External clock source. Use freq as direct TGR count.
        }

        /* Set up actions to perform on compare match. */
        if(MTU_ACTION_OUTPUT & pconfig->timer_d.actions.do_action)    // Output to a pin
        {
            *my_handle->regs.tiorl |=  (pconfig->timer_d.actions.output << 4); // Move bits to upper nibble

            #ifndef BSP_MCU_RX110
            /* Must also turn on Timer Output Master Enable Register for these channels. */
            if (MTU_CHANNEL_3 == channel)
            {
                MTU.TOER.BIT.OE3D = 1;
            }
            if (MTU_CHANNEL_4 == channel)
            {
                MTU.TOER.BIT.OE4D = 1;
            }
            #endif
        }

        if((MTU_ACTION_INTERRUPT & pconfig->timer_d.actions.do_action)
          || (MTU_ACTION_CALLBACK & pconfig->timer_d.actions.do_action)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_D] = 1;            // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->timer_d.actions.do_action)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_D] = 1;          // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIED;                 // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->timer_d.actions.do_action) && (MTU_CLR_TGRD == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;             // Continuous running
        }
    }

    g_mtu_channel_mode[channel] = MTU_MODE_COMPARE_MATCH;   // Tag the channel is in use for compare match.
    g_num_channels_in_use++;                                // Add this channel to the count.
    *g_mtu_handles[channel]->p_callback = pcallback;

    #if MTU_CFG_REQUIRE_LOCK == 1
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
    #endif

    return MTU_SUCCESS;    // Ready to start count operation now with a start command.
}
/* end of function R_MTU_Timer_Open(). */
#endif


#if MTU_CFG_USE_CAPTURE == 1
/***********************************************************************************************************************
* Function Name: R_MTU_Capture_Open
* Description  : This function applies power to the MTU channel,
*                initializes the associated registers,
*                applies user-configurable options,
*                and provides the channel handle for use with other API functions.
* Arguments    : channel -
*                   Number of the MTU channel to be initialized
*                pconfig -
*                   Pointer to MTU channel configuration data structure.
*                pcallback -
*                   Pointer to function called from interrupt
* Return Value : MTU_SUCCESS-
*                   Successful; channel initialized
*                MTU_TIMERS_ERR_BAD_CHAN-
*                   Channel number is invalid for part
*                MTU_TIMERS_ERR_CH_NOT_CLOSED-
*                   Channel currently in operation; Perform R_MTU_Close() first
*                MTU_TIMERS_ERR_NULL_PTR-
*                   pconfig pointer is NULL
*                MTU_TIMERS_ERR_INVALID_ARG-
*                   An element of the pconfig structure contains an invalid value.
*                MTU_TIMERS_ERR_LOCK-
*                      The lock could not be acquired. The channel is busy.
***********************************************************************************************************************/
mtu_err_t   R_MTU_Capture_Open (mtu_channel_t                channel,
                                mtu_capture_chnl_settings_t *pconfig,
                                void                       (*pcallback)(void *pdata))
{
    mtu_handle_t  my_handle;
    uint8_t tcr_bits = 0;

    #if MTU_CFG_REQUIRE_LOCK == 1
    bool        lock_result = false;
    #endif

    #if MTU_CFG_PARAM_CHECKING_ENABLE == 1
    if (MTU_CHANNEL_MAX <= channel)           // First check for channel number out of range
    {
        return MTU_ERR_BAD_CHAN;
    }

    if (NULL == g_mtu_handles[channel])       // Now check that channel has been configured for build
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
        return MTU_ERR_CH_NOT_CLOSED;   // This channel has already been initialized.
    }

    if ((MTU_CHANNEL_1 == channel) || (MTU_CHANNEL_2 == channel))
    {
        if((MTU_ACTION_NONE != pconfig->capture_c.actions) && (MTU_ACTION_NONE != pconfig->capture_d.actions))
        {
            return MTU_ERR_INVALID_ARG; // Resource not present on these channels.
        }
    }

    /* Check counter clearing source for capture. */
    switch (pconfig->clear_src)
    {
        case MTU_CLR_DISABLED:
        case MTU_CLR_TGRA:
        case MTU_CLR_TGRB:
        case MTU_CLR_TGRC:
        case MTU_CLR_TGRD:
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

    mtu_interrupts_disable(channel);
    mtu_interrupts_clear(channel);
    *my_handle->regs.ipr = my_handle->priority; // Set the priority register from config.h value.

    power_on_off(MTU_POWER_ON); // Make sure channel is powered on.

    mtu_channel_clear(channel);

    tcr_bits = g_chnl_clear_src[channel][pconfig->clear_src]; // Select counter clearing source for capture.
    g_mtu_channel_clr_src[channel] = pconfig->clear_src;      // Keep a global copy for ISRs.

    switch (pconfig->clock_src.source)    // Select counter clock source
    {
        case MTU_CLK_SRC_INTERNAL:
        {
            /* Only internal clock (PCLK) can be scaled with a divisor. */
            switch (pconfig->clock_div)
            {
                case MTU_SRC_CLK_DIV_1:
                case MTU_SRC_CLK_DIV_4:
                case MTU_SRC_CLK_DIV_16:
                case MTU_SRC_CLK_DIV_64:
                case MTU_SRC_CLK_DIV_256:
                case MTU_SRC_CLK_DIV_1024:
                {
                    /* Find the bits to set this in the table. Not all channels have this setting. */
                    if(MTU_NOT_SUPP != g_chnl_clk_divs[channel][pconfig->clock_div])
                    {
                        /* Set the clock divisor. */
                        tcr_bits |= g_chnl_clk_divs[channel][pconfig->clock_div]; // Save divisor bits for later.
                    }
                    else
                    {
                        #if MTU_CFG_REQUIRE_LOCK == 1
                        R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                        #endif
                        return MTU_ERR_INVALID_ARG;
                    }
                }
                break;
                default:
                {
                    #if MTU_CFG_REQUIRE_LOCK == 1
                    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                    #endif
                    return MTU_ERR_INVALID_ARG;
                }
            }
        }
        break;

        case MTU_CLK_SRC_EXT_MTCLKA:
        case MTU_CLK_SRC_EXT_MTCLKB:
        case MTU_CLK_SRC_EXT_MTCLKC:
        case MTU_CLK_SRC_EXT_MTCLKD:
        case MTU_CLK_SRC_CASCADE:
        {   /* Find the bits to set this in the table. Not all channels have this setting. */
            if(MTU_NOT_SUPP != g_chnl_ext_clks[channel][pconfig->clock_src.source])
            {
                tcr_bits |= g_chnl_ext_clks[channel][pconfig->clock_src.source];
            }
            else
            {
                #if MTU_CFG_REQUIRE_LOCK == 1
                R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
                #endif
                return MTU_ERR_INVALID_ARG; // Not supported by this channel
            }
        }
        break;

        default:
        {
            #if MTU_CFG_REQUIRE_LOCK == 1
            R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
            #endif
            return MTU_ERR_INVALID_ARG;
        }
    }

    tcr_bits |= pconfig->clock_src.clock_edge;      // Set clock active edge.
    *my_handle->regs.tcr = tcr_bits;                // Copy the completed setting to the TCR register.
    *my_handle->regs.nfcr = my_handle->filt_clk;    // Set the noise filter clock source.

    if(MTU_ACTION_CAPTURE & pconfig->capture_a.actions)
    {
        /* Set up capture pin edge parameters. */
        *my_handle->regs.tiorh |= pconfig->capture_a.capture_edge;  // Set bits in lower nibble

        /* Set up capture pin noise filter parameters. */
        if (true == pconfig->capture_a.filter_enable)
        {
            *my_handle->regs.nfcr |= MTU_FILT_EN_A;
        }
        else
        {
            *my_handle->regs.nfcr &= ~MTU_FILT_EN_A;
        }

        /* Set up actions to perform on capture event. */
        if((MTU_ACTION_INTERRUPT & pconfig->capture_a.actions)
          || (MTU_ACTION_CALLBACK & pconfig->capture_a.actions)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_A] = 1;  // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->capture_a.actions)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_A] = 1; // Do the callback for this interrupt.
            }
        }

        if (MTU_ACTION_TRIGGER_ADC & pconfig->capture_a.actions)
        {
        	*my_handle->regs.tier |= MTU_ADC_TRIG;   // Set ADC TTGE trigger bit in MTU register.
        }

        *my_handle->regs.tier |= MTU_TGIEA;             // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->capture_a.actions) && (MTU_CLR_TGRA == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;         // Continuous running
        }
    }

    if(MTU_ACTION_CAPTURE & pconfig->capture_b.actions)
    {
        /* Set up capture pin edge parameters. */
        *my_handle->regs.tiorh |=  (pconfig->capture_b.capture_edge << 4); // Move bits to upper nibble

        /* Set up capture pin noise filter parameters. */
        if (true == pconfig->capture_b.filter_enable)
        {
            *my_handle->regs.nfcr |= MTU_FILT_EN_B;
        }
        else
        {
            *my_handle->regs.nfcr &= ~MTU_FILT_EN_B;
        }

        if((MTU_ACTION_INTERRUPT & pconfig->capture_b.actions)
          || (MTU_ACTION_CALLBACK & pconfig->capture_b.actions)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_B] = 1;    // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->capture_b.actions)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_B] = 1;  // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIEB;             // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->capture_b.actions) && (MTU_CLR_TGRB == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;         // Continuous running
        }
    }

    if(MTU_ACTION_CAPTURE & pconfig->capture_c.actions) // Non-zero value means use this timer TGR. Zero means not used.
    {
        /* Set up capture pin edge parameters. */
        *my_handle->regs.tiorl |= pconfig->capture_c.capture_edge;  // Set bits in lower nibble

        /* Set up capture pin noise filter parameters. */
        if (true == pconfig->capture_c.filter_enable)
        {
            *my_handle->regs.nfcr |= MTU_FILT_EN_C;
        }
        else
        {
            *my_handle->regs.nfcr &= ~MTU_FILT_EN_C;
        }

        if((MTU_ACTION_INTERRUPT & pconfig->capture_c.actions)
          || (MTU_ACTION_CALLBACK & pconfig->capture_c.actions)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_C] = 1;  // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->capture_c.actions)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_C] = 1; // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIEC;                // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->capture_c.actions) && (MTU_CLR_TGRC == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;            // Continuous running
        }
    }

    if(MTU_ACTION_CAPTURE & pconfig->capture_d.actions)    // Non-zero value = use this timer TGR. Zero = not used.
    {
        /* Set up capture pin edge parameters. */
        *my_handle->regs.tiorl &= 0x0F;                                   // First clear the upper nibble.
        *my_handle->regs.tiorl |= (pconfig->capture_d.capture_edge << 4); // Move bits to upper nibble

        /* Set up capture pin noise filter parameters. */
        if (true == pconfig->capture_d.filter_enable)
        {
            *my_handle->regs.nfcr |= MTU_FILT_EN_D;
        }
        else
        {
            *my_handle->regs.nfcr &= ~MTU_FILT_EN_D;
        }

        if((MTU_ACTION_INTERRUPT & pconfig->capture_d.actions)
          || (MTU_ACTION_CALLBACK & pconfig->capture_d.actions)) // Request an interrupt
        {
            g_mtu_tgi_icu_en_flags[channel][MTU_TIMER_D] = 1;    // Set a software control flag

            if (MTU_ACTION_CALLBACK & pconfig->capture_d.actions)
            {
                 g_mtu_tgr_callbacks[channel][MTU_TIMER_D] = 1;  // Do the callback for this interrupt.
            }
        }

        *my_handle->regs.tier |= MTU_TGIED;             // Always set interrupt enable bit in MTU register.

        /* Set repeat mode if option selected and this TGR is clear source.*/
        if((MTU_ACTION_REPEAT & pconfig->capture_d.actions) && (MTU_CLR_TGRD == pconfig->clear_src))
        {
            g_mtu_channel_repeats[channel] = 1;             // Continuous running
        }
    }

    g_mtu_channel_mode[channel] = MTU_MODE_INPUT_CAPTURE;   // Tag the channel is in use for input capture.
    g_num_channels_in_use++;                                // Add this channel to the count.
    *g_mtu_handles[channel]->p_callback = pcallback;

    #if MTU_CFG_REQUIRE_LOCK == 1
    R_BSP_HardwareUnlock((mcu_lock_t)(BSP_LOCK_MTU0 + channel));
    #endif

    return MTU_SUCCESS;     // Ready to start capture operation now.
}
#endif
/* end of function R_MTU_Capture_Open(). */
