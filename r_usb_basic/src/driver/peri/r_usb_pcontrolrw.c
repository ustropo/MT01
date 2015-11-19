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
* File Name    : r_usb_pcontrolrw.c
* Description  : USB Peripheral control transfer API code
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
Renesas Abstracted Peripheral Control RW API functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_pstd_ControlRead
Description     : Called by R_usb_pstd_ControlRead, see it for description.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint32_t bsize    : Read size in bytes.
                : uint8_t *table    : Start address of read data buffer.
Return value    : uint16_t          : USB_WRITESHRT/USB_WRITE_END/USB_WRITING/
                :                   : USB_FIFOERROR.
******************************************************************************/
uint16_t usb_pstd_ControlRead(USB_UTR_t *ptr, uint32_t bsize, uint8_t *table)
{
    uint16_t    end_flag;

    usb_gcstd_DataCnt[ptr->ip][USB_PIPE0] = bsize;
    usb_gcstd_DataPtr[ptr->ip][USB_PIPE0] = table;

    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, (uint16_t)USB_ISEL);
    
    /* Buffer clear */
    usb_creg_set_bclr( ptr, USB_CUSE );

    usb_creg_clr_sts_bemp( ptr, USB_PIPE0 );

    /* Peripheral Control sequence */
    end_flag = usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE );

    /* Peripheral control sequence */
    switch( end_flag )
    {
    /* End of data write */
    case USB_WRITESHRT:
        /* Enable not ready interrupt */
        usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
        /* Set PID=BUF */
        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
    /* End of data write (not null) */
    case USB_WRITEEND:
        /* Continue */
    /* Continue of data write */
    case USB_WRITING:
        /* Enable empty interrupt */
        usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
        /* Enable not ready interrupt */
        usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
        /* Set PID=BUF */
        usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
        break;
    /* FIFO access error */
    case USB_FIFOERROR:
        break;
    default:
        break;
    }
    /* End or error or continue */
    return (end_flag);
}
/******************************************************************************
End of function usb_pstd_ControlRead
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_ControlEnd
Description     : End control transfer
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint16_t status   : Transfer end status
Return value    : none
******************************************************************************/
void usb_pstd_ControlEnd(USB_UTR_t *ptr, uint16_t status)
{
    /* Interrupt disable */
    /* BEMP0 disable */
    usb_creg_clr_bempenb(ptr, (uint16_t)USB_PIPE0);
    /* BRDY0 disable */
    usb_creg_clr_brdyenb(ptr, (uint16_t)USB_PIPE0);
    /* NRDY0 disable */
    usb_creg_clr_nrdyenb(ptr, (uint16_t)USB_PIPE0);

    if(ptr -> ip == USB_USBIP_0)
    {
        usb_creg_set_mbw( ptr, USB_CUSE, USB0_CFIFO_MBW );
    }
    else if (ptr -> ip == USB_USBIP_1)
    {
        usb_creg_set_mbw( ptr, USB_CUSE, USB1_CFIFO_MBW );
    }

    if( (status == USB_DATA_ERR) || (status == USB_DATA_OVR) )
    {
        /* Request error */
        usb_pstd_SetStallPipe0( ptr );
    }
    else if( status == USB_DATA_STOP )
    {
        /* Pipe stop */
        usb_cstd_SetNak(ptr, (uint16_t)USB_PIPE0);
    }
    else
    {
        /* Set CCPL bit */
        usb_preg_set_ccpl( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_ControlEnd
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
