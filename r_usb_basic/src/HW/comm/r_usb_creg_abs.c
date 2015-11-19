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
* File Name    : r_usb_creg_abs.c
* Description  : Call USB register access function
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
/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
    #define USB_FIFOENDIAN      USB_FIFO_LITTLE
#else   /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
    #define USB_FIFOENDIAN      USB_FIFO_BIG
#endif  /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */

#define USB_BUFSIZE_BIT 10

/******************************************************************************
Static variables and functions
******************************************************************************/
#ifdef USB_DTC_ENABLE
static  void        usb_cstd_D0FifoselSet(USB_UTR_t *ptr);
#endif /* USB_DTC_ENABLE */
/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t    usb_gcstd_RhstBit;
uint16_t    usb_gcstd_DvsqBit;
uint16_t    usb_gcstd_AddrBit;
uint16_t    usb_gcstd_SqmonBit;

/******************************************************************************
Function Name   : usb_cstd_GetUsbIpAdr
Description     : Get base address of the selected USB channel's peripheral 
                : registers.
Argument        : uint16_t ipnum  : USB_USBIP_0 (0), or USB_USBIP_1 (1).
Return          : USB_REGADR_t    : A pointer to the USB_597IP register 
                                  : structure USB_REGISTER containing all USB
                                  : channel's registers.
******************************************************************************/
USB_REGADR_t    usb_cstd_GetUsbIpAdr( uint16_t ipnum )
{
    USB_REGADR_t    ptr;

    if( ipnum == USB_USBIP_0 )
    {
        ptr = (USB_REGADR_t)&USB0;
    }
    else if( ipnum == USB_USBIP_1 )
    {
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
        ptr = (USB_REGADR_t)&USBA;
#else
        ptr = (USB_REGADR_t)&USB1;
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
    }
    else
    {
        USB_DEBUG_HOOK( USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE1 );
    }

    return ptr;
} /* eof usb_cstd_GetUsbIpAdr() */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
Function Name   : usb_cstd_GetD0fifo32Adr
Description     : Get 16 bits of used channel's D0FIFO register content.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return          : Address of D0FIFO
******************************************************************************/
uint32_t    usb_cstd_GetD0fifo32Adr( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_0)
    {
        return (uint32_t)0;
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        return (uint32_t)(&ptr->ipp1->D0FIFO.LONG);
    }

    return (uint32_t)0;
}/* eof usb_cstd_GetD0fifo32Adr() */
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
/******************************************************************************
Function Name   : usb_cstd_GetD0fifo16Adr
Description     : Get 16 bits of used channel's D0FIFO register content.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return          : Address of D0FIFO
******************************************************************************/
uint32_t    usb_cstd_GetD0fifo16Adr( USB_UTR_t *ptr )
{
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if(ptr->ip == USB_USBIP_0)
    {
        return (uint32_t)(&ptr->ipp->D0FIFO.WORD);
    }
    else if(ptr->ip == USB_USBIP_1)
    {
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
        return (uint32_t)(&ptr->ipp1->D0FIFO.WORD.H);

#else
        return (uint32_t)(&ptr->ipp1->D0FIFO.WORD.L);
#endif  /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
    }

    return 0;
#else
    return (uint32_t)(&ptr->ipp->D0FIFO.WORD);
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
}/* eof usb_cstd_GetD0fifo16Adr() */

/******************************************************************************
Function Name   : usb_cstd_GetD0fifo8Adr
Description     : Get 8 bits of used channel's D0FIFO register content.
Arguments       : USB_UTR_t *ptr : USB system internal structure. Selects channel.
Return          : Address of D0FIFO
******************************************************************************/
uint32_t    usb_cstd_GetD0fifo8Adr( USB_UTR_t *ptr )
{
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if(ptr->ip == USB_USBIP_0)
    {
        return (uint32_t)(&ptr->ipp->D0FIFO.BYTE.L);
    }
    else if(ptr->ip == USB_USBIP_1)
    {
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
        return (uint32_t)(&ptr->ipp1->D0FIFO.BYTE.HH);
#else
        return (uint32_t)(&ptr->ipp1->D0FIFO.BYTE.LL);
#endif  /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
    }

    return 0;
#else
    return (uint32_t)(&ptr->ipp->D0FIFO.BYTE.L);
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
}/* eof usb_cstd_GetD0fifo8Adr() */

/******************************************************************************
Function Name   : usb_cstd_AsspConfig
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : not used
Return value    : -
******************************************************************************/
void usb_cstd_AsspConfig(USB_UTR_t *ptr)
{
}/* eof usb_cstd_AsspConfig() */

