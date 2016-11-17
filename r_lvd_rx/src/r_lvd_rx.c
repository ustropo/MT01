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
* File Name    : r_lvd_rx.c
* Description  : This module implements the LVD API that can be used to configure the LVD module.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 18.07.2013 1.00    First Release
*         : 14.02.2014 1.10    Added support for RX110, RX113, RX210, RX63N
*         : 22.12.2014 1.30    Added support for RX113.
*         : 18.03.2015 1.40    Added support for RX64M, 71M.
*         : 09.07.2015 1.50    Added support for RX231.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* include definition for NULL*/
#include <stddef.h>
/* Defines machine level functions used in this file */
#include <machine.h>
/* Includes board and MCU related header files. */
#include "platform.h"
/* Private header file for this package. */
#include "r_lvd_rx.h"
/* Public interface header file for this package. */
#include "r_lvd_rx_if.h"
/* Configuration for this package. */
#include "r_lvd_rx_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
static lvd_err_t        lvd_level_set(lvd_channel_t e_channel, lvd_voltage_level_t e_voltage_level);
static lvd_err_t        lvd_status_get(lvd_channel_t e_channel);
static lvd_err_t        lvd_status_clear(lvd_channel_t e_channel);
static void             lvd_delay(uint32_t time_us);

void (*lvd1_isr_handler)(void *) = NULL;  // Function pointer for LVD Channel 1 ISR
void (*lvd2_isr_handler)(void *) = NULL;  // Function pointer for LVD Channel 2 ISR
static bool g_lvd_ch1_open = false;       // Flag to maintain Initialization status of LVD Channel 1
static bool g_lvd_ch2_open = false;       // Flag to maintain Initialization status of LVD Channel 2
lvd_int_cb_args_t g_lvd_ch1_cb_args;      // Callback argument structure for CH 1
lvd_int_cb_args_t g_lvd_ch2_cb_args;      // Callback argument structure for CH 2

