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
* File Name    : r_flash_api_rx630.h
* Description  : This file has specific information about the ROM and DF on 
*                RX630 Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.09.2012 1.00    First Release
*         : 10.10.2012 1.10    Added FCU_RAM_INIT_REQUIRED macro.
******************************************************************************/

#ifndef _FLASH_API_RX630_H
#define _FLASH_API_RX630_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>

/******************************************************************************
Macro definitions
******************************************************************************/
/* Defines the number of flash areas */
#define NUM_ROM_AREAS           4
/* Defines the start program/erase address for the different flash areas */
#define ROM_AREA_0              (0x00F80000)
#define ROM_AREA_1              (0x00F00000)
#define ROM_AREA_2              (0x00E80000)
#define ROM_AREA_3              (0x00E00000)    
   
/* Defines whether this MCU requires the FCU RAM be enabled and initialized.
   If uncommented, then MCU is required to init FCU RAM.
   If commented out, then MCU is not required. */
#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFF800
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00000020
/* This is used to get the boundary of the 'fake' blocks that are 2KB. */
#define DF_BLOCK_SIZE_LARGE     0x00000800
/* Defines how many DF blocks are on this part */
#define DF_NUM_BLOCKS           16                

/* Defines how many ROM blocks are on this part */
#if BSP_ROM_SIZE_BYTES == 2097152
    #define ROM_NUM_BLOCKS      70    //2MB part
#elif BSP_ROM_SIZE_BYTES == 1572864
    #define ROM_NUM_BLOCKS      62    //1.5MB part
#elif BSP_ROM_SIZE_BYTES == 1048576
    #define ROM_NUM_BLOCKS      54    //1MB part
#elif BSP_ROM_SIZE_BYTES == 786432
    #define ROM_NUM_BLOCKS      46    //768KB part
#elif BSP_ROM_SIZE_BYTES == 524288
    #define ROM_NUM_BLOCKS      38    //512KB part
#elif BSP_ROM_SIZE_BYTES == 393216
    #define ROM_NUM_BLOCKS      30    //384KB part
#endif

/* Programming size for ROM in bytes */
#define ROM_PROGRAM_SIZE        128
/* Programming size for data flash in bytes */
/* NOTE: RX630/631/63N only programs in 2-byte intervals */
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
#define BLOCK_38    38      /*   32KB: 0xFFF78000 - 0xFFF7FFFF */
#define BLOCK_39    39      /*   32KB: 0xFFF70000 - 0xFFF77FFF */
#define BLOCK_40    40      /*   32KB: 0xFFF68000 - 0xFFF6FFFF */
#define BLOCK_41    41      /*   32KB: 0xFFF60000 - 0xFFF67FFF */    
#define BLOCK_42    42      /*   32KB: 0xFFF58000 - 0xFFF5FFFF */
#define BLOCK_43    43      /*   32KB: 0xFFF50000 - 0xFFF57FFF */    
#define BLOCK_44    44      /*   32KB: 0xFFF48000 - 0xFFF4FFFF */
#define BLOCK_45    45      /*   32KB: 0xFFF40000 - 0xFFF47FFF */        
#define BLOCK_46    46      /*   32KB: 0xFFF38000 - 0xFFF3FFFF */
#define BLOCK_47    47      /*   32KB: 0xFFF30000 - 0xFFF37FFF */        
#define BLOCK_48    48      /*   32KB: 0xFFF28000 - 0xFFF2FFFF */
#define BLOCK_49    49      /*   32KB: 0xFFF20000 - 0xFFF27FFF */        
#define BLOCK_50    50      /*   32KB: 0xFFF18000 - 0xFFF1FFFF */
#define BLOCK_51    51      /*   32KB: 0xFFF10000 - 0xFFF17FFF */
#define BLOCK_52    52      /*   32KB: 0xFFF08000 - 0xFFF0FFFF */
#define BLOCK_53    53      /*   32KB: 0xFFF00000 - 0xFFF07FFF */                
#define BLOCK_54    54      /*   64KB: 0xFFEF0000 - 0xFFEFFFFF */
#define BLOCK_55    55      /*   64KB: 0xFFEE0000 - 0xFFEEFFFF */
#define BLOCK_56    56      /*   64KB: 0xFFED0000 - 0xFFEDFFFF */
#define BLOCK_57    57      /*   64KB: 0xFFEC0000 - 0xFFECFFFF */
#define BLOCK_58    58      /*   64KB: 0xFFEB0000 - 0xFFEBFFFF */
#define BLOCK_59    59      /*   64KB: 0xFFEA0000 - 0xFFEAFFFF */
#define BLOCK_60    60      /*   64KB: 0xFFE90000 - 0xFFE9FFFF */
#define BLOCK_61    61      /*   64KB: 0xFFE80000 - 0xFFE8FFFF */
#define BLOCK_62    62      /*   64KB: 0xFFE70000 - 0xFFE7FFFF */
#define BLOCK_63    63      /*   64KB: 0xFFE60000 - 0xFFE6FFFF */
#define BLOCK_64    64      /*   64KB: 0xFFE50000 - 0xFFE5FFFF */
#define BLOCK_65    65      /*   64KB: 0xFFE40000 - 0xFFE4FFFF */
#define BLOCK_66    66      /*   64KB: 0xFFE30000 - 0xFFE3FFFF */
#define BLOCK_67    67      /*   64KB: 0xFFE20000 - 0xFFE2FFFF */
#define BLOCK_68    68      /*   64KB: 0xFFE10000 - 0xFFE1FFFF */
#define BLOCK_69    69      /*   64KB: 0xFFE00000 - 0xFFE0FFFF */

