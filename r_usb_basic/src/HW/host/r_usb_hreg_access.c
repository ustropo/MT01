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
* File Name    : r_usb_hreg_access.c
* Description  : USB IP register access code
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


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Constant macro definitions
******************************************************************************/
#define USB_DEVSEL_NUM_SHIFT    12

/************/
/*  SYSCFG  */
/************/
/******************************************************************************
Function Name   : usb_hreg_set_drpd
Description     : Set DRPD bit of specified port's SYSCFG register. This is only 
                : for when Host, to pull down the D+ and D- lines.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_drpd( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSCFG.WORD |= USB_DRPD;
    }
} /* eof usb_hreg_set_drpd() */

/******************************************************************************
Function Name   : usb_hreg_clr_drpd
Description     : Clear DRPD-bit specified port's SYSCFG register. For 
                : host external circuit, to not pull down the D+ and D- lines.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_drpd( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->SYSCFG.WORD &= ~USB_DRPD;
    }
} /* eof usb_hreg_clr_drpd() */

/************/
/*  SYSSTS0 */
/************/
/* System Configuration Status Register 0 */

/**************/
/*  DVSTCTR0  */
/**************/
/* Device State Control Register 0 */

/******************************************************************************
Function Name   : usb_hreg_set_rwupe
Description     : Set the RWUPE-bit specified port's DVSTCTR0 reg-
                : ister. When host. To allow detection of remote wake-up from 
                : a USB Function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_rwupe( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_RWUPE;
    }
} /* eof usb_hreg_set_rwupe() */

/******************************************************************************
Function Name   : usb_hreg_clr_rwupe
Description     : Clear the RWUPE-bit specified port's DVSTCTR0 reg-
                : ister. When host. To prohibit detection of remote wake-up from 
                : a USB Function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_rwupe( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD &= ~USB_RWUPE;
    }
} /* eof usb_hreg_clr_rwupe() */

/******************************************************************************
Function Name   : usb_hreg_set_resume
Description     : Set the RESUME-bit specified port's DVSTCTR0 register 
                : When host. To allow output of resume signal to a USB Function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_resume( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_RESUME;
    }
} /* eof usb_hreg_set_resume() */

/******************************************************************************
Function Name   : usb_hreg_clr_resume
Description     : Clear the RESUME-bit specified port's DVSTCTR0 register 
                : When host. To prohibit output of resume signal to a USB Func-
                : tion.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_resume( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD &= ~USB_RESUME;
    }
} /* eof usb_hreg_clr_resume() */

/******************************************************************************
Function Name   : usb_hreg_set_uact
Description     : Set UACT-bit (USB Bus Enable) specified port's DVSTCTR0 
                : register. When Host, to output SOF.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_uact( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD |= USB_UACT;
    }
} /* eof usb_hreg_set_uact() */

/******************************************************************************
Function Name   : usb_hreg_clr_uact
Description     : Clear UACT-bit (USB Bus Enable) specified port's DVSTCTR0 
                : register. When Host, to prohibit output SOF.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_uact( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->DVSTCTR0.WORD &= ~USB_UACT;
    }
} /* eof usb_hreg_clr_uact() */

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

/*************/
/*  INTENB1  */
/*************/
/* Interrupt Enable Register 1 */

/******************************************************************************
Function Name   : usb_hreg_read_intenb
Description     : Returns the value of the specified port's INTENB1 register.
                : uint16_t  port  : USB port number. //$REA - not used.
Return value    : INTENB1/INTENB2 content
******************************************************************************/
uint16_t    usb_hreg_read_intenb( USB_UTR_t *ptr, uint16_t port )
{
    return ptr->ipp->INTENB1.WORD;
} /* eof usb_hreg_read_intenb() */

