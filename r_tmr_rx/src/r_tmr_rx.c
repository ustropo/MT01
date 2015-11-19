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
* Copyright (C) 2013, 2014 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_cmt_rx.c
* Description  : This module creates timer ticks using CMT channels or one-shot events based on period in uS. 
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 06.11.2013 2.10    First GSCE Release.
*         : 11.03.2014 2.20    Added support for RX110.
*                              Fixes bug in R_CMT_Control CMT_RX_CMD_GET_NUM_CHANNELS command.
*         : 22.04.2014 2.30    Added support for RX64M.
*         : 10.11.2014 2.40    Added support for RX113M.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "platform.h"
/* Configuration for this package. */
//#include "r_tmr_rx_config.h"
/* Header file for this package. */
#include "r_tmr_rx_if.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define TMR_RX_NUM_CHANNELS 2

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Used to prevent having duplicate code for each channel. This only works if the channels are identical (just at
   different locations in memory). This is easy to tell by looking in iodefine.h and seeing if the same structure
   was used for all channels. */
static volatile struct st_tmr0 __evenaccess * const g_tmr0_channels[TMR_RX_NUM_CHANNELS] =
{
    &TMR0, &TMR2
};

static volatile struct st_tmr1 __evenaccess * const g_tmr1_channels[TMR_RX_NUM_CHANNELS] =
{
    &TMR1, &TMR3
};

static void  (* g_tmr_callbacks[TMR_RX_NUM_CHANNELS*2])(void * pdata);

/***********************************************************************************************************************
* Function Name: R_CMT_CreatePeriodic
* Description  : Sets up a CMT channel and calls a callback function at a set frequency.
* Arguments    : frequency_hz -
*                    Frequency in Hz of how often to call the callback function.
*                callback -
*                    Which function to call when timer expires. If you wish to use R_CMT_Control() to check the status 
*                    of a channel instead of using a callback then use FIT_NO_FUNC for this parameter.
*                channel -
*                    Pointer of where to store which channel was used.
* Return Value : true - 
*                    Channel initialized successfully.
*                false -
*                    Invalid channel or frequency could not be used.
***********************************************************************************************************************/
bool R_TMR_CreatePeriodic(uint32_t frequency_hz, void (* callback)(void * pdata), tmr_ch_t  channel)
{
	return false;
} 

/***********************************************************************************************************************
* Function Name: R_CMT_CreateOneShot
* Description  : Sets up a CMT channel and calls a callback function once after a user-defined amount of time.
* Arguments    : period_us -
*                    How long until compare match occurs. Unit is microseconds.
*                callback -
*                    Which function to call when timer expires. If you wish to use R_CMT_Control() to check the status 
*                    of a channel instead of using a callback then use FIT_NO_FUNC for this parameter.
*                channel -
*                    Pointer of where to store which channel was used.
* Return Value : true - 
*                    Channel initialized successfully.
*                false -
*                    Invalid channel or period could not be used.
***********************************************************************************************************************/
bool R_TMR_CreateOneShot(uint8_t period_us, void (* callback)(void * pdata), tmr_ch_t  channel)
{    
	bool res = true;
	if (channel == TMR_CH0 || channel == TMR_CH2)
	{
		R_BSP_RegisterProtectDisable (BSP_REG_PROTECT_LPC_CGC_SWR);
		MSTP(TMR0) = 0;
		MSTP(TMR1) = 0;
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
		(*g_tmr0_channels[channel]).TCSR.BYTE = 0x00;
		(*g_tmr0_channels[channel]).TCCR.BYTE = 0x89;
		(*g_tmr0_channels[channel]).TCR.BYTE = 0x48;
        /* Setup ICU registers. */
		IR(TMR0, CMIA0)  = 0;                //Clear any previously pending interrupts
		IPR(TMR0, CMIA0) = 5;   			 //Set interrupt priority
		IEN(TMR0, CMIA0) = 1;                //Enable compare match interrupt
		(*g_tmr0_channels[channel]).TCORA = 24*period_us;
		g_tmr_callbacks[channel] = callback;
	}
	else if (channel == TMR_CH1 || channel == TMR_CH3)
	{
		R_BSP_RegisterProtectDisable (BSP_REG_PROTECT_LPC_CGC_SWR);
		MSTP(TMR2) = 0;
		MSTP(TMR3) = 0;
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
		(*g_tmr1_channels[channel]).TCSR.BYTE = 0x00;
		(*g_tmr1_channels[channel]).TCCR.BYTE = 0x09;
		(*g_tmr1_channels[channel]).TCR.BYTE = 0x48;
		(*g_tmr1_channels[channel]).TCORA = 24*period_us;
		g_tmr_callbacks[channel] = callback;
	}
	else
	{
		res = false;
	}
	return res;
}

/***********************************************************************************************************************
* Function Name: R_CMT_Stop
* Description  : Stop a counter and puts it in module stop state to conserve power.
* Arguments    : channel - 
*                    Which channel to use.
* Return Value : true - 
*                    Counter stopped.
*                false -
*                    Could not obtain lock to control CMT. Try again later. 
***********************************************************************************************************************/
bool R_TMR_Stop (tmr_ch_t channel)
{
	return false;
} 

/***********************************************************************************************************************
* Function Name: R_CMT_Control
* Description  : Handles minor functions of this module.
* Arguments    : channel - 
*                    Which channel is being referenced. If not channel is needed input CMT_RX_NO_CHANNEL.
*                command -
*                    What command is being input.
*                pdata - 
*                    Pointer to data to be input or filled in if needed.
* Return Value : true - 
*                    Command completed successfully.
*                false -
*                    Invalid command. 
***********************************************************************************************************************/
bool R_TMR_Control(tmr_ch_t channel, tmr_commands_t command, void * pdata)
{
	return false;
} 

/***********************************************************************************************************************
* Function Name: tmr0_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt tmr0_isr(vect=VECT(TMR0, CMIA0))
static void tmr0_isr (void)
{
	 g_tmr_callbacks[0]((void *)0);
}

/***********************************************************************************************************************
* Function Name: tmr1_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt tmr1_isr(vect=VECT(TMR1, CMIA1))
static void tmr1_isr (void)
{
	g_tmr_callbacks[1]((void *)1);
}

/***********************************************************************************************************************
* Function Name: tmr2_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt tmr2_isr(vect=VECT(TMR2, CMIA2))
static void tmr2_isr (void)
{
	g_tmr_callbacks[2]((void *)2);
}

/***********************************************************************************************************************
* Function Name: tmr1_isr
* Description  : Interrupt when compare match for this channel occurs.
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
#pragma interrupt tmr3_isr(vect=VECT(TMR3, CMIA3))
static void tmr3_isr (void)
{
	g_tmr_callbacks[3]((void *)3);
}


