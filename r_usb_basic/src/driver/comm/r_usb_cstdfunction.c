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
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_cstdfunction.c
* Description  : USB Host and Peripheral common low level functions.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_reg_access.h"

#if defined(BSP_MCU_RX64M)
    #if( USB_SPEED_MODE_PP == USB_HS_PP )
        #error "You can not set USB_HS_PP in USB_SPEED_MODE_PP of r_usb_basic_config.h when using RX64M."
    #endif
#endif


#if( USB_FUNCSEL_USBIP1_PP != USB_HOST_PP )
    #if defined( USB_HOST_BC_ENABLE )
        #error "You can not set USB_HS_ENABLE to USB_SPEED_MODE since USB_FUNCSELIP1_PP is not USB_HOST_PP \
                in r_usb_basic_config.h."
    #endif
#endif

#if( USB_FUNCSEL_USBIP1_PP == USB_HOST_PP )
    #if !defined( USB_HOST_BC_ENABLE )
        #if defined( USB_BC_DCP_ENABLE )
            #error "You can not define USB_BC_DCP_ENABLE since USB_HOST_BC_ENABLE is not defined \
                    in r_usb_basic_config.h."
        #endif
    #endif
#endif

#if defined(BSP_MCU_RX64M)
    #if defined(USB_HS_EL_TEST)
        #error "You can not enable USB_HS_EL_TEST in r_usb_basic_config.h when using RX64M."
    #endif
#endif

#if !defined(USB_HOST_COMPLIANCE_MODE)
    #if defined(USB_HS_EL_TEST)
        #error "You can not enable USB_HS_EL_TEST in r_usb_basic_config.h \
                when USB_HOST_COMPLIANCE_MODE is disabled."
    #endif
#endif
/******************************************************************************
Constant macro definitions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Renesas Abstracted common standard function functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_cstd_set_usbip_mode_sub
Description     : USB init depending on mode (host peripharal). 
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t function : HOST/PERI
Return value    : none
******************************************************************************/
void usb_cstd_set_usbip_mode_sub(USB_UTR_t *ptr, uint16_t function)
{

#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t   else_connect_inf;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    usb_cstd_set_sofcfg_intl( ptr );
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    
    if (function == (uint16_t)USB_PERI)
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        usb_creg_clr_drpd( ptr, USB_PORT0 );
        usb_preg_write_physlew( ptr );
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
        /* Interrupt Enable */
        usb_cstd_BerneEnable(ptr);
        usb_pstd_InitConnect(ptr);
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        usb_creg_set_dcfm( ptr );
        usb_creg_set_drpd( ptr, USB_PORT0 );
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        usb_hreg_write_physlew( ptr );
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */
#if USB_PORTSEL_PP == USB_2PORT_PP
        usb_creg_set_drpd( ptr, USB_PORT1 );
        /* Set CNEN bit for RX64M */
        usb_creg_set_cnen( ptr );
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
        /* Interrupt Enable */
        usb_cstd_BerneEnable(ptr);
        /* Wait 10us */
        usb_cpu_Delay1us((uint16_t)10);
#if USB_PORTSEL_PP == USB_2PORT_PP
        /* Interrupt Enable */
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        usb_hstd_OvrcrEnable(ptr, USB_PORT0);
        usb_hstd_OvrcrEnable(ptr, USB_PORT1);
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */
        /* Vbus ON and Check Connect */
        else_connect_inf = usb_hstd_InitConnect(ptr, USB_PORT0, USB_DETACHED );
        usb_hstd_InitConnect(ptr, USB_PORT1, else_connect_inf);
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        /* Interrupt Enable */
        usb_hstd_OvrcrEnable(ptr, USB_PORT0);
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */
        /* Vbus ON and Check Connect */
        usb_hstd_InitConnect(ptr, USB_PORT0, USB_DETACHED );
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
}
/******************************************************************************
End of function usb_cstd_set_usbip_mode_sub
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
