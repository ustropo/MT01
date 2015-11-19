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
* File Name    : r_mtu_rx_if.h
* Description  : .
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 23.09.2014 1.00    First Release
***********************************************************************************************************************/

#ifndef MTU_RX_IF
#define MTU_RX_IF

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* User configurable options for the MTU timer code */
#include "platform.h"
#include "r_mtu_rx_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/* Defined here for use in configuration file. */
#define  MTU_FILTER_PCLK_DIV_1    (0x00)  // PCLK/1
#define  MTU_FILTER_PCLK_DIV_8    (0x10)  // PCLK/8
#define  MTU_FILTER_PCLK_DIV_32   (0x20)  // PCLK/32
#define  MTU_FILTER_PCLK_EXTERNAL (0x30)  // The clock source for counting is the external clock.

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Enumeration of MTU channel numbers. */
typedef enum
{
    MTU_CHANNEL_0 = 0,
    MTU_CHANNEL_1,
    MTU_CHANNEL_2,
    #ifndef BSP_MCU_RX110   // RX110 has only channels 0, 1, 2.
    MTU_CHANNEL_3,
    MTU_CHANNEL_4,
    #endif
    MTU_CHANNEL_MAX
} mtu_channel_t;

/* Clocking source selections. Index into register settings table. */
typedef enum mtu_clk_sources_e
{
    MTU_CLK_SRC_EXT_MTCLKA = 0x00,      // External clock input on MTCLKA pin
    MTU_CLK_SRC_EXT_MTCLKB = 0x01,      // External clock input on MTCLKB pin
    MTU_CLK_SRC_EXT_MTCLKC = 0x02,      // External clock input on MTCLKC pin
    MTU_CLK_SRC_EXT_MTCLKD = 0x03,      // External clock input on MTCLKD pin
    MTU_CLK_SRC_CASCADE    = 0x04,      // Clock by overflow from other channel counter. (only on certain channels)
    MTU_CLK_SRC_INTERNAL                // Use internal clock (PCLK)
} mtu_clk_sources_t;

/* The possible return codes from the API functions. */
typedef enum mtu_pwm_err_e
{
    MTU_SUCCESS = 0,
    MTU_ERR_BAD_CHAN,              // Invalid channel number.
    MTU_ERR_CH_NOT_OPENED,         // Channel not yet opened.
    MTU_ERR_CH_NOT_CLOSED,         // Channel still open from previous open.
    MTU_ERR_UNKNOWN_CMD,           // Control command is not recognized.
    MTU_ERR_INVALID_ARG,           // Argument is not valid for parameter.
    MTU_ERR_ARG_RANGE,             // Argument is out of range for parameter.
    MTU_ERR_NULL_PTR,              // Received null pointer; missing required argument.
    MTU_ERR_LOCK,                  // The lock procedure failed.
    MTU_ERR_UNDEF                  // Undefined/unknown error
} mtu_err_t;

/* The possible settings for MTU output pins. Register setting values. */
typedef enum mtu_output_states_e
{
    MTU_PIN_NO_OUTPUT = 0x0,    // Output high impedance.
    MTU_PIN_LO_GOLO   = 0x1,    // Initial output is low. Low output at compare match.
    MTU_PIN_LO_GOHI   = 0x2,    // Initial output is low. High output at compare match.
    MTU_PIN_LO_TOGGLE = 0x3,    // Initial output is low. Toggle (alternate) output at compare match.
    MTU_PIN_HI_GOLO   = 0x5,    // Initial output is high. Low output at compare match.
    MTU_PIN_HI_GOHI   = 0x6,    // Initial output is high. High output at compare match.
    MTU_PIN_HI_TOGGLE = 0x7     // Initial output is high. Toggle (alternate) output at compare match.
} mtu_output_states_t;

/* The possible settings for counting clock active edge. Register setting values. */
typedef enum mtu_clk_edges_e
{
    MTU_CLK_RISING_EDGE  = 0x00,
    MTU_CLK_FALLING_EDGE = 0x08,
    MTU_CLK_ANY_EDGE     = 0x10,
} mtu_clk_edges_t;

/* The possible counter clearing source selections. Index into register settings table. */
typedef enum mtu_clear_src_e
{
    MTU_CLR_TIMER_A = 0,    // Clear the channel counter on the "A" compare or capture event.
    MTU_CLR_TIMER_B,        // Clear the channel counter on the "B" compare or capture event.
    MTU_CLR_TIMER_C,        // Clear the channel counter on the "C" compare or capture event.
    MTU_CLR_TIMER_D,        // Clear the channel counter on the "D" compare or capture event.
    MTU_CLR_SYNC,           // Clear the channel counter when another sync'ed channel clears.
    MTU_CLR_DISABLED        // Never clear the channel counter.
} mtu_clear_src_t;

