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
* File Name    : r_usb_cinthandler_usbip1.c
* Description  : USB IP1 Host and Peripheral interrupt handler code
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


#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
External variables and functions
******************************************************************************/
extern USB_UTR_t    usb_gcstd_IntMsg[][USB_INTMSGMAX];  /* Interrupt message */
extern uint16_t     usb_gcstd_IntMsgCnt[];              /* Interrupt message count */
extern void usb2_cstd_d0fifo_handler(void);


/******************************************************************************
Private global variables and functions
******************************************************************************/
USB_UTR_t       usb2_gcstd_IntMsgD0fifo;


/******************************************************************************
Renesas Abstracted common Interrupt handler functions
******************************************************************************/

/******************************************************************************
Function Name   : usb2_cstd_DmaHandler
Description     : DMA interrupt routine. Send message to PCD/HCD task.
Arguments       : none
Return value    : none
******************************************************************************/
void usb2_cstd_DmaHandler(void)
{
#if USB_FUNCSEL_USBIP1_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
#ifdef USB_DTC_ENABLE
    usb2_cstd_d0fifo_handler();
#endif /* USB_DTC_ENABLE */
#endif  /* USB_FUNCSEL_USBIP1_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
}
/******************************************************************************
End of function usb2_cstd_DmaHandler
******************************************************************************/

/******************************************************************************
Function Name   : usb2_cstd_UsbHandler
Description     : USB2 interrupt routine. Analyze which USB interrupt occurred 
                : and send message to PCD/HCD task.
Arguments       : none
Return value    : none
******************************************************************************/
void usb2_cstd_UsbHandler(void)
{
    USB_UTR_t       *ptr;

    /* Initial pointer */
    ptr = &usb_gcstd_IntMsg[1][usb_gcstd_IntMsgCnt[1]];
    ptr->ip  = USB_USBIP_1;
    ptr->ipp = usb_cstd_GetUsbIpAdr( ptr->ip );

    usb_cstd_InterruptClock( ptr );

    /* Check Host or Peripheral */
    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        USB_ER_t        err;

        /* Peripheral Function */
        /* Peripheral Interrupt handler */
        usb_pstd_InterruptHandler( ptr );
        ptr->msghead = (USB_MH_t)USB_NULL;
        /* Send message */
        err = USB_ISND_MSG(USB_PCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### lib_UsbHandler DEF1 isnd_msg error (%ld)\n", err);
        }
#endif /* USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        USB_ER_t        err;

        /* Host Function */
        /* Host Interrupt handler */
        usb_hstd_InterruptHandler( ptr );
        ptr->msghead = (USB_MH_t)USB_NULL;
        /* Send message */
        err = USB_ISND_MSG(USB_HCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### lib_UsbHandler DEF2 isnd_msg error (%ld)\n", err);
        }
#endif  /* USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }

    /* Renewal Message count  */
    usb_gcstd_IntMsgCnt[1]++;
    if( usb_gcstd_IntMsgCnt[1] == USB_INTMSGMAX )
    {
        usb_gcstd_IntMsgCnt[1] = 0;
    }
}
/******************************************************************************
End of function usb2_cstd_UsbHandler
******************************************************************************/
#endif  /* #if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */

/******************************************************************************
End  Of File
******************************************************************************/
