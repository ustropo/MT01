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
* File Name    : r_usb_reg_access.h
* Description  : USB Peripheral signal control code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/


/************/
/*  SYSCFG  */
/************/
uint16_t    usb_creg_read_syscfg( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_write_syscfg( USB_UTR_t *ptr, uint16_t port, uint16_t  data );
void        usb_creg_set_xtal( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_xcke( USB_UTR_t *ptr );
void        usb_creg_set_scke( USB_UTR_t *ptr );
void        usb_creg_clr_scke( USB_UTR_t *ptr );
void        usb_creg_set_cnen( USB_UTR_t *ptr );
void        usb_creg_clr_cnen( USB_UTR_t *ptr );
void        usb_creg_set_hse( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_clr_hse( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_set_dcfm( USB_UTR_t *ptr );
void        usb_creg_clr_dcfm( USB_UTR_t *ptr );
void        usb_creg_set_drpd( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_clr_drpd( USB_UTR_t *ptr, uint16_t port );
void        usb_preg_set_dprpu( USB_UTR_t *ptr );
void        usb_preg_clr_dprpu( USB_UTR_t *ptr );
void        usb_creg_set_usbe( USB_UTR_t *ptr );
void        usb_creg_clr_usbe( USB_UTR_t *ptr );
void        usb_hreg_clr_drpd( USB_UTR_t *ptr, uint16_t port );


/************/
/*  BUSWAIT */
/************/
void    usb_creg_set_bus_wait( USB_UTR_t *ptr );

/************/
/*  SYSSTS0 */
/************/
uint16_t    usb_creg_read_syssts( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_write_syssts( USB_UTR_t *ptr, uint16_t port, uint16_t data );

/**************/
/*  DVSTCTR0  */
/**************/
uint16_t    usb_creg_read_dvstctr( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_write_dvstctr( USB_UTR_t *ptr, uint16_t port, uint16_t data );
void        usb_creg_rmw_dvstctr( USB_UTR_t *ptr, uint16_t port, uint16_t data, uint16_t width );
void        usb_creg_clr_dvstctr( USB_UTR_t *ptr, uint16_t port, uint16_t data );
void        usb_creg_set_vbout( USB_UTR_t *ptr, uint16_t port );
void        usb_creg_clr_vbout( USB_UTR_t *ptr, uint16_t port );
void        usb_preg_set_wkup( USB_UTR_t *ptr );
void        usb_hreg_set_rwupe( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_rwupe( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_resume( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_resume( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_uact( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_uact( USB_UTR_t *ptr, uint16_t port );

/**************/
/*  TESTMODE  */
/**************/
void        usb_creg_set_utst( USB_UTR_t *ptr, uint16_t data );

/************/
/*  PINCFG  */
/************/
void        usb_creg_set_ldrv( USB_UTR_t *ptr );
void        usb_creg_clr_ldrv( USB_UTR_t *ptr );

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/
void        usb_creg_write_dmacfg( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data );

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/
uint32_t    usb_creg_read_fifo32( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_write_fifo32( USB_UTR_t *ptr, uint16_t pipemode, uint32_t data );
uint16_t    usb_creg_read_fifo16( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_write_fifo16( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data );
uint8_t     usb_creg_read_fifo8( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_write_fifo8( USB_UTR_t *ptr, uint16_t pipemode, uint8_t data );
/************************************/
/*  CFIFOSEL, D0FIFOSEL, D1FIFOSEL  */
/************************************/
uint16_t    usb_creg_read_fifosel( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_write_fifosel( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data );
void        usb_creg_rmw_fifosel( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data, uint16_t width );
void        usb_creg_set_dclrm( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_clr_dclrm( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_set_dreqe( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_clr_dreqe( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_set_mbw( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data );
void        usb_creg_set_bigend( USB_UTR_t *ptr, uint16_t pipemode, uint16_t data );
void        usb_creg_set_curpipe( USB_UTR_t *ptr, uint16_t  pipemode, uint16_t  pipeno );

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/
uint16_t    usb_creg_read_fifoctr( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_set_bval( USB_UTR_t *ptr, uint16_t pipemode );
void        usb_creg_set_bclr( USB_UTR_t *ptr, uint16_t pipemode );

/*************/
/*  INTENB0  */
/*************/
uint16_t    usb_creg_read_intenb( USB_UTR_t *ptr );
void        usb_creg_write_intenb( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_intenb( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_clr_enb_vbse( USB_UTR_t *ptr );
void        usb_preg_set_enb_rsme( USB_UTR_t *ptr );
void        usb_preg_clr_enb_rsme( USB_UTR_t *ptr );
void        usb_creg_clr_enb_sofe( USB_UTR_t *ptr );

/*************/
/*  INTENB1  */
/*************/
uint16_t    usb_hreg_read_intenb( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_write_intenb( USB_UTR_t *ptr, uint16_t port, uint16_t data );
void        usb_hreg_set_enb_ovrcre( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_enb_ovrcre( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_enb_bchge( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_enb_bchge( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_enb_dtche( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_enb_dtche( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_enb_attche( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_enb_attche( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_set_enb_signe( USB_UTR_t *ptr );
void        usb_hreg_clr_enb_signe( USB_UTR_t *ptr );
void        usb_hreg_set_enb_sacke( USB_UTR_t *ptr );
void        usb_hreg_clr_enb_sacke( USB_UTR_t *ptr );
void        usb_hreg_set_enb_pddetinte( USB_UTR_t *ptr );
void        usb_hreg_clr_enb_pddetinte( USB_UTR_t *ptr );

/*************/
/*  BRDYENB  */
/*************/
uint16_t    usb_creg_read_brdyenb( USB_UTR_t *ptr );
void        usb_creg_write_brdyenb( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_brdyenb( USB_UTR_t *ptr, uint16_t  pipeno );
void        usb_creg_clr_brdyenb( USB_UTR_t *ptr, uint16_t  pipeno );

/*************/
/*  NRDYENB  */
/*************/
uint16_t    usb_creg_read_nrdyenb( USB_UTR_t *ptr );
void        usb_creg_write_nrdyenb( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_nrdyenb( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_clr_nrdyenb(USB_UTR_t *ptr, uint16_t pipeno );

/*************/
/*  BEMPENB  */
/*************/
uint16_t    usb_creg_read_bempenb( USB_UTR_t *ptr );
void        usb_creg_write_bempenb( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_bempenb( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_clr_bempenb( USB_UTR_t *ptr, uint16_t pipeno );


/*************/
/*  SOFCFG   */
/*************/
void        usb_creg_set_sofcfg( USB_UTR_t *ptr, uint16_t data );



/*************/
/*  SOFCFG   */
/*************/
uint16_t    usb_creg_read_sofcfg( USB_UTR_t *ptr );
void        usb_hreg_set_trnensel( USB_UTR_t *ptr );
void        usb_hreg_clr_trnensel( USB_UTR_t *ptr );

/*************/
/*  PHYSET   */
/*************/
uint16_t    usb_creg_read_pipebuf( USB_UTR_t *ptr );
void        usb_creg_write_clksel( USB_UTR_t *ptr );
void        usb_creg_clr_pllreset( USB_UTR_t *ptr );
void        usb_creg_clr_dirpd( USB_UTR_t *ptr );

#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
void        usb_creg_clr_hseb( USB_UTR_t *ptr );
void        usb_creg_write_repsel( USB_UTR_t *ptr );
#endif  /* defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M) */

/*************/
/*  INTSTS0  */
/*************/
uint16_t    usb_creg_read_intsts( USB_UTR_t *ptr );
void        usb_creg_write_intsts( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_clr_sts_vbint( USB_UTR_t *ptr );
void        usb_preg_clr_sts_resm( USB_UTR_t *ptr );
void        usb_creg_clr_sts_sofr( USB_UTR_t *ptr );
void        usb_preg_clr_sts_dvst( USB_UTR_t *ptr );
void        usb_preg_clr_sts_ctrt( USB_UTR_t *ptr );
void        usb_preg_clr_sts_valid( USB_UTR_t *ptr );

/*************/
/*  INTSTS1  */
/*************/
uint16_t    usb_hreg_read_intsts( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_write_intsts( USB_UTR_t *ptr, uint16_t port, uint16_t data );
void        usb_hreg_clr_sts_ovrcr( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_sts_bchg( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_sts_dtch( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_sts_attch( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_sts_eoferr( USB_UTR_t *ptr, uint16_t port );
void        usb_hreg_clr_sts_sign( USB_UTR_t *ptr );
void        usb_hreg_clr_sts_sack( USB_UTR_t *ptr );
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
void        usb_hreg_clr_sts_pddetint( USB_UTR_t *ptr );
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
/************/
/* BRDYSTS  */
/************/
uint16_t    usb_creg_read_brdysts( USB_UTR_t *ptr );
void        usb_creg_write_brdysts( USB_UTR_t *pt, uint16_t data );
void        usb_creg_clr_sts_brdy( USB_UTR_t *ptr, uint16_t pipeno );

/************/
/* NRDYSTS  */
/************/
uint16_t    usb_creg_read_nrdysts( USB_UTR_t *ptr );
void        usb_creg_write_nrdysts( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_clr_sts_nrdy( USB_UTR_t *ptr, uint16_t pipeno );

/************/
/* BEMPSTS  */
/************/
uint16_t    usb_creg_read_bempsts( USB_UTR_t *ptr );
void        usb_creg_write_bempsts( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_clr_sts_bemp( USB_UTR_t *ptr, uint16_t pipeno );

/************/
/* FRMNUM   */
/************/
uint16_t    usb_creg_read_frmnum( USB_UTR_t *ptr );

/************/
/* USBADDR  */
/************/
uint16_t    usb_creg_read_usbaddr( USB_UTR_t *ptr );
void        usb_creg_set_stsrecov( USB_UTR_t *ptr, uint16_t data );

/************/
/* USBREQ   */
/************/
uint16_t    usb_creg_read_usbreq( USB_UTR_t *ptr );
void        usb_hreg_write_usbreq( USB_UTR_t *ptr, uint16_t data );

/************/
/* USBVAL   */
/************/
uint16_t    usb_creg_read_usbval( USB_UTR_t *ptr );
void        usb_hreg_set_usbval( USB_UTR_t *ptr, uint16_t data );

/************/
/* USBINDX  */
/************/
uint16_t    usb_creg_read_usbindx( USB_UTR_t *ptr );
void        usb_hreg_set_usbindx( USB_UTR_t *ptr, uint16_t data );

/************/
/* USBLENG  */
/************/
uint16_t    usb_creg_read_usbleng( USB_UTR_t *ptr );
void        usb_hreg_set_usbleng( USB_UTR_t *ptr, uint16_t data );

/************/
/* DCPCFG   */
/************/
uint16_t    usb_creg_read_dcpcfg( USB_UTR_t *ptr );
void        usb_creg_write_dcpcfg( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_dcpshtnak( USB_UTR_t *ptr );

/************/
/* DCPMAXP  */
/************/
uint16_t    usb_creg_read_dcpmaxp( USB_UTR_t *ptr );
void    usb_creg_write_dcpmxps( USB_UTR_t *ptr, uint16_t data );

/************/
/* DCPCTR   */
/************/
uint16_t    usb_creg_read_dcpctr( USB_UTR_t *ptr );
void        usb_hreg_write_dcpctr( USB_UTR_t *ptr, uint16_t data );
void        usb_hreg_set_sureq( USB_UTR_t *ptr );
void        usb_hreg_set_sureqclr( USB_UTR_t *ptr );
void        usb_preg_set_ccpl( USB_UTR_t *ptr );

/************/
/* PIPESEL  */
/************/
uint16_t    usb_creg_read_pipesel( USB_UTR_t *ptr );
void        usb_creg_write_pipesel( USB_UTR_t *ptr, uint16_t data );

/************/
/* PIPECFG  */
/************/
uint16_t    usb_creg_read_pipecfg( USB_UTR_t *ptr );
void        usb_creg_write_pipecfg( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_type( USB_UTR_t *ptr, uint16_t data );

/************/
/* PIPEBUF  */
/************/
void        usb_creg_write_pipebuf( USB_UTR_t *ptr, uint16_t data );

/************/
/* PIPEMAXP */
/************/
uint16_t    usb_creg_read_pipemaxp( USB_UTR_t *ptr );
void        usb_creg_write_pipemaxp( USB_UTR_t *ptr, uint16_t data );
void        usb_hreg_set_devsel( USB_UTR_t *ptr, uint16_t data );
void        usb_creg_set_mxps( USB_UTR_t *ptr, uint16_t data );

/************/
/* PIPEPERI */
/************/
uint16_t    usb_creg_read_pipeperi( USB_UTR_t *ptr );
void        usb_creg_write_pipeperi( USB_UTR_t *ptr, uint16_t data );

/********************/
/* DCPCTR, PIPEnCTR */
/********************/
uint16_t    usb_creg_read_pipectr( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_write_pipectr( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data );
void        usb_creg_set_csclr( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_aclrm( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_clr_aclrm( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_sqclr( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_sqset( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_clr_sqset( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_pid( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data );
void        usb_creg_clr_pid( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data );

/************/
/* PIPEnTRE */
/************/
uint16_t    usb_creg_read_pipetre( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_trenb( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_clr_trenb( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_set_trclr( USB_UTR_t *ptr, uint16_t pipeno );

/************/
/* PIPEnTRN */
/************/
uint16_t    usb_creg_read_pipetrn( USB_UTR_t *ptr, uint16_t pipeno );
void        usb_creg_write_pipetrn( USB_UTR_t *ptr, uint16_t pipeno, uint16_t data );

/************/
/* DEVADDn  */
/************/
uint16_t    usb_hreg_read_devadd( USB_UTR_t *ptr, uint16_t devadr );
void        usb_hreg_rmw_devadd( USB_UTR_t *ptr, uint16_t devsel, uint16_t data, uint16_t width );
void        usb_hreg_set_usbspd( USB_UTR_t *ptr, uint16_t devadr, uint16_t data );

/************/
/* DEVADDn  */
/************/
void        usb_hreg_write_physlew( USB_UTR_t *ptr );
void        usb_preg_write_physlew( USB_UTR_t *ptr );

/************/
/* LPSTS    */
/************/
void        usb_creg_set_suspendm( USB_UTR_t *ptr );

/************/
/* BCCTRL   */
/************/
uint16_t    usb_creg_read_bcctrl( USB_UTR_t *ptr );
void        usb_creg_set_vdmsrce( USB_UTR_t *ptr );
void        usb_creg_clr_vdmsrce( USB_UTR_t *ptr );
void        usb_creg_set_idpsinke( USB_UTR_t *ptr );
void        usb_creg_clr_idpsinke( USB_UTR_t *ptr );
void        usb_preg_set_bcctrl( USB_UTR_t *ptr, uint16_t data );
void        usb_preg_clr_bcctrl( USB_UTR_t *ptr, uint16_t data );
void        usb_preg_set_vdpsrce( USB_UTR_t *ptr );
void        usb_preg_clr_vdpsrce( USB_UTR_t *ptr );
void        usb_preg_set_idmsinke( USB_UTR_t *ptr );
void        usb_preg_clr_idmsinke( USB_UTR_t *ptr );
void        usb_preg_set_idpsrce( USB_UTR_t *ptr );
void        usb_preg_clr_idpsrce( USB_UTR_t *ptr );
void        usb_hreg_set_dcpmode( USB_UTR_t *ptr );
void        usb_hreg_clr_dcpmode( USB_UTR_t *ptr );

/******************************************************************************
End of file
******************************************************************************/