/******************************************************************************
Function Name   : usb_cstd_Pinconfig
Description     : Set FIFO select register. This will assign a pipe to the FIFOs, 
                : and control how FIFOs are accessed.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
Return value    : -
******************************************************************************/
void usb_cstd_Pinconfig(USB_UTR_t *ptr)
{
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if(ptr->ip == USB_USBIP_0)
    {
        /* CFIFO Port Select Register  (0x20) */
        usb_creg_write_fifosel( ptr, USB_CUSE, USB0_CFIFO_MBW );
        /* D0FIFO Port Select Register (0x28) */
        usb_creg_write_fifosel( ptr, USB_D0USE, USB0_D0FIFO_MBW );
        /* D1FIFO Port Select Register (0x2C) */
        usb_creg_write_fifosel( ptr, USB_D1USE, USB0_D1FIFO_MBW );
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        /* CFIFO Port Select Register  (0x20) */
        usb_creg_write_fifosel( ptr, USB_CUSE, USB1_CFIFO_MBW );
        /* D0FIFO Port Select Register (0x28) */
        usb_creg_write_fifosel( ptr, USB_D0USE, USB1_D0FIFO_MBW );
        /* D1FIFO Port Select Register (0x2C) */
        usb_creg_write_fifosel( ptr, USB_D1USE, USB1_D1FIFO_MBW );
    }
    /* setting ENDIAN for CFIFOSEL */
    usb_creg_set_bigend( ptr, USB_CUSE, USB_FIFOENDIAN );
    /* setting ENDIAN for D0FIFOSEL */
    usb_creg_set_bigend( ptr, USB_D0USE, USB_FIFOENDIAN );
    /* setting ENDIAN for D1FIFOSEL */
    usb_creg_set_bigend( ptr, USB_D1USE, USB_FIFOENDIAN );
#else
    /* CFIFO Port Select Register  (0x20) */
    usb_creg_write_fifosel( ptr, USB_CUSE, USB0_CFIFO_MBW );
    /* D0FIFO Port Select Register (0x28) */
    usb_creg_write_fifosel( ptr, USB_D0USE, USB0_D0FIFO_MBW );
    /* D1FIFO Port Select Register (0x2C) */
    usb_creg_write_fifosel( ptr, USB_D1USE, USB0_D1FIFO_MBW );
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
} /* eof usb_cstd_Pinconfig() */

/******************************************************************************
Function Name   : usb_cstd_InitialClock
Description     : Enable USB module clock. Resets and starts peripheral.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
Return value    : none
******************************************************************************/
void usb_cstd_InitialClock(USB_UTR_t *ptr)
{
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if (ptr -> ip  == USB_USBIP_0)
    {
        usb_creg_set_scke( ptr );
    }
    else if (ptr -> ip == USB_USBIP_1)
    {

#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
        usb_creg_clr_hseb( ptr );
        usb_creg_write_repsel( ptr );
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */

        usb_creg_write_clksel( ptr );
        usb_creg_clr_pllreset( ptr );
        usb_creg_clr_dirpd( ptr );
        usb_creg_set_suspendm( ptr );

        while(!(ptr-> ipp1->PLLSTA.BIT.PLLLOCK));
    }
    else{
    }
#else
    usb_creg_set_scke( ptr );
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */

}/* eof usb_cstd_InitialClock() */

/******************************************************************************
Function Name   : usb_cstd_InterruptClock
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : Not used
Return value    : none
******************************************************************************/
void usb_cstd_InterruptClock(USB_UTR_t *ptr)
{
}/* eof usb_cstd_InterruptClock() */

/******************************************************************************
Function Name   : usb_cstd_SelfClock
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : Not used
Return value    : none
******************************************************************************/
void usb_cstd_SelfClock(USB_UTR_t *ptr)
{
}/* eof usb_cstd_SelfClock() */

/******************************************************************************
Function Name   : usb_cstd_StopClock
Description     : Not processed as the functionality is provided by R8A66597(ASSP).
Arguments       : USB_UTR_t *ptr    : Not used
Return value    : none
******************************************************************************/
void usb_cstd_StopClock(USB_UTR_t *ptr)
{
}/* eof usb_cstd_StopClock() */

#ifdef USB_DTC_ENABLE
/******************************************************************************
Function Name   : usb_cstd_D0FifoselSet
Description     : Set DOFIFO access width, set to DMA buffer clear mode and 
                : the endian setting.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
Return value    : none
******************************************************************************/
void usb_cstd_D0FifoselSet(USB_UTR_t *ptr)
{
    /* Big endian mode set */
//  usb_creg_set_bigend( ptr, USB_D0DMA, 1 ); 
    /* DMA buffer clear mode set */
    usb_creg_clr_dclrm( ptr, USB_D0DMA );
    /* Maximum bit width for FIFO access set */
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    if(ptr->ip == USB_USBIP_0)
    {
        usb_creg_set_mbw( ptr, USB_D0DMA, USB0_D0FIFO_MBW );
    }
    else if (ptr->ip == USB_USBIP_1)
    {
        usb_creg_set_mbw( ptr, USB_D0DMA, USB1_D0FIFO_MBW );
    }
#else
    usb_creg_set_mbw( ptr, USB_D0DMA, USB0_D0FIFO_MBW );
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */

}/* eof usb_cstd_D0FifoselSet() */
#endif    /* USB_DTC_ENABLE */