/******************************************************************************
Function Name   : usb_hreg_write_intenb
Description     : Write the specified data to the specified port's INTENB register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_write_intenb( USB_UTR_t *ptr, uint16_t port, uint16_t data )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD = data;
    }
} /* eof usb_hreg_write_intenb() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_ovrcre
Description     : Set specified port's OVRCRE-bit (Overcurrent Input Change Int-
                : errupt Status Enable) in the INTENB1 register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_ovrcre( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD |= USB_OVRCRE;
    }
} /* eof usb_hreg_set_enb_ovrcre() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_ovrcre
Description     : Clear the OVRCRE-bit of the specified port's INTENB1 register,
                : to prohibit VBUS interrupts.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_ovrcre( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD &= ~USB_OVRCRE;
    }
} /* eof usb_hreg_clr_enb_ovrcre() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_bchge
Description     : The BCHGE-bit (USB Bus Change Interrupt Enable) is set in the 
                : specified port's INTENB1 register. This will cause a BCHG 
                : interrupt when a change of USB bus state has been detected.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_bchge( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD |= USB_BCHGE;
    }
} /* eof usb_hreg_set_enb_bchge() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_bchge
Description     : The BCHGE-bit (USB Bus Change Interrupt Enable) is cleared in 
                : the specified port's INTENB1 register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_bchge( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD &= ~USB_BCHGE;
    }
} /* eof usb_hreg_clr_enb_bchge() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_dtche
Description     : Enable the specified port's DTCHE-interrupt "Disconnection 
                : Detection" by setting the DTCHE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_dtche( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD |= USB_DTCHE;
    }
} /* eof usb_hreg_set_enb_dtche() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_dtche
Description     : Disable the specified port's DTCHE-interrupt "Disconnection 
                : Detection" by clearing the DTCHE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_dtche( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD &= ~USB_DTCHE;
    }
} /* eof usb_hreg_clr_enb_bchge() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_attche
Description     : Enable the specified port's ATTCHE-interrupt "Connection 
                : Detection" by setting the ATTCHE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_attche( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD |= USB_ATTCHE;
    }
} /* eof usb_hreg_set_enb_attche() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_attche
Description     : Disable the specified port's ATTCHE-interrupt "Disconnection 
                : Detection" by clearing the ATTCHE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_attche( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTENB1.WORD &= ~USB_ATTCHE;
    }
} /* eof usb_hreg_clr_enb_attche() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_signe
Description     : Enable the SIGNE-interrupt "Setup Transaction
                : Error" by setting the SIGNE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_signe( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB1.WORD |= USB_SIGNE;
} /* eof usb_hreg_set_enb_signe() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_signe
Description     : Disable the SIGNE-interrupt "Setup Transac-
                : tion Error" by clearing the SIGNE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_signe( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB1.WORD &= ~USB_SIGNE;
} /* eof usb_hreg_clr_enb_signe() */

/******************************************************************************
Function Name   : usb_hreg_set_enb_sacke
Description     : Enable the SACKE-interrupt "Setup Transaction 
                : Normal Response" by setting the SACKE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_sacke( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB1.WORD |= USB_SACKE;
} /* eof usb_hreg_set_enb_sacke() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_sacke
Description     : Disable the SACKE-interrupt "Setup Transac-
                : tion Normal Response" by clearing the SACKE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_sacke( USB_UTR_t *ptr )
{
    ptr->ipp->INTENB1.WORD &= ~USB_SACKE;
} /* eof usb_hreg_clr_enb_sacke() */
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
/******************************************************************************
Function Name   : usb_hreg_set_enb_pddetinte
Description     : Enable the PDDETINT-interrupt "Connection Detection for 
                : Battery Charging Supporting Device" by setting 
                : the PDDETINTE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_enb_pddetinte( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->INTENB1.WORD |= USB_PDDETINTE;
    }
} /* eof usb_hreg_set_enb_pddetinte() */

/******************************************************************************
Function Name   : usb_hreg_clr_enb_pddetinte
Description     : Disable the PDDETINT-interrupt "Connection Detection for 
                : Battery Charging Supporting Device" by clearing 
                : the PDDETINTE-bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_enb_pddetinte( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->INTENB1.WORD &= ~USB_PDDETINTE;
    }
} /* eof usb_hreg_clr_enb_pddetinte() */
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */
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
/* SOF Output Configuration */

/******************************************************************************
Function Name   : usb_hreg_set_trnensel
Description     : When host, set the TRNENSEL-bit; "Transac-
                : tion-Enabled Time Select" for low-speed USB communication.
                : This bit should be set to 0 if USB Function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_trnensel( USB_UTR_t *ptr )
{
    ptr->ipp->SOFCFG.WORD |= USB_TRNENSEL;
} /* eof usb_hreg_set_trnensel() */

/******************************************************************************
Function Name   : usb_hreg_clr_trnensel
Description     : When host, clear the TRNENSEL-bit; "Transac-
                : tion-Enabled Time Select" for non low-speed communication.
                : This bit should be set to 0 if USB Function.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_trnensel( USB_UTR_t *ptr )
{
    ptr->ipp->SOFCFG.WORD &= ~USB_TRNENSEL;
} /* eof usb_hreg_clr_trnensel() */

/*************/
/*  INTSTS0  */
/*************/

/*************/
/*  INTSTS1  */
/*************/
/* Interrupt Status Register 1  */

/******************************************************************************
Function Name   : usb_hreg_read_intsts
Description     : Returns the value of the specified port's INTSTS1 register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : INTSTS1 content
******************************************************************************/
uint16_t    usb_hreg_read_intsts( USB_UTR_t *ptr, uint16_t port )
{
    return (uint16_t)(ptr->ipp->INTSTS1.WORD);
} /* eof usb_hreg_read_intsts() */