/***********************************************************************************************************************
* Function Name: R_LVD_Open
* Description  : This function configures the LVD channel including voltage level, trigger type and sets the callback
*                function. This function can only be called once for each channel after which R_LVD_Close has to be
*                called before attempting to reconfigure the LVD channel.
* Arguments    : lvd_channel_t e_channel
*                       -Specify the channel
*                lvd_voltage_level_t e_voltage_level
*                       -Specify the LVD voltage level
*                lvd_event_action_t e_event
*                       -Specify the LVD event that should occur when the trigger specified in e-trigger is met.
*                lvd_trigger_t e_trigger
*                       -Specify the LVD voltage level triggering type. It can be configured for when the voltage
*                       level rises above the triggering level,  or when it falls below or both conditions.
*                void (*pcallback)(void)
*                       -callback function pointer. This can be a null if e_event is LVD_EVENT_RESET or LVD_EVENT_POLL
* Return Value :LVD_SUCCESS
*               LVD_ERR_ILL_PARAM
*               LVD_ERR_ILL_REINITIALIZATION
*               LVD_ERR_VDET
*
***********************************************************************************************************************/
lvd_err_t R_LVD_Open(lvd_channel_t e_channel, lvd_config_t *p_cfg, void (*pcallback)(void *))
{
#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_channel >= LVD_CHANNEL_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }

    if (LVD_CHANNEL_2 == e_channel)
    {
        if ((p_cfg->e_voltage_level < LVD_VOLTAGE_CH2_MIN) || (p_cfg->e_voltage_level > LVD_VOLTAGE_CH2_MAX))
        {
            return LVD_ERR_VDET;
        }
    }
    else if (LVD_CHANNEL_1 == e_channel)
    {
        if ((p_cfg->e_voltage_level < LVD_VOLTAGE_CH1_MIN) || (p_cfg->e_voltage_level > LVD_VOLTAGE_CH1_MAX))
        {
            return LVD_ERR_VDET;
        }
    }

    if (p_cfg->e_action >= LVD_ACTION_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }

    if(p_cfg->e_trigger >= LVD_TRIGGER_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }

    if (((pcallback == FIT_NO_FUNC) || (pcallback == NULL)) &&
         ((LVD_ACTION_RESET != p_cfg->e_action) && (LVD_ACTION_POLL != p_cfg->e_action)))
    {
        return LVD_ERR_ILL_PARAM;
    }

#endif

    if (LVD_CHANNEL_1 == e_channel)
    {
#if (LVD_CFG_CHANNEL_1_USED == 1)
        if (g_lvd_ch1_open == true)
        {
            return LVD_ERR_ILL_REINITIALIZATION;
        }

        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);      // unlock LVD control registers
        SYSTEM.LVDLVLR.BIT.LVD1LVL = p_cfg->e_voltage_level;    //set voltage level
#if (BSP_MCU_RX210 == 1)
        SYSTEM.LVCMPCR.BIT.EXVREFINP1 = 0;          // Select internal reference voltage
        SYSTEM.LVCMPCR.BIT.EXVCCINP1 = 0;           // Select Vcc to compare
        SYSTEM.LVD1CR0.BIT.LVD1DFDIS = 1;           // Disable digital filter
#endif
        if (LVD_ACTION_RESET == p_cfg->e_action)
        {
            /*Disable Interrupt/RESET. Enable RESET on Vcc falling to or below Vdet1, Set Reset Negation Stabilization*/
            SYSTEM.LVD1CR0.BYTE = (uint8_t)( 0x40 | (LVD_CFG_STABILIZATION_CHANNEL_1 << 7));
            SYSTEM.LVD1SR.BIT.LVD1DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD1CR0.BIT.LVD1RIE == 0)
            {
                SYSTEM.LVD1SR.BIT.LVD1DET = 0;      // 2 PCLKB delay
            }
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 1;         // Enable LVD1 Interrupt/RESET
            SYSTEM.LVCMPCR.BIT.LVD1E = 1;           // Enable the LVD1 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD1CR0.BIT.LVD1CMPE = 1;        // Enable comparison output
        }
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
        else if(LVD_ACTION_IRQ == p_cfg->e_action)
        {
            lvd1_isr_handler = pcallback;                   // assign interrupt callback
            g_lvd_ch1_cb_args.vector = BSP_INT_SRC_LVD1;    // Set vector source for ISR callback argument

            /*Disable Interrupt/RESET. Enable Interrupt on Vcc crossing Vdet1, Set Reset Negation Stabilization*/
            SYSTEM.LVD1CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_1 << 7));
            SYSTEM.LVD1CR1.BYTE = (uint8_t)(p_cfg->e_trigger | (0x04)); // Set detection type and IRQ interrupt
            SYSTEM.LVCMPCR.BIT.LVD1E = 1;           // Enable the LVD1 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD1CR0.BIT.LVD1CMPE = 1;        // Enable comparison output
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 0;         // Disable LVD1 Interrupt/RESET. Set to 0 before clearing LVDnDET next
            SYSTEM.LVD1SR.BIT.LVD1DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD1CR0.BIT.LVD1RIE == 0)
            {
                SYSTEM.LVD1SR.BIT.LVD1DET = 0;      // 2 PCLKB delay
            }
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 1;         // Enable LVD1 Interrupt/RESET
            IPR(LVD,LVD1) = LVD_CFG_INTERRUPT_PRIORITY_CHANNEL_1;
            IEN(LVD,LVD1) = 1;
        }
#endif
        else if(LVD_ACTION_NMI == p_cfg->e_action)
        {
            R_BSP_InterruptWrite(BSP_INT_SRC_LVD1, (bsp_int_cb_t)pcallback); // assign interrupt callback

            /*Disable Interrupt/RESET. Enable Interrupt on Vcc crossing Vdet1, Set Reset Negation Stabilization*/
            SYSTEM.LVD1CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_1 << 7));
            SYSTEM.LVD1CR1.BYTE = (uint8_t)(p_cfg->e_trigger & (0x07)); // Set detection type and NMI interrupt
            SYSTEM.LVCMPCR.BIT.LVD1E = 1;           // Enable the LVD1 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD1CR0.BIT.LVD1CMPE = 1;        // Enable comparison output
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 0;         // Disable LVD1 Interrupt/RESET. Set to 0 before clearing LVDnDET next
            SYSTEM.LVD1SR.BIT.LVD1DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD1CR0.BIT.LVD1RIE == 0)
            {
                SYSTEM.LVD1SR.BIT.LVD1DET = 0;      // 2 PCLKB delay
            }
