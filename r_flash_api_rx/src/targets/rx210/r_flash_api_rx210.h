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
* File Name    : r_flash_api_rx210.h
* Description  : This file has specific information about the ROM and DF on 
*                RX210 Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 15.10.2012 1.00    First Release
******************************************************************************/

#ifndef _FLASH_API_RX210_H
#define _FLASH_API_RX210_H

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
#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFF800
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00000080
/* This is used to get the boundary of the 'fake' blocks that are 2KB. */
#define DF_BLOCK_SIZE_LARGE     0x00000800
/* Defines how many DF blocks are on this part */
#define DF_NUM_BLOCKS           4                

/* Defines how many ROM blocks are on this part */
#if BSP_ROM_SIZE_BYTES == 131072
    #define ROM_NUM_BLOCKS  64    	  //128KB part
#elif BSP_ROM_SIZE_BYTES == 262144
    #define ROM_NUM_BLOCKS  128  	  //256KB part
#elif BSP_ROM_SIZE_BYTES == 393216
    #define ROM_NUM_BLOCKS  192  	  //384KB part
#elif BSP_ROM_SIZE_BYTES == 524288
    #define ROM_NUM_BLOCKS  256  	  //512KB part
#else
    #error "ERROR - Flash API - This ROM size is not defined. Update r_flash_api_rx210.h with this MCUs information." 
#endif

/* Programming size for ROM in bytes. This is being set as 2 to comply with older versions where there was only 1
   size option.  */
#define ROM_PROGRAM_SIZE          2
/* Programming size for ROM in bytes. RX210 support 128, 8 and 2 byte options. */
#define ROM_PROGRAM_SIZE_LARGE    128
#define ROM_PROGRAM_SIZE_MEDIUM   8
#define ROM_PROGRAM_SIZE_SMALL    2
/* Programming size for data flash in bytes */
/* NOTE: RX210 can program 2 or 8 bytes at a time. */
#define DF_PROGRAM_SIZE_LARGE     8
#define DF_PROGRAM_SIZE_SMALL     2

/* NOTE:
   The RX210 actually has 64 x 128 byte blocks instead of the 
   4 x 2Kbyte blocks shown below. These are grouped into blocks to
   make it easier for the user to delete larger sections of the data 
   flash. The user can still delete individual blocks but they will 
   need to use the new flash erase function that takes addresses 
   instead of blocks. */

/* Defines that we are grouping data flash blocks for this MCU. */
#define DF_GROUPED_BLOCKS       (1)

