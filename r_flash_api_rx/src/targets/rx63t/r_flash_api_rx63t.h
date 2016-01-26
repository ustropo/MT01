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
* File Name    : r_flash_api_rx63t.h
* Description  : This file has specific information about the ROM and DF on 
*                RX63T Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 10.10.2012 1.00    First Release
******************************************************************************/

#ifndef _FLASH_API_RX63T_H
#define _FLASH_API_RX63T_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>

/******************************************************************************
Macro definitions
******************************************************************************/
/* Defines the number of flash areas */
#define NUM_ROM_AREAS           1
/* Defines the start program/erase address for the different flash areas */
#define ROM_AREA_0              (0x00F80000)
   
/* Defines whether this MCU requires the FCU RAM be enabled and initialized.
   If uncommented, then MCU is required to init FCU RAM.
   If commented out, then MCU is not required. */
//#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFF800
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00000020
/* This is used to get the boundary of the 'fake' blocks that are 2KB. */
#define DF_BLOCK_SIZE_LARGE     0x00000800

/* Defines how many DF blocks are on this part */
#if BSP_DATA_FLASH_SIZE_BYTES == 32768
    #define DF_NUM_BLOCKS       16    //32KB DF part
#else
    #define DF_NUM_BLOCKS       4     //8KB DF part
#endif

/* Defines how many ROM blocks are on this part */
#if   BSP_ROM_SIZE_BYTES == 524288
    #define ROM_NUM_BLOCKS      38    //512KB part
#elif BSP_ROM_SIZE_BYTES == 393216
    #define ROM_NUM_BLOCKS      30    //384KB part
#elif BSP_ROM_SIZE_BYTES == 262144
    #define ROM_NUM_BLOCKS      22    //256KB part
#elif BSP_ROM_SIZE_BYTES == 65536
    #define ROM_NUM_BLOCKS      10    //64KB part
#elif BSP_ROM_SIZE_BYTES == 49152
    #define ROM_NUM_BLOCKS      9     //48KB part
#elif BSP_ROM_SIZE_BYTES == 32768
    #define ROM_NUM_BLOCKS      8     //32KB part
#endif

/* Programming size for ROM in bytes */
#define ROM_PROGRAM_SIZE        128
/* Programming size for data flash in bytes */
/* NOTE: RX63T only programs in 2-byte intervals */
#define DF_PROGRAM_SIZE_SMALL   2

/* NOTE:
   The RX63T actually has 1024 x 32 byte blocks instead of the 
   16 x 2Kbyte blocks shown below. These are grouped into 16 blocks to
   make it easier for the user to delete larger sections of the data 
   flash. The user can still delete individual blocks but they will 
   need to use the new flash erase function that takes addresses 
   instead of blocks. */
/* Defines that we are grouping data flash blocks for this MCU. */
#define DF_GROUPED_BLOCKS       (1)