/* PCLK divisor  for internal clocking source. Index into register settings table. */
typedef enum mtu_pclk_divisor_e
{
    MTU_SRC_CLK_DIV_1 = 0, // PCLK/1
    MTU_SRC_CLK_DIV_4,     // PCLK/4
    MTU_SRC_CLK_DIV_16,    // PCLK/16
    MTU_SRC_CLK_DIV_64,    // PCLK/64
    MTU_SRC_CLK_DIV_256,   // PCLK/256
    MTU_SRC_CLK_DIV_1024   // PCLK/1024
} mtu_src_clk_divisor_t;

/* Actions to be done upon timer or capture event. Multiple selections to be ORed together. */
typedef enum mtu_actions_e
{
    MTU_ACTION_NONE      = 0x00,      // Do nothing with this timer.
    MTU_ACTION_OUTPUT    = 0x01,      // Change state of output pin.
    MTU_ACTION_INTERRUPT = 0x02,      // Generate interrupt request.
    MTU_ACTION_CALLBACK  = 0x04,      // Generate interrupt request and execute user-defined callback on interrupt.
    MTU_ACTION_REPEAT    = 0x10,      // Continuously repeat the timer cycle and actions
    MTU_ACTION_TRIGGER_ADC = 0x20,    // Trigger ADC on this event. Timer A events only.
    MTU_ACTION_CAPTURE   = 0x40,      // Default input capture action. Placeholder value, does not to be specified.
} mtu_actions_t;


/************ Type defines used with the R_MTU_Control function. ***************/
/* Control function command codes. */
typedef enum mtu_cmd_e
{
    MTU_CMD_START,              // Activate clocking
    MTU_CMD_STOP,               // Pause clocking
    MTU_CMD_SAFE_STOP,          // Stop clocking and set outputs to safe state
    MTU_CMD_RESTART,            // Zero the counter then resume clocking
    MTU_CMD_SYNCHRONIZE,        // Specify channels to group for synchronized clearing.
    MTU_CMD_GET_STATUS,         // Retrieve the current status of the channel
    MTU_CMD_CLEAR_EVENTS,       // Clears the interrupt flags for the channel
    MTU_CMD_SET_CAPT_EDGE,      // Sets the detection edge polarity for input capture.
    MTU_CMD_UNKNOWN             // Not a valid command.
} mtu_cmd_t;

/* Used as bit-field identifiers to identify channels assigned to a group for group operations.
 * Add multiple channels to group by ORing these values together. */
typedef enum
{
    MTU_GRP_CH0 = 0x01,
    MTU_GRP_CH1 = 0x02,
    MTU_GRP_CH2 = 0x04,
    #ifndef BSP_MCU_RX110
    MTU_GRP_CH3 = 0x40,
    MTU_GRP_CH4 = 0x80,
    #endif
} mtu_group_t;

typedef struct mtu_timer_status_s
{
    uint32_t timer_count;           // The current channel counter value.
    bool     timer_running;         // True = timer currently counting, false = counting stopped.
} mtu_timer_status_t;

typedef struct mtu_capture_status_s
{
    uint32_t capt_a_count;          // The count at input capture A event.
    uint32_t capt_b_count;          // The count at input capture B event.
    uint32_t capt_c_count;          // The count at input capture C event.
    uint32_t capt_d_count;          // The count at input capture D event.
    uint32_t timer_count;           // The current channel counter value.
    uint8_t  capture_flags;         // 1 if a capture event occurred, 0 if still waiting.
} mtu_capture_status_t;

typedef struct mtu_pwm_status_s
{
    bool     running;
    uint16_t pwm_timer_count;      // The current channel counter value.
    uint16_t pwm_a_value;          // The count at input capture A event.
    uint16_t pwm_b_value;          // The count at input capture B event.
    uint16_t pwm_c_value;          // The count at input capture C event.
    uint16_t pwm_d_value;          // The count at input capture D event.
} mtu_pwm_status_t;

/************ Type defines used for callback functions. ***************/
/* Specifies the timer to which an operation is associated. Returned in callback data structure. */
typedef enum
{
    MTU_TIMER_A = 0,    //Corresponds to MTU TGRA register operations
    MTU_TIMER_B,        //Corresponds to MTU TGRB register operations
    MTU_TIMER_C,        //Corresponds to MTU TGRC register operations
    MTU_TIMER_D,        //Corresponds to MTU TGRD register operations
    MTU_NUM_TIMERS
} mtu_timer_num_t;

/************ Type defines used for callback functions. ***************/
/* Data structure passed to User callback upon pwm interrupt. */
typedef struct mtu_callback_data_s
{
    mtu_channel_t   channel;
    mtu_timer_num_t timer_num;
    uint32_t        count;
} mtu_callback_data_t;


/************ Type defines used with the R_MTU_Timer_Open and R_MTU_Capture_Open functions. ***************/
typedef struct mtu_timer_clk_src_s
{
    mtu_clk_sources_t   source;      // Internal clock or external clock input
    mtu_clk_edges_t     clock_edge;  // Specify the clock active edge.
} mtu_clk_src_t;

