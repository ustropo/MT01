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
* File Name    : r_usb_preg_abs.c
* Description  : Call USB Peripheral register access function
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
External variables and functions
******************************************************************************/
extern uint16_t usb_gpstd_intsts0;
extern void     R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl);

/******************************************************************************
Function Name   : usb_pstd_InterruptHandler
Description     : Determine which USB interrupt occurred and report results to 
                : the USB_UTR_t argument's ipp, keyword, and status members.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_InterruptHandler(USB_UTR_t *ptr)
{
    uint16_t    intsts0, intenb0, ists0;
    uint16_t    brdysts, brdyenb, bsts;
    uint16_t    nrdysts, nrdyenb, nsts;
    uint16_t    bempsts, bempenb, ests;

    /* Register Save */
    intsts0 = ptr->ipp->INTSTS0.WORD;
    brdysts = ptr->ipp->BRDYSTS.WORD;
    nrdysts = ptr->ipp->NRDYSTS.WORD;
    bempsts = ptr->ipp->BEMPSTS.WORD;
    intenb0 = ptr->ipp->INTENB0.WORD;
    brdyenb = ptr->ipp->BRDYENB.WORD;
    nrdyenb = ptr->ipp->NRDYENB.WORD;
    bempenb = ptr->ipp->BEMPENB.WORD;
    
    ptr->keyword = USB_INT_UNKNOWN;
    ptr->status  = 0;

    /* Interrupt status get */
    ists0 = (uint16_t)(intsts0 & intenb0);
    bsts  = (uint16_t)(brdysts & brdyenb);
    nsts  = (uint16_t)(nrdysts & nrdyenb);
    ests  = (uint16_t)(bempsts & bempenb);

    if( (intsts0 & (USB_VBINT|USB_RESM|USB_SOFR|USB_DVST|USB_CTRT|USB_BEMP|USB_NRDY|USB_BRDY)) == 0u )
    {
        return;
    }

    /***** Processing USB bus signal *****/
    /***** Resume signal *****/
    if( (ists0 & USB_RESM) == USB_RESM )
    {
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_RESM;
        ptr->keyword = USB_INT_RESM;
    }
    /***** Vbus change *****/
    else if( (ists0 & USB_VBINT) == USB_VBINT )
    {
        /* Status clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_VBINT;
        ptr->keyword = USB_INT_VBINT;
    }
    /***** SOFR change *****/
    else if( (ists0 & USB_SOFR) == USB_SOFR )
    {
        /* SOFR Clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_SOFR;
        ptr->keyword = USB_INT_SOFR;
    }

    /***** Processing device state *****/
    /***** DVST change *****/
    else if( (ists0 & USB_DVST) == USB_DVST )
    {
        /* DVST clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_DVST;
        ptr->keyword = USB_INT_DVST;
        ptr->status  = intsts0;
    }

    /***** Processing PIPE0 data *****/
    else if( ((ists0 & USB_BRDY) == USB_BRDY) && ((bsts & USB_BRDY0) == USB_BRDY0) )
    {
        ptr->ipp->BRDYSTS.WORD = (uint16_t)~USB_BRDY0;
        ptr->keyword = USB_INT_BRDY;
        ptr->status  = USB_BRDY0;
    }
    else if( ((ists0 & USB_BEMP) == USB_BEMP) && ((ests & USB_BEMP0) == USB_BEMP0) )
    {
        ptr->ipp->BEMPSTS.WORD = (uint16_t)~USB_BEMP0;
        ptr->keyword = USB_INT_BEMP;
        ptr->status  = USB_BEMP0;
    }
    else if( ((ists0 & USB_NRDY) == USB_NRDY) && ((nsts & USB_NRDY0) == USB_NRDY0) )
    {
        ptr->ipp->NRDYSTS.WORD = (uint16_t)~USB_NRDY0;
        ptr->keyword = USB_INT_NRDY;
        ptr->status  = USB_NRDY0;
    }

    /***** Processing setup transaction *****/
    else if( (ists0 & USB_CTRT) == USB_CTRT )
    {
        /* CTSQ bit changes later than CTRT bit for ASSP. */
        /* CTSQ reloading */
        ptr->status = usb_creg_read_intsts( ptr );
        /* USB_CTRT clear */
        ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_CTRT;
        ptr->keyword = USB_INT_CTRT;

        if( (uint8_t)(ptr->status & USB_CTSQ) == USB_CS_SQER )
        {
            usb_preg_clr_sts_valid( ptr );
            ptr->keyword = USB_INT_UNKNOWN;
            ptr->status  = 0;
            return;
        }
    }

    /***** Processing PIPE1-MAX_PIPE_NO data *****/
    /***** EP0-7 BRDY *****/
    else if( (ists0 & USB_BRDY) == USB_BRDY )
    {
        ptr->ipp->BRDYSTS.WORD = (uint16_t)~bsts;
        ptr->keyword = USB_INT_BRDY;
        ptr->status  = bsts;
    }
    /***** EP0-7 BEMP *****/
    else if( (ists0 & USB_BEMP) == USB_BEMP )
    {
        ptr->ipp->BEMPSTS.WORD = (uint16_t)~ests;
        ptr->keyword = USB_INT_BEMP;
        ptr->status  = ests;
    }
    /***** EP0-7 NRDY *****/
    else if( (ists0 & USB_NRDY) == USB_NRDY )
    {
        ptr->ipp->NRDYSTS.WORD = (uint16_t)~nsts;
        ptr->keyword = USB_INT_NRDY;
        ptr->status  = nsts;
    }
    else
    {
    }
} /* eof usb_pstd_InterruptHandler() */

/******************************************************************************
Function Name   : usb_pstd_SaveRequest
Description     : Save received USB command.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_SaveRequest(USB_UTR_t *ptr)
{
    uint16_t    buf;

    /* Valid clear */
    usb_preg_clr_sts_valid( ptr );
    buf = usb_creg_read_usbreq( ptr );

    usb_gpstd_ReqType      = (uint16_t)(buf & USB_BMREQUESTTYPE);
    usb_gpstd_ReqTypeType  = (uint16_t)(buf & USB_BMREQUESTTYPETYPE);
    usb_gpstd_ReqTypeRecip = (uint16_t)(buf & USB_BMREQUESTTYPERECIP);
    usb_gpstd_ReqRequest   = (uint16_t)(buf & USB_BREQUEST);

    usb_gpstd_ReqValue  = usb_creg_read_usbval( ptr );
    usb_gpstd_ReqIndex  = usb_creg_read_usbindx( ptr );
    usb_gpstd_ReqLength = usb_creg_read_usbleng( ptr );
} /* eof usb_pstd_SaveRequest() */

/******************************************************************************
Function Name   : usb_pstd_ChkConfigured
Description     : Check if USB Device is in a CONFIGURED state. 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : Configuration state (YES/NO)
******************************************************************************/
uint16_t usb_pstd_ChkConfigured(USB_UTR_t *ptr)
{
    uint16_t    buf;

    buf = usb_creg_read_intsts( ptr );
    /* Device Status - Configured check */
    if( (buf & USB_DVSQ) == USB_DS_CNFG )
    {
        /* Configured */
        return USB_YES;
    }
    else
    {
        /* not Configured */
        return USB_NO;
    }
} /* eof usb_pstd_ChkConfigured() */

/******************************************************************************
Function Name   : usb_pstd_InterruptEnable
Description     : Enable the VBSE interrupt (VBUS), and the DVSE (Device State 
                : Transition) and CTRE (Control Transfer Stage Transition) int-
                : errupts.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_InterruptEnable(USB_UTR_t *ptr)
{
    /* Enable VBSE, DVSE, CTRE */
    usb_creg_set_intenb( ptr, (USB_VBSE | USB_DVSE | USB_CTRE ));
} /* eof usb_pstd_InterruptEnable() */

