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
* File Name    : r_usb_hatapi_define.h
* Description  : USB common extern header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
***********************************************************************************************************************/

#ifndef __R_USB_CATAPI_DEFINE_H__
#define __R_USB_CATAPI_DEFINE_H__

/******************************************************************************
Constant macro definitions
******************************************************************************/
/* prevent allow key */
#define USB_MEDIA_UNLOCK    0u          /* Media unlock */
#define USB_MEDIA_LOCK      1u          /* Media Lock */

/* Peripheral Device Type (InquiryRequest) */
#define USB_PDT_DIRECT      0x00u
#define USB_PDT_SEQUENTIAL  0x01u
#define USB_PDT_WRITEONCE   0x04u
#define USB_PDT_CDROM       0x05u
#define USB_PDT_OPTICAL     0x07u
#define USB_PDT_UNKNOWN     0x1Fu

/* max Partiton */
#define USB_MAXUNITNUM      4u
#define USB_BOOTPARTNUM     4u

/* Partision check */
#define USB_PBR_ADDR        0x01u
#define USB_MBR_ADDR        0x02u
#define USB_EMBR_ADDR       0x03u
#define USB_BOOT_ERROR      0x05u
#define USB_BOOTRECORD_SIG  0xAA55u
#define USB_STARTDISK       0x80u
#define USB_NOTSTARTDISK    0x00u
#define USB_NOPCODE         0x90u
#define USB_JMPCODE1        0xEBu
#define USB_JMPCODE2        0xE9u

/* Partition type */
#define USB_PT_NONE         0x00u
#define USB_PT_FAT12A       0x01u
#define USB_PT_FAT16A       0x04u
#define USB_PT_EPRTA        0x05u
#define USB_PT_FAT16B       0x06u
#define USB_PT_FAT32A       0x0Bu
#define USB_PT_FAT32X       0x0Cu
#define USB_PT_FAT16X       0x0Eu
#define USB_PT_EPRTB        0x0Fu
#define USB_PT_EPRTC        0x85u

#define USB_PT_FAT12        0x01u
#define USB_PT_FAT16        0x04u
#define USB_PT_FAT32        0x0Bu
#define USB_PT_EPRT         0x05u

/* FAT TYPE */
#define USB_PMSC_FATTYPE_12_PP      1
#define USB_PMSC_FATTYPE_16_PP      2

#define USB_PMSC_FATTYPE_PP USB_PMSC_FATTYPE_12_PP
//#define USB_PMSC_FATTYPE_PP   USB_PMSC_FATTYPE_16_PP

/*****************************************************************************
Typedef definitions
******************************************************************************/
/* MBR */
typedef struct
{
    uint8_t JMPcode;
    uint8_t JMPaddr;
    uint8_t NOPcode;
    uint8_t BSRcode[443];
    uint8_t PartitionTable[64];
    uint8_t Signature[2];
} USB_MBR_t;

/* PTBL */
typedef struct
{
    uint8_t ActiveFlag;
    uint8_t StartHead;
    uint8_t StartCS[2];
    uint8_t PartitionType;
    uint8_t StopHead;
    uint8_t StopCS[2];
    uint8_t StartSectorNum[4];
    uint8_t PartitionSect[4];
} USB_PTBL_t;

/* PBR */
typedef struct
{
    uint8_t JMPcode;
    uint8_t JMPaddr;
    uint8_t NOPcode;
    uint8_t Name[8];
    uint8_t SectorSize[2];
    uint8_t ClusterSize;
    uint8_t ReservedSector[2];
    uint8_t FatCount;
    uint8_t RootDirTop[2];
    uint8_t TotalSector0[2];
    uint8_t DfsMediaType;
    uint8_t FATSector[2];
    uint8_t TrackSector[2];
    uint8_t CylinderSector[2];
    uint8_t OffsetSector[4];
    uint8_t TotalSector1[4];
    uint8_t FATSigData[474];
    uint8_t Signature[2];
} USB_PBR_t;

/* FAT12 */
typedef struct
{
    uint8_t DriveNum;
    uint8_t Reserve;
    uint8_t BootSig;
    uint8_t VolSirial[4];
    uint8_t VolLabel[11];
    uint8_t FileSystemType[8];
} USB_FAT1216_t;