/* Data Flash Block Area         Size: Start Addr -   End Addr */
#define BLOCK_DB0    70     /*    2KB: 0x00100000 - 0x001007FF */
#define BLOCK_DB1    71     /*    2KB: 0x00100800 - 0x00100FFF */
#define BLOCK_DB2    72     /*    2KB: 0x00101000 - 0x001017FF */
#define BLOCK_DB3    73     /*    2KB: 0x00101800 - 0x00101FFF */
#define BLOCK_DB4    74     /*    2KB: 0x00102000 - 0x001027FF */
#define BLOCK_DB5    75     /*    2KB: 0x00102800 - 0x00102FFF */
#define BLOCK_DB6    76     /*    2KB: 0x00103000 - 0x001037FF */
#define BLOCK_DB7    77     /*    2KB: 0x00103800 - 0x00103FFF */
#define BLOCK_DB8    78     /*    2KB: 0x00104000 - 0x001047FF */
#define BLOCK_DB9    79     /*    2KB: 0x00104800 - 0x00104FFF */
#define BLOCK_DB10   80     /*    2KB: 0x00105000 - 0x001057FF */
#define BLOCK_DB11   81     /*    2KB: 0x00105800 - 0x00105FFF */
#define BLOCK_DB12   82     /*    2KB: 0x00106000 - 0x001067FF */
#define BLOCK_DB13   83     /*    2KB: 0x00106800 - 0x00106FFF */
#define BLOCK_DB14   84     /*    2KB: 0x00107000 - 0x001077FF */
#define BLOCK_DB15   85     /*    2KB: 0x00107800 - 0x00107FFF */

/* Array of flash addresses used for writing */
#if defined(FLASH_BLOCKS_DECLARE)
const uint32_t g_flash_BlockAddresses[86] = { 
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
        0x00F78000,  /* EB38 */ 
        0x00F70000,  /* EB39 */ 
        0x00F68000,  /* EB40 */ 
        0x00F60000,  /* EB41 */ 
        0x00F58000,  /* EB42 */ 
        0x00F50000,  /* EB43 */ 
        0x00F48000,  /* EB44 */ 
        0x00F40000,  /* EB45 */ 
        0x00F38000,  /* EB46 */ 
        0x00F30000,  /* EB47 */ 
        0x00F28000,  /* EB48 */ 
        0x00F20000,  /* EB49 */ 
        0x00F18000,  /* EB50 */ 
        0x00F10000,  /* EB51 */ 
        0x00F08000,  /* EB52 */ 
        0x00F00000,  /* EB53 */ 
        0x00EF0000,  /* EB54 */ 
        0x00EE0000,  /* EB55 */ 
        0x00ED0000,  /* EB56 */ 
        0x00EC0000,  /* EB57 */ 
        0x00EB0000,  /* EB58 */ 
        0x00EA0000,  /* EB59 */ 
        0x00E90000,  /* EB60 */ 
        0x00E80000,  /* EB61 */ 
        0x00E70000,  /* EB62 */ 
        0x00E60000,  /* EB63 */ 
        0x00E50000,  /* EB64 */ 
        0x00E40000,  /* EB65 */ 
        0x00E30000,  /* EB66 */ 
        0x00E20000,  /* EB67 */ 
        0x00E10000,  /* EB68 */ 
        0x00E00000,  /* EB69 */ 
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
extern const uint32_t g_flash_BlockAddresses[86];
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

/*  According to HW Manual the Max Erasure Time for a 64kB block is
    around 1152ms.  This is with a FCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 1152ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(1152000 * (50.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))    

/******************************************************************************
Error checking
******************************************************************************/
/* FCLK must be between 4MHz and 50MHz. */
#if (FLASH_CLOCK_HZ > 50000000) || (FLASH_CLOCK_HZ < 4000000)
    #error "ERROR - Flash API - FCLK on RX630 must be between 4MHz and 50MHz!"
#endif

#endif /* _FLASH_API_RX630_H */
