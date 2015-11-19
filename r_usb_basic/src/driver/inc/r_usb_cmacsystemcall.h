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
* File Name    : r_usb_cmacsystemcall.h
* Description  : uITRON System Call Definition Header File
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/


#ifndef __R_USB_CMACSYSTEMCALL_H__
#define __R_USB_CMACSYSTEMCALL_H__


/*****************************************************************************
Macro definitions
******************************************************************************/
/* uITRON4.0 system call */
#ifndef USB_NO_SYSTEM_PP
    #define USB_CRE_TSK(ID,INFO)        cre_tsk( (USB_ID_t)ID, (USB_TSK_t*)INFO )
    #define USB_DEL_TSK(ID)             del_tsk( (USB_ID_t)ID )
    #define USB_STA_TSK(ID,CODE)        sta_tsk( (USB_ID_t)ID, (USB_VI_t)CODE )
    #define USB_ACT_TSK(ID)             act_tsk( (USB_ID_t)ID )
    #define R_USB_ACT_TSK(ID)           act_tsk( (USB_ID_t)ID )
    #define USB_TER_TSK(ID)             ter_tsk( (USB_ID_t)ID )
    #define USB_EXT_TSK()               ext_tsk( )
    #define USB_REF_TST(ID, STS)        ref_tst( (USB_ID_t)ID, (USB_RTST_t*)STS )

    #define USB_DLY_TSK(TIME)           dly_tsk( (USB_RT_t)TIME )
    #define R_USB_DLY_TSK(TIME)         dly_tsk( (USB_RT_t)TIME )

    #define USB_CRE_MBX(ID, INFO)       cre_mbx( (USB_ID_t)ID, (USB_MBX_t*)INFO )
    #define USB_DEL_MBX(ID)             del_mbx( (USB_ID_t)ID )
    #define USB_SND_MSG(ID, MESS)       snd_mbx( (USB_ID_t)ID, (USB_MSG_t*)MESS )
    #define R_USB_SND_MSG(ID, MESS)     snd_mbx( (USB_ID_t)ID, (USB_MSG_t*)MESS )
    #define USB_ISND_MSG(ID, MESS)      isnd_mbx( (USB_ID_t)ID, (USB_MSG_t*)MESS )
    #define USB_RCV_MSG(ID, MESS)       rcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS )
    #define R_USB_RCV_MSG(ID, MESS)     rcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS )
    #define USB_PRCV_MSG(ID, MESS)      prcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS )
    #define R_USB_PRCV_MSG(ID, MESS)    prcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS )
    #define USB_TRCV_MSG(ID, MESS, TM)  trcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)TM )
    #define R_USB_TRCV_MSG(ID, MESS, TM)    trcv_mbx( (USB_ID_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)TM )

    #define USB_CRE_MPL(ID, INFO)       cre_mpf( (USB_ID_t)ID, (USB_MPL_t*)INFO )
    #define USB_DEL_MPL(ID)             del_mpf( (USB_ID_t)ID )
    #define USB_PGET_BLK(ID, BLK)       pget_mpf( (USB_ID_t)ID, (USB_MH_t*)BLK )
    #define R_USB_PGET_BLK(ID, BLK)     pget_mpf( (USB_ID_t)ID, (USB_MH_t*)BLK )
    #define USB_IPGET_BLK(ID, BLK)      ipget_mpf( (USB_ID_t)ID, (USB_MH_t*)BLK )
    #define USB_REL_BLK(ID, BLK)        rel_mpf( (USB_ID_t)ID, (USB_MH_t)BLK )
    #define R_USB_REL_BLK(ID, BLK)      rel_mpf( (USB_ID_t)ID, (USB_MH_t)BLK )

    #define USB_CRE_SEM(ID, INFO)       cre_sem( (USB_ID_t)ID, (USB_SEM_t*)INFO )
    #define USB_WAI_SEM(ID)             wai_sem( (USB_ID_t)ID )
    #define USB_POL_SEM(ID)             pol_sem( (USB_ID_t)ID )
    #define USB_SIG_SEM(ID)             sig_sem( (USB_ID_t)ID )

    #define USB_CRE_ALM(ID, INFO)       cre_alm( (USB_ID_t)ID, (USB_ALM_t*)INFO )
    #define USB_STA_ALM(ID, TIME)       sta_alm( (USB_ID_t)ID, (USB_RT_t)TIME )
    #define USB_STP_ALM(ID)             stp_alm( (USB_ID_t)ID )
    #define USB_DEL_ALM(ID)             del_alm( (USB_ID_t)ID )

    #define USB_REL_WAI(ID)             rel_wai( (USB_ID_t)ID )
    #define USB_IREL_WAI(ID)            irel_wai( (USB_ID_t)ID )
