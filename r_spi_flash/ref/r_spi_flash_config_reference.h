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
* File Name    : r_spi_flash_config.c
* Description  : Configures the SPI Flash package. 
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 29.02.2012 1.00    First Release            
*         : 20.04.2012 1.10    Added support for Numonyx M25P16 SPI flash.
*         : 10.05.2012 1.20    Updated to be compliant with FIT Module Spec v0.7
*         : 13.02.2013 1.30    Updated to be compliant with FIT Module Spec v1.02
***********************************************************************************************************************/
#ifndef SPI_FLASH_CONFIG_HEADER_FILE
#define SPI_FLASH_CONFIG_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Which SPI flash is chosen is decided by the header file that is included below. You may only choose 1 chip at a 
   time. If you're developing on a RSK/RDK that already has a SPI flash on it then you will not need to modify this file
   because the appropriate SPI flash is automatically chosen below. If you are using your own board then put a #include
   in the #else ... #endif below for the SPI flash you are using. If your SPI flash is not listed then please copy one 
   of the already defined header files in the 'src/chips' directory, rename it for your chip, modify the macros for the 
   specifics of your chip and add an include path to it here. 
   For quick reference here are the SPI flashes that are used and automatically chosen for Renesas boards.
   Numonyx P5Q PCM - RDKRX62N, RDKRX63N
   SST 25 Series - RSKRX62N, RSKRX63N
   */
#if   defined(BSP_BOARD_RDKRX62N) || defined(BSP_BOARD_RDKRX63N)
/* Numonyx P5Q PCM. */
#include "src/chips/r_spi_flash_p5q.h"
#elif defined(BSP_BOARD_RSKRX62N) || defined(BSP_BOARD_RSKRX63N)
/* SST 25 Series. */
#include "src/chips/r_spi_flash_sst25.h"
#elif defined(BSP_BOARD_RSKRX210)
/* Numonyx M25P16. The RSKRX210 does not come with this SPI flash installed. An expansion board was used which came 
   with a M25P16 installed which is why it is shown here. */
#include "src/chips/r_spi_flash_m25p16.h"
#else
/* If your board or SPI flash is not shown above then you will need to add an include to the header file for your
   SPI flash here. */
#include "src/chips/r_spi_flash_xxx.h"
#endif

#endif /* SPI_FLASH_CONFIG_HEADER_FILE */