/******************************************************************************
Function Name   : usb_cstd_GetBufSize
Description     : Return buffer size, or max packet size, of specified pipe.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe number.
Return value    : uint16_t          : FIFO buffer size or max packet size.
******************************************************************************/
uint16_t usb_cstd_GetBufSize(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t    size, buffer;

    if(ptr->ip == USB_USBIP_0)
    {
        if( pipe == USB_PIPE0 )
        {
            /* Not continuation transmit */
            buffer = usb_creg_read_dcpmaxp( ptr );
        }
        else
        {
            /* Pipe select */
            usb_creg_write_pipesel( ptr, pipe );
            buffer = usb_creg_read_pipemaxp( ptr );
        }
        /* Max Packet Size */
        size = (uint16_t)(buffer & USB_MXPS);
    }
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
    else
    {
        if( pipe == USB_PIPE0 )
        {
            buffer = usb_creg_read_dcpcfg( ptr );
            if( (buffer & USB_CNTMDFIELD) == USB_CNTMDFIELD )
            {
                /* Continuation transmit */
                /* Buffer Size */
                size = USB_PIPE0BUF;
            }
            else
            {
                /* Not continuation transmit */
                buffer = usb_creg_read_dcpmaxp( ptr );
                /* Max Packet Size */
                size = (uint16_t)(buffer & USB_MAXP);
            }
        }
        else
        {
            /* Pipe select */
            usb_creg_write_pipesel( ptr, pipe );
            
            /* Read CNTMD */
            buffer = usb_creg_read_pipecfg( ptr );
            if( (buffer & USB_CNTMDFIELD) == USB_CNTMDFIELD )
            {
                buffer = usb_creg_read_pipebuf( ptr );
                /* Buffer Size */
                size = (uint16_t)((uint16_t)((buffer >> USB_BUFSIZE_BIT) + 1) * USB_PIPEXBUF);
            }
            else
            {
                buffer = usb_creg_read_pipemaxp( ptr );
                /* Max Packet Size */
                size = (uint16_t)(buffer & USB_MXPS);
            }
        }
    }
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
    return size;
}/* eof usb_cstd_GetBufSize() */

/******************************************************************************
Function Name   : usb_cstd_pipe_init
Description     : Initialization of registers associated with specified pipe.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe Number
                : uint16_t *tbl     : ep table
                : uint16_t ofs      : ep table offset
Return value    : none
******************************************************************************/
void usb_cstd_pipe_init(USB_UTR_t *ptr, uint16_t pipe, uint16_t *tbl, uint16_t ofs)
{

    usb_gcstd_Pipe[ptr->ip][pipe] = (USB_UTR_t*)USB_NULL;

    /* Interrupt Disable */
    /* Ready         Int Disable */
    usb_creg_clr_brdyenb( ptr, pipe );

    /* NotReady      Int Disable */
    usb_creg_clr_nrdyenb( ptr, pipe );

    /* Empty/SizeErr Int Disable */
    usb_creg_clr_bempenb( ptr, pipe );

    /* PID=NAK & clear STALL */
    usb_cstd_ClrStall(ptr, pipe);
    
    /* PIPE Configuration */
    usb_creg_write_pipesel( ptr, pipe );

    if( USB_D0DMA == tbl[ofs + 5] )
    {
        tbl[ofs + 1] |= USB_BFREON;
    }

    usb_creg_write_pipecfg( ptr, tbl[ofs + 1]);

    usb_creg_write_pipebuf( ptr, tbl[ofs + 2] );
    usb_creg_write_pipemaxp( ptr, tbl[ofs + 3] );
    usb_creg_write_pipeperi( ptr, tbl[ofs + 4] );

    /* FIFO buffer DATA-PID initialized */
    usb_creg_write_pipesel( ptr, USB_PIPE0 );

    /* SQCLR */
    usb_creg_set_sqclr(ptr, pipe);
    /* ACLRM */
    usb_cstd_DoAclrm(ptr, pipe);
    /* CSSTS */
    usb_creg_set_csclr(ptr, pipe);
    
    /* Interrupt status clear */
    /* Ready         Int Clear */
    usb_creg_clr_sts_brdy( ptr, pipe );

    /* NotReady      Int Clear */
    usb_creg_clr_sts_nrdy( ptr, pipe );

    /* Empty/SizeErr Int Clear */
    usb_creg_clr_sts_bemp( ptr, pipe );
}/* eof usb_cstd_pipe_init() */

