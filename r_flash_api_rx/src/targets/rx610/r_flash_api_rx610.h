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
* File Name    : r_flash_api_rx610.h
* Description  : This file has specific information about the ROM and DF on 
*                RX610 Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.09.2012 1.00    First Release
*         : 10.10.2012 1.10    Added FCU_RAM_INIT_REQUIRED macro.
******************************************************************************/

#ifndef _FLASH_API_RX610_H
#define _FLASH_API_RX610_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>

/******************************************************************************
Macro definitions
******************************************************************************/
/* Defines the number of flash areas */
#define NUM_ROM_AREAS           2    
/* Defines the start program/erase address for the different flash areas */
#define ROM_AREA_0              (0x00F00000)
#define ROM_AREA_1              (0x00E00000)

/* Defines whether this MCU requires the FCU RAM be enabled and initialized.
   If uncommented, then MCU is required to init FCU RAM.
   If commented out, then MCU is not required. */
#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFE000
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00002000
/* Defines how many DF blocks are on this part */
#define DF_NUM_BLOCKS           4

/* Defines how many ROM blocks are on this part */
#if BSP_ROM_SIZE_BYTES == 2097152
    #define ROM_NUM_BLOCKS      28    //2MB part
#elif BSP_ROM_SIZE_BYTES == 1572864
    #define ROM_NUM_BLOCKS      24    //1.5MB part
#elif BSP_ROM_SIZE_BYTES == 1048576
    #define ROM_NUM_BLOCKS      20    //1MB part
#elif BSP_ROM_SIZE_BYTES == 786432
    #define ROM_NUM_BLOCKS      18    //768KB part
#endif

/* Programming size for ROM in bytes */
#define ROM_PROGRAM_SIZE        256
/* Programming sizes for data flash in bytes. Some MCUs have two sizes
   (e.g. 8-bytes or 128-bytes) that's why there is a LARGE and SMALL */
#define DF_PROGRAM_SIZE_LARGE   128
#define DF_PROGRAM_SIZE_SMALL   8

/* User ROM Block Area           Size: Start Addr -   End Addr */
#define BLOCK_0     0       /*    8KB: 0xFFFFE000 - 0xFFFFFFFF */
#define BLOCK_1     1       /*    8KB: 0xFFFFC000 - 0xFFFFDFFF */
#define BLOCK_2     2       /*    8KB: 0xFFFFA000 - 0xFFFFBFFF */
#define BLOCK_3     3       /*    8KB: 0xFFFF8000 - 0xFFFF9FFF */
#define BLOCK_4     4       /*    8KB: 0xFFFF6000 - 0xFFFF7FFF */
#define BLOCK_5     5       /*    8KB: 0xFFFF4000 - 0xFFFF5FFF */
#define BLOCK_6     6       /*    8KB: 0xFFFF2000 - 0xFFFF3FFF */
#define BLOCK_7     7       /*    8KB: 0xFFFF0000 - 0xFFFF1FFF */
#define BLOCK_8     8       /*   64KB: 0xFFFE0000 - 0xFFFEFFFF */
#define BLOCK_9     9       /*   64KB: 0xFFFD0000 - 0xFFFDFFFF */
#define BLOCK_10    10      /*   64KB: 0xFFFC0000 - 0xFFFCFFFF */
#define BLOCK_11    11      /*   64KB: 0xFFFB0000 - 0xFFFBFFFF */
#define BLOCK_12    12      /*   64KB: 0xFFFA0000 - 0xFFFAFFFF */
#define BLOCK_13    13      /*   64KB: 0xFFF90000 - 0xFFF9FFFF */
#define BLOCK_14    14      /*   64KB: 0xFFF80000 - 0xFFF8FFFF */
#define BLOCK_15    15      /*   64KB: 0xFFF70000 - 0xFFF7FFFF */
#define BLOCK_16    16      /*   64KB: 0xFFF60000 - 0xFFF6FFFF */
#define BLOCK_17    17      /*  128KB: 0xFFF40000 - 0xFFF5FFFF */
#define BLOCK_18    18      /*  128KB: 0xFFF20000 - 0xFFF3FFFF */
#define BLOCK_19    19      /*  128KB: 0xFFF00000 - 0xFFF1FFFF */
#define BLOCK_20    20      /*  128KB: 0xFFEE0000 - 0xFFEFFFFF */
#define BLOCK_21    21      /*  128KB: 0xFFEC0000 - 0xFFEDFFFF */
#define BLOCK_22    22      /*  128KB: 0xFFEA0000 - 0xFFEBFFFF */
#define BLOCK_23    23      /*  128KB: 0xFFE80000 - 0xFFE9FFFF */
#define BLOCK_24    24      /*  128KB: 0xFFE60000 - 0xFFE7FFFF */
#define BLOCK_25    25      /*  128KB: 0xFFE40000 - 0xFFE5FFFF */
#define BLOCK_26    26      /*  128KB: 0xFFE20000 - 0xFFE3FFFF */
#define BLOCK_27    27      /*  128KB: 0xFFE00000 - 0xFFE1FFFF */

