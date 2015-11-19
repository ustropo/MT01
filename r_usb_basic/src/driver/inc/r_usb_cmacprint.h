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
* File Name    : r_usb_cmacprint.h
* Description  : Print Function Definition Header File
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/


#ifndef __R_USB_CMACPRINT_H__
#define __R_USB_CMACPRINT_H__


/*****************************************************************************
Macro definitions
******************************************************************************/
#if (USB_DEBUG_OUTPUT_PP == USB_DEBUG_ON_PP)
   #define       USB_DEBUGPRINT_PP       /* IDE Console */
#endif/* USB_DEBUG_OUTPUT_PP == USB_DEBUG_ON_PP */

/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGUART_PP
    #include    <stdlib.h>          /* @@@MISRA del */
    #include    <stdio.h>           /* @@@MISRA del */
    #define USB_SPRINTF0(FORM)                          fprintf(stderr,FORM)
    #define USB_SPRINTF1(FORM,x1)                       fprintf(stderr,FORM,x1)
    #define USB_SPRINTF2(FORM,x1,x2)                    fprintf(stderr,FORM,x1,x2)
    #define USB_SPRINTF3(FORM,x1,x2,x3)                 fprintf(stderr,FORM,x1,x2,x3)
    #define USB_SPRINTF4(FORM,x1,x2,x3,x4)              fprintf(stderr,FORM,x1,x2,x3,x4)
    #define USB_SPRINTF5(FORM,x1,x2,x3,x4,x5)           fprintf(stderr,FORM,x1,x2,x3,x4,x5)
    #define USB_SPRINTF6(FORM,x1,x2,x3,x4,x5,x6)        fprintf(stderr,FORM,x1,x2,x3,x4,x5,x6)
    #define USB_SPRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)     fprintf(stderr,FORM,x1,x2,x3,x4,x5,x6,x7)
    #define USB_SPRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)  fprintf(stderr,FORM,x1,x2,x3,x4,x5,x6,x7,x8)
#else   /* USB_DEBUGUART_PP */
    #define USB_SPRINTF0(FORM)
    #define USB_SPRINTF1(FORM,x1)
    #define USB_SPRINTF2(FORM,x1,x2)
    #define USB_SPRINTF3(FORM,x1,x2,x3)
    #define USB_SPRINTF4(FORM,x1,x2,x3,x4)
    #define USB_SPRINTF5(FORM,x1,x2,x3,x4,x5)
    #define USB_SPRINTF6(FORM,x1,x2,x3,x4,x5,x6)
    #define USB_SPRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)
    #define USB_SPRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)
#endif  /* USB_DEBUGUART_PP */

/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    #include    <stdlib.h>          /* @@@MISRA del */
    #include    <stdio.h>           /* @@@MISRA del */
    #define USB_PRINTF0(FORM)                           printf(FORM)
    #define USB_PRINTF1(FORM,x1)                        printf(FORM,x1)
    #define USB_PRINTF2(FORM,x1,x2)                     printf(FORM,x1,x2)
    #define USB_PRINTF3(FORM,x1,x2,x3)                  printf(FORM,x1,x2,x3)
    #define USB_PRINTF4(FORM,x1,x2,x3,x4)               printf(FORM,x1,x2,x3,x4)
    #define USB_PRINTF5(FORM,x1,x2,x3,x4,x5)            printf(FORM,x1,x2,x3,x4,x5)
    #define USB_PRINTF6(FORM,x1,x2,x3,x4,x5,x6)         printf(FORM,x1,x2,x3,x4,x5,x6)
    #define USB_PRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)      printf(FORM,x1,x2,x3,x4,x5,x6,x7)
    #define USB_PRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)   printf(FORM,x1,x2,x3,x4,x5,x6,x7,x8)
#else   /* USB_DEBUGPRINT_PP */
    #define USB_PRINTF0(FORM)
    #define USB_PRINTF1(FORM,x1)
    #define USB_PRINTF2(FORM,x1,x2)
    #define USB_PRINTF3(FORM,x1,x2,x3)
    #define USB_PRINTF4(FORM,x1,x2,x3,x4)
    #define USB_PRINTF5(FORM,x1,x2,x3,x4,x5)
    #define USB_PRINTF6(FORM,x1,x2,x3,x4,x5,x6)
    #define USB_PRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)
    #define USB_PRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)
#endif  /* USB_DEBUGPRINT_PP */


#endif  /* __R_USB_CMACPRINT_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