/******************************************************************************
Function Name   : usb_cstd_ClrPipeCnfg
Description     : Clear specified pipe configuration register.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe_no  : pipe number
Return value    : none
******************************************************************************/
void usb_cstd_ClrPipeCnfg(USB_UTR_t *ptr, uint16_t pipe_no)
{
#ifdef USB_DTC_ENABLE
    uint16_t    buffer;
#endif    /* USB_DTC_ENABLE */

    usb_gcstd_Pipe[ptr->ip][pipe_no] = (USB_UTR_t*)USB_NULL;

    /* PID=NAK & clear STALL */
    usb_cstd_ClrStall(ptr, pipe_no);
    
    /* Interrupt disable */
    /* Ready         Int Disable */
    usb_creg_clr_brdyenb( ptr, pipe_no );

    /* NotReady      Int Disable */
    usb_creg_clr_nrdyenb( ptr, pipe_no );

    /* Empty/SizeErr Int Disable */
    usb_creg_clr_bempenb( ptr, pipe_no );

    /* PIPE Configuration */
    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
#ifdef USB_DTC_ENABLE   
    /* Clear D0FIFO-port */
    buffer = usb_creg_read_fifosel( ptr, USB_D0DMA );

    if( (buffer & USB_CURPIPE) == pipe_no )
    {
        usb_cpu_d0fifo_stop_dma(ptr);

        usb_cstd_D0fifoStopUsb(ptr);
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
    }
    /* Clear D1FIFO-port */
    buffer = usb_creg_read_fifosel( ptr, USB_D1DMA );

    if( (buffer & USB_CURPIPE) == pipe_no )
    {
        if(ptr->ip == USB_USBIP_0)
        {
            usb_creg_set_mbw( ptr, USB_D1USE, USB0_D1FIFO_MBW );
        }
        else if (ptr->ip == USB_USBIP_1)
        {
            usb_creg_set_mbw( ptr, USB_D1USE, USB1_D1FIFO_MBW );
        }

        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
    }
#endif    /* USB_DTC_ENABLE */
    usb_creg_write_pipesel( ptr, pipe_no );
    usb_creg_write_pipecfg( ptr, 0 );

    usb_creg_write_pipebuf( ptr, 0 );
    usb_creg_write_pipemaxp( ptr, 0 );
    usb_creg_write_pipeperi( ptr, 0 );
    usb_creg_write_pipesel( ptr, 0 );

    /* FIFO buffer DATA-PID initialized */
    /* SQCLR */
    usb_creg_set_sqclr(ptr, pipe_no);
    /* ACLRM */
    usb_cstd_DoAclrm(ptr, pipe_no);
    /* CSSTS */
    usb_creg_set_csclr(ptr, pipe_no);
    usb_cstd_ClrTransactionCounter(ptr, pipe_no);
    
    /* Interrupt status clear */
    /* Ready         Int Clear */
    usb_creg_clr_sts_brdy( ptr, pipe_no );

    /* NotReady      Int Clear */
    usb_creg_clr_sts_nrdy( ptr, pipe_no );

    /* Empty/SizeErr Int Clear */
    usb_creg_clr_sts_bemp( ptr, pipe_no );
}/* eof usb_cstd_ClrPipeCnfg() */

/******************************************************************************
Function Name   : usb_cstd_WaitUsbip
Description     : Wait USB ASSP ready
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
Return value    : none
******************************************************************************/
void usb_cstd_WaitUsbip(USB_UTR_t *ptr)
{
    /* XCKE Mode Flag */
    usb_gcstd_XckeMode = USB_NO;

    if(ptr->ip == USB_USBIP_0)
    {
        /* Hi-speed enable */
        usb_gcstd_HsEnable[ptr->ip] = USB_HS_DISABLE;
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        /* Hi-speed enable */
#if( USB_SPEED_MODE_PP == USB_HS_PP )
        usb_gcstd_HsEnable[ptr->ip] = USB_HS_ENABLE;
#else   /* USB_SPEED_MODE_PP == USB_HS_PP  */
        usb_gcstd_HsEnable[ptr->ip] = USB_HS_DISABLE;
#endif  /* USB_SPEED_MODE_PP == USB_HS_PP */
    }
}/* eof usb_cstd_WaitUsbip() */

/******************************************************************************
Function Name   : usb_cstd_SetNak
Description     : Set up to NAK the specified pipe.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe Number
Return value    : none
******************************************************************************/
void usb_cstd_SetNak(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        buf, n;

    /* Set NAK */
    usb_creg_clr_pid( ptr, pipe, (uint16_t)USB_PID_BUF );

    /* The state of PBUSY continues while transmitting the packet when it is a detach. */
    /* 1ms comes off when leaving because the packet duration might not exceed 1ms.  */
    /* Whether it is PBUSY release or 1ms passage can be judged. */
    for( n = 0; n < 0xFFFFu; ++n )
    {
        /* PIPE control reg read */
        buf = usb_creg_read_pipectr( ptr, pipe );
        if( (uint16_t)(buf & USB_PBUSY) == 0 )
        {
            n = 0xFFFEu;
        }
    }
}/* eof usb_cstd_SetNak() */