/* User ROM Block Area        Size: Start Addr -   End Addr */
#define BLOCK_0     0     /*   2KB: 0xFFFFF800 - 0xFFFFFFFF */
#define BLOCK_1     1     /*   2KB: 0xFFFFF000 - 0xFFFFF7FF */
#define BLOCK_2     2     /*   2KB: 0xFFFFE800 - 0xFFFFEFFF */
#define BLOCK_3     3     /*   2KB: 0xFFFFE000 - 0xFFFFE7FF */
#define BLOCK_4     4     /*   2KB: 0xFFFFD800 - 0xFFFFDFFF */
#define BLOCK_5     5     /*   2KB: 0xFFFFD000 - 0xFFFFD7FF */
#define BLOCK_6     6     /*   2KB: 0xFFFFC800 - 0xFFFFCFFF */
#define BLOCK_7     7     /*   2KB: 0xFFFFC000 - 0xFFFFC7FF */
#define BLOCK_8     8     /*   2KB: 0xFFFFB800 - 0xFFFFBFFF */
#define BLOCK_9     9     /*   2KB: 0xFFFFB000 - 0xFFFFB7FF */
#define BLOCK_10    10    /*   2KB: 0xFFFFA800 - 0xFFFFAFFF */
#define BLOCK_11    11    /*   2KB: 0xFFFFA000 - 0xFFFFA7FF */
#define BLOCK_12    12    /*   2KB: 0xFFFF9800 - 0xFFFF9FFF */
#define BLOCK_13    13    /*   2KB: 0xFFFF9000 - 0xFFFF97FF */
#define BLOCK_14    14    /*   2KB: 0xFFFF8800 - 0xFFFF8FFF */
#define BLOCK_15    15    /*   2KB: 0xFFFF8000 - 0xFFFF87FF */
#define BLOCK_16    16    /*   2KB: 0xFFFF7800 - 0xFFFF7FFF */
#define BLOCK_17    17    /*   2KB: 0xFFFF7000 - 0xFFFF77FF */
#define BLOCK_18    18    /*   2KB: 0xFFFF6800 - 0xFFFF6FFF */
#define BLOCK_19    19    /*   2KB: 0xFFFF6000 - 0xFFFF67FF */
#define BLOCK_20    20    /*   2KB: 0xFFFF5800 - 0xFFFF5FFF */
#define BLOCK_21    21    /*   2KB: 0xFFFF5000 - 0xFFFF57FF */
#define BLOCK_22    22    /*   2KB: 0xFFFF4800 - 0xFFFF4FFF */
#define BLOCK_23    23    /*   2KB: 0xFFFF4000 - 0xFFFF47FF */
#define BLOCK_24    24    /*   2KB: 0xFFFF3800 - 0xFFFF3FFF */
#define BLOCK_25    25    /*   2KB: 0xFFFF3000 - 0xFFFF37FF */
#define BLOCK_26    26    /*   2KB: 0xFFFF2800 - 0xFFFF2FFF */
#define BLOCK_27    27    /*   2KB: 0xFFFF2000 - 0xFFFF27FF */
#define BLOCK_28    28    /*   2KB: 0xFFFF1800 - 0xFFFF1FFF */
#define BLOCK_29    29    /*   2KB: 0xFFFF1000 - 0xFFFF17FF */
#define BLOCK_30    30    /*   2KB: 0xFFFF0800 - 0xFFFF0FFF */
#define BLOCK_31    31    /*   2KB: 0xFFFF0000 - 0xFFFF07FF */
#define BLOCK_32    32    /*   2KB: 0xFFFEF800 - 0xFFFEFFFF */
#define BLOCK_33    33    /*   2KB: 0xFFFEF000 - 0xFFFEF7FF */
#define BLOCK_34    34    /*   2KB: 0xFFFEE800 - 0xFFFEEFFF */
#define BLOCK_35    35    /*   2KB: 0xFFFEE000 - 0xFFFEE7FF */
#define BLOCK_36    36    /*   2KB: 0xFFFED800 - 0xFFFEDFFF */
#define BLOCK_37    37    /*   2KB: 0xFFFED000 - 0xFFFED7FF */
#define BLOCK_38    38    /*   2KB: 0xFFFEC800 - 0xFFFECFFF */
#define BLOCK_39    39    /*   2KB: 0xFFFEC000 - 0xFFFEC7FF */
#define BLOCK_40    40    /*   2KB: 0xFFFEB800 - 0xFFFEBFFF */
#define BLOCK_41    41    /*   2KB: 0xFFFEB000 - 0xFFFEB7FF */
#define BLOCK_42    42    /*   2KB: 0xFFFEA800 - 0xFFFEAFFF */
#define BLOCK_43    43    /*   2KB: 0xFFFEA000 - 0xFFFEA7FF */
#define BLOCK_44    44    /*   2KB: 0xFFFE9800 - 0xFFFE9FFF */
#define BLOCK_45    45    /*   2KB: 0xFFFE9000 - 0xFFFE97FF */
#define BLOCK_46    46    /*   2KB: 0xFFFE8800 - 0xFFFE8FFF */
#define BLOCK_47    47    /*   2KB: 0xFFFE8000 - 0xFFFE87FF */
#define BLOCK_48    48    /*   2KB: 0xFFFE7800 - 0xFFFE7FFF */
#define BLOCK_49    49    /*   2KB: 0xFFFE7000 - 0xFFFE77FF */
#define BLOCK_50    50    /*   2KB: 0xFFFE6800 - 0xFFFE6FFF */
#define BLOCK_51    51    /*   2KB: 0xFFFE6000 - 0xFFFE67FF */
#define BLOCK_52    52    /*   2KB: 0xFFFE5800 - 0xFFFE5FFF */
#define BLOCK_53    53    /*   2KB: 0xFFFE5000 - 0xFFFE57FF */
#define BLOCK_54    54    /*   2KB: 0xFFFE4800 - 0xFFFE4FFF */
#define BLOCK_55    55    /*   2KB: 0xFFFE4000 - 0xFFFE47FF */
#define BLOCK_56    56    /*   2KB: 0xFFFE3800 - 0xFFFE3FFF */
#define BLOCK_57    57    /*   2KB: 0xFFFE3000 - 0xFFFE37FF */
#define BLOCK_58    58    /*   2KB: 0xFFFE2800 - 0xFFFE2FFF */
#define BLOCK_59    59    /*   2KB: 0xFFFE2000 - 0xFFFE27FF */
#define BLOCK_60    60    /*   2KB: 0xFFFE1800 - 0xFFFE1FFF */
#define BLOCK_61    61    /*   2KB: 0xFFFE1000 - 0xFFFE17FF */
#define BLOCK_62    62    /*   2KB: 0xFFFE0800 - 0xFFFE0FFF */
#define BLOCK_63    63    /*   2KB: 0xFFFE0000 - 0xFFFE07FF */
#define BLOCK_64    64    /*   2KB: 0xFFFDF800 - 0xFFFDFFFF */
#define BLOCK_65    65    /*   2KB: 0xFFFDF000 - 0xFFFDF7FF */
#define BLOCK_66    66    /*   2KB: 0xFFFDE800 - 0xFFFDEFFF */
#define BLOCK_67    67    /*   2KB: 0xFFFDE000 - 0xFFFDE7FF */
#define BLOCK_68    68    /*   2KB: 0xFFFDD800 - 0xFFFDDFFF */
#define BLOCK_69    69    /*   2KB: 0xFFFDD000 - 0xFFFDD7FF */
#define BLOCK_70    70    /*   2KB: 0xFFFDC800 - 0xFFFDCFFF */
#define BLOCK_71    71    /*   2KB: 0xFFFDC000 - 0xFFFDC7FF */
#define BLOCK_72    72    /*   2KB: 0xFFFDB800 - 0xFFFDBFFF */
#define BLOCK_73    73    /*   2KB: 0xFFFDB000 - 0xFFFDB7FF */
#define BLOCK_74    74    /*   2KB: 0xFFFDA800 - 0xFFFDAFFF */
#define BLOCK_75    75    /*   2KB: 0xFFFDA000 - 0xFFFDA7FF */
#define BLOCK_76    76    /*   2KB: 0xFFFD9800 - 0xFFFD9FFF */
#define BLOCK_77    77    /*   2KB: 0xFFFD9000 - 0xFFFD97FF */
#define BLOCK_78    78    /*   2KB: 0xFFFD8800 - 0xFFFD8FFF */
#define BLOCK_79    79    /*   2KB: 0xFFFD8000 - 0xFFFD87FF */
#define BLOCK_80    80    /*   2KB: 0xFFFD7800 - 0xFFFD7FFF */
#define BLOCK_81    81    /*   2KB: 0xFFFD7000 - 0xFFFD77FF */
#define BLOCK_82    82    /*   2KB: 0xFFFD6800 - 0xFFFD6FFF */
#define BLOCK_83    83    /*   2KB: 0xFFFD6000 - 0xFFFD67FF */
#define BLOCK_84    84    /*   2KB: 0xFFFD5800 - 0xFFFD5FFF */
#define BLOCK_85    85    /*   2KB: 0xFFFD5000 - 0xFFFD57FF */
#define BLOCK_86    86    /*   2KB: 0xFFFD4800 - 0xFFFD4FFF */
#define BLOCK_87    87    /*   2KB: 0xFFFD4000 - 0xFFFD47FF */
#define BLOCK_88    88    /*   2KB: 0xFFFD3800 - 0xFFFD3FFF */
#define BLOCK_89    89    /*   2KB: 0xFFFD3000 - 0xFFFD37FF */
#define BLOCK_90    90    /*   2KB: 0xFFFD2800 - 0xFFFD2FFF */
#define BLOCK_91    91    /*   2KB: 0xFFFD2000 - 0xFFFD27FF */
#define BLOCK_92    92    /*   2KB: 0xFFFD1800 - 0xFFFD1FFF */
#define BLOCK_93    93    /*   2KB: 0xFFFD1000 - 0xFFFD17FF */
#define BLOCK_94    94    /*   2KB: 0xFFFD0800 - 0xFFFD0FFF */
#define BLOCK_95    95    /*   2KB: 0xFFFD0000 - 0xFFFD07FF */
#define BLOCK_96    96    /*   2KB: 0xFFFCF800 - 0xFFFCFFFF */
#define BLOCK_97    97    /*   2KB: 0xFFFCF000 - 0xFFFCF7FF */
#define BLOCK_98    98    /*   2KB: 0xFFFCE800 - 0xFFFCEFFF */
#define BLOCK_99    99    /*   2KB: 0xFFFCE000 - 0xFFFCE7FF */
#define BLOCK_100   100   /*   2KB: 0xFFFCD800 - 0xFFFCDFFF */
#define BLOCK_101   101   /*   2KB: 0xFFFCD000 - 0xFFFCD7FF */
#define BLOCK_102   102   /*   2KB: 0xFFFCC800 - 0xFFFCCFFF */
#define BLOCK_103   103   /*   2KB: 0xFFFCC000 - 0xFFFCC7FF */
#define BLOCK_104   104   /*   2KB: 0xFFFCB800 - 0xFFFCBFFF */
#define BLOCK_105   105   /*   2KB: 0xFFFCB000 - 0xFFFCB7FF */
#define BLOCK_106   106   /*   2KB: 0xFFFCA800 - 0xFFFCAFFF */
#define BLOCK_107   107   /*   2KB: 0xFFFCA000 - 0xFFFCA7FF */
#define BLOCK_108   108   /*   2KB: 0xFFFC9800 - 0xFFFC9FFF */
#define BLOCK_109   109   /*   2KB: 0xFFFC9000 - 0xFFFC97FF */
#define BLOCK_110   110   /*   2KB: 0xFFFC8800 - 0xFFFC8FFF */
#define BLOCK_111   111   /*   2KB: 0xFFFC8000 - 0xFFFC87FF */
#define BLOCK_112   112   /*   2KB: 0xFFFC7800 - 0xFFFC7FFF */
#define BLOCK_113   113   /*   2KB: 0xFFFC7000 - 0xFFFC77FF */
#define BLOCK_114   114   /*   2KB: 0xFFFC6800 - 0xFFFC6FFF */
#define BLOCK_115   115   /*   2KB: 0xFFFC6000 - 0xFFFC67FF */
#define BLOCK_116   116   /*   2KB: 0xFFFC5800 - 0xFFFC5FFF */
#define BLOCK_117   117   /*   2KB: 0xFFFC5000 - 0xFFFC57FF */
#define BLOCK_118   118   /*   2KB: 0xFFFC4800 - 0xFFFC4FFF */
#define BLOCK_119   119   /*   2KB: 0xFFFC4000 - 0xFFFC47FF */
#define BLOCK_120   120   /*   2KB: 0xFFFC3800 - 0xFFFC3FFF */
#define BLOCK_121   121   /*   2KB: 0xFFFC3000 - 0xFFFC37FF */
#define BLOCK_122   122   /*   2KB: 0xFFFC2800 - 0xFFFC2FFF */
#define BLOCK_123   123   /*   2KB: 0xFFFC2000 - 0xFFFC27FF */
#define BLOCK_124   124   /*   2KB: 0xFFFC1800 - 0xFFFC1FFF */
#define BLOCK_125   125   /*   2KB: 0xFFFC1000 - 0xFFFC17FF */
#define BLOCK_126   126   /*   2KB: 0xFFFC0800 - 0xFFFC0FFF */
#define BLOCK_127   127   /*   2KB: 0xFFFC0000 - 0xFFFC07FF */
#define BLOCK_128   128   /*   2KB: 0xFFFBF800 - 0xFFFBFFFF */
#define BLOCK_129   129   /*   2KB: 0xFFFBF000 - 0xFFFBF7FF */
#define BLOCK_130   130   /*   2KB: 0xFFFBE800 - 0xFFFBEFFF */
#define BLOCK_131   131   /*   2KB: 0xFFFBE000 - 0xFFFBE7FF */
#define BLOCK_132   132   /*   2KB: 0xFFFBD800 - 0xFFFBDFFF */
#define BLOCK_133   133   /*   2KB: 0xFFFBD000 - 0xFFFBD7FF */
#define BLOCK_134   134   /*   2KB: 0xFFFBC800 - 0xFFFBCFFF */
#define BLOCK_135   135   /*   2KB: 0xFFFBC000 - 0xFFFBC7FF */
#define BLOCK_136   136   /*   2KB: 0xFFFBB800 - 0xFFFBBFFF */
#define BLOCK_137   137   /*   2KB: 0xFFFBB000 - 0xFFFBB7FF */
#define BLOCK_138   138   /*   2KB: 0xFFFBA800 - 0xFFFBAFFF */
#define BLOCK_139   139   /*   2KB: 0xFFFBA000 - 0xFFFBA7FF */
#define BLOCK_140   140   /*   2KB: 0xFFFB9800 - 0xFFFB9FFF */
#define BLOCK_141   141   /*   2KB: 0xFFFB9000 - 0xFFFB97FF */
#define BLOCK_142   142   /*   2KB: 0xFFFB8800 - 0xFFFB8FFF */
#define BLOCK_143   143   /*   2KB: 0xFFFB8000 - 0xFFFB87FF */
#define BLOCK_144   144   /*   2KB: 0xFFFB7800 - 0xFFFB7FFF */
#define BLOCK_145   145   /*   2KB: 0xFFFB7000 - 0xFFFB77FF */
#define BLOCK_146   146   /*   2KB: 0xFFFB6800 - 0xFFFB6FFF */
#define BLOCK_147   147   /*   2KB: 0xFFFB6000 - 0xFFFB67FF */
#define BLOCK_148   148   /*   2KB: 0xFFFB5800 - 0xFFFB5FFF */
#define BLOCK_149   149   /*   2KB: 0xFFFB5000 - 0xFFFB57FF */
#define BLOCK_150   150   /*   2KB: 0xFFFB4800 - 0xFFFB4FFF */
#define BLOCK_151   151   /*   2KB: 0xFFFB4000 - 0xFFFB47FF */
#define BLOCK_152   152   /*   2KB: 0xFFFB3800 - 0xFFFB3FFF */
#define BLOCK_153   153   /*   2KB: 0xFFFB3000 - 0xFFFB37FF */
#define BLOCK_154   154   /*   2KB: 0xFFFB2800 - 0xFFFB2FFF */
#define BLOCK_155   155   /*   2KB: 0xFFFB2000 - 0xFFFB27FF */
#define BLOCK_156   156   /*   2KB: 0xFFFB1800 - 0xFFFB1FFF */
#define BLOCK_157   157   /*   2KB: 0xFFFB1000 - 0xFFFB17FF */
#define BLOCK_158   158   /*   2KB: 0xFFFB0800 - 0xFFFB0FFF */
#define BLOCK_159   159   /*   2KB: 0xFFFB0000 - 0xFFFB07FF */
#define BLOCK_160   160   /*   2KB: 0xFFFAF800 - 0xFFFAFFFF */
#define BLOCK_161   161   /*   2KB: 0xFFFAF000 - 0xFFFAF7FF */
#define BLOCK_162   162   /*   2KB: 0xFFFAE800 - 0xFFFAEFFF */
#define BLOCK_163   163   /*   2KB: 0xFFFAE000 - 0xFFFAE7FF */
#define BLOCK_164   164   /*   2KB: 0xFFFAD800 - 0xFFFADFFF */
#define BLOCK_165   165   /*   2KB: 0xFFFAD000 - 0xFFFAD7FF */
#define BLOCK_166   166   /*   2KB: 0xFFFAC800 - 0xFFFACFFF */
#define BLOCK_167   167   /*   2KB: 0xFFFAC000 - 0xFFFAC7FF */
#define BLOCK_168   168   /*   2KB: 0xFFFAB800 - 0xFFFABFFF */
#define BLOCK_169   169   /*   2KB: 0xFFFAB000 - 0xFFFAB7FF */
#define BLOCK_170   170   /*   2KB: 0xFFFAA800 - 0xFFFAAFFF */
#define BLOCK_171   171   /*   2KB: 0xFFFAA000 - 0xFFFAA7FF */
#define BLOCK_172   172   /*   2KB: 0xFFFA9800 - 0xFFFA9FFF */
#define BLOCK_173   173   /*   2KB: 0xFFFA9000 - 0xFFFA97FF */
#define BLOCK_174   174   /*   2KB: 0xFFFA8800 - 0xFFFA8FFF */
#define BLOCK_175   175   /*   2KB: 0xFFFA8000 - 0xFFFA87FF */
#define BLOCK_176   176   /*   2KB: 0xFFFA7800 - 0xFFFA7FFF */
#define BLOCK_177   177   /*   2KB: 0xFFFA7000 - 0xFFFA77FF */
#define BLOCK_178   178   /*   2KB: 0xFFFA6800 - 0xFFFA6FFF */
#define BLOCK_179   179   /*   2KB: 0xFFFA6000 - 0xFFFA67FF */
#define BLOCK_180   180   /*   2KB: 0xFFFA5800 - 0xFFFA5FFF */
#define BLOCK_181   181   /*   2KB: 0xFFFA5000 - 0xFFFA57FF */
#define BLOCK_182   182   /*   2KB: 0xFFFA4800 - 0xFFFA4FFF */
#define BLOCK_183   183   /*   2KB: 0xFFFA4000 - 0xFFFA47FF */
#define BLOCK_184   184   /*   2KB: 0xFFFA3800 - 0xFFFA3FFF */
#define BLOCK_185   185   /*   2KB: 0xFFFA3000 - 0xFFFA37FF */
#define BLOCK_186   186   /*   2KB: 0xFFFA2800 - 0xFFFA2FFF */
#define BLOCK_187   187   /*   2KB: 0xFFFA2000 - 0xFFFA27FF */
#define BLOCK_188   188   /*   2KB: 0xFFFA1800 - 0xFFFA1FFF */
#define BLOCK_189   189   /*   2KB: 0xFFFA1000 - 0xFFFA17FF */
#define BLOCK_190   190   /*   2KB: 0xFFFA0800 - 0xFFFA0FFF */
#define BLOCK_191   191   /*   2KB: 0xFFFA0000 - 0xFFFA07FF */
#define BLOCK_192   192   /*   2KB: 0xFFF9F800 - 0xFFF9FFFF */
#define BLOCK_193   193   /*   2KB: 0xFFF9F000 - 0xFFF9F7FF */
#define BLOCK_194   194   /*   2KB: 0xFFF9E800 - 0xFFF9EFFF */
#define BLOCK_195   195   /*   2KB: 0xFFF9E000 - 0xFFF9E7FF */
#define BLOCK_196   196   /*   2KB: 0xFFF9D800 - 0xFFF9DFFF */
#define BLOCK_197   197   /*   2KB: 0xFFF9D000 - 0xFFF9D7FF */
#define BLOCK_198   198   /*   2KB: 0xFFF9C800 - 0xFFF9CFFF */
#define BLOCK_199   199   /*   2KB: 0xFFF9C000 - 0xFFF9C7FF */
#define BLOCK_200   200   /*   2KB: 0xFFF9B800 - 0xFFF9BFFF */
#define BLOCK_201   201   /*   2KB: 0xFFF9B000 - 0xFFF9B7FF */
#define BLOCK_202   202   /*   2KB: 0xFFF9A800 - 0xFFF9AFFF */
#define BLOCK_203   203   /*   2KB: 0xFFF9A000 - 0xFFF9A7FF */
#define BLOCK_204   204   /*   2KB: 0xFFF99800 - 0xFFF99FFF */
#define BLOCK_205   205   /*   2KB: 0xFFF99000 - 0xFFF997FF */
#define BLOCK_206   206   /*   2KB: 0xFFF98800 - 0xFFF98FFF */
#define BLOCK_207   207   /*   2KB: 0xFFF98000 - 0xFFF987FF */
#define BLOCK_208   208   /*   2KB: 0xFFF97800 - 0xFFF97FFF */
#define BLOCK_209   209   /*   2KB: 0xFFF97000 - 0xFFF977FF */
#define BLOCK_210   210   /*   2KB: 0xFFF96800 - 0xFFF96FFF */
#define BLOCK_211   211   /*   2KB: 0xFFF96000 - 0xFFF967FF */
#define BLOCK_212   212   /*   2KB: 0xFFF95800 - 0xFFF95FFF */
#define BLOCK_213   213   /*   2KB: 0xFFF95000 - 0xFFF957FF */
#define BLOCK_214   214   /*   2KB: 0xFFF94800 - 0xFFF94FFF */
#define BLOCK_215   215   /*   2KB: 0xFFF94000 - 0xFFF947FF */
#define BLOCK_216   216   /*   2KB: 0xFFF93800 - 0xFFF93FFF */
#define BLOCK_217   217   /*   2KB: 0xFFF93000 - 0xFFF937FF */
#define BLOCK_218   218   /*   2KB: 0xFFF92800 - 0xFFF92FFF */
#define BLOCK_219   219   /*   2KB: 0xFFF92000 - 0xFFF927FF */
#define BLOCK_220   220   /*   2KB: 0xFFF91800 - 0xFFF91FFF */
#define BLOCK_221   221   /*   2KB: 0xFFF91000 - 0xFFF917FF */
#define BLOCK_222   222   /*   2KB: 0xFFF90800 - 0xFFF90FFF */
#define BLOCK_223   223   /*   2KB: 0xFFF90000 - 0xFFF907FF */
#define BLOCK_224   224   /*   2KB: 0xFFF8F800 - 0xFFF8FFFF */
#define BLOCK_225   225   /*   2KB: 0xFFF8F000 - 0xFFF8F7FF */
#define BLOCK_226   226   /*   2KB: 0xFFF8E800 - 0xFFF8EFFF */
#define BLOCK_227   227   /*   2KB: 0xFFF8E000 - 0xFFF8E7FF */
#define BLOCK_228   228   /*   2KB: 0xFFF8D800 - 0xFFF8DFFF */
#define BLOCK_229   229   /*   2KB: 0xFFF8D000 - 0xFFF8D7FF */
#define BLOCK_230   230   /*   2KB: 0xFFF8C800 - 0xFFF8CFFF */
#define BLOCK_231   231   /*   2KB: 0xFFF8C000 - 0xFFF8C7FF */
#define BLOCK_232   232   /*   2KB: 0xFFF8B800 - 0xFFF8BFFF */
#define BLOCK_233   233   /*   2KB: 0xFFF8B000 - 0xFFF8B7FF */
#define BLOCK_234   234   /*   2KB: 0xFFF8A800 - 0xFFF8AFFF */
#define BLOCK_235   235   /*   2KB: 0xFFF8A000 - 0xFFF8A7FF */
#define BLOCK_236   236   /*   2KB: 0xFFF89800 - 0xFFF89FFF */
#define BLOCK_237   237   /*   2KB: 0xFFF89000 - 0xFFF897FF */
#define BLOCK_238   238   /*   2KB: 0xFFF88800 - 0xFFF88FFF */
#define BLOCK_239   239   /*   2KB: 0xFFF88000 - 0xFFF887FF */
#define BLOCK_240   240   /*   2KB: 0xFFF87800 - 0xFFF87FFF */
#define BLOCK_241   241   /*   2KB: 0xFFF87000 - 0xFFF877FF */
#define BLOCK_242   242   /*   2KB: 0xFFF86800 - 0xFFF86FFF */
#define BLOCK_243   243   /*   2KB: 0xFFF86000 - 0xFFF867FF */
#define BLOCK_244   244   /*   2KB: 0xFFF85800 - 0xFFF85FFF */
#define BLOCK_245   245   /*   2KB: 0xFFF85000 - 0xFFF857FF */
#define BLOCK_246   246   /*   2KB: 0xFFF84800 - 0xFFF84FFF */
#define BLOCK_247   247   /*   2KB: 0xFFF84000 - 0xFFF847FF */
#define BLOCK_248   248   /*   2KB: 0xFFF83800 - 0xFFF83FFF */
#define BLOCK_249   249   /*   2KB: 0xFFF83000 - 0xFFF837FF */
#define BLOCK_250   250   /*   2KB: 0xFFF82800 - 0xFFF82FFF */
#define BLOCK_251   251   /*   2KB: 0xFFF82000 - 0xFFF827FF */
#define BLOCK_252   252   /*   2KB: 0xFFF81800 - 0xFFF81FFF */
#define BLOCK_253   253   /*   2KB: 0xFFF81000 - 0xFFF817FF */
#define BLOCK_254   254   /*   2KB: 0xFFF80800 - 0xFFF80FFF */
#define BLOCK_255   255   /*   2KB: 0xFFF80000 - 0xFFF807FF */

