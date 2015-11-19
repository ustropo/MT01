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
* File Name    : r_usb_cstdapi.c
* Description  : A USB Host and Peripheral common low level API.
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
#include "r_usb_reg_access.h"       /* USB register access function */
#include "r_usb_api.h"


/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
External variables and functions
******************************************************************************/
extern usb_err_t    usb_cpu_module_start( usb_ip_t ip_type );
extern usb_err_t    usb_cpu_module_stop( usb_ip_t ip_type );

/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
Renesas Abstracted common Signal functions
******************************************************************************/

/*****************************************************************************
* Function Name: R_USB_GetVersion
* Description  : Returns the version of this module. The version number is 
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
#pragma inline(R_USB_GetVersion)
uint32_t R_USB_GetVersion(void)
{
    uint32_t version = 0;

    version = (USB_VERSION_MAJOR << 16) | USB_VERSION_MINOR;

    return version;
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Open
Description     : Initializes the USB module. It's only called once.
Arguments       : usb_ip_t ip_type  : USB_IP0/USB_IP1
Return value    : USB_SUCCESS -
                    USB module is initialized successfully
                  USB_ERR_OPENED
                    USB is opened already
                  USB_ERR_BUSY
                    Lock has already been acquired by another task
******************************************************************************/
usb_err_t R_USB_Open( usb_ip_t ip_type )
{
    return usb_cpu_module_start( ip_type );
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Close
Description     : Initializes the USB module. It's only called once.
Arguments       : usb_ip_t ip_type  : USB_IP0/USB_IP1
Return value    : USB_SUCCESS -
                    USB module is initialized successfully
                  USB_ERR_OPENED
                    USB is opened already
                  USB_ERR_BUSY
                    Lock has already been acquired by another task
******************************************************************************/
usb_err_t R_USB_Close( usb_ip_t ip_type )
{
    return usb_cpu_module_stop( ip_type );
}
/******************************************************************************
End of function
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_GetUsbIpAdr  
Description     : Return USB register base address of specified USB IP.
Arguments       : uint16_t  ipno        : USB IP No. that requires the base 
                                          address value 
Return value    : USB_REGADR_t          : Address value
******************************************************************************/
USB_REGADR_t    R_usb_cstd_GetUsbIpAdr( uint16_t ipno )
{
    return (USB_REGADR_t)usb_cstd_GetUsbIpAdr( ipno );
}/* eof R_usb_cstd_GetUsbIpAdr */
 
/******************************************************************************
Function Name   : R_usb_cstd_UsbIpInit
Description     : Initialize the USB IP.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t  usb_mode    : USB mode (Host/Peripheral).
Return value    : none
******************************************************************************/
void R_usb_cstd_UsbIpInit( USB_UTR_t *ptr, uint16_t usb_mode )
{
    usb_cstd_WaitUsbip( ptr );      /* Wait USB-H/W access enable */
    usb_cstd_AsspConfig( ptr );     /* Set ASSP pin_config */
    usb_cstd_InitialClock( ptr );   /* Start clock */
    R_usb_cstd_ClearHwFunction( ptr );  /* nitinalize USB register (Host/Peripheral common) */
    usb_cstd_Pinconfig( ptr );      /* Set pin_config */

    usb_cstd_set_usbip_mode( ptr, usb_mode );
}/* eof R_usb_cstd_UsbIpInit */
 
/******************************************************************************
Function Name   : R_usb_cstd_ClearHwFunction
Description     : Initinalize USB register (Host/Peripheral common)
Arguments       : USB_UTR_t *ptr : USB system internal structure.
Return value    : none
******************************************************************************/
void R_usb_cstd_ClearHwFunction(USB_UTR_t *ptr)
{
    usb_cstd_SelfClock(ptr);

    usb_cstd_SetNak(ptr, USB_PIPE0);
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    usb_creg_set_bus_wait( ptr );
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    usb_cstd_ClearIntEnb( ptr );

    /* Interrupt Disable(BRDY,NRDY,USB_BEMP) */
    usb_creg_write_brdyenb( ptr, 0 );
    usb_creg_write_nrdyenb( ptr, 0 );
    usb_creg_write_bempenb( ptr, 0 );

    /* Interrupt status clear */
    usb_cstd_ClearIntSts( ptr );

    /* Interrupt status clear(USB_BRDY,NRDY,USB_BEMP) */
    usb_creg_write_brdysts( ptr, 0 );
    usb_creg_write_nrdysts( ptr, 0 );
    usb_creg_write_bempsts( ptr, 0 );

    /* D+/D- control line set */
    usb_cstd_ClearDline( ptr );

    usb_creg_clr_hse( ptr, USB_PORT0 );
    usb_creg_clr_hse( ptr, USB_PORT1 );

    /* Function controller select */
    usb_creg_clr_dcfm( ptr ); 
    usb_cstd_SwReset(ptr);

}/* eof R_usb_cstd_ClearHwFunction */

/******************************************************************************
Function Name   : R_usb_cstd_SetRegDvstctr0
Description     : Setting the value(2nd argument) to DVSTCTR0 register
Arguments       : USB_UTR_t *ptr       ; USB internal structure. Selects USB channel.
                : uint16_t   val        : setting value
Return value    : none
******************************************************************************/
void    R_usb_cstd_SetRegDvstctr0( USB_UTR_t *ptr, uint16_t val )
{

    usb_creg_write_dvstctr( ptr, USB_PORT0, val );

}
/******************************************************************************
End of function R_usb_cstd_SetRegDvstctr0
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_SetRegPipeCtr
Description     : Setting the value(3rd argument) to PIPExCTR register
Arguments       : USB_UTR_t *ptr      ; USB internal structure. Selects USB channel.
                : uint16_t   pipeno    ; Pipe No. 
                : uint16_t   val       ; setting value
Return value    : none
******************************************************************************/
void    R_usb_cstd_SetRegPipeCtr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t val )
{

    usb_creg_write_pipectr( ptr, pipeno, val );

}
/******************************************************************************
End of function R_usb_cstd_SetRegPipeCtr
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_SetBuf
Description     : Set PID (packet ID) of the specified pipe to BUF.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe number.
Return value    : none
******************************************************************************/
void R_usb_cstd_SetBuf(USB_UTR_t *ptr, uint16_t pipe)
{
    usb_cstd_SetBuf(ptr, pipe);
}
/******************************************************************************
End of function R_usb_cstd_SetRegPipeCtr
******************************************************************************/

/******************************************************************************
Function Name   : R_usb_cstd_debug_hook
Description     : Debug hook
Arguments       : uint16_t error_code          : error code
Return value    : none
******************************************************************************/
void R_usb_cstd_debug_hook(uint16_t error_code)
{
   while(1);
}   /* eof R_usb_cstd_debug_hook() */

/******************************************************************************
End  Of File
******************************************************************************/
