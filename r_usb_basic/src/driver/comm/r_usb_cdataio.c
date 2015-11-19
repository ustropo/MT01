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
* File Name    : r_usb_cdataio.c
* Description  : USB Host and Peripheral low level data I/O code
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


/******************************************************************************
Constant macro definitions
******************************************************************************/

/******************************************************************************
External variables and functions
******************************************************************************/

/******************************************************************************
Static variables and functions
******************************************************************************/
static  void usb_cstd_SelectNak(USB_UTR_t *ptr, uint16_t pipe);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* USB data transfer */
/* PIPEn Buffer counter */
uint32_t        usb_gcstd_DataCnt[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];
/* DMA0 direction */
uint16_t        usb_gcstd_Dma0Dir[USB_NUM_USBIP];
/* DMA0 buffer size */
uint32_t        usb_gcstd_Dma0Size[USB_NUM_USBIP];
/* DMA0 FIFO buffer size */
uint16_t        usb_gcstd_Dma0Fifo[USB_NUM_USBIP];
/* DMA0 pipe number */
uint16_t        usb_gcstd_Dma0Pipe[USB_NUM_USBIP];
/* PIPEn Buffer pointer(8bit) */
uint8_t         *usb_gcstd_DataPtr[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];
/* Message pipe */
USB_UTR_t       *usb_gcstd_Pipe[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];
/* XCKE Mode Flag */
uint16_t        usb_gcstd_XckeMode;
/* Hi-speed enable */
uint16_t        usb_gcstd_HsEnable[USB_NUM_USBIP];

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
Renesas Abstracted common data I/O functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_SendStart
Description     : Start data transmission using CPU/DMA transfer to USB host/
                : /device.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe no.
