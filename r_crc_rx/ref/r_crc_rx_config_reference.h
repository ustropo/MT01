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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_crc_rx_config.c
* Description  : Configures the CRC code.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 28.02.2012 1.00    First Release            
*         : 10.05.2012 1.10    Updated to be compliant with FIT Module Spec v0.7
*         : 13.02.2013 1.20    Updated to be compliant with FIT Module Spec v1.02
***********************************************************************************************************************/
#ifndef CRC_CONFIG_HEADER_FILE
#define CRC_CONFIG_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Determines which polynomial is used for CRC calculation. Only choose one option below.
*/
/* X^8 + X^2 + X + 1 - CRC-8-CCITT */
//#define CRC_CFG_POLY_X8_X2_X_1          (1)
/* X^16 + X^15 + X^2 + 1 - CRC-16, CRC-16-IBM, CRC-16-ANSI */
//#define CRC_CFG_POLY_X16_X15_X2_1       (1)
/* X^16 + X^12 + X^5 + 1 - CRC-16-CCITT */
#define CRC_CFG_POLY_X16_X12_X5_1       (1)

/* These macro determine whether MSB-first or LSB-first communication is being used. Only choose one of these options.
*/
//#define CRC_CFG_LSB_FIRST               (1)
#define CRC_CFG_MSB_FIRST               (1)

#endif /* CRC_CONFIG_HEADER_FILE */