/* User ROM Block Area           Size: Start Addr -   End Addr */
#define BLOCK_0     0       /*    4KB: 0xFFFFF000 - 0xFFFFFFFF */
#define BLOCK_1     1       /*    4KB: 0xFFFFE000 - 0xFFFFEFFF */
#define BLOCK_2     2       /*    4KB: 0xFFFFD000 - 0xFFFFDFFF */
#define BLOCK_3     3       /*    4KB: 0xFFFFC000 - 0xFFFFCFFF */
#define BLOCK_4     4       /*    4KB: 0xFFFFB000 - 0xFFFFBFFF */
#define BLOCK_5     5       /*    4KB: 0xFFFFA000 - 0xFFFFAFFF */
#define BLOCK_6     6       /*    4KB: 0xFFFF9000 - 0xFFFF9FFF */
#define BLOCK_7     7       /*    4KB: 0xFFFF8000 - 0xFFFF8FFF */
#define BLOCK_8     8       /*   16KB: 0xFFFF4000 - 0xFFFF7FFF */
#define BLOCK_9     9       /*   16KB: 0xFFFF0000 - 0xFFFF3FFF */
#define BLOCK_10    10      /*   16KB: 0xFFFEC000 - 0xFFFEFFFF */
#define BLOCK_11    11      /*   16KB: 0xFFFE8000 - 0xFFFEBFFF */
#define BLOCK_12    12      /*   16KB: 0xFFFE4000 - 0xFFFE7FFF */
#define BLOCK_13    13      /*   16KB: 0xFFFE0000 - 0xFFFE3FFF */
#define BLOCK_14    14      /*   16KB: 0xFFFDC000 - 0xFFFDFFFF */
#define BLOCK_15    15      /*   16KB: 0xFFFD8000 - 0xFFFDBFFF */
#define BLOCK_16    16      /*   16KB: 0xFFFD4000 - 0xFFFD7FFF */
#define BLOCK_17    17      /*   16KB: 0xFFFD0000 - 0xFFFD3FFF */
#define BLOCK_18    18      /*   16KB: 0xFFFCC000 - 0xFFFCFFFF */
#define BLOCK_19    19      /*   16KB: 0xFFFC8000 - 0xFFFCBFFF */
#define BLOCK_20    20      /*   16KB: 0xFFFC4000 - 0xFFFC7FFF */
#define BLOCK_21    21      /*   16KB: 0xFFFC0000 - 0xFFFC3FFF */
#define BLOCK_22    22      /*   16KB: 0xFFFBC000 - 0xFFFBFFFF */
#define BLOCK_23    23      /*   16KB: 0xFFFB8000 - 0xFFFBBFFF */
#define BLOCK_24    24      /*   16KB: 0xFFFB4000 - 0xFFFB7FFF */
#define BLOCK_25    25      /*   16KB: 0xFFFB0000 - 0xFFFB3FFF */
#define BLOCK_26    26      /*   16KB: 0xFFFAC000 - 0xFFFAFFFF */
#define BLOCK_27    27      /*   16KB: 0xFFFA8000 - 0xFFFABFFF */
#define BLOCK_28    28      /*   16KB: 0xFFFA4000 - 0xFFFA7FFF */
#define BLOCK_29    29      /*   16KB: 0xFFFA0000 - 0xFFFA3FFF */
#define BLOCK_30    30      /*   16KB: 0xFFF9C000 - 0xFFF9FFFF */
#define BLOCK_31    31      /*   16KB: 0xFFF98000 - 0xFFF9BFFF */
#define BLOCK_32    32      /*   16KB: 0xFFF94000 - 0xFFF97FFF */
#define BLOCK_33    33      /*   16KB: 0xFFF90000 - 0xFFF93FFF */
#define BLOCK_34    34      /*   16KB: 0xFFF8C000 - 0xFFF8FFFF */
#define BLOCK_35    35      /*   16KB: 0xFFF88000 - 0xFFF8BFFF */
#define BLOCK_36    36      /*   16KB: 0xFFF84000 - 0xFFF87FFF */
#define BLOCK_37    37      /*   16KB: 0xFFF80000 - 0xFFF83FFF */

/* Data Flash Block Area         Size: Start Addr -   End Addr */
#define BLOCK_DB0    38     /*    2KB: 0x00100000 - 0x001007FF */
#define BLOCK_DB1    39     /*    2KB: 0x00100800 - 0x00100FFF */
#define BLOCK_DB2    40     /*    2KB: 0x00101000 - 0x001017FF */
#define BLOCK_DB3    41     /*    2KB: 0x00101800 - 0x00101FFF */
#define BLOCK_DB4    42     /*    2KB: 0x00102000 - 0x001027FF */
#define BLOCK_DB5    43     /*    2KB: 0x00102800 - 0x00102FFF */
#define BLOCK_DB6    44     /*    2KB: 0x00103000 - 0x001037FF */
#define BLOCK_DB7    45     /*    2KB: 0x00103800 - 0x00103FFF */
#define BLOCK_DB8    46     /*    2KB: 0x00104000 - 0x001047FF */
#define BLOCK_DB9    47     /*    2KB: 0x00104800 - 0x00104FFF */
#define BLOCK_DB10   48     /*    2KB: 0x00105000 - 0x001057FF */
#define BLOCK_DB11   49     /*    2KB: 0x00105800 - 0x00105FFF */
#define BLOCK_DB12   50     /*    2KB: 0x00106000 - 0x001067FF */
#define BLOCK_DB13   51     /*    2KB: 0x00106800 - 0x00106FFF */
#define BLOCK_DB14   52     /*    2KB: 0x00107000 - 0x001077FF */
#define BLOCK_DB15   53     /*    2KB: 0x00107800 - 0x00107FFF */

