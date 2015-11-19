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
* File Name    : r_usb_creg_dmadtc.c
* Description  : Setting code of DMA/DTC
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

#ifdef USB_DTC_ENABLE

#include "r_dtc_rx_if.h"            /* Defines for DTC support */
/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_BYTE_SIZE_0             0           /* 0Byte size */
#define USB_BYTE_SIZE_1             1           /* 1Byte size */
#define USB_BYTE_SIZE_2             2           /* 2Byte size */
#define USB_BYTE_SIZE_3             3           /* 3Byte size */
#define USB_BYTE_SIZE_4             4           /* 4Byte size */

#define USB_DTC_CRA_VAL_MAX         0xff        /* MAX Value for DTC Transfer count reg A */
#define USB_DTC_4ALIGNMENT_MASK     0xfffffffc;

/******************************************************************************
External variables and functions
******************************************************************************/
extern void         usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr );
extern void         usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr );
extern uint32_t     usb_cpu_get_dtc_Source_address(USB_UTR_t *ptr);
extern uint16_t     usb_cpu_get_dtc_block_count(USB_UTR_t *ptr);

extern USB_UTR_t    usb_gcstd_IntMsgD0fifo;
extern USB_UTR_t    usb2_gcstd_IntMsgD0fifo;

/******************************************************************************
Private global variables and functions
******************************************************************************/
void        usb_cstd_dtc_write_not_4alignment(USB_UTR_t *ptr);
void        usb_cstd_dtc_not_4alignment(USB_UTR_t *ptr);

uint8_t     usb_dtc_alignment_size;         /* Rounded 4Byte Alignent size for USB DTC Transfer data size */
uint8_t     usb_dtc_alignment_data[4];      /* Store rounded 4Byte Alignent data for USB DTC Transfer data */

/******************************************************************************
Function Name   : usb_cstd_Buf2D0fifoStartUsb
Description     : Setup to start DMA/DTC transfer from data buffer to D0FIFO.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_Buf2D0fifoStartUsb(USB_UTR_t *ptr)
{
    uint16_t    mbw;

    /* Write (MEMORY -> FIFO) : USB register set */
    if(ptr->ip == USB_USBIP_0)  /* USB0 */
    {
        if((usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) == 0u)
        {
            mbw = USB_MBW_16;
        }
        else
        {
            mbw = USB_MBW_8;
        }
    }
    else if(ptr->ip == USB_USBIP_1) /* USBHS */
    {
        if((usb_gcstd_Dma0Size[ptr->ip] & 0x0003u) == 0u)
        {
            mbw = USB_MBW_32;
        }
        else
        {
            mbw = USB_MBW_8;
        }
    }

    /* Change MBW setting */
    usb_creg_set_mbw( ptr, USB_D0DMA, mbw );

    /* DTC(D0FIFO) interrupt enable */
    usb_cpu_d0fifo_enable_dma(ptr);

    /* Set DREQ enable */
    usb_creg_set_dreqe( ptr, USB_D0DMA );
}/* eof usb_cstd_Buf2D0fifoStartUsb() */

/******************************************************************************
Function Name   : usb_cstd_D0fifo2BufStartUsb
Description     : Setup to start DMA/DTC transfer D0FIFO to buffer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifo2BufStartUsb(USB_UTR_t *ptr)
{
    /* Read (FIFO -> MEMORY) : USB register set */
    /* DMA buffer clear mode & MBW set */
    if(ptr->ip == USB_USBIP_0)  /* USB0 */
    {
        usb_creg_set_mbw( ptr, USB_D0DMA, (uint16_t)(USB_MBW_16) );
    }
    else if(ptr->ip == USB_USBIP_1) /* USBHS */
    {
        usb_creg_set_mbw( ptr, USB_D0DMA, (uint16_t)(USB_MBW_32) );
    }
    usb_creg_clr_dclrm( ptr, USB_D0DMA );

    /* Set DREQ enable */
    usb_creg_set_dreqe( ptr, USB_D0DMA );
}/* eof usb_cstd_D0fifo2BufStartUsb */

