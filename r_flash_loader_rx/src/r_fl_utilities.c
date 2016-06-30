/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer *
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_fl_utilities.c
* Version      : 3.00
* Description  : Contains functions for FlashLoader use such as CRC and Reset
******************************************************************************/  
/******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.04.2010 1.00    First Release
*         : 22.03.2011 2.00    First Release for YRDK
*         : 21.09.2011 2.01    Fixed calculation for timer register in 
*                              fl_start_timer.
*         : 23.02.2012 3.00    Removed 'LOWEST_ROM_ADDRESS' macro. Instead 
*                              getting this info from Flash API. Made code
*                              compliant with CS v4.0. Removed code to start
*                              state machine timer because it is now the users
*                              responsibility to call the state machine. Added
*                              R_FL_GetVersion() function to this file.
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Used for offsetof() */
#include <stddef.h>
/* Flash Loader project includes. */
#include "r_fl_includes.h"
/* Used to get ROM_SIZE_BYTES which is in mcu_info.h and for LCD API. */
#include "platform.h"
/* Uses r_crc_rx package for CRC calculations. */
#include "r_crc_rx_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/*  Bottom of User Flash Area */
#define ROM_START_ADDRESS     (0x100000000-BSP_ROM_SIZE_BYTES)

/******************************************************************************
* Function Name: fl_check_application
* Description  : Does a CRC on MCU flash to make sure current image is valid
* Arguments    : none
* Return value : CRC16-CCITT value of image in MCU flash
******************************************************************************/
uint16_t fl_check_application(void)
{
    uint32_t start_address;
    uint16_t calc_crc;
    
    /* Get lowest flash address. ROM_PE_ADDR is the lowest address with the 
       MSB set to 0x00. To get the read address just make the MSB 0xFF. */
    start_address = ROM_START_ADDRESS;
    
    /* Calculate CRC up to the location where the linker put the CRC value */
    R_CRC_Compute( RX_LINKER_SEED, 
                   (uint8_t *) start_address,
                   ((uint32_t)__sectop("APPHEADER_1")) + \
                   offsetof(fl_image_header_t, raw_crc) - \
                   start_address,
                   &calc_crc);

    /* Move start_address to right after 'raw_crc' */                              
    start_address = ((uint32_t)__sectop("APPHEADER_1")) + \
                    offsetof(fl_image_header_t, raw_crc) + \
                    sizeof(((fl_image_header_t *) 0)->raw_crc);                              

    /* Calculate the rest of flash after the CRC in memory */
    R_CRC_Compute( calc_crc, 
                   (uint8_t *) start_address,
                   (0xFFFFFFFF - start_address) + 1,
                   &calc_crc);   

    /* The RX linker does a bitwise NOT on the data after the 
       CRC has finished */
    calc_crc = (uint16_t)(~calc_crc);
    
    return calc_crc;                                      
}
/******************************************************************************
End of function fl_check_application
******************************************************************************/

/******************************************************************************
* Function Name: fl_reset
* Description  : Performs internal reset on the MCU. WDT is used in examples
*                below.
* Arguments    : none
* Return value : none
******************************************************************************/
void fl_reset(void)
{   
#if defined(BSP_MCU_RX61_ALL) || defined(BSP_MCU_RX62_ALL)

    /* Setup WDT so that it will cause internal reset */
    /* Count using PCLK/4, use in Watchdog timer mode */  
    /* Write to TCSR through WINA register */
    WDT.WRITE.WINA = 0xA540;                  
    
    /* Cause reset when watchdog overflows */
    /* Write to RSTE bit through WINB register */
    WDT.WRITE.WINB = 0x5A5F;    
    
    /* Start WDT */
    /* Write to TCSR through WINA register */
    WDT.WRITE.WINA = 0xA560;                      

#elif defined(BSP_MCU_RX21_ALL) || defined(BSP_MCU_RX63_ALL)

    /* Setup WDT so that it will reset MCU as quickly as possible. */
    /* Reset on expiration of WDT. */
    WDT.WDTRCR.BYTE = 0x80;

    /* Choose PCLK/4 and 1,024 cycles. */
    WDT.WDTCR.WORD = 0x0010;

    /* Start WDT counting. This is done by writing 0x00 and then 0xFF. */
    WDT.WDTRR = 0x00;
    WDT.WDTRR = 0xFF;

#else
    #error "Error! Add code to fl_reset() in r_fl_utilities.c for your MCU."
#endif
                     
    while(1) 
    {
        /* Wait for WDT reset */
    }
}
/******************************************************************************
End of function fl_reset
******************************************************************************/

/******************************************************************************
* Function Name: fl_signal
* Description  : Signal to outside world that no valid image is in MCU, only
*                the FL Downloader state machine is running.
* Arguments    : none
* Return value : none
******************************************************************************/
void fl_signal(void)
{
#if defined(BSP_BOARD_RSKRX62N) || defined(BSP_BOARD_RSKRX63N) || \
    defined(BSP_BOARD_RSKRX630) || defined(BSP_BOARD_RSKRX62T) || \
    defined(BSP_BOARD_RSKRX210)   
    LED0 = LED_ON;
    LED1 = LED_ON;
    LED2 = LED_ON;
    LED3 = LED_ON;
#elif defined(BSP_BOARD_RDKRX62N) || defined(BSP_BOARD_RDKRX63N)
    /* The Glyph lib used for the LCD on the RDK will not fit in the 16KB
       User Boot area so we are just turning on LEDs. */
    LED4 = LED_ON;
    LED5 = LED_ON;
    LED6 = LED_ON;
    LED7 = LED_ON;
#else

#endif    
}
/******************************************************************************
End of function fl_signal
******************************************************************************/

/******************************************************************************
* Function Name: fl_check_bootloader_bypass
* Description  : Checks to see if the user is requesting that the Bootloader
*                bypass normal load image and application checking and wants
*                to just wait for a new load image.
* Arguments    : none
* Return value : true - 
*                    Bypass is requested
*                false - 
*                    Bypass is not requested
******************************************************************************/
bool fl_check_bootloader_bypass(void)
{
        /* Bypass is not requested */
        return false;
}
/******************************************************************************
End of function fl_check_bootloader_bypass
******************************************************************************/

/******************************************************************************
* Function Name: R_FL_GetVersion
* Description  : Returns the current version of this module. The version number
*                is encoded where the top 2 bytes are the major version number 
*                and the bottom 2 bytes are the minor version number. For 
*                example, Version 4.25 would be returned as 0x00040019.
* Arguments    : none
* Return Value : Version of this module.
******************************************************************************/
#pragma inline(R_FL_GetVersion)
uint32_t R_FL_GetVersion (void)
{
    /* These version macros are defined in r_flash_loader_rx_if.h. */
    return ((((uint32_t)FLASH_LOADER_RX_VERSION_MAJOR) << 16) | (uint32_t)FLASH_LOADER_RX_VERSION_MINOR);
}
/******************************************************************************
End of function R_FL_GetVersion
******************************************************************************/


