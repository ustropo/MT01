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
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_api_rx_config.h
* Description  : Flash programming for the RX600 & RX200 Series. This file has 
*                options to let the user customize parts of the Flash API.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 21.12.2009 1.00    First Release
*         : 13.01.2010 1.10    Made function names and variables RAPI compliant
*         : 11.02.2010 1.20    Fixed other RAPI issues and fixed I flag issue
*         : 29.04.2010 1.30    Added support for RX621/N Group. Moved most
*                              device specific data to header file.
*         : 26.05.2010 1.40    Added support for RX62T Group
*         : 28.07.2010 1.41    Fixed bug when performing a blank check on an
*                              entire data flash block.  Also declared 
*                              functions not in the API as 'static'.
*         : 23.08.2010 1.42    Updated source to raise the coding standard, to
*                              meet GSCE & RSK standards.
*         : 15.02.2011 1.43    Fixed bug in blank check routine when handling
*                              input arguments and moved _Flash_Init() to
*                              _Enter_PE_Mode() function.
*         : 21.04.2011 2.00    Added BGO capabilities for data flash. Made 
*                              some more changes to fit coding standard. Added
*                              ability to do ROM to ROM or DF to DF transfers.
*                              Added the ability to use the lock bit feature
*                              on ROM blocks.  Added BGO capabilities for
*                              ROM operations.
*         : 06.07.2011 2.10    Added support for RX630, RX631, and RX63N.
*                              Also added R_FlashEraseRange() for devices like
*                              RX63x that have finer granularity data flash.
*                              Various bug fixes as well. Example bug fix was
*                              removing DATA_FLASH_OPERATION_PIPL and 
*                              ROM_OPERATION_PIPL #defines since the IPL was
*                              not restored when leaving flash ready interrupt.
*         : 29.11.2011 2.20    Renamed private functions according to new 
*                              Coding Standard. Removed unused 'bytes' argument 
*                              from enter_pe_mode() function. Removed 'far' 
*                              keyword since it is not needed. Fixed where some 
*                              functions were being placed in RAM when this was
*                              not needed. Uses platform.h now instead of 
*                              having multiple iodefine_rxXXX.h's. Added 
*                              __evenaccess directive to FCU accesses. This 
*                              ensures proper bus width accesses. Added
*                              R_FlashCodeCopy() function. When clearing the
*                              FENTRYR register, the register is read back to
*                              ensure its value is 0x0000. Added call to 
*                              exit_pe_mode() when enter_pe_mode() function 
*                              fails to protect against reading ROM in P/E
*                              mode. Added option to use r_bsp package.
*         : 11.09.2012 2.30    Updated to meet FIT v0.7 spec. Now uses r_bsp
*                              locking. Removed FLASH_API_USE_R_BSP macro. Code 
*                              now automatically detects r_bsp. Moved MCU 
*                              specific information into 'ports' folder instead 
*                              of keep on expanding r_flash_api_rx_if.h.
*                              Added R_FlashGetVersion() function. Added 
*                              timeout for while() loop reading back FENTRYR 
*                              value.
*         : 29.11.2012 2.40    Updated to meet FIT v1.0 spec. Added support
*                              for RX62G and RX63T MCU groups. Cleaned up 
*                              code some by removing almost all preprocessor
*                              statements that tested against MCU groups. Now
*                              have 'targets' directory for easier addition
*                              of new MCUs. Updated to use r_bsp v2.00.
******************************************************************************/

#ifndef _FLASH_API_CONFIG_H
#define _FLASH_API_CONFIG_H

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
 ENABLE ROM PROGRAMMING              
******************************************************************************/
/* When uncommented, code is included to program the User ROM program area of
   Flash. Since this code must be executed from within RAM, the sections
   'PFRAM'(ROM) and 'RPFRAM'(RAM) must be added to the linker settings. Also
   the linker option '-rom=PFRAM=RPFRAM' must be added. Finally, the 
   initialization of the 'RPFRAM' section must be done before calling a Flash 
   API function. This can be done by using the R_FlashCodeCopy() function or
   you can have it done on reset by adding the section to the 'dbsct.c' file as 
   such:
   { __sectop("PFRAM"), __secend("PFRAM"), __sectop("RPFRAM") } 

   If this macro is not defined (commented out) then the user does not need to
   setup and initialize the PFRAM and RPFRAM sections. If the user calls the
   R_FlashCodeCopy() function with this macro commented out then it will just
   return without doing anything. */