#if ((BSP_MCU_RX63N == 1) || (BSP_MCU_RX64M == 1) || (BSP_MCU_RX71M == 1))
            if (SYSTEM.LOCOCR.BIT.LCSTP == 1)
            {
                SYSTEM.LVD1CR0.BIT.LVD1DFDIS = 1;   // Disable digital filter if LOCO turned off. Otherwise LVD will not monitor.
            }
#endif
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 1;         // Enable LVD1 Interrupt/RESET
            ICU.NMIER.BIT.LVD1EN = 1;               // Enable the LVD NMI interrupt
        }
        else if(LVD_ACTION_POLL == p_cfg->e_action)
        {
             /*Disable Interrupt/RESET. Set Reset Negation Stabilization*/
            SYSTEM.LVD1CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_1 << 7));
            SYSTEM.LVCMPCR.BIT.LVD1E = 1;           // Enable the LVD1 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD1CR0.BIT.LVD1CMPE = 1;        // Enable comparison output
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 0;         // Disable LVD1 Interrupt/RESET. Set to 0 before clearing LVDnDET next
        }
        else
        {
            //nothing here
        }

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);           // lock LVD control registers
        g_lvd_ch1_open = true;                                      // set static flag to prevent improper reintialization
#else
        return LVD_ERR_ILL_PARAM;                                   // Code not enabled for this channel
#endif
    }
    else if (LVD_CHANNEL_2 == e_channel)
    {
#if (LVD_CFG_CHANNEL_2_USED == 1)
        if (g_lvd_ch2_open == true)            // check for improper reinitialization
        {
            return LVD_ERR_ILL_REINITIALIZATION;
        }

        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);          // unlock LVD control registers
        SYSTEM.LVDLVLR.BIT.LVD2LVL = p_cfg->e_voltage_level;        //set voltage level
#if (BSP_MCU_RX210 == 1)
        SYSTEM.LVCMPCR.BIT.EXVREFINP2 = 0;                          // Select internal reference voltage
        SYSTEM.LVD1CR0.BIT.LVD1DFDIS = 1;                           // Disable digital filter
#endif
#if ((BSP_MCU_RX63N == 1) || (BSP_MCU_RX64M == 1) || (BSP_MCU_RX71M == 1))
        //Only Vcc can be monitored.
#else
        SYSTEM.LVCMPCR.BIT.EXVCCINP2 = LVD_CFG_VDET2_VCC_CMPA2;     // Select Vcc or CMPA2 to compare
#endif

        if (LVD_ACTION_RESET == p_cfg->e_action)
        {
             /*Disable Interrupt/RESET. Enable RESET on Vcc falling to or below Vdet2, Set Reset Negation Stabilization*/
            SYSTEM.LVD2CR0.BYTE = (uint8_t)( 0x40 | (LVD_CFG_STABILIZATION_CHANNEL_2 << 7));
            SYSTEM.LVD2SR.BIT.LVD2DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD2CR0.BIT.LVD2RIE == 0)
            {
                SYSTEM.LVD2SR.BIT.LVD2DET = 0;      // 2 PCLKB delay
            }
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 1;         // Enable LVD2 Interrupt/RESET
            SYSTEM.LVCMPCR.BIT.LVD2E = 1;           // Enable the LVD2 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD2CR0.BIT.LVD2CMPE = 1;        // Enable comparison output
        }
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
	 (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
        else if(LVD_ACTION_IRQ == p_cfg->e_action)
        {
            lvd2_isr_handler = pcallback;                   // assign interrupt callback
            g_lvd_ch2_cb_args.vector = BSP_INT_SRC_LVD2;    // Set vector source for ISR callback argument

            /*Disable Interrupt/RESET. Enable Interrupt on Vcc crossing Vdet1, Set Reset Negation Stabilization*/
            SYSTEM.LVD2CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_2 << 7));
            SYSTEM.LVD2CR1.BYTE = (uint8_t)(p_cfg->e_trigger | (0x04)); // Set detection type and IRQ interrupt
            SYSTEM.LVCMPCR.BIT.LVD2E = 1;           // Enable the LVD2 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD2CR0.BIT.LVD2CMPE = 1;        // Enable comparison output
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 0;         // Disable LVD2 Interrupt/RESET. Set to 0 before clearing LVDnDET next
            SYSTEM.LVD2SR.BIT.LVD2DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD2CR0.BIT.LVD2RIE == 0)
            {
                SYSTEM.LVD2SR.BIT.LVD2DET = 0;      // 2 PCLKB delay
            }
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 1;         // Enable LVD2 Interrupt/RESET
            IPR(LVD,LVD2) = LVD_CFG_INTERRUPT_PRIORITY_CHANNEL_2;
            IEN(LVD,LVD2) = 1;
        }
