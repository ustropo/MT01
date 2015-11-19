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
* File Name    : r_usb_pintfifo.c
* Description  : USB Peripheral FIFO access code
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


#if (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP)

/******************************************************************************
Renesas Abstracted Peripheral FIFO access functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_BrdyPipe
Description     : Execute data transfer for the PIPE for which a BRDY interrupt 
                : occurred.
Arguments       : uint16_t bitsts : BRDYSTS register & BRDYENB register.
Return value    : none
******************************************************************************/
void usb_pstd_BrdyPipe(USB_UTR_t *ptr, uint16_t bitsts)
{
    /* When operating by the peripheral function, usb_cstd_brdy_pipe() is executed with PIPEx request because */
    /* two BRDY messages are issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BRDY0) == USB_BRDY0 )
    {
        switch( usb_cstd_read_data( ptr, USB_PIPE0, USB_CUSE ) )
        {
            /* End of data read */
            case USB_READEND:
                /* Continue */
            /* End of data read */
            case USB_READSHRT:
                usb_creg_clr_brdyenb(ptr, (uint16_t)USB_PIPE0);
            break;
            /* Continue of data read */
            case USB_READING:
                /* PID = BUF */
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
            break;
            /* FIFO access error */
            case USB_READOVER:
                USB_PRINTF0("### Receive data over PIPE0 \n");
                /* Clear BVAL */
                usb_creg_set_bclr( ptr, USB_CUSE );
                /* Control transfer stop(end) */
                usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_OVR);
            break;
            /* FIFO access error */
            case USB_FIFOERROR:
                USB_PRINTF0("### FIFO access error \n");
                /* Control transfer stop(end) */
                usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
            default:
            break;
        }
    }
    else
    {
        /* not PIPE0 */
        usb_cstd_BrdyPipe(ptr, bitsts);
    }
}
/******************************************************************************
End of function usb_pstd_BrdyPipe
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_NrdyPipe
Description     : Execute appropriate processing for the PIPE for which a NRDY 
                : interrupt occurred.
Arguments       : uint16_t bitsts : NRDYSTS register & NRDYENB register.
Return value    : none
******************************************************************************/
void usb_pstd_NrdyPipe(USB_UTR_t *ptr, uint16_t bitsts)
{
    /* The function for peripheral driver is created here. */
    if( (bitsts & USB_NRDY0) == USB_NRDY0 )
    {
    }
    else
    {
        /* Nrdy Pipe interrupt */
        usb_cstd_NrdyPipe(ptr, bitsts);
    }
}
/******************************************************************************
End of function usb_pstd_NrdyPipe
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_BempPipe
Description     : Execute data transfer for the PIPE for which a BEMP interrupt 
                : occurred.
Arguments       : uint16_t bitsts : BEMPSTS register & BEMPENB register.
Return value    : none
******************************************************************************/
void usb_pstd_BempPipe(USB_UTR_t *ptr, uint16_t bitsts)
{
    /* When operating by the peripheral function, usb_cstd_bemp_pipe() is executed with PIPEx request because */
    /* two BEMP messages are issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BEMP0) == USB_BEMP0 )
    {
        switch( usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE ) )
        {
            /* End of data write (not null) */
            case USB_WRITEEND:
                /* Continue */
            /* End of data write */
            case USB_WRITESHRT:
                /* Enable empty interrupt */
                usb_creg_clr_bempenb(ptr, (uint16_t)USB_PIPE0);
            break;
            /* Continue of data write */
            case USB_WRITING:
                /* PID = BUF */
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
            break;
            /* FIFO access error */
            case USB_FIFOERROR:
                USB_PRINTF0("### FIFO access error \n");
                /* Control transfer stop(end) */
                usb_pstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            break;
            default:
            break;
        }
    }
    else
    {
        /* BEMP interrupt */
        usb_cstd_BempPipe(ptr, bitsts);
    }
}
/******************************************************************************
End of function usb_pstd_BempPipe
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/