/******************************************************************************
Function Name   : usb_hreg_write_intsts
Description     : Write the specified data to the specified port's INTSTS1 reg-
                : ister.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_write_intsts( USB_UTR_t *ptr, uint16_t port, uint16_t data )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = data;
    }
} /* eof usb_hreg_write_intsts() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_ovrcr
Description     : Clear the specified port's OVRCR-bit; "Overcurrent 
                : Input Change Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_ovrcr( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_OVRCR;
    }
} /* eof usb_hreg_clr_sts_ovrcr() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_bchg
Description     : Clear the specified port's BCHG-bit; "USB Bus Change Interrupt 
                : Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_bchg( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_BCHG;
    }
} /* eof usb_hreg_clr_sts_bchg() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_dtch
Description     : Clear the specified port's DTCH-bit; "USB Disconnection Detec-
                : tion Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_dtch( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_DTCH;
    }
} /* eof usb_hreg_clr_sts_dtch() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_attch
Description     : Clear the specified port's ATTCH-bit; "ATTCH Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_attch( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_ATTCH;
    }
} /* eof usb_hreg_clr_sts_attch() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_eoferr
Description     : Clear the specified port's EOFERR-bit; "EOF Error Detection 
                : Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  port  : USB port number.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_eoferr( USB_UTR_t *ptr, uint16_t port )
{
    if( USB_PORT0 == port )
    {
        ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_EOFERR;
    }
} /* eof usb_hreg_clr_sts_eoferr() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_sign
Description     : Clear the SIGN-bit; "Setup Transaction Error
                : Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_sign( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_SIGN;
} /* eof usb_hreg_clr_sts_sign() */

/******************************************************************************
Function Name   : usb_hreg_clr_sts_sack
Description     : Clear the SACK-bit; "Setup Transaction Normal
                : Response Interrupt Status".
                : Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_sack( USB_UTR_t *ptr )
{
    ptr->ipp->INTSTS1.WORD = (uint16_t)~USB_SACK;
} /* eof usb_hreg_clr_sts_sack() */
#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/******************************************************************************
Function Name   : usb_hreg_clr_sts_pddetint
Description     : Clear the PDDETINT-bit; "
                : ".
                : Interrupt Status".
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_sts_pddetint( USB_UTR_t *ptr, uint16_t port )
{
    if(port == USB_PORT0)
    {
        if(ptr->ip == USB_USBIP_1)
        {
            ptr->ipp1->INTSTS1.WORD = (uint16_t)~USB_PDDETINT;
        }
    }
} /* eof usb_hreg_clr_sts_sack() */
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
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
/* USB Request Type */

/******************************************************************************
Function Name   : usb_hreg_write_usbreq
Description     : Write bRequest and bmRequestType to USBREQ register.
                : When Host, the values of bRequest and bmRequestType 
                : to be transmitted are written. (When Function, the received 
                : values of bRequest and bmRequestType are stored in USBREQ).
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_write_usbreq( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->USBREQ.WORD = data;
} /* eof usb_hreg_write_usbreq() */

/************/
/* USBVAL   */
/************/
/* USB Request Value Register */

/******************************************************************************
Function Name   : usb_hreg_set_usbval
Description     : Write the specified 'wValue' to USBVAL register,
                : to write the USB request. When Host, the value of 
                : wValue to be transmitted is set. (When Function, the value of 
                : wValue that has been received is stored in USBREQ.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_set_usbval( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->USBVAL = data;
} /* eof usb_hreg_set_usbval() */

/************/
/* USBINDX  */
/************/
/* USB Request Index */

/******************************************************************************
Function Name   : usb_hreg_set_usbindx
Description     : Write the specified 'wIndex', the USB request, to USBINDX
                : register, for host setup requests for control 
                : transfers.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_set_usbindx( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->USBINDX = data;
} /* eof usb_hreg_set_usbindx() */

/************/
/* USBLENG  */
/************/
/* USB Request Length */

/******************************************************************************
Function Name   : usb_hreg_set_usbleng
Description     : Write the specified 'wLength' value to USBINDX register, 
                : for host setup requests for control.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_set_usbleng( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->USBLENG = data;
} /* eof usb_hreg_set_usbleng() */

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
Function Name   : usb_hreg_write_dcpctr
Description     : Write the specified data value to the DCPCTR register.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_write_dcpctr( USB_UTR_t *ptr, uint16_t data )
{
    ptr->ipp->DCPCTR.WORD = data;
} /* eof usb_hreg_write_dcpctr() */

/******************************************************************************
Function Name   : usb_hreg_set_sureq
Description     : Set te SUREQ-bit in the DCPCTR register 
                : (Set SETUP packet send when HostController function is selected)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_sureq( USB_UTR_t *ptr )
{
    ptr->ipp->DCPCTR.WORD |= USB_SUREQ;
} /* eof usb_hreg_set_sureq() */