/******************************************************************************
Function Name   : usb_pstd_RemoteWakeup
Description     : Set the USB peripheral to implement remote wake up.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_RemoteWakeup(USB_UTR_t *ptr)
{
    uint16_t    buf;
    uint16_t    dev_info[8];

    /* Get USB Device information */
    R_usb_pstd_DeviceInformation(ptr, (uint16_t *)&dev_info );

    /* Support remote wakeup ? */
    if( dev_info[4] == USB_YES )
    {
        /* RESM interrupt disable */
        usb_preg_clr_enb_rsme( ptr );
        
        /* External clock enable */
        usb_creg_set_xcke( ptr );

        /* RESM status read */
        buf = usb_creg_read_intsts( ptr );
        if( (buf & USB_RESM) != (uint16_t)0 )
        {
            /* RESM status clear */
            usb_preg_clr_sts_resm( ptr );
        }
        else
        {
            if( (buf & USB_DS_SUSP) != (uint16_t)0 )
            {
                /* Remote wakeup set */
                usb_preg_set_wkup( ptr );
                usb_gpstd_intsts0   &= (uint16_t)(~USB_DS_SUSP);
            }
        }
    }
} /* eof usb_pstd_RemoteWakeup() */

/******************************************************************************
Function Name   : usb_pstd_TestMode
Description     : USB Peripheral test mode function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_TestMode( USB_UTR_t *ptr )
{
#if ((( USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) && (USB0_IPTYPE_PP == USB_HS_PP))\
   ||(( USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) && (USB1_IPTYPE_PP == USB_HS_PP)))
    switch( (uint16_t)(usb_gpstd_TestModeSelect & USB_TEST_SELECT) )
    {
    case USB_TEST_J:
        /* Continue */
    case USB_TEST_K:
        /* Continue */
    case USB_TEST_SE0_NAK:
        /* Continue */
    case USB_TEST_PACKET:
        usb_creg_set_utst( ptr, 0 );
        usb_creg_set_utst( ptr, (uint16_t)(usb_gpstd_TestModeSelect >> 8) );
        break;
    case USB_TEST_FORCE_ENABLE:
        /* Continue */
    default:
        break;
    }
#endif /* USB0_IPTYPE_PP == USB_HS_PP || USB1_IPTYPE_PP == USB_HS_PP*/
} /* eof usb_pstd_TestMode() */

/******************************************************************************
Function Name   : usb_pstd_ResumeProcess
Description     : Set USB registers to implement resume processing.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_ResumeProcess(USB_UTR_t *ptr)
{
    /* RESM status clear */
    usb_preg_clr_sts_resm( ptr );

    /* RESM interrupt disable */
    usb_preg_clr_enb_rsme( ptr );

} /* eof usb_pstd_ResumeProcess() */

/******************************************************************************
Function Name   : usb_pstd_SetStall
Description     : Set the specified pipe's PID to STALL.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t pipe   : Pipe Number
Return value    : none
******************************************************************************/
void usb_pstd_SetStall(USB_UTR_t *ptr, uint16_t pipe)
{
    /* PIPE control reg set */
    usb_creg_set_pid( ptr, pipe, USB_PID_STALL );
} /* eof usb_pstd_SetStall() */

/******************************************************************************
Function Name   : usb_pstd_SetStallPipe0
Description     : Set pipe "0" PID to STALL.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void usb_pstd_SetStallPipe0(USB_UTR_t *ptr)
{
    /* PIPE control reg set */
    usb_creg_set_pid( ptr, USB_PIPE0, USB_PID_STALL );
} /* eof usb_pstd_SetStallPipe0() */

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End of file
******************************************************************************/