/******************************************************************************
Function Name   : usb_cstd_write_fifo
Description     : Write specified amount of data to specified USB FIFO. 
Arguments       : USB_UTR_t *ptr        : USB system internal structure. Selects channel.
                : uint16_t  count       : Write size.
                : uint16_t  pipemode    : The mode of CPU/DMA(D0)/DMA(D1).
                : uint16_t  *write_p    : Address of buffer of data to write.
Return value    : The incremented address of last argument (write_p).
******************************************************************************/
uint8_t *usb_cstd_write_fifo( USB_UTR_t *ptr, uint16_t count, uint16_t pipemode, uint8_t *write_p )
{
    uint16_t    even;
    uint16_t    odd;

    if(ptr->ip == USB_USBIP_0)
    {
        for( even = (uint16_t)(count >> 1); (even != 0); --even )
        {
            /* 16bit access */
            usb_creg_write_fifo16( ptr, pipemode, *((uint16_t *)write_p) );

            /* Renewal write pointer */
            write_p += sizeof(uint16_t);
        }

        if( (count & (uint16_t)0x0001u) != 0u )
        {
            /* 8bit access */
            /* count == odd */
            /* Change FIFO access width */
            usb_creg_set_mbw( ptr, pipemode, USB_MBW_8 );

            /* FIFO write */
            usb_creg_write_fifo8( ptr, pipemode, *write_p );

            /* Return FIFO access width */
            usb_creg_set_mbw( ptr, pipemode, USB_MBW_16 );

            /* Renewal write pointer */
            write_p++;
        }
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        for( even = (uint16_t)(count >> 2); (even != 0); --even )
        {
            /* 16bit access */
            usb_creg_write_fifo32( ptr, pipemode, *((uint32_t *)write_p) );

            /* Renewal write pointer */
            write_p += sizeof(uint32_t);
        }
        odd = count % 4;
        if( (odd & (uint16_t)0x0002u) != 0u )
        {
            /* 16bit access */
            /* Change FIFO access width */
            usb_creg_set_mbw( ptr, pipemode, USB_MBW_16 );
            /* FIFO write */
            usb_creg_write_fifo16( ptr, pipemode, *((uint16_t *)write_p) );

            /* Renewal write pointer */
            write_p += sizeof(uint16_t);
        }
        if( (odd & (uint16_t)0x0001u) != 0u )
        {
            /* 8bit access */
            /* count == odd */
            /* Change FIFO access width */
            usb_creg_set_mbw( ptr, pipemode, USB_MBW_8 );

            /* FIFO write */
            usb_creg_write_fifo8( ptr, pipemode, *write_p );

            /* Renewal write pointer */
            write_p++;
        }
        /* Return FIFO access width */
        usb_creg_set_mbw( ptr, pipemode, USB_MBW_32 );
    }
    return write_p;

}/* eof usb_cstd_write_fifo() */

/******************************************************************************
Function Name   : usb_cstd_read_fifo
Description     : Read specified buffer size from the USB FIFO.
Arguments       : USB_UTR_t *ptr        : USB system internal structure. Selects channel.
                : uint16_t  count       : Read size.
                : uint16_t  pipemode    : The mode of CPU/DMA(D0)/DMA(D1).
                : uint16_t  *write_p    : Address of buffer to store the read data.
Return value    : Pointer to a buffer that contains the data to be read next.
******************************************************************************/
uint8_t *usb_cstd_read_fifo( USB_UTR_t *ptr, uint16_t count, uint16_t pipemode, uint8_t *read_p )
{
    uint16_t    even;
    uint16_t    odd;
    uint32_t    odd_byte_data_temp;
#if USB_CPUBYTE_PP != USB_BYTE_LITTLE_PP
    uint16_t    i;
#endif
    if(ptr->ip == USB_USBIP_0)
    {
        for( even = (uint16_t)(count >> 1); (even != 0); --even )
        {
            /* 16bit FIFO access */
            *(uint16_t *)read_p= usb_creg_read_fifo16( ptr, pipemode );

            /* Renewal read pointer */
            read_p += sizeof( uint16_t );
        }
        if( (count & (uint16_t)0x0001) != 0 )
        {
             /* 16bit FIFO access */
            odd_byte_data_temp = usb_creg_read_fifo16( ptr, pipemode );
/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
            *read_p = (uint8_t)( odd_byte_data_temp & 0x00ff);
#else    /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
            *read_p = (uint8_t)( odd_byte_data_temp >> 8 );
#endif    /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
            /* Renewal read pointer */
            read_p += sizeof( uint8_t );
        }
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        for( even = (uint16_t)(count >> 2); (even != 0); --even )
        {
            /* 32bit FIFO access */
            *(uint32_t *)read_p= usb_creg_read_fifo32( ptr, pipemode );

            /* Renewal read pointer */
            read_p += sizeof( uint32_t );
        }
        odd = count % 4;
        if(count < 4)
        {
            odd = count;
        }
        if( odd != 0 )
        {
             /* 32bit FIFO access */
            odd_byte_data_temp = usb_creg_read_fifo32( ptr, pipemode );
/* Condition compilation by the difference of the endian */
#if USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP
            do{
                *read_p = (uint8_t)( odd_byte_data_temp & 0x000000ff );
                odd_byte_data_temp = odd_byte_data_temp >> 8;
                /* Renewal read pointer */
                read_p += sizeof( uint8_t );
                odd--;
            }while( odd != 0 );
#else    /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
            for(i = 0; i < odd; i++)
            {
                *read_p = (uint8_t)( ( odd_byte_data_temp >> (24 -(i*8))) & 0x000000ff );
                /* Renewal read pointer */
                read_p += sizeof( uint8_t );
            }
#endif    /* USB_CPUBYTE_PP == USB_BYTE_LITTLE_PP */
        }
    }
    return read_p;
}/* eof usb_cstd_read_fifo() */

