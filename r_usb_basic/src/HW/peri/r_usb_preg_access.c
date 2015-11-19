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
* File Name    : r_usb_preg_access.c
* Description  : USB Peripheral signal control code
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


#if (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP)

/************/
/*  SYSCFG  */
/************/

/******************************************************************************
Function Name   : usb_preg_set_dprpu
Description     : Set DPRPU-bit SYSCFG register.
                : (Enable D+Line pullup when PeripheralController function is selected) 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_dprpu( USB_UTR_t *ptr )
{
#ifdef LSFUNC
    ptr->ipp->SYSCFG.WORD |= USB_DMRPU;
#else
    ptr->ipp->SYSCFG.WORD |= USB_DPRPU;
#endif
} /* eof usb_preg_set_dprpu() */

/******************************************************************************
Function Name   : usb_preg_clr_dprpu
Description     : Clear DPRPU-bit of the SYSCFG register.
                : (Disable D+Line pullup when PeripheralController function is 
                : selected.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_dprpu( USB_UTR_t *ptr )
{
#ifdef LSFUNC
    ptr->ipp->SYSCFG.WORD &= ~USB_DMRPU;
#else
    ptr->ipp->SYSCFG.WORD &= ~USB_DPRPU;
#endif
} /* eof usb_preg_clr_dprpu() */

/************/
/*  SYSSTS0 */
/************/

/**************/
/*  DVSTCTR0  */
/**************/
/******************************************************************************
Function Name   : usb_preg_set_wkup
Description     : Set WKUP-bit DVSTCTR register.
                : (Output Remote wakeup signal when PeripheralController function is selected)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_wkup( USB_UTR_t *ptr )
{
    ptr->ipp->DVSTCTR0.WORD |= USB_WKUP;
} /* eof usb_preg_set_wkup() */

/**************/
/*  TESTMODE  */
/**************/

/************/
/*  PINCFG  */
/************/

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/

/**********************************/
/* CFIFOSEL, D0FIFOSEL, D1FIFOSEL */
/**********************************/

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/

/*************/
/*  INTENB0  */
/*************/
/******************************************************************************
Function Name   : usb_preg_set_enb_rsme
Description     : Enable interrupt from RESUME
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_enb_rsme( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD |= USB_RSME;
} /* eof usb_preg_set_enb_rsme() */

/******************************************************************************
Function Name   : usb_preg_clr_enb_rsme
Description     : Disable interrupt from RESUME
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_enb_rsme( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB0.WORD &= ~USB_RSME;
} /* eof usb_preg_set_enb_rsme() */

/*************/
/*  INTENB1  */
/*************/

/*************/
/*  BRDYENB  */
/*************/

/*************/
/*  NRDYENB  */
/*************/

/*************/
/*  BEMPENB  */
/*************/

/*************/
/*  SOFCFG   */
/*************/

/*************/
/*  INTSTS0  */
/*************/
/******************************************************************************
Function Name   : usb_preg_clr_sts_resm
Description     : Clear interrupt status of RESUME.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_resm( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_RESM;
} /* eof usb_preg_clr_sts_resm() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_dvst
Description     : Clear Device State Transition interrupt flag. 
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_dvst( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_DVST;
} /* eof usb_preg_clr_sts_dvst() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_ctrt
Description     : Clear Control Transfer Stage Transition interrupt flag.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_ctrt( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_CTRT;
} /* eof usb_preg_clr_sts_dvst() */

/******************************************************************************
Function Name   : usb_preg_clr_sts_valid
Description     : Clear the Setup Packet Reception interrupt flag.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_clr_sts_valid( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS0.WORD = (uint16_t)~USB_VALID;
} /* eof usb_preg_clr_sts_valid() */

/*************/
/*  INTSTS1  */
/*************/

/************/
/* BRDYSTS  */
/************/

/************/
/* NRDYSTS  */
/************/

/************/
/* BEMPSTS  */
/************/

/************/
/* FRMNUM   */
/************/

/************/
/* USBADDR  */
/************/

/************/
/* USBREQ   */
/************/

/************/
/* USBVAL   */
/************/

/************/
/* USBINDX  */
/************/

/************/
/* USBLENG  */
/************/

/************/
/* DCPCFG   */
/************/

/************/
/* DCPMAXP  */
/************/

/************/
/* DCPCTR   */
/************/
/******************************************************************************
Function Name   : usb_preg_clr_sts_valid
Description     : Enable termination of control transfer status stage.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void    usb_preg_set_ccpl( USB_UTR_t *ptr )
{
    ptr->ipp->DCPCTR.WORD |= USB_CCPL;
} /* eof usb_preg_set_ccpl() */

/************/
/* PIPESEL  */
/************/

/************/
/* PIPECFG  */
/************/

/************/
/* PIPEBUF  */
/************/

/************/
/* PIPEMAXP */
/************/

/************/
/* PIPEPERI */
/************/

/********************/
/* DCPCTR, PIPEnCTR */
/********************/

/************/
/* PIPEnTRE */
/************/

/************/
/* PIPEnTRN */
/************/

/************/
/* DEVADDn  */
/************/

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/************/
/* PHYSLEW  */
/************/

/******************************************************************************
Function Name   : usb_preg_write_physlew
Description     : Set the PHYSLEW register's for funcrion mode
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_write_physlew( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_0)
    {
        ptr->ipp->PHYSLEW.LONG = 0x00000005;
    }
} /* eof usb_preg_write_physlew() */


/************/
/* BCCTRL   */
/************/

/******************************************************************************
Function Name   : usb_preg_set_bcctrl
Description     : Set BCCTRL's bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t data   : 
Return value    : none
******************************************************************************/
void    usb_preg_set_bcctrl( USB_UTR_t *ptr, uint16_t data )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD |= data;
    }
} /* eof usb_preg_set_bcctrl() */

/******************************************************************************
Function Name   : usb_preg_clr_bcctrl
Description     : Clear BCCTRL's bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t data   : 
Return value    : none
******************************************************************************/
void    usb_preg_clr_bcctrl( USB_UTR_t *ptr, uint16_t data )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD &= ~data;
    }
} /* eof usb_preg_clr_bcctrl() */

/******************************************************************************
Function Name   : usb_preg_set_vdpsrce
Description     : Set VDPSRCE bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_set_vdpsrce( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD |= USB_VDPSRCE;
    }
} /* eof usb_preg_set_vdpsrce() */

/******************************************************************************
Function Name   : usb_preg_clr_vdpsrce
Description     : Clear VDPSRCE bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_clr_vdpsrce( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD &= ~USB_VDPSRCE;
    }
} /* eof usb_preg_clr_vdpsrce() */

/******************************************************************************
Function Name   : usb_preg_set_idmsinke
Description     : Set IDMSINKE bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_set_idmsinke( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD |= USB_IDMSINKE;
    }
} /* eof usb_preg_set_idmsinke() */

/******************************************************************************
Function Name   : usb_preg_clr_idmsinke
Description     : Clear IDMSINKE bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_clr_idmsinke( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD &= ~USB_IDMSINKE;
    }
} /* eof usb_preg_clr_idmsinke() */

/******************************************************************************
Function Name   : usb_preg_set_idpsrce
Description     : Set IDPSRCE bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_set_idpsrce( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD |= USB_IDPSRCE;
    }
} /* eof usb_preg_set_idpsrce() */

/******************************************************************************
Function Name   : usb_preg_clr_idpsrce
Description     : Clear IDPSRCE bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_preg_clr_idpsrce( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD &= ~USB_IDPSRCE;
    }
} /* eof usb_preg_clr_idpsrce() */
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */
#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End of file
******************************************************************************/