Return value    : none
******************************************************************************/
void usb_cstd_SendStart(USB_UTR_t *ptr, uint16_t pipe)
{
    USB_UTR_t       *pp;
    uint32_t        length;
    uint16_t        useport;

    /* Evacuation pointer */
    pp = usb_gcstd_Pipe[ptr->ip][pipe];
    length  = pp->tranlen;

    /* Check transfer count */
    if( pp->segment == USB_TRAN_CONT )
    {
        /* Sequence toggle */
        usb_cstd_DoSqtgl(ptr, pipe, pp->pipectr);
    }

    /* Select NAK */
    usb_cstd_SelectNak(ptr, pipe);
    /* Set data count */
    usb_gcstd_DataCnt[ptr->ip][pipe] = length;
    /* Set data pointer */
    usb_gcstd_DataPtr[ptr->ip][pipe] = (uint8_t*)pp->tranadr;
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][pipe] = (uint16_t)0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

    /* BEMP Status Clear */
    usb_creg_clr_sts_bemp( ptr, pipe );
    /* BRDY Status Clear */
    usb_creg_clr_sts_brdy( ptr, pipe );

    /* Pipe number to FIFO port select */
    useport = usb_cstd_Pipe2Fport(ptr, pipe);
    /* Check use FIFO access */
    switch( useport )
    {
    /* CFIFO use */
    case USB_CUSE:
        /* Buffer to FIFO data write */
        usb_cstd_Buf2Fifo(ptr, pipe, useport);
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        break;
    /* D0FIFO use */
    case USB_D0USE:
        /* D0 FIFO access is NG */
        USB_PRINTF1("### USB-ITRON is not support(SND-D0USE:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    /* D1FIFO use */
    case USB_D1USE:
        /* Buffer to FIFO data write */
        usb_cstd_Buf2Fifo(ptr, pipe, useport);
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        break;

#ifdef USB_DTC_ENABLE
    /* D0FIFO DMA */
    case USB_D0DMA:
        /* Setting for use PIPE number */
        usb_gcstd_Dma0Pipe[ptr->ip] = pipe;
        /* PIPE direction */
        usb_gcstd_Dma0Dir[ptr->ip]  = usb_cstd_GetPipeDir(ptr, pipe);
        /* Buffer size */
        usb_gcstd_Dma0Fifo[ptr->ip] = usb_cstd_GetBufSize(ptr, pipe);
        /* Check data count */
        if( usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] <= usb_gcstd_Dma0Fifo[ptr->ip] )
        {
            /* Transfer data size */
            usb_gcstd_Dma0Size[ptr->ip] = (uint16_t)usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]];
            /* Enable Empty Interrupt */
            usb_creg_set_bempenb(ptr, usb_gcstd_Dma0Pipe[ptr->ip]);
        }
        else
        {
            /* Data size == FIFO size */
            usb_gcstd_Dma0Size[ptr->ip] = usb_gcstd_Dma0Fifo[ptr->ip];
        }

        usb_cstd_Buf2fifoStartDma( ptr, pipe, useport );

        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        break;
    /* D1FIFO DMA */
    case USB_D1DMA:
        /* D1 FIFO access is NG */
        USB_PRINTF1("### USB-ITRON is not support(SND-D1DMA:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
#endif    /* USB_DTC_ENABLE */

    default:
        /* Access is NG */
        USB_PRINTF1("### USB-ITRON is not support(SND-else:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
End of function usb_cstd_SendStart
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_Buf2Fifo
Description     : Set USB registers as required to write from data buffer to USB 
                : FIFO, to have USB FIFO to write data to bus.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe     : Pipe no.
                : uint16_t useport  : Port no.
Return value    : none
******************************************************************************/
void usb_cstd_Buf2Fifo(USB_UTR_t *ptr, uint16_t pipe, uint16_t useport)
{
    uint16_t    end_flag;

    /* Disable Ready Interrupt */
    usb_creg_clr_brdyenb(ptr, pipe);
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][pipe] = (uint16_t)0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

    end_flag = usb_cstd_write_data( ptr, pipe, useport );

    /* Check FIFO access sequence */
    switch( end_flag )
    {
    case USB_WRITING:
        /* Continue of data write */
        /* Enable Ready Interrupt */
        usb_creg_set_brdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        break;
    case USB_WRITEEND:
        /* End of data write */
        /* continue */
    case USB_WRITESHRT:
        /* End of data write */
        /* Enable Empty Interrupt */
        usb_creg_set_bempenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        break;
    case USB_FIFOERROR:
        /* FIFO access error */
        USB_PRINTF0("### FIFO access error \n");
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    default:
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
End of function usb_cstd_Buf2Fifo
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_write_data
Description     : Switch PIPE, request the USB FIFO to write data, and manage 
                : the size of written data.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe         : Pipe no.
                : uint16_t pipemode     : CUSE/D0DMA/D1DMA
Return value    : uint16_t end_flag
******************************************************************************/
uint16_t usb_cstd_write_data(USB_UTR_t *ptr, uint16_t pipe, uint16_t pipemode )
{
    uint16_t    size, count, buffer, mxps;
    uint16_t    end_flag;

    /* Changes FIFO port by the pipe. */
    if( (pipemode == USB_CUSE) && (pipe == USB_PIPE0) )
    {
        buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t)USB_CUSE, (uint16_t)USB_ISEL);
    }
    else
    {
        buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t)pipemode, USB_NO);
    }

    /* Check error */
    if( buffer == USB_FIFOERROR )
    {
        /* FIFO access error */
        return (USB_FIFOERROR);
    }
    /* Data buffer size */
    size = usb_cstd_GetBufSize(ptr, pipe);
    /* Max Packet Size */
    mxps = usb_cstd_GetMaxPacketSize(ptr, pipe);

    /* Data size check */
    if( usb_gcstd_DataCnt[ptr->ip][pipe] <= (uint32_t)size )
    {
        count = (uint16_t)usb_gcstd_DataCnt[ptr->ip][pipe];
        /* Data count check */
        if( count == 0 )
        {
            /* Null Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else if( (count % mxps) != 0 )
        {
            /* Short Packet is end of write */
            end_flag = USB_WRITESHRT;
        }
        else
        {
            if( pipe == USB_PIPE0 )
            {
                /* Just Send Size */
                end_flag = USB_WRITING;
            }
            else
            {
                /* Write continues */
                end_flag = USB_WRITEEND;
            }
        }
    }
    else
    {
        /* Write continues */
        end_flag = USB_WRITING;
        count = size;
    }

    usb_gcstd_DataPtr[ptr->ip][pipe] = usb_cstd_write_fifo( ptr, count, pipemode, usb_gcstd_DataPtr[ptr->ip][pipe] );

    /* Check data count to remain */
    if( usb_gcstd_DataCnt[ptr->ip][pipe] < (uint32_t)size )
    {
        /* Clear data count */
        usb_gcstd_DataCnt[ptr->ip][pipe] = (uint32_t)0u;
        /* Read CFIFOCTR */
        buffer = usb_creg_read_fifoctr( ptr, pipemode );
        /* Check BVAL */
        if( (buffer & USB_BVAL) == 0u )
        {
            /* Short Packet */
            usb_creg_set_bval( ptr, pipemode );
        }
    }
    else
    {
        /* Total data count - count */
        usb_gcstd_DataCnt[ptr->ip][pipe] -= count;
    }
    /* End or Err or Continue */
    return end_flag;
}
/******************************************************************************
End of function usb_cstd_write_data
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_ReceiveStart
Description     : Start data reception using CPU/DMA transfer to USB Host/USB
                : device.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe no.
Return value    : none
******************************************************************************/
void usb_cstd_ReceiveStart(USB_UTR_t *ptr, uint16_t pipe)
{
    USB_UTR_t       *pp;
    uint32_t        length;
    uint16_t        mxps, useport;
#ifdef ISO_USE
//    uint16_t        bsts;
#endif /* ISO_USE */

    /* Evacuation pointer */
    pp      = usb_gcstd_Pipe[ptr->ip][pipe];
    length  = pp->tranlen;

    /* Check transfer count */
    if( pp->segment == USB_TRAN_CONT )
    {
        /* Sequence toggle */
        usb_cstd_DoSqtgl(ptr, pipe, pp->pipectr);
    }

    /* Select NAK */
    usb_cstd_SelectNak(ptr, pipe);
    /* Set data count */
    usb_gcstd_DataCnt[ptr->ip][pipe] = length;
    /* Set data pointer */
    usb_gcstd_DataPtr[ptr->ip][pipe] = (uint8_t*)pp->tranadr;

#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][pipe] = (uint16_t)0u;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

    /* Pipe number to FIFO port select */
    useport = usb_cstd_Pipe2Fport(ptr, pipe);

    /* Check use FIFO access */
    switch( useport )
    {
    /* D0FIFO use */
    case USB_D0USE:
        /* D0 FIFO access is NG */
        USB_PRINTF1("### USB-ITRON is not support(RCV-D0USE:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
        
    /* CFIFO use */
    case USB_CUSE:
        /* continue */
        
    /* D1FIFO use */
    case USB_D1USE:
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* Max Packet Size */
        mxps = usb_cstd_GetMaxPacketSize(ptr, pipe);
        if( length != (uint32_t)0u )
        {
            /* Data length check */
            if( (length % mxps) == (uint32_t)0u )
            {
                /* Set Transaction counter */
                usb_cstd_SetTransactionCounter(ptr, pipe, (uint16_t)(length / mxps));
            }
            else
            {
                /* Set Transaction counter */
                usb_cstd_SetTransactionCounter(ptr, pipe, (uint16_t)((length / mxps) + (uint32_t)1u));
            }
        }
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        /* Enable Ready Interrupt */
        usb_creg_set_brdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        break;
        
#ifdef USB_DTC_ENABLE
    /* D0FIFO DMA */
    case USB_D0DMA:
        /* Setting for use PIPE number */
        usb_gcstd_Dma0Pipe[ptr->ip] = pipe;
        /* PIPE direction */
        usb_gcstd_Dma0Dir[ptr->ip]  = usb_cstd_GetPipeDir(ptr, pipe);
        /* Buffer size */
        usb_gcstd_Dma0Fifo[ptr->ip] = usb_cstd_GetBufSize(ptr, pipe);
        /* Check data count */
        if( usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] < usb_gcstd_Dma0Fifo[ptr->ip] )
        {
            /* Transfer data size */
            usb_gcstd_Dma0Size[ptr->ip] = (uint16_t)usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]];
        }
        else
        {
            /* Data size == FIFO size */
            usb_gcstd_Dma0Size[ptr->ip] = usb_gcstd_Dma0Fifo[ptr->ip];
        }

        usb_cstd_Fifo2BufStartDma( ptr, pipe, useport, length );

        break;
        
    /* D1FIFO DMA */
    case USB_D1DMA:
        /* D1 FIFO access is NG */
        USB_PRINTF1("### USB-ITRON is not support(RCV-D1DMA:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
#endif    /* USB_DTC_ENABLE */

    default:
        USB_PRINTF1("### USB-ITRON is not support(RCV-else:pipe%d)\n", pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
End of function usb_cstd_ReceiveStart
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_Fifo2Buf
Description     : Request readout from USB FIFO to buffer and process depending
                : on status; read complete or reading.
Arguments       : USB_UTR_t *ptr : USB internal structure. Selects e.g. channel.
                : uint16_t pipe  : Pipe no.
Return value    : none
******************************************************************************/
void usb_cstd_Fifo2Buf(USB_UTR_t *ptr, uint16_t pipe, uint16_t useport)
{
    uint16_t    end_flag;

#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][pipe] = (uint16_t)0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    end_flag = USB_ERROR;

    end_flag = usb_cstd_read_data( ptr, pipe, useport );

    /* Check FIFO access sequence */
    switch( end_flag )
    {
    case USB_READING:
        /* Continue of data read */
        break;
        
    case USB_READEND:
        /* End of data read */
        usb_cstd_DataEnd(ptr, pipe, (uint16_t)USB_DATA_OK);
        break;
        
    case USB_READSHRT:
        /* End of data read */
        usb_cstd_DataEnd(ptr, pipe, (uint16_t)USB_DATA_SHT);
        break;
        
    case USB_READOVER:
        /* Buffer over */
        USB_PRINTF1("### Receive data over PIPE%d\n",pipe);
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_OVR);
        break;
        
    case USB_FIFOERROR:
        /* FIFO access error */
        USB_PRINTF0("### FIFO access error \n");
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
        
    default:
        usb_cstd_ForcedTermination(ptr, pipe, (uint16_t)USB_DATA_ERR);
        break;
    }
}
/******************************************************************************
End of function usb_cstd_Fifo2Buf
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_read_data
Description     : Request to read data from USB FIFO, and manage the size of 
                : the data read.
Arguments       : uint16_t pipe            : Pipe no.
Return value    : uint16_t end_flag
******************************************************************************/
uint16_t usb_cstd_read_data(USB_UTR_t *ptr, uint16_t pipe, uint16_t pipemode)
{
    uint16_t    count, buffer, mxps, dtln;
    uint16_t    end_flag;

    /* Changes FIFO port by the pipe. */
    buffer = usb_cstd_is_set_frdy(ptr, pipe, (uint16_t)pipemode, USB_NO);
    if( buffer == USB_FIFOERROR )
    {
//#ifndef ISO_USE
        /* FIFO access error */
        return (USB_FIFOERROR);
//#endif
    }
    dtln = (uint16_t)(buffer & USB_DTLN);
    /* Max Packet Size */
    mxps = usb_cstd_GetMaxPacketSize(ptr, pipe);

    if( usb_gcstd_DataCnt[ptr->ip][pipe] < dtln )
    {
        /* Buffer Over ? */
        end_flag = USB_READOVER;
        /* Set NAK */
        usb_cstd_SetNak(ptr, pipe);
        count = (uint16_t)usb_gcstd_DataCnt[ptr->ip][pipe];
        usb_gcstd_DataCnt[ptr->ip][pipe] = dtln;
    }
    else if( usb_gcstd_DataCnt[ptr->ip][pipe] == dtln )
    {
        /* Just Receive Size */
        count = dtln;
        if( (pipe == USB_PIPE0) && ((dtln % mxps) == 0) )
        {
            /* Just Receive Size */
            if( usb_cstd_is_host_mode(ptr) == USB_NO )
            {
                /* Peripheral Function */
                end_flag = USB_READING;
            }
            else
            {
                /* Host Function */
                end_flag = USB_READEND;
                /* Set NAK */
                usb_cstd_SelectNak(ptr, pipe);
            }
        }
        else
        {
            end_flag = USB_READEND;
            /* Set NAK */
            usb_cstd_SelectNak(ptr, pipe);
        }
    }
    else
    {
        /* Continus Receive data */
        count = dtln;
        end_flag = USB_READING;
        if( count == 0 )
        {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            /* Select NAK */
            usb_cstd_SelectNak(ptr, pipe);
        }
        if( (count % mxps) != 0 )
        {
            /* Null Packet receive */
            end_flag = USB_READSHRT;
            /* Select NAK */
            usb_cstd_SelectNak(ptr, pipe);
        }
    }

    if( dtln == 0 )
    {
        /* 0 length packet */
        /* Clear BVAL */
        usb_creg_set_bclr( ptr, pipemode );
    }
    else
    {
        usb_gcstd_DataPtr[ptr->ip][pipe] = usb_cstd_read_fifo( ptr, count, pipemode, usb_gcstd_DataPtr[ptr->ip][pipe] );
    }
    usb_gcstd_DataCnt[ptr->ip][pipe] -= count;

    /* End or Err or Continue */
    return (end_flag);
}
/******************************************************************************
End of function usb_cstd_read_data
******************************************************************************/


/******************************************************************************
Function Name   : usb_cstd_DataEnd
Description     : Set USB registers as appropriate after data transmission/re-
                : ception, and call the callback function as transmission/recep-
                : tion is complete.
Arguments       : uint16_t pipe     : Pipe no.
                : uint16_t status   : Transfer status type.
Return value    : none
******************************************************************************/
void usb_cstd_DataEnd(USB_UTR_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t    useport;

    /* PID = NAK */
    /* Set NAK */
    usb_cstd_SelectNak(ptr, pipe);
    /* Pipe number to FIFO port select */
    useport = usb_cstd_Pipe2Fport(ptr, pipe);

    /* Disable Interrupt */
    /* Disable Ready Interrupt */
    usb_creg_clr_brdyenb(ptr, pipe);
    /* Disable Not Ready Interrupt */
    usb_creg_clr_nrdyenb(ptr, pipe);
    /* Disable Empty Interrupt */
    usb_creg_clr_bempenb(ptr, pipe);

    /* Disable Transaction count */
    usb_cstd_ClrTransactionCounter(ptr, pipe);

    /* Check use FIFO */
    switch( useport )
    {
    /* CFIFO use */
    case USB_CUSE:
        break;
    /* D0FIFO use */
    case USB_D0USE:
        break;

#ifdef USB_DTC_ENABLE
    /* D0FIFO DMA */
    case USB_D0DMA:
        /* DMA buffer clear mode clear */
        usb_creg_clr_dclrm( ptr, USB_D0DMA );
        if(ptr -> ip == USB_USBIP_0)
        {
            usb_creg_set_mbw( ptr, USB_D0DMA, USB0_D0FIFO_MBW );
        }
        else if (ptr -> ip == USB_USBIP_1)
        {
            usb_creg_set_mbw( ptr, USB_D0DMA, USB1_D0FIFO_MBW );
        }
        usb_creg_write_dmacfg( ptr, USB_D0DMA, USB_CPU_ADR_RD_WR );
        break;
#endif    /* USB_DTC_ENABLE */

    /* D1FIFO use */
    case USB_D1USE:
        break;

#ifdef USB_DTC_ENABLE
    /* D1FIFO DMA */
    case USB_D1DMA:
        /* continue */
#endif    /* USB_DTC_ENABLE */

    default:
        break;
    }

    /* Call Back */
    if( usb_gcstd_Pipe[ptr->ip][pipe] != USB_NULL )
    {
        /* Check PIPE TYPE */
        if( usb_cstd_GetPipeType(ptr, pipe) != USB_ISO )
        {
            /* Transfer information set */
            usb_gcstd_Pipe[ptr->ip][pipe]->tranlen  = usb_gcstd_DataCnt[ptr->ip][pipe];
            usb_gcstd_Pipe[ptr->ip][pipe]->status   = status;
            usb_gcstd_Pipe[ptr->ip][pipe]->pipectr  = usb_creg_read_pipectr(ptr, pipe);
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
            usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = (uint8_t)usb_ghstd_IgnoreCnt[ptr->ip][pipe];
#else  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
            usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = 0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

            usb_gcstd_Pipe[ptr->ip][pipe]->ipp  = ptr->ipp;
            usb_gcstd_Pipe[ptr->ip][pipe]->ip   = ptr->ip;

            (usb_gcstd_Pipe[ptr->ip][pipe]->complete)(usb_gcstd_Pipe[ptr->ip][pipe], 0, 0);
            usb_gcstd_Pipe[ptr->ip][pipe] = (USB_UTR_t*)USB_NULL;
        }
        else
        {
            /* Transfer information set */
            usb_gcstd_Pipe[ptr->ip][pipe]->tranlen  = usb_gcstd_DataCnt[ptr->ip][pipe];
            usb_gcstd_Pipe[ptr->ip][pipe]->pipectr  = usb_creg_read_pipectr(ptr, pipe);
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
            usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = (uint8_t)usb_ghstd_IgnoreCnt[ptr->ip][pipe];
#else  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
            usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = 0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

            usb_gcstd_Pipe[ptr->ip][pipe]->ipp  = ptr->ipp;
            usb_gcstd_Pipe[ptr->ip][pipe]->ip   = ptr->ip;

            /* Data Transfer (restart) */
            if( usb_cstd_GetPipeDir(ptr, pipe) == USB_BUF2FIFO )
            {
                /* OUT Transfer */
                usb_gcstd_Pipe[ptr->ip][pipe]->status   = USB_DATA_WRITING;
                (usb_gcstd_Pipe[ptr->ip][pipe]->complete)(usb_gcstd_Pipe[ptr->ip][pipe], 0, 0);
            }
            else
            {
                /* IN Transfer */
                usb_gcstd_Pipe[ptr->ip][pipe]->status   = USB_DATA_READING;
                (usb_gcstd_Pipe[ptr->ip][pipe]->complete)(usb_gcstd_Pipe[ptr->ip][pipe], 0, 0);
            }
        }
    }
}
/******************************************************************************
End of function usb_cstd_DataEnd
******************************************************************************/

/******************************************************************************
Function Name   : void usb_cstd_SelectNak(uint16_t pipe)
Description     : Set the specified pipe PID to send a NAK if the transfer type 
                : is BULK/INT. 
Arguments       : uint16_t pipe     : Pipe number.
Return value    : none
******************************************************************************/
void usb_cstd_SelectNak(USB_UTR_t *ptr, uint16_t pipe)
{
    /* Check PIPE TYPE */
    if( usb_cstd_GetPipeType(ptr, pipe) != USB_ISO )
    {
        usb_cstd_SetNak(ptr, pipe);
    }
}
/******************************************************************************
End of function usb_cstd_SelectNak
******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