#endif
        else if(LVD_ACTION_NMI == p_cfg->e_action)
        {
            R_BSP_InterruptWrite(BSP_INT_SRC_LVD2, (bsp_int_cb_t)pcallback); // assign interrupt callback

            /*Disable Interrupt/RESET. Enable Interrupt on Vcc crossing Vdet1, Set Reset Negation Stabilization*/
            SYSTEM.LVD2CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_2 << 7));
            SYSTEM.LVD2CR1.BYTE = (uint8_t)(p_cfg->e_trigger & (0x07)); // Set detection type and NMI interrupt
            SYSTEM.LVCMPCR.BIT.LVD2E = 1;           // Enable the LVD2 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD2CR0.BIT.LVD2CMPE = 1;        // Enable comparison output
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 0;         // Disable LVD2 Interrupt/RESET. Set to 0 before clearing LVDnDET next
            SYSTEM.LVD2SR.BIT.LVD2DET = 0;          // Clear the passage detection status bit
            // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
               The SFR registers are read over the peripheral bus and thus clocked by PCLKB
            if (SYSTEM.LVD2CR0.BIT.LVD2RIE == 0)
            {
                SYSTEM.LVD2SR.BIT.LVD2DET = 0;      // 2 PCLKB delay
            }
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 1;         // Enable LVD2 Interrupt/RESET
#if ((BSP_MCU_RX63N == 1) || (BSP_MCU_RX64M == 1) || (BSP_MCU_RX71M == 1))
            if (SYSTEM.LOCOCR.BIT.LCSTP == 1)
            {
                SYSTEM.LVD2CR0.BIT.LVD2DFDIS = 1;   // Disable digital filter if LOCO turned off. Otherwise LVD will not monitor.
            }
#endif
            ICU.NMIER.BIT.LVD2EN = 1;               // Enable the LVD NMI interrupt
        }
        else if(LVD_ACTION_POLL == p_cfg->e_action)
        {
             /*Disable Interrupt/RESET. Set Reset Negation Stabilization*/
            SYSTEM.LVD2CR0.BYTE = (uint8_t)( 0x80 & (LVD_CFG_STABILIZATION_CHANNEL_2 << 7));
            SYSTEM.LVCMPCR.BIT.LVD2E = 1;           // Enable the LVD2 circuit
            lvd_delay(300);                         // Implement 300usec wait
            SYSTEM.LVD2CR0.BIT.LVD2CMPE = 1;        // Enable comparison output
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 0;         // Disable LVD2 Interrupt/RESET. Set to 0 before clearing LVDnDET next
            SYSTEM.LVD2SR.BIT.LVD2DET = 0;          // Clear the passage detection status bit
        }
        else
        {
            //nothing here
        }

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);           // lock LVD control registers
        g_lvd_ch2_open = true;                                      // set static flag to prevent reintialization before closing channel
#else
        return LVD_ERR_ILL_PARAM;                                   // Code not enabled for this channel
#endif
    }
    else
    {
        //nothing
    }

    return LVD_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: R_LVD_Control
