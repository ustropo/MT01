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
* File Name	   : r_spi_flash_if.h
* Description  : This module implements the protocol used by many SPI flashes. This code does not support some of the 
*                more advanced commands that are specific to some SPI flash chips.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 29.02.2012 1.00    First Release            
*         : 20.04.2012 1.10    Added support for Numonyx M25P16 SPI flash.
*         : 10.05.2012 1.20    Updated to be compliant with FIT Module Spec v0.7
*         : 13.02.2013 1.30    Updated to be compliant with FIT Module Spec v1.02
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>
/* Configuration for this package. */
#include "r_spi_flash_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Version Number of API. */
#define SPI_FLASH_VERSION_MAJOR           (1)
#define SPI_FLASH_VERSION_MINOR           (30)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/
bool    R_SF_Erase(uint8_t channel, const uint32_t address, const sf_erase_sizes_t size);
bool    R_SF_ReadData(uint8_t channel, const uint32_t address, uint8_t * data, const uint32_t size);
bool    R_SF_ReadID(uint8_t channel, uint8_t * data, uint32_t size);
bool    R_SF_WriteData (uint8_t channel, uint32_t address, uint8_t * data, uint32_t size);
uint8_t R_SF_ReadStatus(uint8_t channel);
void R_SF_Init(uint8_t channel);