#define FLASH_API_RX_CFG_ENABLE_ROM_PROGRAMMING

/******************************************************************************
 ENABLE ROM to ROM or DF to DF programs              
******************************************************************************/
/* If you are doing ROM to ROM writes or DF to DF writes then this DEFINE
   needs to be uncommented. This is necessary because when programming a ROM
   area the MCU cannot read from ROM and when programming a DF area the MCU
   cannot read from the DF. To get by this, if this DEFINE is uncommented 
   then data that is moving from either ROM to ROM or DF to DF is first 
   buffered in a RAM array, then written. If you are never doing one of these
   operations then you can comment out this DEFINE and save yourself 
   the RAM needed to buffer one ROM write (RX62x = 256 bytes, 
   RX630 = 128 bytes). */
//#define FLASH_API_RX_CFG_FLASH_TO_FLASH   

/******************************************************************************
 ENABLE BGO & NON-BLOCKING DATA FLASH OPERATIONS
******************************************************************************/
/* If this is defined then the flash ready interrupt will be used and 
   FlashAPI routines that deal with the data flash will exit after the
   operation has been started instead of polling for it to finish. */
//#define FLASH_API_RX_CFG_DATA_FLASH_BGO

/******************************************************************************
 ENABLE BGO & NON-BLOCKING ROM OPERATIONS
 EXTRA CARE SHOULD BE TAKEN IF YOU ENABLE FLASH_API_RX_CFG_ROM_BGO. SINCE 
 PROGRAM/ERASE FUNCTIONS WILL BE NON-BLOCKING THE API WILL RETURN BEFORE THE 
 ROM OPERATION HAS FINISHED. THIS MEANS THAT THE USER CODE THAT CALLS THE FLASH
 API FUNCTION MUST BE IN RAM OR EXTERNAL MEMORY. THIS WOULD ALSO MEAN THAT THE
 USER HAS TO RELOCATE THE INTERRUPT VECTOR TABLE TO SOMEWHERE OTHER THAN
 ROM OR DATA FLASH. IF THE USER ATTEMPTS TO ACCESS ROM DURING A ROM PROGRAM/
 ERASE OPERATION THE FLASH CONTROL UNIT WILL THROW AN ERROR.
******************************************************************************/
/* If this is defined then the flash ready interrupt will be used and 
   FlashAPI routines that deal with the on-chip ROM will exit after the
   operation has been started instead of polling for it to finish. */
//#define FLASH_API_RX_CFG_ROM_BGO

/******************************************************************************
 SET IPL OF FLASH READY INTERRUPT
******************************************************************************/
/* If using BGO then the Flash Ready Interrupt will need to have an IPL. */
#define FLASH_API_RX_CFG_FLASH_READY_IPL     5

/******************************************************************************
 ENABLE OR DISABLE LOCK BIT PROTECTION              
******************************************************************************/
/* Each erasure block has a corresponding lock bit that can be used to 
   protect that block from being programmed/erased after the lock bit is
   set. The use of lock bits can be used or ignored. If the #define below
   is uncommented then lock bits will be ignored and programs/erases to a
   block will not be limited. This only applies to ROM since the DF does
   not have lock bits. */
#define FLASH_API_RX_CFG_IGNORE_LOCK_BITS

/******************************************************************************
 COPY API CODE TO RAM USING API FUNCTION OR DBSCT.C
******************************************************************************/
/* After a reset parts of the Flash API must be copied to RAM before the API
   can be used. This originally was done by editing the dbsct.c file to
   copy the code over when other RAM sections are initialized. There is now
   the R_FlashCodeCopy() function which does the same thing. Uncomment this 
   macro if you will be using the R_FlashCodeCopy() function. Comment out this
   macro if you are using the original dbsct.c method. */
#define FLASH_API_RX_CFG_COPY_CODE_BY_API

#endif /* _FLASH_API_CONFIG_H */