* Description  : This function is used as a routing function to implement a control and execute method for
*                available LVD features including
*                1. Changing detection voltage level
*                2. Reading status of the LVD Channel
*                3. Clearing the status of an LVD Channel
* Arguments    : lvd_cmd_t const e_cmd
*                       -Specify the command
*                void *param
*                       -Pointer to any arguments that may be required to execute the command.
* Return Value : LVD_SUCCESS
*                LVD_ERR_ILL_PARAM
*                LVD_ERR_NOT_INITIALIZED
*                LVD_ERR_VDET
*                LVD_ERR_DISABLED
*                LVD_ERR_VDET_BELOW_AND_NOT_CROSSED
*                LVD_ERR_VDET_BELOW_AND_CROSSED
*                LVD_ERR_VDET_ABOVE_AND_NOT_CROSSED
*                LVD_ERR_VDET_ABOVE_AND_CROSSED
***********************************************************************************************************************/
lvd_err_t R_LVD_Control(lvd_cmd_t const e_cmd, void *param)
{
    lvd_err_t err;

#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_cmd >= LVD_CMD_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }
    if ((param == NULL) || (param == FIT_NO_PTR))
    {
        return LVD_ERR_ILL_PARAM;
    }

#endif
    switch (e_cmd)
    {
        case LVD_CMD_LEVEL_SET:
            err = lvd_level_set(((lvd_lvl_cfg_t *)param)->e_channel, ((lvd_lvl_cfg_t *)param)->e_voltage_lvl);
        break;

        case LVD_CMD_STATUS_GET:
            err = lvd_status_get(((lvd_status_t *)param)->e_channel);
        break;

        case LVD_CMD_STATUS_CLEAR:
            err = lvd_status_clear(((lvd_status_t *)param)->e_channel);
        break;
        default:
            err = LVD_ERR_ILL_PARAM;
    }

    return err;
}

/***********************************************************************************************************************
* Function Name: R_LVD_Close
* Description  : This function closes the specified LVD channel.
* Arguments    : lvd_channel_t e_channel
*                       -Specify the channel
* Return Value : LVD_SUCCESS
*                LVD_ERR_ILL_PARAM
***********************************************************************************************************************/
lvd_err_t R_LVD_Close(lvd_channel_t e_channel)
{
#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_channel >= LVD_CHANNEL_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }
#endif
    if (LVD_CHANNEL_1 == e_channel)
    {
#if (LVD_CFG_CHANNEL_1_USED ==1)
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers
        SYSTEM.LVD1CR0.BYTE = 0x00;                         // Disable LVD Monitoring and comparison output
        SYSTEM.LVCMPCR.BIT.LVD1E = 0;                       // Disable Voltage Detection circuit
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);   // lock LVD control registers
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
        IEN(LVD,LVD1) = 0;                                  // Disable interrupt
#endif
        g_lvd_ch1_open = false;                             // Set static status flag
#else
        return LVD_ERR_ILL_PARAM;                           // Code not enabled for this channel
#endif
    }
    else if (LVD_CHANNEL_2 == e_channel)
    {
#if (LVD_CFG_CHANNEL_2_USED ==1)
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers
        SYSTEM.LVD2CR0.BYTE = 0x00;                         // Disable LVD Monitoring and comparison output
        SYSTEM.LVCMPCR.BIT.LVD2E = 0;                       // Disable Voltage Detection circuit
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);   // lock LVD control registers
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
        IEN(LVD,LVD2) = 0;                                  // Disable interrupt
#endif
        g_lvd_ch2_open = false;                             // Set static status flag
#else
        return LVD_ERR_ILL_PARAM;                           // Code not enabled for this channel
#endif
    }
    else
    {
        //nothing
    }

    lvd_status_clear(e_channel);                            // Clear Status registers
    return LVD_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: lvd_level_set