/* Array of flash addresses used for writing */
#if defined(FLASH_BLOCKS_DECLARE)
const uint32_t g_flash_BlockAddresses[54] = { 
        /* Caution. ID CODE(FFFFFFA0-FFFFFFAF) is excluded. */ 
        0x00FFF000,  /* EB00 */ 
        0x00FFE000,  /* EB01 */ 
        0x00FFD000,  /* EB02 */ 
        0x00FFC000,  /* EB03 */ 
        0x00FFB000,  /* EB04 */ 
        0x00FFA000,  /* EB05 */ 
        0x00FF9000,  /* EB06 */ 
        0x00FF8000,  /* EB07 */ 
        0x00FF4000,  /* EB08 */ 
        0x00FF0000,  /* EB09 */ 
        0x00FEC000,  /* EB10 */ 
        0x00FE8000,  /* EB11 */ 
        0x00FE4000,  /* EB12 */ 
        0x00FE0000,  /* EB13 */ 
        0x00FDC000,  /* EB14 */ 
        0x00FD8000,  /* EB15 */ 
        0x00FD4000,  /* EB16 */ 
        0x00FD0000,  /* EB17 */ 
        0x00FCC000,  /* EB18 */ 
        0x00FC8000,  /* EB19 */ 
        0x00FC4000,  /* EB20 */ 
        0x00FC0000,  /* EB21 */ 
        0x00FBC000,  /* EB22 */ 
        0x00FB8000,  /* EB23 */ 
        0x00FB4000,  /* EB24 */ 
        0x00FB0000,  /* EB25 */ 
        0x00FAC000,  /* EB26 */ 
        0x00FA8000,  /* EB27 */ 
        0x00FA4000,  /* EB28 */ 
        0x00FA0000,  /* EB29 */ 
        0x00F9C000,  /* EB30 */ 
        0x00F98000,  /* EB31 */ 
        0x00F94000,  /* EB32 */ 
        0x00F90000,  /* EB33 */ 
        0x00F8C000,  /* EB34 */ 
        0x00F88000,  /* EB35 */ 
        0x00F84000,  /* EB36 */ 
        0x00F80000,  /* EB37 */ 
        0x00100000,  /* DB00 */ 
        0x00100800,  /* DB01 */ 
        0x00101000,  /* DB02 */ 
        0x00101800,  /* DB03 */ 
        0x00102000,  /* DB04 */ 
        0x00102800,  /* DB05 */ 
        0x00103000,  /* DB06 */ 
        0x00103800,  /* DB07 */ 
        0x00104000,  /* DB08 */ 
        0x00104800,  /* DB09 */ 
        0x00105000,  /* DB10 */ 
        0x00105800,  /* DB11 */ 
        0x00106000,  /* DB12 */ 
        0x00106800,  /* DB13 */ 
        0x00107000,  /* DB14 */ 
        0x00107800}; /* DB15 */      

#else 
extern const uint32_t g_flash_BlockAddresses[54];
#endif

/* Define the clock frequency supplied to the FCU. On the RX610 and Rx62x
   this is the PCLK. On the RX63x it is the FCLK. */
#define FLASH_CLOCK_HZ BSP_FCLK_HZ

/*  According to HW Manual the Max Programming Time for 128 bytes (ROM)
    is 12ms.  This is with a FCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 12ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(12000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 2 bytes
    (Data Flash) is 2ms.  This is with a FCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 5ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(2000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 2k bytes
    (Data Flash) is 0.7ms.  This is with a FCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 0.7ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(700 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))
        
/*  According to HW Manual the max timeout value when using the peripheral
    clock notification command is 60us. This is with a FCLK of 50MHz. The 
    calculation below calculates the number of ICLK ticks needed for the 
    timeout delay. The 10us number is adjusted linearly depending on 
    the FCLK frequency.
*/
#define WAIT_MAX_NOTIFY_FCU_CLOCK \
        ((int32_t)(60 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))         

/*  According to HW Manual the Max Erasure Time for a 16kB block is
    around 288ms.  This is with a FCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 288ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(288000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))    

/******************************************************************************
Error checking
******************************************************************************/
/* FCLK must be between 4MHz and 50MHz. */
#if (FLASH_CLOCK_HZ > 50000000) || (FLASH_CLOCK_HZ < 4000000)
    #error "ERROR - Flash API - FCLK on RX63T must be between 4MHz and 50MHz!"
#endif

#endif /* _FLASH_API_RX63T_H */


