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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_lvd_rx.h
* Description  : This module implements the LVD API that can be used to configure the LVD module.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 18.07.2013 1.00    First Release
*         : 14.02.2014 1.10    Added support for RX110, RX113, RX210, RX63N
*         : 22.12.2014 1.30    Added support for RX113.
*         : 18.03.2015 1.40    Added support for RX64M, 71M.
*         : 09.07.2015 1.50    Added support for RX231.
***********************************************************************************************************************/

#ifndef LVD_PRIVATE_HEADER_FILE
#define LVD_PRIVATE_HEADER_FILE

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>

#include "r_lvd_rx_if.h"
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Tests to make sure the user configured the code correctly. */

/***********************************************************************************************************************
 Error checking for user defined equates
***********************************************************************************************************************/
/* LVD_CFG_VDET2_VCC_CMPA2 = 1 (CMPA2 pin input voltage) is only available for RX110, 111, 113, 210 and 231. */
#if (LVD_CFG_VDET2_VCC_CMPA2 != 0)
  #if (! (defined BSP_MCU_RX110 || defined BSP_MCU_RX111 || defined BSP_MCU_RX113 || \
          defined BSP_MCU_RX210 || defined BSP_MCU_RX231))
  #error "LVD_CFG_VDET2_VCC_CMPA2 = 1 is not supported for this MCU. Please set to 0 in r_lvd_rx_config.h."
  #endif
#endif

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

#endif/* LVD_PRIVATE_HEADER_FILE */