/******************************************************************************
Function Name   : usb_cstd_D0fifoStopUsb
Description     : Setup external variables used for USB data transfer; to reg-
                : ister if you want to stop the transfer of DMA/DTC.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifoStopUsb(USB_UTR_t *ptr)
{
    uint16_t    ip = ptr->ip;
    uint16_t    pipe = usb_gcstd_Dma0Pipe[ip];
    uint32_t    transfer_size = usb_gcstd_Dma0Size[ip];
    uint32_t    *request_size = &usb_gcstd_DataCnt[ip][pipe];
    uint8_t     *tran_data_ptr = (uint8_t *)&usb_gcstd_DataPtr[ip][pipe];

    usb_creg_clr_dreqe( ptr, USB_D0DMA );

    /* Direction check */
    if( usb_gcstd_Dma0Dir[ip] == USB_BUF2FIFO )
    {
        /* Buffer to FIFO */
        if( *request_size < transfer_size )
        {
            /* >yes then set BVAL */
            *tran_data_ptr += *request_size;
            *request_size = (uint32_t)0u;
            /* Disable Ready Interrupt */
            usb_creg_clr_brdyenb(ptr, pipe);
            /* Set BVAL */
            usb_creg_set_bval( ptr, USB_D0DMA );
        }
        else
        {
            *tran_data_ptr += transfer_size;
            /* Set data count to remain */
            *request_size -= transfer_size;
        }
    }
    else
    {
        /* FIFO to Buffer */
        *tran_data_ptr += transfer_size;
        /* Set data count to remain */
        if( *request_size < transfer_size )
        {
            *request_size = transfer_size - *request_size;
        }
        else
        {
            *request_size -= transfer_size;
        }
    }
}/* eof usb_cstd_D0fifoStopUsb() */

/******************************************************************************
Function Name   : usb_cstd_D0fifoInt
Description     : Set end of DMA/DTC transfer. Set to restart DMA/DTC trans-
                : fer according to data size of remaining functions to be pro-
                : cessed.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0fifoInt(USB_UTR_t *ptr)
{
    uint16_t pipe;
    uint16_t ip;
    uint16_t maxps;
    uint32_t *transfer_count;

    ip = ptr->ip;
    pipe = usb_gcstd_Dma0Pipe[ip];
    maxps = usb_gcstd_Dma0Fifo[ip];
    transfer_count = &usb_gcstd_DataCnt[ip][pipe];

    /* Transfer count >= MAXPS */
    if ( *transfer_count >= maxps )
    {
        /* DMA Transfer count update */
        *transfer_count %= maxps;
        /* Transfer continue check */
        if( *transfer_count != 0 )
        {
            /* Transfer count != MAXPS * N */
            /* Odd size data check  */
            if(ptr->ip == USB_USBIP_0)
            {
                if( (*transfer_count & 0x0001u) != 0u )
                {
                    /* if count == odd */
                    usb_creg_set_mbw( ptr, USB_D0DMA, USB_MBW_8 );
                }
            }
            else if(ptr->ip == USB_USBIP_1)
            {
                if( (*transfer_count & 0x0003u) != 0u )
                {
                    /* if count == odd */
                    usb_creg_set_mbw( ptr, USB_D0DMA, USB_MBW_8 );
                }
            }

            /* DMA Transfer size update */
            usb_gcstd_Dma0Size[ip] = *transfer_count;
            /* DMA Restart */
            usb_cpu_d0fifo_restart_dma(ptr);

            /* DTC(D0FIFO) interrupt enable */
            usb_cpu_d0fifo_enable_dma(ptr);

            /* DMA Transfer Request Set */
            usb_creg_set_dreqe( ptr, USB_D0DMA );
        }
        else
        {
            /* Check Rounded data for 4Byte Alignment */
            if( usb_dtc_alignment_size )
            {
                /* count == odd ( 1 to 3 ) */
                *transfer_count = usb_dtc_alignment_size;
                usb_creg_set_mbw( ptr, USB_D0DMA, USB_MBW_8 );

                /* DMA Transfer size update */
                usb_gcstd_Dma0Size[ip] = *transfer_count;
                /* DMA Restart */
                usb_cpu_d0fifo_restart_dma(ptr);

                /* DTC(D0FIFO) interrupt enable */
                usb_cpu_d0fifo_enable_dma(ptr);

                /* DMA Transfer Request Set */
                usb_creg_set_dreqe( ptr, USB_D0DMA );
                usb_dtc_alignment_size = 0;
            }
        }
    }
    else if( *transfer_count == 0 )
    {
        /* More than enough Interrupt */
        return;
    }
    else
    {
        /* Write Rounded data for D0FIFO */
        usb_cstd_dtc_write_not_4alignment(ptr);

        /* Transfer count < MAXPS */
        usb_creg_set_bval( ptr, USB_D0DMA );
        /* Transfer complete */
        *transfer_count = 0;
    }

    /* Transfer complete check */
    if( *transfer_count == 0 )
    {
        /* Enable Empty Interrupt */
        usb_creg_set_bempenb(ptr, pipe);
    }
}/* eof usb_cstd_D0fifoInt() */



