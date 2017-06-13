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
* File Name    : main.c
* Description  : main process
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include "platform.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"
#include "tinyg.h"		// #1
#include "config.h"		// #2
#include "hardware.h"
#include "r_cmt_rx_if.h"
#include "r_lvd_rx_if.h"
#include "eeprom.h"
#include "config_SwTimers.h"
#include "state_functions.h"
/* Flash Loader project includes. */
#include "r_fl_includes.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lcd.h"

#ifdef RELEASE

/* For HMSC Flashloader */
#define USERAPP_SECURITY_CODE   0x55AA55AA

void UserAppStart(void);
/*******************************************************************************
    APPLICATION INTERFACE HEADER
    The purpose of the header is for an external application to be able to read
    certain values from known addresses.
    - Start address of UserApp.
    - Security code must match what PCDC Flashloader expects.
    - For revision purposes of applications etc.
    - Do not change the order of these variables!
*******************************************************************************/
#pragma section C app_start

/* START ADDRESS of user application header data    - Appheader address + 0x00. */
const uint32_t  userapp_entry_addr =    (uint32_t)  UserAppStart;
                                            /*      - Appheader address + 0x04. */
const uint32_t  userapp_sec_code =      (uint32_t)  USERAPP_SECURITY_CODE;

/* USER APP SW ID (Not used by default.)            - Appheader address + 0x08. */
const uint32_t  userapp_id_code =                    0x00000001;
/*******************************************************************************/
#endif

#define USB_HOST_USBIP_NUM  USB_USBIP_0

/******************************************************************************
External variables and functions
******************************************************************************/
void usb_main(void);
void usb_mcu_init(void);
extern void	usb_cstd_IdleTaskStart(void);
extern void	usb_hmsc_task_start(void);
extern void	usb_apl_task_switch(void);
extern void FreeRTOSConfig(void);

maq_name maq;
USB_UTR_t       msc_utr;
TimerHandle_t swTimers[TIMER_SIZE];

/******************************************************************************
Function Name   : main
Description     : Main task
Arguments       : none
Return value    : none
******************************************************************************/
void main(void)
{
	lvd_err_t err;
	lvd_config_t channel1_cfg;
 	channel1_cfg.e_action = LVD_ACTION_RESET;
	channel1_cfg.e_trigger = LVD_TRIGGER_FALL;
	channel1_cfg.e_voltage_level =LVD_VOLTAGE_CH1_2_95;
	err = R_LVD_Open(LVD_CHANNEL_1, &channel1_cfg, NULL);

	//IWDT.IWDTCR.WORD = 0x3323;
	bool ret = false;
	/* Reserve the CMT0 for FreeRTOS */
	ret = R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_CMT0));
	while (false == ret) /* can't lock the CMT0 resource */
	{
		while (1);
	}
	WDT_FEED

 /* Inicializa��o das variaveis EEPROM */
	eepromInit();
	g_maq = check_machine_type();
	eepromConsistencyCheck();

	/* Initialize USB */
	usb_main();


	/* Initialize RTOS */
	FreeRTOSConfig();

	/* Start tasks - only returns if something bad happened! */
	vTaskStartScheduler();
    while (1)
    {

    }
}


void usb_main()
{
    usb_err_t   usb_err = USB_SUCCESS;
    usb_mcu_init();
    /* Determine which port is host. */
    msc_utr.ip  = USB_HOST_USBIP_NUM;
    msc_utr.ipp     = R_usb_cstd_GetUsbIpAdr( msc_utr.ip ); /* Get the USB IP base address. */

    /* Initialize the USB driver hardware */
    usb_cpu_target_init();

    /* USB driver open */
    usb_err = R_USB_Open( (usb_ip_t)msc_utr.ip );
    if( USB_E_OK != usb_err )
    {
        while(1);   /* Error stop */
    }
}
/******************************************************************************
End of function main()
******************************************************************************/

/******************************************************************************
Function Name   : usb_mcu_init
Description     : USB pin function and port mode setting.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_mcu_init(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

  #if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP

    PORT1.PMR.BIT.B4 = 1u;  /* P14 set USB0_OVRCURA */

    PORT1.PMR.BIT.B6 = 1u;  /* P16 set VBUS_USB */
    MPC.P16PFS.BYTE = 0x12; /* USB0_VBUSEN */
  #endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP */

  #if USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    PORT1.PMR.BIT.B0    = 1u;
    MPC.P10PFS.BYTE = 0x15; /* USBHS_OVRCURA */
    PORT1.PMR.BIT.B1    = 1u;
    MPC.P11PFS.BYTE = 0x15; /* USBHS_VBUSEN */
  #endif  /* USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
}   /* eof usb_mcu_init() */
/******************************************************************************
End  Of File
******************************************************************************/