* Description  : This function can be used to change the voltage detection levels.
*                Note that this function WILL clear the detection status bit.
* Arguments    : lvd_channel_t e_channel
*                       -Specify the channel
*                lvd_voltage_level_t e_voltage_level
*                       -Set the new voltage detection level
* Return Value : LVD_SUCCESS
*                LVD_ERR_ILL_PARAM
*                LVD_ERR_NOT_INITIALIZED
*                LVD_ERR_VDET
***********************************************************************************************************************/
static lvd_err_t  lvd_level_set(lvd_channel_t e_channel, lvd_voltage_level_t e_voltage_level)
{
    uint8_t temp_status_reg;

#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_channel >= LVD_CHANNEL_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }

    if (LVD_CHANNEL_2 == e_channel)
    {
        if ((e_voltage_level < LVD_VOLTAGE_CH2_MIN) || (e_voltage_level > LVD_VOLTAGE_CH2_MAX))
        {
            return LVD_ERR_VDET;
        }
    }
    else if (LVD_CHANNEL_1 == e_channel)
    {
        if ((e_voltage_level < LVD_VOLTAGE_CH1_MIN) || (e_voltage_level > LVD_VOLTAGE_CH1_MAX))
        {
            return LVD_ERR_VDET;
        }
    }
#endif

    if (LVD_CHANNEL_1 == e_channel)
    {
#if (LVD_CFG_CHANNEL_1_USED ==1)
        if (g_lvd_ch1_open == false)                        // check for initialization
        {
            return LVD_ERR_NOT_INITIALIZED;
        }

        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers

        SYSTEM.LVCMPCR.BIT.LVD1E = 0;                       //Disable the LVD1 circuit
        SYSTEM.LVD1CR0.BIT.LVD1CMPE = 0;                    //Disable LVD comparison output
        SYSTEM.LVDLVLR.BIT.LVD1LVL = e_voltage_level;       //set voltage level
        SYSTEM.LVCMPCR.BIT.LVD1E = 1;                       // Enable the LVD1 circuit
        lvd_delay(300);                                     // Implement 300usec wait
        SYSTEM.LVD1CR0.BIT.LVD1CMPE = 1;                    // Enable LVD comparison output
        temp_status_reg = SYSTEM.LVD1CR0.BIT.LVD1RIE;        // save interrupt/reset setting
        SYSTEM.LVD1CR0.BIT.LVD1RIE = 0;                     // Disable LVD2 Interrupt/RESET. Set to 0 before clearing LVDnDET next
        SYSTEM.LVD1SR.BIT.LVD1DET = 0;                      // Clear the passage detection status bit
        // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
           The SFR registers are read over the peripheral bus and thus clocked by PCLKB
        if (SYSTEM.LVD1CR0.BIT.LVD1RIE == 0)
        {
            SYSTEM.LVD1SR.BIT.LVD1DET = 0;                  // 2 PCLKB delay
        }
        if (temp_status_reg)
        {
            SYSTEM.LVD1CR0.BIT.LVD1RIE = 1;                 // Enable LVD1 Interrupt/RESET
        }

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);   // lock LVD control registers
#else
        return LVD_ERR_ILL_PARAM;             // Code not enabled for this channel
#endif
    }
    else if (LVD_CHANNEL_2 == e_channel)
    {
#if (LVD_CFG_CHANNEL_2_USED ==1)
        if (g_lvd_ch2_open == false)                        // check for initialization
        {
            return LVD_ERR_NOT_INITIALIZED;
        }
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers

        SYSTEM.LVCMPCR.BIT.LVD2E = 0;                       //Disable the LVD2 circuit
        SYSTEM.LVD2CR0.BIT.LVD2CMPE = 0;                    //Disable LVD comparison output
        SYSTEM.LVDLVLR.BIT.LVD2LVL = e_voltage_level;       //set voltage level
        SYSTEM.LVCMPCR.BIT.LVD2E = 1;                       // Enable the LVD2 circuit
        lvd_delay(300);                                     // Implement 300usec wait
        SYSTEM.LVD2CR0.BIT.LVD2CMPE = 1;                    // Enable LVD comparison output
        temp_status_reg = SYSTEM.LVD2CR0.BIT.LVD2RIE;       // save interrupt/reset setting
        SYSTEM.LVD2CR0.BIT.LVD2RIE = 0;                     // Disable LVD2 Interrupt/RESET. Set to 0 before clearing LVDnDET next
        SYSTEM.LVD2SR.BIT.LVD2DET = 0;                      // Clear the passage detection status bit
        // At least 2 PCLKB cycles req'd before LVDnRIE can be set after clearing. This is just an easy way to do it\
           The SFR registers are read over the peripheral bus and thus clocked by PCLKB
        if (SYSTEM.LVD2CR0.BIT.LVD2RIE == 0)
        {
            SYSTEM.LVD2SR.BIT.LVD2DET = 0;                  // 2 PCLKB delay
        }
        if (temp_status_reg)
        {
            SYSTEM.LVD2CR0.BIT.LVD2RIE = 1;                 // Enable LVD2 Interrupt/RESET
        }

        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD); // lock LVD control registers
