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
* File Name    : r_flash_loader_rx_config.c
* Version      : 3.00
* Description  : Configures the Flash Loader package.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 02.03.2012 3.00    First Release            
***********************************************************************************************************************/
#ifndef FLASH_LOADER_CONFIG_HEADER_FILE
#define FLASH_LOADER_CONFIG_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Maximum block data size supported. sizeof(fl_block_header_t) is added to this #define when declaring the receive
   buffer. This is because the r_fl_mot_converter.py program accepts a parameter to set the data block size. That 
   parameter does not take into account the block header size so neither does this one. */
#define FL_CFG_DATA_BLOCK_MAX_BYTES         (2048)

/* Whether to use a timeout or not. A timeout occurs when FL_CFG_TIMEOUT_TICKS go by without receiving an expected 
   response from the host. If a reply is not expected then a timeout will not occur. Using a timeout helps the state 
   machine not get stuck when the host goes down during communications.
   '0' means do not use a timeout.
   '1' means do use a timeout. */
#define FL_CFG_TIMEOUT_ENABLE               (1)

/* Number of ticks of the state machine before a timeout occurs. The time for each tick will depend on the frequency
   of the timer that is used to call the state machine. For example, if the state machine is called at 50Hz then the 
   time for each tick is 20ms. */
#define FL_CFG_TIMEOUT_TICKS                (100)

/* Number of load file slots available. */
#define FL_CFG_MEM_NUM_LOAD_IMAGES          (2)

/* Starting address of where Flash Loader load images are stored. The address for each load image will be based on this
   address. For example, if FL_CFG_MEM_MAX_LI_SIZE_BYTES is 0x10000 then the addresses would be the following if 
   FL_CFG_MEM_NUM_LOAD_IMAGES was set to 4:
   Address of Load Image 0: 0x00000000
   Address of Load Image 1: 0x00010000
   Address of Load Image 2: 0x00020000
   Address of Load Image 3: 0x00030000
*/
#define FL_CFG_MEM_BASE_ADDR                (0)

/* Maximum supported load image size. If a host sends a request to download a new image that is larger then this, the 
   MCU will deny the request. */
#define FL_CFG_MEM_MAX_LI_SIZE_BYTES        (0x100000)

#endif /* FLASH_LOADER_CONFIG_HEADER_FILE */