/******************************************************************************
Function Name   : usb_cstd_is_set_frdy
Description     : Changes the specified FIFO port by the specified pipe.
                : Please change the wait time for your MCU.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe Number
                : uint16_t fifosel  : FIFO select
                : uint16_t isel     : ISEL bit status
Return value    : FRDY status
******************************************************************************/
uint16_t usb_cstd_is_set_frdy(USB_UTR_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel)
{
    uint16_t    buffer, i;

    /* Changes the FIFO port by the pipe. */
    usb_cstd_chg_curpipe(ptr, pipe, fifosel, isel);

    for( i = 0; i < 4; i++ )
    {
        buffer = usb_creg_read_fifoctr( ptr, fifosel );

        if( (uint16_t)(buffer & USB_FRDY) == USB_FRDY )
        {
            return (buffer);
        }
        USB_PRINTF1("*** FRDY wait pipe = %d\n", pipe);
        
        /* Caution!!!
     * Depending on the external bus speed of CPU, you may need to wait
     * for 100ns here.
     * For details, please look at the data sheet.   */
    /***** The example of reference. *****/
        buffer = usb_creg_read_syscfg( ptr, USB_PORT0 );
        buffer = usb_creg_read_syssts( ptr, USB_PORT0 );
    /*************************************/
    }
    return (USB_FIFOERROR);
}/* eof of function usb_cstd_is_set_frdy() */

/******************************************************************************
Function Name   : usb_cstd_chg_curpipe
Description     : Switch FIFO and pipe number.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe number.
                : uint16_t fifosel  : FIFO selected (CPU, D0, D1..)
                : uint16_t isel     : CFIFO Port Access Direction.(Pipe1 to 9:Set to 0)
Return value    : none
******************************************************************************/
void usb_cstd_chg_curpipe(USB_UTR_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel)
{
    uint16_t    buffer;

    /* Select FIFO */
    switch( fifosel )
    {
    /* CFIFO use */
    case USB_CUSE:
        /* ISEL=1, CURPIPE=0 */
        usb_creg_rmw_fifosel( ptr, USB_CUSE, (USB_RCNT|isel|pipe), (USB_RCNT|USB_ISEL|USB_CURPIPE) );
        do
        {
            buffer = usb_creg_read_fifosel( ptr, USB_CUSE );
        }
        while( (buffer & (uint16_t)(USB_ISEL|USB_CURPIPE))
            != (uint16_t)(isel|pipe) );
    break;
    /* D0FIFO use */
    case USB_D0USE:
    /* continue */
#ifdef USB_DTC_ENABLE
    /* D0FIFO DMA */
    case USB_D0DMA:
        /* D0FIFO pipe select */
        usb_creg_set_curpipe( ptr, USB_D0DMA, pipe );
        do
        {
            buffer = usb_creg_read_fifosel( ptr, USB_D0DMA );
        }
        while( (uint16_t)(buffer & USB_CURPIPE) != pipe );
#endif    /* USB_DTC_ENABLE */
        break;
    /* D1FIFO use */
    case USB_D1USE:
        /* continue */
#ifdef USB_DTC_ENABLE
    /* D1FIFO DMA */
    case USB_D1DMA:
        /* D1FIFO pipe select */
        usb_creg_set_curpipe( ptr, USB_D1DMA, pipe );

        do
        {
            buffer = usb_creg_read_fifosel( ptr, USB_D1DMA );
        }
        while( (uint16_t)(buffer & USB_CURPIPE) != pipe );
#endif    /* USB_DTC_ENABLE */
        break;
    default:
        break;
    }
}/* eof usb_cstd_chg_curpipe() */