/******************************************************************************
Function Name   : usb_hreg_set_sureqclr
Description     : Set the SUREQCLR-bit in the DCPCTR register.
                : (Disable SETUP packet send setting when HostController func-
                :  tion is selected)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_sureqclr( USB_UTR_t *ptr )
{
    ptr->ipp->DCPCTR.WORD |= USB_SUREQCLR;
} /* eof usb_hreg_set_sureqclr() */

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
/******************************************************************************
Function Name   : usb_hreg_set_devsel
Description     : Write the address specified by the argument to the DEVSEL-bit 
                : of the PIPEMAXP register. (Set the address of the USB Device 
                : when HostController function is selected.)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_set_devsel( USB_UTR_t *ptr, uint16_t data )
{
    volatile uint16_t *reg_p;
    
    reg_p = (uint16_t *)&ptr->ipp->PIPEMAXP;
    *reg_p &= ~USB_DEVSEL;
    *reg_p |= data << USB_DEVSEL_NUM_SHIFT;
} /* eof usb_hreg_set_devsel() */

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
/******************************************************************************
Function Name   : usb_hreg_read_devadd
Description     : Return the DEVADD register value for the specified USB device 
                ; address.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  devsel      ; USB device address value 
Return value    : DEVADDx content
******************************************************************************/
uint16_t    usb_hreg_read_devadd( USB_UTR_t *ptr, uint16_t devsel )
{
    volatile uint16_t    *reg_p;
    uint16_t    devadr;
    uint16_t    return_value;

    devadr = devsel >> USB_DEVADDRBIT;

    if(devadr > USB_MAXDEVADDR)
    {
        return USB_ERROR;
    }
    else
    {
        reg_p = (uint16_t *)&(ptr->ipp->DEVADD0) + devadr;
        return_value = (*reg_p & (USB_UPPHUB | USB_HUBPORT | USB_USBSPD));
        return return_value;
    }
} /* eof usb_hreg_read_devadd() */

/******************************************************************************
Function Name   : usb_hreg_rmw_devadd
Description     : Read-modify-write the specified devsel's DEVADD.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  devsel: Device address
                : uint16_t  data  : The value to write.
                : uint16_t  width : Bit pattern to read-modify-write.
Return value    : none
******************************************************************************/
void    usb_hreg_rmw_devadd( USB_UTR_t *ptr, uint16_t devsel, uint16_t data, uint16_t width )
{
    volatile uint16_t    *reg_p;
    uint16_t    buf;
    uint16_t    devadr;

    devadr = devsel >> USB_DEVADDRBIT;

    reg_p = (uint16_t *)&(ptr->ipp->DEVADD0) + devadr;

    buf = *reg_p;
    buf &= ~width;
    buf |= (data & width);
    *reg_p = buf;
} /* eof usb_hreg_rmw_devadd() */

/******************************************************************************
Function Name   : usb_hreg_set_usbspd
Description     : Set the DEVADD register's USBSPD for the specified device add-
                : ress.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
                : uint16_t  devsel      ; USB device address value
                : uint16_t  data  : The value to write.
Return value    : none
******************************************************************************/
void    usb_hreg_set_usbspd( USB_UTR_t *ptr, uint16_t devsel, uint8_t data )
{
    volatile uint16_t    *reg_p;
    uint16_t    devadr;

    devadr = devsel >> USB_DEVADDRBIT;

    reg_p = (uint16_t *)&(ptr->ipp->DEVADD0) + devadr;

    *reg_p &= ~USB_USBSPD;
    *reg_p |= data;
} /* eof usb_hreg_set_usbspd() */

#if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M)
/************/
/* PHYSLEW  */
/************/

/******************************************************************************
Function Name   : usb_hreg_write_physlew
Description     : Set the PHYSLEW register's for host mode
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_write_physlew( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_0)
    {
        ptr->ipp->PHYSLEW.LONG = 0x0000000E;
    }
} /* eof usb_hreg_write_physlew() */


/************/
/* BCCTRL   */
/************/

/******************************************************************************
Function Name   : usb_hreg_set_dcpmode
Description     : Set DCPMODE bit.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_set_dcpmode( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD |= USB_DCPMODE;
    }
} /* eof usb_hreg_set_dcpmode() */

/******************************************************************************
Function Name   : usb_hreg_clr_dcpmode
Description     : Clear DCPMODE bits.
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel.
Return value    : none
******************************************************************************/
void    usb_hreg_clr_dcpmode( USB_UTR_t *ptr )
{
    if(ptr->ip == USB_USBIP_1)
    {
        ptr->ipp1->BCCTRL.WORD &= ~USB_DCPMODE;
    }
} /* eof usb_hreg_clr_dcpmode() */
#endif /* #if defined(BSP_MCU_RX64M) | (BSP_MCU_RX71M) */
#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End of file
******************************************************************************/