/************ Type defines used with the R_MTU_Capture_Open function. ***************/
typedef enum
{
    MTU_CAP_SRC_A = 0,
    MTU_CAP_SRC_B,
    MTU_CAP_SRC_C,
    MTU_CAP_SRC_D
} mtu_cap_src_t;

/* The possible settings for input capture signal active edge. Register setting values. */
typedef enum mtu_cap_edges_e
{
    MTU_CAP_RISING_EDGE  = 0x08,
    MTU_CAP_FALLING_EDGE = 0x09,
    MTU_CAP_ANY_EDGE     = 0x0A,
} mtu_cap_edges_t;

typedef struct mtu_capture_set_edge_s // Used with the MTU_TIMER_CMD_SET_CAPT_EDGE command.
{
    mtu_cap_src_t    capture_src;        // The capture source.
    mtu_cap_edges_t  capture_edge;       // Specify transition polarities.
} mtu_capture_set_edge_t;

typedef struct mtu_capture_settings_s
{
    mtu_actions_t    actions;
    mtu_cap_edges_t  capture_edge;       // Specify transition polarities.
    bool             filter_enable;      // Noise filter on or off.
} mtu_capture_settings_t;

typedef struct mtu_capture_chnl_settings_s
{
    mtu_clk_src_t     clock_src;   // Specify clocking source.
    mtu_src_clk_divisor_t   clock_div;   // Internal clock divisor selection.
    mtu_clear_src_t         clear_src;   // Specify the counter clearing source.
    mtu_capture_settings_t  capture_a;
    mtu_capture_settings_t  capture_b;
    mtu_capture_settings_t  capture_c;
    mtu_capture_settings_t  capture_d;
} mtu_capture_chnl_settings_t;


/************ Type defines used with the R_MTU_Timer_Open function. ***************/
typedef struct mtu_timer_actions_config_s
{
    mtu_actions_t           do_action;  // Various actions that can be done at timer event.
    mtu_output_states_t     output;     // Output pin transition type when output action is selected.
} mtu_timer_actions_cfg_t;

typedef struct mtu_timer_settings_s
{
    uint32_t   freq; // If internal clock source, the desired event frequency, or if external the Compare-match count.
    mtu_timer_actions_cfg_t actions;
} mtu_timer_settings_t;

typedef struct mtu_timer_chnl_settings_s
{
    mtu_clk_src_t  clock_src;      // Specify clocking source.
    mtu_clear_src_t      clear_src;      // Specify the counter clearing source.
    mtu_timer_settings_t timer_a;
    mtu_timer_settings_t timer_b;
    mtu_timer_settings_t timer_c;
    mtu_timer_settings_t timer_d;
} mtu_timer_chnl_settings_t;


/************ Type defines used with the R_MTU_PWM_Open function. ***************/
/* Available PWM operating modes. */
typedef enum mtu_pwm_mode_e
{
    MTU_PWM_MODE_1 = 0x02,
    MTU_PWM_MODE_2 = 0x03
} mtu_pwm_mode_t;

typedef struct mtu_pwm_settings_s
{
    uint16_t             duty;
    mtu_actions_t        actions;
    mtu_output_states_t  outputs;       // Specify transition polarities.
} mtu_pwm_settings_t;

typedef struct mtu_pwm_chnl_settings_s
{
    mtu_clk_src_t       clock_src;   // Specify clocking source.
    uint32_t                cycle_freq;  // Cycle frequency for the channel
    mtu_clear_src_t         clear_src;   // Specify the counter clearing source.
    mtu_pwm_mode_t          pwm_mode;    // Specify mode 1 or mode 2
    mtu_pwm_settings_t  pwm_a;
    mtu_pwm_settings_t  pwm_b;
    mtu_pwm_settings_t  pwm_c;
    mtu_pwm_settings_t  pwm_d;
} mtu_pwm_chnl_settings_t;

/***********************************************************************************************************************
Public Functions
***********************************************************************************************************************/
mtu_err_t  R_MTU_Timer_Open (mtu_channel_t              channel,
                             mtu_timer_chnl_settings_t *pconfig,
                             void                     (*pcallback)(void *pdata));

mtu_err_t  R_MTU_Capture_Open (mtu_channel_t                channel,
                               mtu_capture_chnl_settings_t *pconfig,
                               void                       (*pcallback)(void *pdata));

mtu_err_t  R_MTU_PWM_Open (mtu_channel_t            channel,
                           mtu_pwm_chnl_settings_t *pconfig,
                           void                   (*pcallback)(void *pdata));

mtu_err_t  R_MTU_Control (mtu_channel_t channel,
                          mtu_cmd_t     cmd,
                          void         *pcmd_data);

mtu_err_t  R_MTU_Close (mtu_channel_t channel);


uint32_t  R_MTU_GetVersion (void);

#endif /* MTU_TIMER_IF */