#else   /* USB_NO_SYSTEM_PP */
    /* nonOS */
    #define USB_CRE_TSK(ID,INFO)        USB_NG
    #define USB_DEL_TSK(ID)             USB_NG
    #define USB_STA_TSK(ID,CODE)        USB_NG
    #define USB_ACT_TSK(ID)             USB_NG
    #define USB_TER_TSK(ID)             USB_NG
    #define USB_EXT_TSK()               USB_NG
    #define USB_REF_TST(ID, STS)        USB_NG

    #define USB_DLY_TSK(TIME)
    #define R_USB_DLY_TSK(TIME)

    #define USB_CRE_MBX(ID, INFO)       USB_NG
    #define USB_DEL_MBX(ID)             USB_NG
    #define USB_SND_MSG(ID, MESS)       usb_cstd_SndMsg( (uint8_t)ID, (USB_MSG_t*)MESS )
    #define R_USB_SND_MSG(ID, MESS)     R_usb_cstd_SndMsg( (uint8_t)ID, (USB_MSG_t*)MESS )
    #define USB_ISND_MSG(ID, MESS)      usb_cstd_iSndMsg( (uint8_t)ID, (USB_MSG_t*)MESS )
    #define R_USB_ISND_MSG(ID, MESS)    R_usb_cstd_iSndMsg( (uint8_t)ID, (USB_MSG_t*)MESS )
    #define USB_WAI_MSG(ID, MESS, TM)   usb_cstd_WaiMsg( (uint8_t)ID, (USB_MSG_t*)MESS, (USB_TM_t)TM )
    #define R_USB_WAI_MSG(ID, MESS, TM) R_usb_cstd_WaiMsg( (uint8_t)ID, (USB_MSG_t*)MESS, (USB_TM_t)TM )
    #define USB_RCV_MSG(ID, MESS)       usb_cstd_RecMsg( (uint8_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)0u )
    #define R_USB_RCV_MSG(ID, MESS)     R_usb_cstd_RecMsg( (uint8_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)0u )
    #define USB_PRCV_MSG(ID, MESS)      USB_NG
    #define R_USB_TRCV_MSG(ID, MESS, TM)    R_usb_cstd_RecMsg( (uint8_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)TM )
    #define USB_TRCV_MSG(ID, MESS, TM)  usb_cstd_RecMsg( (uint8_t)ID, (USB_MSG_t**)MESS, (USB_TM_t)TM )
    
    #define USB_CRE_MPL(ID, INFO)       USB_NG
    #define USB_DEL_MPL(ID)             USB_NG
    #define R_USB_PGET_BLK(ID, BLK)     R_usb_cstd_PgetBlk( (uint8_t)ID, (USB_UTR_t**)BLK )
    #define USB_PGET_BLK(ID, BLK)       usb_cstd_PgetBlk( (uint8_t)ID, (USB_UTR_t**)BLK )
    #define USB_IPGET_BLK(ID, BLK)      USB_NG
    #define R_USB_REL_BLK(ID, BLK)      R_usb_cstd_RelBlk( (uint8_t)ID,  (USB_UTR_t*)BLK )
    #define USB_REL_BLK(ID, BLK)        usb_cstd_RelBlk( (uint8_t)ID,  (USB_UTR_t*)BLK )

    #define USB_CRE_SEM(ID, INFO)       USB_NG
    #define USB_WAI_SEM(ID)             USB_NG
    #define USB_POL_SEM(ID)             USB_NG
    #define USB_SIG_SEM(ID)             USB_NG

    #define USB_CRE_ALM(ID, INFO)       USB_NG
    #define USB_STA_ALM(ID, TIME)       USB_NG
    #define USB_STP_ALM(ID)             USB_NG
    #define USB_DEL_ALM(ID)             USB_NG

    #define USB_REL_WAI(ID)
    #define USB_IREL_WAI(ID)
#endif  /* USB_NO_SYSTEM_PP */


#endif  /* __R_USB_CMACSYSTEMCALL_H__ */

/******************************************************************************
End  Of File
******************************************************************************/
