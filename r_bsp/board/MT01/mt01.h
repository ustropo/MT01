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

/* I/0 port 1 */


/* I/0 port 2 */


/* I/O port 3 */


/* I/O port 4: switches and various peripherals  */
#define SW_ACTIVE           (0)
#define SW1                 PORT4.PIDR.BIT.B0   
#define SW1_PMR             PORT4.PMR.BIT.B0
#define SW1_PDR             PORT4.PDR.BIT.B0
#define SW2                 PORT4.PIDR.BIT.B1    
#define SW2_PMR             PORT4.PMR.BIT.B1
#define SW2_PDR             PORT4.PDR.BIT.B1
#define POTENTIOMETER       PORT4.PODR.BIT.B2
#define JN1_AN3             PORT4.PODR.BIT.B3
#define SW3                 PORT4.PIDR.BIT.B4 
#define SW3_PMR             PORT4.PMR.BIT.B4
#define SW3_PDR             PORT4.PDR.BIT.B4
#define MICROPHONE          PORT4.PODR.BIT.B5
#define AUDIO_IN_R          PORT4.PODR.BIT.B6
#define PWMLP_IN            PORT4.PODR.BIT.B7

/* I/O port 5  */
#define KLINE				PORT5.PIDR.BYTE

/* I/O port A:  Extension slot (JN2) and E/N pins */


/* I/O port B:  Ethernet I/O pins definitions */


/* I/O port C:  RSPI and peripheral I/O pins definitions */
#define KCOL				PORTC.PDR.BYTE
#define KC1					(0x05uL)
#define KC2					(0x06uL)
#define KC3					(0x0CuL)
#define KC4					(0x14uL)
#define LCD_CS              PORTC.PODR.BIT.B2        /* SSL 3 */

/* I/O ports D & E:  LEDs */
#define LED4                PORTD.PODR.BIT.B5
#define LED5                PORTE.PODR.BIT.B3
#define LED6                PORTD.PODR.BIT.B2     
#define LED7                PORTE.PODR.BIT.B0
#define LED8                PORTD.PODR.BIT.B4
#define LED9                PORTE.PODR.BIT.B2
#define LED10               PORTD.PODR.BIT.B1
#define LED11               PORTD.PODR.BIT.B7
#define LED12               PORTD.PODR.BIT.B3
#define LED13               PORTE.PODR.BIT.B1
#define LED14               PORTD.PODR.BIT.B0
#define LED15               PORTD.PODR.BIT.B6

/* WiFi chip select */
#define WIFI_CS             PORTJ.PODR.BIT.B3        

#endif /* MT01_H */

