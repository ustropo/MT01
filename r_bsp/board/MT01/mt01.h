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
* File Name    : yrdkrx63n.h
* H/W Platform : YRDKRX63N
* Description  : Board specific definitions for the RDKRX63N.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 30.11.2011 1.00     First Release
***********************************************************************************************************************/

#ifndef MT01_H
#define MT01_H


/* Local defines */
#define LED_ON              (0)
#define LED_OFF             (1)

#define KLINE				PORT4.PIDR.BYTE

#define KCOL				PORTD.PDR.BYTE
#define KC0					(0x01uL)
#define KC1					(0x02uL)
#define KC2					(0x04uL)
#define LCD_CS              PORTC.PODR.BIT.B2
#define SPIFLASH_CS 		PORTE.PODR.BIT.B0

#define WDT_FEED       IWDT.IWDTRR = 0x00u; \
    				   IWDT.IWDTRR = 0xFFu;

#define RESET			R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR); \
						SYSTEM.SWRR = 0xA501;

#endif /* MT01_H */