/* Data Flash Block Area      Size: Start Addr -   End Addr */
#define BLOCK_DB0   256   /*   2KB: 0x00100000 - 0x001007FF */
#define BLOCK_DB1   257   /*   2KB: 0x00100800 - 0x00100FFF */
#define BLOCK_DB2   258   /*   2KB: 0x00101000 - 0x001017FF */
#define BLOCK_DB3   259   /*   2KB: 0x00101800 - 0x00101FFF */

/* Array of flash addresses used for writing */
#if defined(FLASH_BLOCKS_DECLARE)
const uint32_t g_flash_BlockAddresses[260] = {
    0x00FFF800,  /* EB000 */
    0x00FFF000,  /* EB001 */
    0x00FFE800,  /* EB002 */
    0x00FFE000,  /* EB003 */
    0x00FFD800,  /* EB004 */
    0x00FFD000,  /* EB005 */
    0x00FFC800,  /* EB006 */
    0x00FFC000,  /* EB007 */
    0x00FFB800,  /* EB008 */
    0x00FFB000,  /* EB009 */
    0x00FFA800,  /* EB010 */
    0x00FFA000,  /* EB011 */
    0x00FF9800,  /* EB012 */
    0x00FF9000,  /* EB013 */
    0x00FF8800,  /* EB014 */
    0x00FF8000,  /* EB015 */
    0x00FF7800,  /* EB016 */
    0x00FF7000,  /* EB017 */
    0x00FF6800,  /* EB018 */
    0x00FF6000,  /* EB019 */
    0x00FF5800,  /* EB020 */
    0x00FF5000,  /* EB021 */
    0x00FF4800,  /* EB022 */
    0x00FF4000,  /* EB023 */
    0x00FF3800,  /* EB024 */
    0x00FF3000,  /* EB025 */
    0x00FF2800,  /* EB026 */
    0x00FF2000,  /* EB027 */
    0x00FF1800,  /* EB028 */
    0x00FF1000,  /* EB029 */
    0x00FF0800,  /* EB030 */
    0x00FF0000,  /* EB031 */
    0x00FEF800,  /* EB032 */
    0x00FEF000,  /* EB033 */
    0x00FEE800,  /* EB034 */
    0x00FEE000,  /* EB035 */
    0x00FED800,  /* EB036 */
    0x00FED000,  /* EB037 */
    0x00FEC800,  /* EB038 */
    0x00FEC000,  /* EB039 */
    0x00FEB800,  /* EB040 */
    0x00FEB000,  /* EB041 */
    0x00FEA800,  /* EB042 */
    0x00FEA000,  /* EB043 */
    0x00FE9800,  /* EB044 */
    0x00FE9000,  /* EB045 */
    0x00FE8800,  /* EB046 */
    0x00FE8000,  /* EB047 */
    0x00FE7800,  /* EB048 */
    0x00FE7000,  /* EB049 */
    0x00FE6800,  /* EB050 */
    0x00FE6000,  /* EB051 */
    0x00FE5800,  /* EB052 */
    0x00FE5000,  /* EB053 */
    0x00FE4800,  /* EB054 */
    0x00FE4000,  /* EB055 */
    0x00FE3800,  /* EB056 */
    0x00FE3000,  /* EB057 */
    0x00FE2800,  /* EB058 */
    0x00FE2000,  /* EB059 */
    0x00FE1800,  /* EB060 */
    0x00FE1000,  /* EB061 */
    0x00FE0800,  /* EB062 */
    0x00FE0000,  /* EB063 */
    0x00FDF800,  /* EB064 */
    0x00FDF000,  /* EB065 */
    0x00FDE800,  /* EB066 */
    0x00FDE000,  /* EB067 */
    0x00FDD800,  /* EB068 */
    0x00FDD000,  /* EB069 */
    0x00FDC800,  /* EB070 */
    0x00FDC000,  /* EB071 */
    0x00FDB800,  /* EB072 */
    0x00FDB000,  /* EB073 */
    0x00FDA800,  /* EB074 */
    0x00FDA000,  /* EB075 */
    0x00FD9800,  /* EB076 */
    0x00FD9000,  /* EB077 */
    0x00FD8800,  /* EB078 */
    0x00FD8000,  /* EB079 */
    0x00FD7800,  /* EB080 */
    0x00FD7000,  /* EB081 */
    0x00FD6800,  /* EB082 */
    0x00FD6000,  /* EB083 */
    0x00FD5800,  /* EB084 */
    0x00FD5000,  /* EB085 */
    0x00FD4800,  /* EB086 */
    0x00FD4000,  /* EB087 */
    0x00FD3800,  /* EB088 */
    0x00FD3000,  /* EB089 */
    0x00FD2800,  /* EB090 */
    0x00FD2000,  /* EB091 */
    0x00FD1800,  /* EB092 */
    0x00FD1000,  /* EB093 */
    0x00FD0800,  /* EB094 */
    0x00FD0000,  /* EB095 */
    0x00FCF800,  /* EB096 */
    0x00FCF000,  /* EB097 */
    0x00FCE800,  /* EB098 */
    0x00FCE000,  /* EB099 */
    0x00FCD800,  /* EB100 */
    0x00FCD000,  /* EB101 */
    0x00FCC800,  /* EB102 */
    0x00FCC000,  /* EB103 */
    0x00FCB800,  /* EB104 */
    0x00FCB000,  /* EB105 */
    0x00FCA800,  /* EB106 */
    0x00FCA000,  /* EB107 */
    0x00FC9800,  /* EB108 */
    0x00FC9000,  /* EB109 */
    0x00FC8800,  /* EB110 */
    0x00FC8000,  /* EB111 */
    0x00FC7800,  /* EB112 */
    0x00FC7000,  /* EB113 */
    0x00FC6800,  /* EB114 */
    0x00FC6000,  /* EB115 */
    0x00FC5800,  /* EB116 */
    0x00FC5000,  /* EB117 */
    0x00FC4800,  /* EB118 */
    0x00FC4000,  /* EB119 */
    0x00FC3800,  /* EB120 */
    0x00FC3000,  /* EB121 */
    0x00FC2800,  /* EB122 */
    0x00FC2000,  /* EB123 */
    0x00FC1800,  /* EB124 */
    0x00FC1000,  /* EB125 */
    0x00FC0800,  /* EB126 */
    0x00FC0000,  /* EB127 */
    0x00FBF800,  /* EB128 */
    0x00FBF000,  /* EB129 */
    0x00FBE800,  /* EB130 */
    0x00FBE000,  /* EB131 */
    0x00FBD800,  /* EB132 */
    0x00FBD000,  /* EB133 */
    0x00FBC800,  /* EB134 */
    0x00FBC000,  /* EB135 */
    0x00FBB800,  /* EB136 */
    0x00FBB000,  /* EB137 */
    0x00FBA800,  /* EB138 */
    0x00FBA000,  /* EB139 */
    0x00FB9800,  /* EB140 */
    0x00FB9000,  /* EB141 */
    0x00FB8800,  /* EB142 */
    0x00FB8000,  /* EB143 */
    0x00FB7800,  /* EB144 */
    0x00FB7000,  /* EB145 */
    0x00FB6800,  /* EB146 */
    0x00FB6000,  /* EB147 */
    0x00FB5800,  /* EB148 */
    0x00FB5000,  /* EB149 */
    0x00FB4800,  /* EB150 */
    0x00FB4000,  /* EB151 */
    0x00FB3800,  /* EB152 */
    0x00FB3000,  /* EB153 */
    0x00FB2800,  /* EB154 */
    0x00FB2000,  /* EB155 */
    0x00FB1800,  /* EB156 */
    0x00FB1000,  /* EB157 */
    0x00FB0800,  /* EB158 */
    0x00FB0000,  /* EB159 */
    0x00FAF800,  /* EB160 */
    0x00FAF000,  /* EB161 */
    0x00FAE800,  /* EB162 */
    0x00FAE000,  /* EB163 */
    0x00FAD800,  /* EB164 */
    0x00FAD000,  /* EB165 */
    0x00FAC800,  /* EB166 */
    0x00FAC000,  /* EB167 */
    0x00FAB800,  /* EB168 */
    0x00FAB000,  /* EB169 */
    0x00FAA800,  /* EB170 */
    0x00FAA000,  /* EB171 */
    0x00FA9800,  /* EB172 */
    0x00FA9000,  /* EB173 */
    0x00FA8800,  /* EB174 */
    0x00FA8000,  /* EB175 */
    0x00FA7800,  /* EB176 */
    0x00FA7000,  /* EB177 */
    0x00FA6800,  /* EB178 */
    0x00FA6000,  /* EB179 */
    0x00FA5800,  /* EB180 */
    0x00FA5000,  /* EB181 */
    0x00FA4800,  /* EB182 */
    0x00FA4000,  /* EB183 */
    0x00FA3800,  /* EB184 */
    0x00FA3000,  /* EB185 */
    0x00FA2800,  /* EB186 */
    0x00FA2000,  /* EB187 */
    0x00FA1800,  /* EB188 */
    0x00FA1000,  /* EB189 */
    0x00FA0800,  /* EB190 */
    0x00FA0000,  /* EB191 */
    0x00F9F800,  /* EB192 */
    0x00F9F000,  /* EB193 */
    0x00F9E800,  /* EB194 */
    0x00F9E000,  /* EB195 */
    0x00F9D800,  /* EB196 */
    0x00F9D000,  /* EB197 */
    0x00F9C800,  /* EB198 */
    0x00F9C000,  /* EB199 */
    0x00F9B800,  /* EB200 */
    0x00F9B000,  /* EB201 */
    0x00F9A800,  /* EB202 */
    0x00F9A000,  /* EB203 */
    0x00F99800,  /* EB204 */
    0x00F99000,  /* EB205 */
    0x00F98800,  /* EB206 */
    0x00F98000,  /* EB207 */
    0x00F97800,  /* EB208 */
    0x00F97000,  /* EB209 */
    0x00F96800,  /* EB210 */
    0x00F96000,  /* EB211 */
    0x00F95800,  /* EB212 */
    0x00F95000,  /* EB213 */
    0x00F94800,  /* EB214 */
    0x00F94000,  /* EB215 */
    0x00F93800,  /* EB216 */
    0x00F93000,  /* EB217 */
    0x00F92800,  /* EB218 */
    0x00F92000,  /* EB219 */
    0x00F91800,  /* EB220 */
    0x00F91000,  /* EB221 */
    0x00F90800,  /* EB222 */
    0x00F90000,  /* EB223 */
    0x00F8F800,  /* EB224 */
    0x00F8F000,  /* EB225 */
    0x00F8E800,  /* EB226 */
    0x00F8E000,  /* EB227 */
    0x00F8D800,  /* EB228 */
    0x00F8D000,  /* EB229 */
    0x00F8C800,  /* EB230 */
    0x00F8C000,  /* EB231 */
    0x00F8B800,  /* EB232 */
    0x00F8B000,  /* EB233 */
    0x00F8A800,  /* EB234 */
    0x00F8A000,  /* EB235 */
    0x00F89800,  /* EB236 */
    0x00F89000,  /* EB237 */
    0x00F88800,  /* EB238 */
    0x00F88000,  /* EB239 */
    0x00F87800,  /* EB240 */
    0x00F87000,  /* EB241 */
    0x00F86800,  /* EB242 */
    0x00F86000,  /* EB243 */
    0x00F85800,  /* EB244 */
    0x00F85000,  /* EB245 */
    0x00F84800,  /* EB246 */
    0x00F84000,  /* EB247 */
    0x00F83800,  /* EB248 */
    0x00F83000,  /* EB249 */
    0x00F82800,  /* EB250 */
    0x00F82000,  /* EB251 */
    0x00F81800,  /* EB252 */
    0x00F81000,  /* EB253 */
    0x00F80800,  /* EB254 */
    0x00F80000,  /* EB255 */
    0x00100000,  /* DB000 */
    0x00100800,  /* DB001 */
    0x00101000,  /* DB002 */
    0x00101800   /* DB003 */
};
#else 
extern const uint32_t g_flash_BlockAddresses[260];
#endif             

