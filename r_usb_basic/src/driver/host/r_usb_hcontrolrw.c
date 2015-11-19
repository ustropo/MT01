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
* File Name    : r_usb_hcontrolrw.c
* Description  : USB Host Control read/write
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


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Renesas USB FIFO Read/Write Host Driver API functions
******************************************************************************/

#ifdef USB_HOST_COMPLIANCE_MODE
    uint16_t    usb_ghstd_responce_counter;
#endif /* USB_HOST_COMPLIANCE_MODE */


/******************************************************************************
Function Name   : usb_hstd_ControlWriteStart
Description     : Start data stage of Control Write transfer.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint32_t  Bsize   : Data Size
                : uint8_t   *Table  : Data Table Address
Return          : uint16_t          : USB_WRITEEND / USB_WRITING
                :                   : USB_WRITESHRT / USB_FIFOERROR
******************************************************************************/
uint16_t usb_hstd_ControlWriteStart(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table)
{
    uint16_t    end_flag, toggle;

    /* PID=NAK & clear STALL */
    usb_cstd_ClrStall(ptr, (uint16_t)USB_PIPE0);
    /* Transfer size set */
    usb_gcstd_DataCnt[ptr->ip][USB_PIPE0] = Bsize;
    /* Transfer address set */
    usb_gcstd_DataPtr[ptr->ip][USB_PIPE0] = Table;

    /* DCP Configuration Register  (0x5C) */
    usb_creg_write_dcpcfg( ptr, (USB_CNTMDFIELD | USB_DIRFIELD) );
    /* SQSET=1, PID=NAK */
    usb_creg_set_sqset( ptr, USB_PIPE0 );
    if( usb_ghstd_Ctsq[ptr->ip] == USB_DATAWRCNT)
    {
        /* Next stage is Control read data stage */
        toggle = usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->pipectr;
        /* Do pipe SQTGL */
        usb_cstd_DoSqtgl(ptr, (uint16_t)USB_PIPE0, toggle);
    }

    usb_creg_clr_sts_bemp( ptr, USB_PIPE0 );

    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0] = (uint16_t)0;

    /* Host Control sequence */
    end_flag = usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE );

    switch( end_flag )
    {
        /* End of data write */
        case USB_WRITESHRT:
            /* Next stage is Control write status stage */
            usb_ghstd_Ctsq[ptr->ip] = USB_STATUSWR;
            /* Enable Empty Interrupt */
            usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
            /* Enable Not Ready Interrupt */
            usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
            /* Set BUF */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);    
            break;
        /* End of data write (not null) */
        case USB_WRITEEND:
            /* continue */
        /* Continue of data write */
        case USB_WRITING:
            if( usb_ghstd_Ctsq[ptr->ip] == USB_SETUPWR )
            {
                /* Next stage is Control read data stage */
                /* Next stage is Control write data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_DATAWR;
            }
            else
            {
                /* Next stage is Control read data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_DATAWRCNT;
            }
            /* Enable Empty Interrupt */
            usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
            /* Enable Not Ready Interrupt */
            usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
            /* Set BUF */
            usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
            break;
        /* FIFO access error */
        case USB_FIFOERROR:
            break;
        default:
            break;
    }
    /* End or Err or Continue */
    return (end_flag);
}
/******************************************************************************
End of function usb_hstd_ControlWriteStart
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ControlReadStart
Description     : Start data stage of Control Read transfer.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
                : uint32_t Bsize    : Data Size
                : uint8_t  *Table   : Data Table Address
Return          : none
******************************************************************************/
void usb_hstd_ControlReadStart(USB_UTR_t *ptr, uint32_t Bsize, uint8_t *Table)
{
    uint16_t    toggle;

#ifdef USB_HOST_COMPLIANCE_MODE
    usb_ghstd_responce_counter = 0;
    
    usb_creg_clr_sts_sofr( ptr );
    usb_creg_set_intenb( ptr, USB_SOFE );
#endif /* USB_HOST_COMPLIANCE_MODE */

    /* PID=NAK & clear STALL */
    usb_cstd_ClrStall(ptr, (uint16_t)USB_PIPE0);
    /* Transfer size set */
    usb_gcstd_DataCnt[ptr->ip][USB_PIPE0] = Bsize;
    /* Transfer address set */
    usb_gcstd_DataPtr[ptr->ip][USB_PIPE0] = Table;
    /* DCP Configuration Register  (0x5C) */
    usb_creg_write_dcpcfg( ptr,USB_SHTNAKFIELD);
    /* SQSET=1, PID=NAK */
    usb_hreg_write_dcpctr( ptr,USB_SQSET);
    if( usb_ghstd_Ctsq[ptr->ip] == USB_DATARDCNT )
    {
        /* Next stage is Control read data stage */
        toggle = usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->pipectr;
        usb_cstd_DoSqtgl(ptr, (uint16_t)USB_PIPE0, toggle);
    }

    /* Host Control sequence */
    if( usb_ghstd_Ctsq[ptr->ip] == USB_SETUPRD )
    {
        /* Next stage is Control read data stage */
        /* Next stage is Control read data stage */
        usb_ghstd_Ctsq[ptr->ip] = USB_DATARD;
    }
    else
    {
        /* Next stage is Control read data stage */
        usb_ghstd_Ctsq[ptr->ip] = USB_DATARDCNT;
    }

    /* Ignore count clear */
    usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0] = (uint16_t)0;
    /* Interrupt enable */
    /* Enable Ready Interrupt */
    usb_creg_set_brdyenb(ptr, (uint16_t)USB_PIPE0);
    /* Enable Not Ready Interrupt */
    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
    /* Set BUF */
    usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
}
/******************************************************************************
End of function usb_hstd_ControlReadStart
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_StatusStart
Description     : Start status stage of Control Command.
Argument        : none
Return          : none
******************************************************************************/
void usb_hstd_StatusStart(USB_UTR_t *ptr)
{
    uint16_t    end_flag;
    uint8_t     buf1[16];

    /* Interrupt Disable */
    /* BEMP0 Disable */
    usb_creg_clr_bempenb(ptr, (uint16_t)USB_PIPE0);
    /* BRDY0 Disable */
    usb_creg_clr_brdyenb(ptr, (uint16_t)USB_PIPE0);
    /* Transfer size set */
    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->tranlen = usb_gcstd_DataCnt[ptr->ip][USB_PIPE0];

    /* Branch  by the Control transfer stage management */
    switch( usb_ghstd_Ctsq[ptr->ip] )
    {
        /* Control Read Data */
        case USB_DATARD:
            /* continue */
        /* Control Read Data */
        case USB_DATARDCNT:
            /* Control read Status */
            usb_ghstd_Ctsq[ptr->ip] = USB_DATARD;
            /* Control write start */
            end_flag = usb_hstd_ControlWriteStart(ptr, (uint32_t)0, (uint8_t*)&buf1);
            if( end_flag == USB_FIFOERROR )
            {
                USB_PRINTF0("### FIFO access error \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            }
            else
            {
                /* Host Control sequence */
                /* Next stage is Control read status stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_STATUSRD;
            }
            break;
        /* Control Write Data */
        case USB_STATUSWR:
            /* continue */
        /* NoData Control */
        case USB_SETUPNDC:
            /* Control Read Status */
            usb_hstd_ControlReadStart(ptr, (uint32_t)0, (uint8_t*)&buf1);
            /* Host Control sequence */
            /* Next stage is Control write status stage */
            usb_ghstd_Ctsq[ptr->ip] = USB_STATUSWR;
            break;
        default:
            return;
            break;
    }
}
/******************************************************************************
End of function usb_hstd_StatusStart
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ControlEnd
Description     : Call the user registered callback function that notifies 
                : completion of a control transfer.
                : Command.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t  status           : Transfer status
Return          : none
******************************************************************************/
void usb_hstd_ControlEnd(USB_UTR_t *ptr, uint16_t Status)
{
    /* Interrupt Disable */
    /* BEMP0 Disable */
    usb_creg_clr_bempenb(ptr, (uint16_t)USB_PIPE0);
    /* BRDY0 Disable */
    usb_creg_clr_brdyenb(ptr, (uint16_t)USB_PIPE0);
    /* NRDY0 Disable */
    usb_creg_clr_nrdyenb(ptr, (uint16_t)USB_PIPE0);
    /* PID=NAK & clear STALL */
    usb_cstd_ClrStall(ptr, (uint16_t)USB_PIPE0);
    if(ptr -> ip == USB_USBIP_0)
    {
        usb_creg_set_mbw( ptr, USB_CUSE, USB0_CFIFO_MBW );
    }
    else if (ptr -> ip == USB_USBIP_1)
    {
        usb_creg_set_mbw( ptr, USB_CUSE, USB1_CFIFO_MBW );
    }
    /* CSCLR=1, SUREQ=1, SQCLR=1, PID=NAK */
    usb_hreg_write_dcpctr( ptr, (uint16_t)(USB_CSCLR|USB_SUREQCLR|USB_SQCLR) );
    /* CFIFO buffer clear */
    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, USB_NO);
    /* Clear BVAL */
    usb_creg_set_bclr( ptr, USB_CUSE );

    usb_cstd_chg_curpipe(ptr, (uint16_t)USB_PIPE0, (uint16_t)USB_CUSE, (uint16_t)USB_ISEL);
    /* Clear BVAL */
    usb_creg_set_bclr( ptr, USB_CUSE );

    /* Host Control sequence */
    if( (Status != USB_CTRL_READING) && (Status != USB_CTRL_WRITING) )
    {
        /* Next stage is idle */
        usb_ghstd_Ctsq[ptr->ip] = USB_IDLEST;
    }

    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->status    = Status;
    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->pipectr = usb_creg_read_pipectr(ptr, (uint16_t)USB_PIPE0);
    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->errcnt = (uint8_t)usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0];

    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->ipp = ptr->ipp;
    usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->ip  = ptr->ip;

    /* Callback */
    if( USB_NULL != usb_gcstd_Pipe[ptr->ip][USB_PIPE0] )
    {
        if( USB_NULL != (usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->complete) )
        {
            /* Process Done Callback */
            (usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->complete)(usb_gcstd_Pipe[ptr->ip][USB_PIPE0], 0, 0);
        }
    }
    usb_gcstd_Pipe[ptr->ip][USB_PIPE0] = (USB_UTR_t*)USB_NULL;

#ifdef USB_HOST_COMPLIANCE_MODE
    usb_creg_clr_enb_sofe( ptr );
#endif /* USB_HOST_COMPLIANCE_MODE */

}
/******************************************************************************
End of function usb_hstd_ControlEnd
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SetupStart
Description     : Start control transfer setup stage. (Set global function re-
                : quired to start control transfer, and USB register).
Arguments       : USB_UTR_t *ptr : USB system internal structure.
Return          : none
******************************************************************************/
void usb_hstd_SetupStart(USB_UTR_t *ptr)
{
    uint16_t        segment;
    uint16_t        dir, setup_req, setup_val, setup_indx, setup_leng;
    uint16_t        *p_setup;

    segment        = (uint16_t)(usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->segment);
    p_setup        = usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->setup;
    /* Set Request data */
    setup_req   = *p_setup++;
    /* Set wValue data */
    setup_val   = *p_setup++;
    /* Set wIndex data */
    setup_indx  = *p_setup++;
    /* Set wLength data */
    setup_leng  = *p_setup++;

    /* Max Packet Size + Device Number select */
    usb_creg_write_dcpmxps( ptr, usb_ghstd_DcpRegister[ptr->ip][*p_setup] );

    /* Transfer Length check */

    /* Check Last flag */
    if( segment == (uint16_t)USB_TRAN_END )
    {
        if( usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->tranlen < setup_leng )
        {
            setup_leng = (uint16_t)usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->tranlen;
        }
    }
    if( setup_leng < usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->tranlen )
    {
        usb_gcstd_Pipe[ptr->ip][USB_PIPE0]->tranlen = (uint32_t)setup_leng;
    }

    /* Control sequence setting */
    dir    = (uint16_t)(setup_req & USB_BMREQUESTTYPEDIR);
    /* Check wLength field */
    if( setup_leng == 0 )
    {
        /* Check Dir field */
        if( dir == 0 )
        {
            /* Check Last flag */
            if( segment == (uint16_t)USB_TRAN_END )
            {
                /* Next stage is NoData control status stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_SETUPNDC;
            }
            else
            {
                /* Error */
                usb_ghstd_Ctsq[ptr->ip] = USB_IDLEST;
            }
        }
        else
        {
            /* Error */
            usb_ghstd_Ctsq[ptr->ip] = USB_IDLEST;
        }
    }
    else
    {
        /* Check Dir field */
        if( dir == 0 )
        {
            /* Check Last flag */
            if( segment == (uint16_t)USB_TRAN_END )
            {
                /* Next stage is Control Write data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_SETUPWR;
            }
            else
            {
                /* Next stage is Control Write data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_SETUPWRCNT;
            }
        }
        else
        {
            /* Check Last flag */
            if( segment == (uint16_t)USB_TRAN_END )
            {
                /* Next stage is Control read data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_SETUPRD;
            }
            else
            {
                /* Next stage is Control read data stage */
                usb_ghstd_Ctsq[ptr->ip] = USB_SETUPRDCNT;
            }
        }
    }

    /* Control transfer idle stage? */
    if( usb_ghstd_Ctsq[ptr->ip] == USB_IDLEST )
    {
        /* Control Read/Write End */
        usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_STOP);
    }
    else
    {
        /* SETUP request set */
        /* Set Request data */
        usb_hreg_write_usbreq( ptr, setup_req );

        usb_hreg_set_usbval( ptr, setup_val );
        usb_hreg_set_usbindx( ptr, setup_indx );
        usb_hreg_set_usbleng( ptr, setup_leng );

        /* Ignore count clear */
        usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0] = (uint16_t)0;

        usb_hreg_clr_sts_sign( ptr );
        usb_hreg_clr_sts_sack( ptr );
        usb_hreg_set_enb_signe( ptr );
        usb_hreg_set_enb_sacke( ptr );
        usb_hreg_set_sureq( ptr );
    }
}
/******************************************************************************
End of function usb_hstd_SetupStart
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