/* Data Flash Block Area         Size: Start Addr -   End Addr */
#define BLOCK_DB0   28      /*    8KB: 0x00100000 - 0x00101FFF */
#define BLOCK_DB1   29      /*    8KB: 0x00102000 - 0x00103FFF */
#define BLOCK_DB2   30      /*    8KB: 0x00104000 - 0x00105FFF */
#define BLOCK_DB3   31      /*    8KB: 0x00106000 - 0x00107FFF */

/* Array of flash addresses used for writing */
#if defined(FLASH_BLOCKS_DECLARE)            
const uint32_t g_flash_BlockAddresses[32] = { 
        /* Caution. ID CODE(FFFFFFA0-FFFFFFAF) is excluded. */ 
        0x00FFE000,  /* EB00 */ 
        0x00FFC000,  /* EB01 */ 
        0x00FFA000,  /* EB02 */ 
        0x00FF8000,  /* EB03 */ 
        0x00FF6000,  /* EB04 */ 
        0x00FF4000,  /* EB05 */ 
        0x00FF2000,  /* EB06 */ 
        0x00FF0000,  /* EB07 */ 
        0x00FE0000,  /* EB08 */ 
        0x00FD0000,  /* EB09 */ 
        0x00FC0000,  /* EB10 */ 
        0x00FB0000,  /* EB11 */ 
        0x00FA0000,  /* EB12 */ 
        0x00F90000,  /* EB13 */ 
        0x00F80000,  /* EB14 */ 
        0x00F70000,  /* EB15 */ 
        0x00F60000,  /* EB16 */ 
        0x00F40000,  /* EB17 */ 
        0x00F20000,  /* EB18 */ 
        0x00F00000,  /* EB19 */ 
        0x00EE0000,  /* EB20 */ 
        0x00EC0000,  /* EB21 */ 
        0x00EA0000,  /* EB22 */ 
        0x00E80000,  /* EB23 */ 
        0x00E60000,  /* EB24 */ 
        0x00E40000,  /* EB25 */ 
        0x00E20000,  /* EB26 */ 
        0x00E00000,  /* EB27 */ 
        0x00100000,  /* DB00 */ 
        0x00102000,  /* DB01 */ 
        0x00104000,  /* DB02 */ 
        0x00106000}; /* DB03 */     
#else 
extern const uint32_t g_flash_BlockAddresses[32];
#endif                                

/* Define the clock frequency supplied to the FCU. On the RX610 and Rx62x
   this is the PCLK. On the RX63x it is the FCLK. */
#define FLASH_CLOCK_HZ BSP_PCLK_HZ  

/*  According to HW Manual the Max Programming Time for 256 bytes (ROM)
    is 12ms.  This is with a PCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 12ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(12000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 128 bytes
    (Data Flash) is 5ms.  This is with a PCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 5ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(5000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 2k bytes
    (Data Flash) is 0.7ms.  This is with a PCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 0.7ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(700 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))
        
/*  According to HW Manual the max timeout value when using the peripheral
    clock notification command is 60us. This is with a PCLK of 50MHz. The 
    calculation below calculates the number of ICLK ticks needed for the 
    timeout delay. The 10us number is adjusted linearly depending on 
    the PCLK frequency.
*/
#define WAIT_MAX_NOTIFY_FCU_CLOCK \
        ((int32_t)(60 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))         

/*  According to HW Manual the Max Erasure Time for a 128kB block
    is 1750ms.  This is with a PCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 1750ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(1750000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))    

/******************************************************************************
Error checking
******************************************************************************/
/* PCLK must be between 8MHz and 50MHz. */
#if (FLASH_CLOCK_HZ > 50000000) || (FLASH_CLOCK_HZ < 8000000)
    #error "ERROR - Flash API - PCLK on RX610 must be between 8MHz and 50MHz!"
#endif

#endif /* _FLASH_API_RX610_H */