/* FAT32 */
typedef struct
{
    uint8_t FATSector[4];
    uint8_t ExtendedFlag[2];
    uint8_t FileSystemVer[2];
    uint8_t RootDirCluster[4];
    uint8_t FSinfoSector[2];
    uint8_t BackupBootSector[2];
    uint8_t Reserve12[12];
    uint8_t DriveNum;
    uint8_t Reserve;
    uint8_t BootSig;
    uint8_t VolSirial[4];
    uint8_t VolLabel[11];
    uint8_t FileSystemType[8];
} USB_FAT32_t;


/* Callback Message format define. */
typedef struct
{
    uint32_t    ar_rst;
    uint32_t    ul_size;
}
USB_PMSC_CBM_t;

/******************************************************************************
Bit Order Definition "LEFT"
******************************************************************************/
#pragma bit_order left

/* Command Descriptor Block format define. */
typedef union
{
    struct
    {
        uint8_t uc_OpCode;
        struct
        {
            uint8_t b_LUN:3;
            uint8_t b_reserved:5;
        }
        s_LUN;
        uint8_t uc_data;
    }
    s_usb_ptn0;
    struct
    {
        uint8_t uc_OpCode;
        struct
        {
            uint8_t b_LUN:3;
            uint8_t b_reserved4:4;
            uint8_t b_immed:1;
        }
        s_LUN;
        uint8_t uc_rsv2[2];
        uint8_t uc_Allocation;
        uint8_t uc_rsv1[1];
        uint8_t uc_rsv6[6];
    }
    s_usb_ptn12;
    struct
    {
        uint8_t uc_OpCode;
        struct
        {
            uint8_t b_LUN:3;
            uint8_t b_FmtData:1;
            uint8_t b_CmpList:1;
            uint8_t b_Defect:3;
        }
        s_LUN;
        uint8_t ul_LBA0;
        uint8_t ul_LBA1;
        uint8_t ul_LBA2;
        uint8_t ul_LBA3;
        uint8_t uc_rsv6[6];
    }
    s_usb_ptn378;
    struct
    {
        uint8_t uc_OpCode;
        struct
        {
            uint8_t b_LUN:3;
            uint8_t b_1:1;
            uint8_t b_reserved2:2;
            uint8_t b_ByteChk:1;
            uint8_t b_SP:1;
        }
        s_LUN;
        /* Logical block */
        uint8_t ul_LogicalBlock0;
        uint8_t ul_LogicalBlock1;
        uint8_t ul_LogicalBlock2;
        uint8_t ul_LogicalBlock3;
        uint8_t uc_rsv1[1];
        uint8_t us_Length_Hi;
        uint8_t us_Length_Lo;
        uint8_t uc_rsv3[3];
    }
    s_usb_ptn4569;
}
USB_PMSC_CDB_t;

/******************************************************************************
Bit Order Definition default
******************************************************************************/
#pragma bit_order


/*****************************************************************************
Enum definitions
******************************************************************************/
enum usb_gpmsc_AtapiResult
{
    USB_ATAPI_SUCCESS                           = 0x11,
    /* Command receive events */
    USB_ATAPI_NO_DATA                           = 0x21,
    USB_ATAPI_A_SND_DATA                        = 0x22,
    USB_ATAPI_A_RCV_DATA                        = 0x23,
    USB_ATAPI_SND_DATAS                         = 0x24,
    USB_ATAPI_RCV_DATAS                         = 0x25,
    USB_ATAPI_NOT_SUPPORT                       = 0x26,
    /* Complete events */
    USB_ATAPI_CMD_CONTINUE                      = 0x31,
    USB_ATAPI_CMD_COMPLETE                      = 0x32,
    USB_ATAPI_CMD_FAILED                        = 0x33,
    /* ATAPI Start events */
    USB_ATAPI_READY                             = 0x41,
    // respond error
    USB_ATAPI_ERROR                             = 0x51,
    /*** ERR CODE ***/
    USB_ATAPI_ERR_CODE_SEPARATER                = 0x100,
    USB_ATAPI_ERR_INVAL                         = 0x61
};


#endif  /* __R_USB_CATAPI_DEFINE_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