/******************************************************************************
Function Name   : usb_cstd_Buf2fifoStartDma
Description     : Start transfer using DMA/DTC. If transfer size is 0, write 
                : more data to buffer.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
Return value    : none
******************************************************************************/
void    usb_cstd_Buf2fifoStartDma( USB_UTR_t *ptr, uint16_t pipe, uint16_t useport )
{
    /* Transfer size check */
    if(usb_gcstd_Dma0Size[ptr->ip] != 0)
    {
        if(ptr->ip == USB_USBIP_0)
        {
            if((usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) == 0u)
            {
                /* 16bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo16Adr(ptr));
            }
            else
            {
                /* 8bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
            }
        }
        else if(ptr->ip == USB_USBIP_1)
        {
            /* USB Transfer data size 4Byte Alignment. */
            usb_cstd_dtc_not_4alignment(ptr);

            if((usb_gcstd_Dma0Size[ptr->ip] & 0x0003u) == 0u)
            {
                /* 32bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo32Adr(ptr));
            }
            else
            {
                /* 8bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_buf2d0fifo_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
            }
        }

        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
        /* CPU access Buffer to FIFO start */
        usb_cstd_Buf2D0fifoStartUsb(ptr);
    }
    else
    {
        /* Buffer to FIFO data write */
        usb_cstd_Buf2Fifo(ptr, pipe, useport);
    }
}/* eof usb_cstd_Buf2fifoStartDma() */


/******************************************************************************
Function Name   : usb_cstd_Fifo2BufStartDma
Description     : Start transfer using DMA/DTC. If transfer size is 0, clear DMA. 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipe   : Pipe nr.
                : uint16_t useport: FIFO select
                : uint32_t length
Return value    : none
******************************************************************************/
void    usb_cstd_Fifo2BufStartDma( USB_UTR_t *ptr, uint16_t pipe, uint16_t useport, uint32_t length )
{
    uint16_t        mxps;

    /* Data size check */
    if( usb_gcstd_Dma0Size[ptr->ip] != 0u )
    {
        if(ptr->ip == USB_USBIP_0)
        {
            if((usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) == 0u)
            {
                /* 16bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo16Adr(ptr));
            }
            else
            {
                /* 8bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
            }
        }
        else if(ptr->ip == USB_USBIP_1)
        {
            if((usb_gcstd_Dma0Size[ptr->ip] & 0x0003u) == 0u)
            {
                /* 32bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo32Adr(ptr));
            }
            else
            {
                /* 8bit access */
                /* DMA access Buffer to FIFO start */
                usb_cpu_d0fifo2buf_start_dma(ptr, usb_cstd_GetD0fifo8Adr(ptr));
            }
        }

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
        usb_cstd_D0fifo2BufStartUsb(ptr);
    }
    else
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, pipe, useport, USB_NO);
        /* DMA buffer clear mode set */
        usb_creg_set_dclrm( ptr, USB_D0DMA );
        /* Set BUF */
        usb_cstd_SetBuf(ptr, pipe);
        /* Enable Ready Interrupt */
        usb_creg_set_brdyenb(ptr, pipe);
        /* Enable Not Ready Interrupt */
        usb_cstd_NrdyEnable(ptr, pipe);
    }
} /* eof usb_cstd_Fifo2BufStartDma() */


#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP0_PP == USB_PERI_PP
/******************************************************************************
Function Name   : usb_cstd_d0fifo_handler
Description     : DMA interrupt routine. Send message to PCD/HCD task.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cstd_d0fifo_handler(void)
{
    USB_UTR_t       *ptr;

    ptr = &usb_gcstd_IntMsgD0fifo;
    ptr->msghead = (USB_MH_t)USB_NULL;
    ptr->keyword = 0;
    ptr->status  = 0;
    ptr->ip  = USB_USBIP_0;
    ptr->ipp = usb_cstd_GetUsbIpAdr( ptr->ip );

    usb_creg_clr_dreqe( ptr, USB_D0DMA );       /* DMA Transfer request disable */
    usb_cpu_d0fifo_stop_dma(ptr);               /* Stop DMA,FIFO access */

    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_PERI_PP
        USB_ER_t        err;

        /* Peripheral Function */
        ptr->msginfo = USB_MSG_PCD_D0FIFO_INT;
        /* Send message */
        err = USB_ISND_MSG(USB_PCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
                USB_PRINTF1("### DmaHandler DEF1 isnd_msg error (%ld)\n", err);
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_PERI_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP
        USB_ER_t        err;

        ptr->msginfo = USB_MSG_HCD_D0FIFO_INT;
        /* Send message */
        err = USB_ISND_MSG(USB_HCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### DmaHandler DEF2 isnd_msg error (%ld)\n", err);
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP */
    }
} /* eof usb_cstd_d0fifo_handler() */
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP0_PP == USB_PERI_PP */

#if USB_FUNCSEL_USBIP1_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
/******************************************************************************
Function Name   : usb2_cstd_d0fifo_handler
Description     : DMA interrupt routine. Send message to PCD/HCD task.
Arguments       : none
Return value    : none
******************************************************************************/
void usb2_cstd_d0fifo_handler(void)
{
/* Conditional compile dep. on difference of USB function */
#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
    USB_UTR_t       *ptr;

    ptr = &usb2_gcstd_IntMsgD0fifo;
    ptr->msghead = (USB_MH_t)USB_NULL;
    ptr->keyword = 0;
    ptr->status  = 0;
    ptr->ip  = USB_USBIP_1;
    ptr->ipp = usb_cstd_GetUsbIpAdr( ptr->ip );

    usb_creg_clr_dreqe( ptr, USB_D0DMA );       /* DMA Transfer request disable */
    usb_cpu_d0fifo_stop_dma(ptr);               /* Stop DMA,FIFO access */

    if( usb_cstd_is_host_mode(ptr) == USB_NO )
    {
#if USB_FUNCSEL_USBIP1_PP == USB_PERI_PP
        USB_ER_t        err;

        /* Peripheral Function */
        ptr->msginfo = USB_MSG_PCD_D0FIFO_INT;
        /* Send message */
        err = USB_ISND_MSG(USB_PCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### DmaHandler DEF1 isnd_msg error (%ld)\n", err);
        }
#endif /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */
    }
    else
    {
#if USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        USB_ER_t        err;

        /* Host Function */
        ptr->msginfo = USB_MSG_HCD_D0FIFO_INT;
        /* Send message */
        err = USB_ISND_MSG(USB_HCD_MBX, (USB_MSG_t*)ptr);
        if( err != USB_E_OK )
        {
            USB_PRINTF1("### DmaHandler DEF2 isnd_msg error (%ld)\n", err);
        }
#endif /* USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
#endif  /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */
} /* eof usb2_cstd_d0fifo_handler() */
#endif  /* USB_FUNCSEL_USBIP1_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_PERI_PP */

/******************************************************************************
Function Name   : usb_cstd_dtc_not_4alignment
Description     : USB Transfer data size 4Byte Alignment.
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void usb_cstd_dtc_not_4alignment(USB_UTR_t *ptr)
{
    uint32_t    offset;
    uint8_t     *data_ptr;
    uint16_t pipe;

    usb_dtc_alignment_size = USB_BYTE_SIZE_0;

    /* Get DTC Transfer pipe no. */
    pipe = usb_gcstd_Dma0Pipe[ptr->ip];

    /* Check transfer size.(Over 8bit size?) */
    if( usb_gcstd_DataCnt[ptr->ip][pipe] > USB_DTC_CRA_VAL_MAX )
    {
        /* Check 4Byte alignment */
        if( ( usb_gcstd_DataCnt[ptr->ip][pipe] % USB_BYTE_SIZE_4 ) != USB_BYTE_SIZE_0 )
        {
            /* Get transfer data top address. */
            data_ptr = usb_gcstd_DataPtr[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]];

            /* Get alignment size */
            usb_dtc_alignment_size = usb_gcstd_DataCnt[ptr->ip][pipe] % USB_BYTE_SIZE_4;
            /* Round transfer data size */
            usb_gcstd_DataCnt[ptr->ip][pipe] &= USB_DTC_4ALIGNMENT_MASK;

            /* Store alignment data */
            offset = usb_gcstd_DataCnt[ptr->ip][pipe];
            if( usb_dtc_alignment_size == USB_BYTE_SIZE_3 )
            {
                usb_dtc_alignment_data[0] = data_ptr[offset];
                usb_dtc_alignment_data[1] = data_ptr[offset+1];
                usb_dtc_alignment_data[2] = data_ptr[offset+2];
            }
            else if( usb_dtc_alignment_size == USB_BYTE_SIZE_2 )
            {
                usb_dtc_alignment_data[0] = data_ptr[offset];
                usb_dtc_alignment_data[1] = data_ptr[offset+1];
            }
            else
            {
                usb_dtc_alignment_data[0] = data_ptr[offset];
            }
        }
    }
}   /* eof usb_cstd_dtc_not_4alignment() */