/* Define the clock frequency supplied to the FCU. On the RX610 and Rx62x
   this is the PCLK. On the RX63x it is the FCLK. */
#define FLASH_CLOCK_HZ BSP_FCLK_HZ

/*  According to HW Manual the Max Programming Time for 128 bytes (ROM)
    is 6ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 6ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(6000 * (32.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 8 bytes
    (Data Flash) is 3.2ms.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 3.2ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(3200 * (32.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 2k bytes
    (Data Flash) is 2.5ms.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 2.5ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(2500 * (32.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))
        
/*  According to HW Manual the max timeout value when using the peripheral
    clock notification command is 60us. This is with a FCLK of 32MHz. The 
    calculation below calculates the number of ICLK ticks needed for the 
    timeout delay. The 60us number is adjusted linearly depending on 
    the FCLK frequency.
*/
#define WAIT_MAX_NOTIFY_FCU_CLOCK \
        ((int32_t)(60 * (32.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))         

/*  According to HW Manual the Max Erasure Time for a 2kB block is
    around 60ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 60ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(60000 * (32.0/(FLASH_CLOCK_HZ/1000000)))*(BSP_ICLK_HZ/1000000))    

/******************************************************************************
Error checking
******************************************************************************/
/* FCLK must be between 4MHz and 32MHz. */
#if (FLASH_CLOCK_HZ > 32000000) || (FLASH_CLOCK_HZ < 4000000)
    #error "ERROR - Flash API - FCLK on RX210 must be between 4MHz and 32MHz!"
#endif

#endif /* _FLASH_API_RX210_H */