#else
        return LVD_ERR_ILL_PARAM;             // Code not enabled for this channel
#endif
    }

    return LVD_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: lvd_status_get
* Description  : This function returns the status of the LVD circuit for the specified channel.
* Arguments    : lvd_channel_t e_channel
*                       -Specify the channel
* Return Value : LVD_ERR_ILL_PARAM
*                LVD_ERR_DISABLED
*                LVD_ERR_VDET_BELOW_AND_NOT_CROSSED
*                LVD_ERR_VDET_BELOW_AND_CROSSED
*                LVD_ERR_VDET_ABOVE_AND_NOT_CROSSED
*                LVD_ERR_VDET_ABOVE_AND_CROSSED
***********************************************************************************************************************/
static lvd_err_t lvd_status_get(lvd_channel_t e_channel)
{
    uint8_t temp_status_reg;

#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_channel >= LVD_CHANNEL_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }
#endif
    if (LVD_CHANNEL_1 == e_channel)
    {
#if (LVD_CFG_CHANNEL_1_USED ==1)
        if ((0 == SYSTEM.LVCMPCR.BIT.LVD1E) || (0 == SYSTEM.LVD1CR0.BIT.LVD1CMPE))
        {
            return LVD_ERR_DISABLED;
        }
        else
        {
            temp_status_reg = (uint8_t)(SYSTEM.LVD1SR.BYTE & 0x03);     // zero out the unused higher 6 bits
            return (lvd_err_t)(temp_status_reg | 0x10);                 // setting the msb to 1 for unique return error code
        }
#else
        return LVD_ERR_ILL_PARAM;             // Code not enabled for this channel
#endif
    }
    else if (LVD_CHANNEL_2 == e_channel)
    {
#if (LVD_CFG_CHANNEL_2_USED ==1)
        if ((0 == SYSTEM.LVCMPCR.BIT.LVD2E) ||(0 == SYSTEM.LVD2CR0.BIT.LVD2CMPE))
        {
            return LVD_ERR_DISABLED;
        }
        else
        {
            temp_status_reg = (uint8_t)(SYSTEM.LVD2SR.BYTE & 0x03);     // zero out the unused higher 6 bits;
            return (lvd_err_t)(temp_status_reg | 0x10);                 // setting the msb to 1 for unique return error code
        }
#else
        return LVD_ERR_ILL_PARAM;             // Code not enabled for this channel
#endif
    }
    else
    {
        return LVD_ERR_ILL_PARAM;
    }
}