/******************************************************************************
Function Name   : usb_cstd_dtc_write_not_4alignment
Description     : Write Rounded data for D0FIFO
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void usb_cstd_dtc_write_not_4alignment(USB_UTR_t *ptr)
{
    /* Check alignment data */
    if( usb_dtc_alignment_size > USB_BYTE_SIZE_0 )
    {
        /* DMA buffer clear mode & MBW set */
        usb_creg_set_mbw( ptr, USB_D0DMA, USB_MBW_8 );

        /* Write alignment data for D0FIFO */
        if( usb_dtc_alignment_size == USB_BYTE_SIZE_3 )
        {
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[0] );
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[1] );
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[2] );
        }
        else if( usb_dtc_alignment_size == USB_BYTE_SIZE_2 )
        {
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[0] );
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[1] );
        }
        else
        {
            usb_creg_write_fifo8( ptr, USB_D0DMA, usb_dtc_alignment_data[0] );
        }

        usb_dtc_alignment_size = USB_BYTE_SIZE_0;
    }
}   /* eof usb_cstd_dtc_write_not_4alignment() */

#endif /* USB_DTC_ENABLE */

/******************************************************************************
Function Name   : usb_cstd_brdy_pipe
Description     : Search for the PIPE No. that BRDY interrupt occurred, and 
                  request data transmission/reception from the PIPE
Arguments       : USB_UTR_t *ptr
                : uint16_t bitsts       ; BRDYSTS Register & BRDYENB Register
Return value    : none
******************************************************************************/
void usb_cstd_brdy_pipe(USB_UTR_t *ptr, uint16_t bitsts)
{
    uint16_t        useport;
    uint16_t        i;
    uint16_t ip;

#ifdef USB_DTC_ENABLE
    uint16_t        buffer;
    uint16_t maxps;
    uint16_t set_dtc_brock_cnt;
    uint16_t trans_dtc_block_cnt;
#endif /* USB_DTC_ENABLE */

    ip = ptr->ip;
#ifdef USB_DTC_ENABLE
    maxps = usb_gcstd_Dma0Fifo[ip];
#endif /* USB_DTC_ENABLE */
    for( i = USB_PIPE1; i <= USB_MAX_PIPE_NO; i++ )
    {
        if( (bitsts & USB_BITSET(i)) != 0 )
        {
            /* Interrupt check */
            usb_creg_clr_sts_bemp( ptr, i );

            if( usb_gcstd_Pipe[ip][i] != USB_NULL )
            {
                /* Pipe number to FIFO port select */
                useport = usb_cstd_Pipe2Fport(ptr, i);
                if( useport == USB_D0DMA )
                {
#ifdef USB_DTC_ENABLE
                    /* DMA Transfer request disable */
                    usb_creg_clr_dreqe( ptr, USB_D0DMA );

                    /* DMA stop */
                    usb_cpu_d0fifo_stop_dma(ptr);

                    /* Changes FIFO port by the pipe. */
                    buffer = usb_cstd_is_set_frdy(ptr, i, useport, USB_NO);

                    set_dtc_brock_cnt = (uint16_t)((usb_gcstd_DataCnt[ip][usb_gcstd_Dma0Pipe[ip]] -1)
                        / usb_gcstd_Dma0Fifo[ip]) +1;

                    trans_dtc_block_cnt = usb_cpu_get_dtc_block_count(ptr);
                    /* Get D0fifo Receive Data Length */
                    usb_gcstd_Dma0Size[ip]
                        = (uint32_t)(buffer & USB_DTLN) + (set_dtc_brock_cnt - (trans_dtc_block_cnt + 1)) * maxps;

                    /* Check data count */
                    if( usb_gcstd_Dma0Size[ip] == usb_gcstd_DataCnt[ptr->ip][i] )
                    {
                        usb_gcstd_DataCnt[ip][i] = 0;
                        /* End of data transfer */
                        usb_cstd_DataEnd(ptr, i, (uint16_t)USB_DATA_OK);
                    }
                    else if( usb_gcstd_Dma0Size[ip] > usb_gcstd_DataCnt[ip][i] )
                    {
                        /* D0FIFO access DMA stop */
                        usb_cstd_D0fifoStopUsb(ptr);
                        /* End of data transfer */
                        usb_cstd_DataEnd(ptr, i, (uint16_t)USB_DATA_OVR);
                    }
                    else
                    {
                        /* D0FIFO access DMA stop */
                        usb_cstd_D0fifoStopUsb(ptr);
                        /* End of data transfer */
                        usb_cstd_DataEnd(ptr, i, (uint16_t)USB_DATA_SHT);
                    }
                    /* Set BCLR */
                    usb_creg_set_bclr( ptr, USB_D0DMA );
#endif /* USB_DTC_ENABLE */
                }
                else
                {
                    if( usb_cstd_GetPipeDir(ptr, i) == USB_BUF2FIFO )
                    {
                        /* Buffer to FIFO data write */
                        usb_cstd_Buf2Fifo(ptr, i, useport);
                    }
                    else
                    {
                        /* FIFO to Buffer data read */
                        usb_cstd_Fifo2Buf(ptr, i, useport);
                    }
                }
            }
        }
    }
}/* eof usb_cstd_brdy_pipe() */

/******************************************************************************
End of file
******************************************************************************/