/******************************************************************************
Function Name   : usb_cstd_FifoClr
Description     : Clear the specified PIPE FIFO using Auto Buffer Clear Mode.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe Number
Return value    : none
******************************************************************************/
void usb_cstd_FifoClr(USB_UTR_t *ptr, uint16_t pipe)
{
    uint16_t        buf, i;

    if( pipe == USB_USEPIPE )
    {
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
        for( i = USB_MIN_PIPE_NO; i <= USB_MAX_PIPE_NO; i++ )
        {
            /* Do pipe ACLRM */
            usb_cstd_DoAclrm(ptr, i);
        }
    }
    else
    {
        buf = usb_creg_read_fifosel( ptr, USB_D0USE );
        if( (buf & USB_CURPIPE) == pipe )
        {
            /* Changes the FIFO port by the pipe. */
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
        }
        buf = usb_creg_read_fifosel( ptr, USB_D1USE );
        if( (buf & USB_CURPIPE) == pipe )
        {
            /* Changes the FIFO port by the pipe. */
            usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
        }
        /* Do pipe ACLRM */
        usb_cstd_DoAclrm(ptr, pipe);
    }
}/* eof usb_cstd_FifoClr() */

/******************************************************************************
Function Name   : usb_cstd_SetTransactionCounter
Description     : Set specified Pipe Transaction Counter Register.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t trnreg   : Pipe number
                : uint16_t trncnt       : Transaction counter
Return value    : none
******************************************************************************/
void usb_cstd_SetTransactionCounter(USB_UTR_t *ptr, uint16_t trnreg, uint16_t trncnt)
{

    usb_creg_set_trclr( ptr, trnreg );
    usb_creg_write_pipetrn( ptr, trnreg, trncnt );
    usb_creg_set_trenb( ptr, trnreg );

}/* eof usb_cstd_SetTransactionCounter() */

/******************************************************************************
Function Name   : usb_cstd_ClrTransactionCounter
Description     : Clear specified Pipe Transaction Counter Register.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t trnreg   : Pipe Number
Return value    : none
******************************************************************************/
void usb_cstd_ClrTransactionCounter(USB_UTR_t *ptr, uint16_t trnreg)
{
    usb_creg_clr_trenb( ptr, trnreg );
    usb_creg_set_trclr( ptr, trnreg );
}/* eof usb_cstd_ClrTransactionCounter() */

/******************************************************************************
Function Name   : usb_cstd_ForcedTermination
Description     : Terminate data transmission and reception.
Arguments       : USB_UTR_t *ptr    : USB system internal structure. Selects channel.
                : uint16_t pipe     : Pipe Number
                : uint16_t status   : Transfer status type
Return value    : none
Note            : In the case of timeout status, it does not call back.
******************************************************************************/
void usb_cstd_ForcedTermination(USB_UTR_t *ptr, uint16_t pipe, uint16_t status)
{
    uint16_t    buffer;

    /* PID = NAK */
    /* Set NAK */
    usb_cstd_SetNak(ptr, pipe);

    /* Disable Interrupt */
    /* Disable Ready Interrupt */
    usb_creg_clr_brdyenb(ptr, pipe);
    /* Disable Not Ready Interrupt */
    usb_creg_clr_nrdyenb(ptr, pipe);
    /* Disable Empty Interrupt */
    usb_creg_clr_bempenb(ptr, pipe);

    usb_cstd_ClrTransactionCounter(ptr, pipe);

    /* Clear D1FIFO-port */
    buffer = usb_creg_read_fifosel( ptr, USB_CUSE );
    if( (buffer & USB_CURPIPE) == pipe )
    {
        if(ptr->ip == USB_USBIP_0)
        {
            usb_creg_set_mbw( ptr, USB_CUSE, USB0_CFIFO_MBW );
        }
        else if (ptr->ip == USB_USBIP_1)
        {
            usb_creg_set_mbw( ptr, USB_CUSE, USB1_CFIFO_MBW );
        }

        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
    }
#ifdef USB_DTC_ENABLE
    /* Clear D0FIFO-port */
    buffer = usb_creg_read_fifosel( ptr, USB_D0DMA );
    if( (buffer & USB_CURPIPE) == pipe )
    {
        /* Stop DMA,FIFO access */
        usb_cpu_d0fifo_stop_dma(ptr);

        usb_cstd_D0fifoStopUsb(ptr);
        usb_cstd_D0FifoselSet(ptr);
        if(ptr->ip == USB_USBIP_0)
        {
            usb_creg_write_dmacfg( ptr, USB_D0DMA, USB_CPU_ADR_RD_WR );
        }
        else if (ptr->ip == USB_USBIP_1)
        {
//            usb_creg_set_mbw( ptr, USB_D1USE, USB1_D1FIFO_MBW );
        }
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D0USE, USB_NO);
    }
    /* Clear D1FIFO-port */
    buffer = usb_creg_read_fifosel( ptr, USB_D1DMA );
    if( (buffer & USB_CURPIPE) == pipe )
    {
        if(ptr->ip == USB_USBIP_0)
        {
            usb_creg_set_mbw( ptr, USB_D1USE, USB0_D1FIFO_MBW );
        }
        else if (ptr->ip == USB_USBIP_1)
        {
            usb_creg_set_mbw( ptr, USB_D1USE, USB1_D1FIFO_MBW );
        }
        /* Changes the FIFO port by the pipe. */
        usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_D1USE, USB_NO);
    }