/***********************************************************************************************************************
* Function Name: lvd_status_clear
* Description  : Clear the LVD passage detection status bit. This function clears the passage detection bit that is
*                set when Vcc or voltage on the CMPA2 pin crosses the Level set by the configured LVD channel. This bit
*                has to be cleared once it is set to allow for detection of future events on the same channel.
* Arguments    : lvd_channel_t e_channel
*                       -Specify the channel
* Return Value : LVD_SUCCESS
*                LVD_ERR_ILL_PARAM
*                LVD_ERR_NOT_INITIALIZED
***********************************************************************************************************************/
static lvd_err_t lvd_status_clear(lvd_channel_t e_channel)
{
#if (LVD_CFG_PARAM_CHECKING_ENABLE == 1)
    if (e_channel >= LVD_CHANNEL_INVALID)
    {
        return LVD_ERR_ILL_PARAM;
    }
#endif
    if (LVD_CHANNEL_1 == e_channel)
    {
#if (LVD_CFG_CHANNEL_1_USED ==1)
        if (g_lvd_ch1_open == false)                        // check for initialization
        {
            return LVD_ERR_NOT_INITIALIZED;
        }
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers
        SYSTEM.LVD1SR.BIT.LVD1DET = 0;                      // Clear the passage detection status bit
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);   // lock LVD control registers
#else
        return LVD_ERR_ILL_PARAM;                           // Code not enabled for this channel
#endif
    }
    else if (LVD_CHANNEL_2 == e_channel)
    {
#if (LVD_CFG_CHANNEL_2_USED ==1)
        if (g_lvd_ch2_open == false)                        // check for initialization
        {
            return LVD_ERR_NOT_INITIALIZED;
        }
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LVD);  // unlock LVD control registers
        SYSTEM.LVD2SR.BIT.LVD2DET = 0;                      // Clear the passage detection status bit
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LVD);   // lock LVD control registers
#else
        return LVD_ERR_ILL_PARAM;                           // Code not enabled for this channel
#endif
    }
    else
    {
        //nothing
    }

    nop();  // it takes two system clock cycles for the bit to the cleared.
    nop();

    return LVD_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: R_LVD_GetVersion
* Description  : Returns the current version of this module. The version number is encoded where the top 2 bytes are the
*                major version number and the bottom 2 bytes are the minor version number. For example, Version 4.25 
*                would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
***********************************************************************************************************************/
uint32_t R_LVD_GetVersion (void)
{
    /* These version macros are defined in r_ldv_rx_if.h. */
    return ((((uint32_t)LVD_RX_VERSION_MAJOR) << 16) | (uint32_t)LVD_RX_VERSION_MINOR);
} 

/***********************************************************************************************************************
* Function Name: lvd_delay
* Description  : Implements a software time delay
* Arguments    : time_us-
*                       Time to delay in usec
* Return Value : none
***********************************************************************************************************************/
static void lvd_delay(uint32_t time_us)
{
    volatile uint32_t iclk_freq_hz, delay_count;
    volatile uint32_t delay_time;
    iclk_freq_hz = BSP_ICLK_HZ; // get the current iclk frequency

    //delay_time = ((time_us * (iclk_freq_hz/1000000))/12);
     /* The delay loops used below have been measured to take 12 cycles per iteration. This has been verified using the
       Renesas RX Toolchain with optimizations set to 2, size priority. The same result was obtained using 2, speed
       priority. The amount of times to run the loop is adjusted linearly based on this info along with the speed
       at which the core is running. */
   for(delay_count = 0; delay_count < ((time_us * (iclk_freq_hz/1000000))/12); delay_count++)
    {
        nop();
    }

}

/***********************************************************************************************************************
* Function Name: lvd1_isr
* Description  : ISR for LVD channel 1
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if (LVD_CFG_CHANNEL_1_USED ==1)
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
#pragma interrupt lvd1_isr(vect=VECT(LVD,LVD1))
static void lvd1_isr(void)
{
    if (lvd1_isr_handler != NULL)
    {
        lvd1_isr_handler((void *)&g_lvd_ch1_cb_args);
    }
}
#endif
#endif
/***********************************************************************************************************************
* Function Name: lvd2_isr
* Description  : ISR for LVD channel 2
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#if (LVD_CFG_CHANNEL_2_USED ==1)
#if ((BSP_MCU_RX111 == 1) || (BSP_MCU_RX110 == 1) || (BSP_MCU_RX113 == 1) || \
     (BSP_MCU_RX210 == 1) || (BSP_MCU_RX231 == 1))
#pragma interrupt lvd2_isr(vect=VECT(LVD,LVD2))
static void lvd2_isr(void)
{
    if (lvd2_isr_handler != NULL)
    {
        lvd2_isr_handler((void *)&g_lvd_ch2_cb_args);
    }
}
#endif
#endif