#endif    /* USB_DTC_ENABLE */

    /* Changes the FIFO port by the pipe. */
    usb_cstd_chg_curpipe(ptr, pipe, (uint16_t)USB_CUSE, USB_NO);
    buffer = usb_creg_read_fifoctr( ptr, USB_CUSE );
    if( (uint16_t)(buffer & USB_FRDY) == USB_FRDY )
    {
        /* Clear BVAL */
        usb_creg_set_bclr( ptr, USB_CUSE );
    }

    /* FIFO buffer SPLIT transaction initialized */
    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
    usb_creg_set_csclr(ptr, pipe);

    /* Call Back */
    if( usb_gcstd_Pipe[ptr->ip][pipe] != USB_NULL )
    {
        /* Transfer information set */
        usb_gcstd_Pipe[ptr->ip][pipe]->tranlen  = usb_gcstd_DataCnt[ptr->ip][pipe];
        usb_gcstd_Pipe[ptr->ip][pipe]->status   = status;
        usb_gcstd_Pipe[ptr->ip][pipe]->pipectr  = usb_creg_read_pipectr(ptr, pipe);
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = (uint8_t)usb_ghstd_IgnoreCnt[ptr->ip][pipe];
#else   /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
        usb_gcstd_Pipe[ptr->ip][pipe]->errcnt   = 0;
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */

        usb_gcstd_Pipe[ptr->ip][pipe]->ipp  = ptr->ipp;
        usb_gcstd_Pipe[ptr->ip][pipe]->ip   = ptr->ip;

        if( USB_NULL != ( usb_gcstd_Pipe[ptr->ip][pipe]->complete ) )
        {
            (usb_gcstd_Pipe[ptr->ip][pipe]->complete)(usb_gcstd_Pipe[ptr->ip][pipe], 0, 0);
        }
        usb_gcstd_Pipe[ptr->ip][pipe] = (USB_UTR_t*)USB_NULL;
    }
}/* eof usb_cstd_ForcedTermination() */

/******************************************************************************
Function Name   : usb_cstd_nrdy_endprocess
Description     : NRDY interrupt processing. (Forced termination of data trans-
                : mission and reception of specified pipe.)
Arguments       : USB_UTR_t *ptr       : USB system internal structure. Selects channel.
                : uint16_t  pipe        : Pipe No
Return value    : none
Note            : none
******************************************************************************/
void    usb_cstd_nrdy_endprocess( USB_UTR_t *ptr, uint16_t pipe )
{
    if( usb_cstd_is_host_mode(ptr) == USB_YES )
    {
#if USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP
        uint16_t    buffer;

        /*
            Host Function
        */
        buffer = usb_cstd_GetPid(ptr, pipe);
        /* STALL ? */
        if( (buffer & USB_PID_STALL) == USB_PID_STALL )
        {
            USB_PRINTF1("### STALL Pipe %d\n", pipe);
            /* @4 */
            /* End of data transfer */
            usb_cstd_ForcedTermination(ptr, pipe, USB_DATA_STALL);
        }
        else
        {
            /* Wait for About 60ns */
            buffer = usb_creg_read_syssts( ptr, USB_PORT0 );
            /* @3 */
            usb_ghstd_IgnoreCnt[ptr->ip][pipe]++;
            USB_PRINTF2("### IGNORE Pipe %d is %d times \n", pipe, usb_ghstd_IgnoreCnt[ptr->ip][pipe]);
            if( usb_ghstd_IgnoreCnt[ptr->ip][pipe] == USB_PIPEERROR )
            {
                /* Data Device Ignore X 3 call back */
                /* End of data transfer */
                usb_cstd_ForcedTermination(ptr, pipe, USB_DATA_ERR);
            }
            else
            {
                /* 5ms wait */
                usb_cpu_DelayXms(5);
                /* PIPEx Data Retry */
                usb_cstd_SetBuf(ptr, pipe);
            }
        }
#endif  /* USB_FUNCSEL_USBIP0_PP == USB_HOST_PP || USB_FUNCSEL_USBIP1_PP == USB_HOST_PP */
    }
}/* eof usb_cstd_nrdy_endprocess() */

/******************************************************************************
End of file
******************************************************************************/

