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
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_hstorage_driver.c
* Description  : USB sample data declaration
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_hatapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_usb_hmsc_define.h"      /* Host Mass Storage Class Driver */
#include "r_usb_hmsc_extern.h"      /* MSC global definition */

#include "r_usb_api.h"
#include "r_usb_hmsc_config.h"
#include "r_usb_hmsc_if.h"

/******************************************************************************
Renesas Abstracted Peripheral Driver functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_hmsc_SmpDevNextDriveSearch
Description     : Next drive search
Arguments       : none
Return value    : uint16_t             : 
******************************************************************************/
uint16_t usb_hmsc_SmpDevNextDriveSearch(USB_UTR_t *ptr)
{
    uint16_t i;

    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( usb_ghmsc_DriveChk[i][0] == USB_NO )
        {
            return i;
        }
    }
    return (uint16_t)0;
}   /* eof usb_hmsc_SmpDevNextDriveSearch() */


/******************************************************************************
Function Name   : usb_hmsc_StrgDriveSearchAct
Description     : Storage drive search
Arguments       : USB_CLSINFO_t *mess   : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgDriveSearchAct(USB_CLSINFO_t *mess)
{
    uint32_t    j, result;
    uint16_t    resultk;
/* Condition compilation by the difference of quality control */
 #ifdef USB_DEBUGPRINT_PP
    uint32_t    i;
    uint8_t     pdata[32];
 #endif /* USB_DEBUGPRINT_PP */
    uint16_t    offset, new_count, addr;
    USB_ER_t err,err2;
    USB_MH_t p_blf;
    USB_CLSINFO_t *cp;
    uint16_t    drive_no;

    switch( usb_shmsc_StrgDriveSearchSeq[mess->ip] )
    {
    case USB_SEQ_0:
        USB_PRINTF0("\n*** Drive search !\n");
        /* Unit number set */
        addr = mess -> keyword;
        usb_ghmsc_RootDevaddr[mess->ip] = addr;
        usb_shmsc_command_result[mess->ip] = mess->complete;

        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        err = R_usb_hmsc_GetMaxUnit(mess, addr, (USB_CB_t)usb_hmsc_StrgCheckResult);
        if( err == USB_E_QOVR )
        {
            /* Resend message */
            err = R_USB_PGET_BLK(USB_HSTRG_MPL, &p_blf);
            if( err == USB_E_OK )
            {
                cp = (USB_CLSINFO_t*)p_blf;
                cp->msginfo = mess -> msginfo;
                cp->keyword = mess -> keyword;
                cp->result  = mess -> result;
                cp->complete = mess->complete;
                cp->ip      = mess->ip;
                cp->ipp     = mess->ipp;

                /* Send message */
                err = R_USB_SND_MSG(USB_HSTRG_MBX, (USB_MSG_t*)p_blf);
                if( err != USB_E_OK )
                {
                    USB_PRINTF1("### hmsc_StrgDriveSearch snd_msg error (%ld)\n", err);
                    err2 = R_USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)p_blf);
                    if( err2 != USB_E_OK )
                    {
                        USB_PRINTF1("### hmsc_StrgDriveSearch rel_blk error (%ld)\n", err2);
                    }
                }
            }
            else
            {
                USB_PRINTF1("### hmsc_StrgDriveSearch pget_blk error (%ld)\n", err);
            }
        }
        else
        {
            usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        }
        break;
    case USB_SEQ_1:
        addr = usb_ghmsc_RootDevaddr[mess->ip];

        /* Get MAX_LUN */
        usb_ghmsc_MaxLUN[mess->ip] =  usb_hmsc_GetMaxUnitCheck(mess, mess->result);
        if( usb_ghmsc_MaxLUN[mess->ip] == USB_ERROR )
        {
            usb_ghmsc_MaxLUN[mess->ip] = (uint16_t)0;
            USB_PRINTF1("*** Unit information error, set unit number %d !\n", usb_ghmsc_MaxLUN[mess->ip]);
        }
        else if( usb_ghmsc_MaxLUN[mess->ip] > (uint32_t)USB_MAXUNITNUM )
        {
            USB_PRINTF2("*** Max Unit number(%d) is error, set unit number %d !\n", usb_ghmsc_MaxLUN[mess->ip], USB_MAXUNITNUM);
            usb_ghmsc_MaxLUN[mess->ip] = USB_MAXUNITNUM - 1u;
        }
        else
        {
            USB_PRINTF1(" Unit number is %d\n", usb_ghmsc_MaxLUN[mess->ip]);
        }

        drive_no = R_usb_hmsc_ref_drvno( mess->ip, addr );

        /* Set pipe information */
        offset  = (uint16_t)( 2u * USB_EPL * drive_no );
        usb_ghmsc_PipeTable[mess->ip][offset + 3u] |= (uint16_t)(addr << USB_DEVADDRBIT);
        usb_ghmsc_PipeTable[mess->ip][(offset + 3u) + USB_EPL] |= (uint16_t)(addr << USB_DEVADDRBIT);

        /* Check connection */
        USB_PRINTF0("\nPlease wait device ready\n");
        usb_cpu_DelayXms(100);
        /* Drive yes */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][0] = USB_YES;
        /* Device address */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][3] = addr;
        usb_ghmsc_DriveChk[USB_MAXDRIVE][5] = mess->ip;

        /* Device number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][4] = drive_no;

        usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    case USB_SEQ_2:
        addr = usb_ghmsc_RootDevaddr[mess->ip];
        /* Unit Number */
        usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)usb_shmsc_StrgDriveSearchCount[mess->ip];
        /* Inquiry */
        resultk = R_usb_hmsc_Inquiry(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data[mess->ip]);
        usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount[mess->ip]] = USB_PDT_UNKNOWN;
        
        usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;
    case USB_SEQ_3:
        addr = usb_ghmsc_RootDevaddr[mess->ip];
        resultk = mess -> result;
        if( resultk == USB_HMSC_OK )
        {
            usb_shmsc_DeviceReady[usb_shmsc_StrgDriveSearchCount[mess->ip]] = usb_ghmsc_Data[mess->ip][0];
/* Condition compilation by the difference of quality control */
 #ifdef USB_DEBUGPRINT_PP
            /* Unit number */
            for( i = (uint32_t)0; i < (uint32_t)8; i++ )
            {
                pdata[i] = usb_ghmsc_Data[mess->ip][i + (uint32_t)8];
            }
            USB_PRINTF1("\n    Unit number %d .\n", usb_shmsc_StrgDriveSearchCount[mess->ip]);
            pdata[8] = 0;
            USB_PRINTF1("    Vender Identification : %s\n", pdata);
            /* Product Identification */
            for( i = (uint32_t)0; i < (uint32_t)16; i++ )
            {
                pdata[i] = usb_ghmsc_Data[mess->ip][i + (uint32_t)16];
            }
            pdata[16] = 0;
            USB_PRINTF1("    Product Identification : %s\n", pdata);
 #endif /* USB_DEBUGPRINT_PP*/
            usb_shmsc_StrgDriveSearchErrCount[mess->ip] = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount[mess->ip]++;
            usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount[mess->ip] > usb_ghmsc_MaxLUN[mess->ip] )
            {
                usb_shmsc_StrgDriveSearchCount[mess->ip] = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_4;
            }

        }
        else if( resultk == USB_HMSC_CSW_ERR )
        {
            /* Inquiry error */
            USB_PRINTF1("### inquiry error ( %d times )\n", (usb_shmsc_StrgDriveSearchErrCount[mess->ip] + 1));
            usb_shmsc_StrgDriveSearchErrCount[mess->ip]++;
            usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_2;
            if( usb_shmsc_StrgDriveSearchErrCount[mess->ip] >= 3 )
            {
                usb_shmsc_StrgDriveSearchErrCount[mess->ip] = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchCount[mess->ip]++;
                if( usb_shmsc_StrgDriveSearchCount[mess->ip] > usb_ghmsc_MaxLUN[mess->ip] )
                {
                    usb_shmsc_StrgDriveSearchCount[mess->ip] = USB_SEQ_0;
                    usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_4;
                }
            }
        }
        else
        {
            USB_PRINTF0("### inquiry error\n");

            usb_shmsc_StrgDriveSearchErrCount[mess->ip] = USB_SEQ_0;
            usb_shmsc_StrgDriveSearchCount[mess->ip]++;
            usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_2;

            if( usb_shmsc_StrgDriveSearchCount[mess->ip] > usb_ghmsc_MaxLUN[mess->ip] )
            {
                usb_shmsc_StrgDriveSearchCount[mess->ip] = USB_SEQ_0;
                usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_4;
            }
        }
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;

    case USB_SEQ_4:
        /* Read Format Capacity */
        R_usb_hmsc_ReadFormatCapacity(mess, USB_MAXDRIVE
            , (uint8_t*)&usb_ghmsc_Data[mess->ip]);
        usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_5:
        /* Read Capacity */
        R_usb_hmsc_ReadCapacity(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data[mess->ip]);
        usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        break;

    case USB_SEQ_6:
        resultk = mess -> result;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit(mess, USB_MAXDRIVE);
            usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Pass TestUnitReady  */
            usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_8;
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        };
        break;

    case USB_SEQ_7:
        resultk = mess -> result;
        if( resultk != USB_HMSC_OK )
        {
            /* TestUnitReady */
            R_usb_hmsc_TestUnit(mess, USB_MAXDRIVE);
            usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_7;
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        }
        else
        {
            /* Read Capacity */
            R_usb_hmsc_ReadCapacity(mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data[mess->ip]);
            usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
        };
        break;

    case USB_SEQ_8:
        addr = usb_ghmsc_RootDevaddr[mess->ip];
        /* Read & set partition information */
        USB_PRINTF0("\nPartition information\n");
        result = 0;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN[mess->ip]; j++ )
        {
            /* Set sector size & block address */
            switch( usb_shmsc_DeviceReady[j] )
            {
            case USB_PDT_DIRECT:
                USB_PRINTF1("    Unit %d is direct access device.\n", j);
                /* Unit Number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = (uint16_t)j;
                offset = usb_hmsc_SmpDevReadPartition(mess, (uint16_t)j, (uint32_t)512);
                result++;                                   
                break;
            case USB_PDT_SEQUENTIAL:
                /* Not support: Sequential device */
                USB_PRINTF1("### Unit %d sequential device.(not support)\n", j);
                break;
            case USB_PDT_WRITEONCE:
                /* Not support: Write once device */
                USB_PRINTF1("### Unit %d write once device.(not support)\n", j);
                break;
            case USB_PDT_CDROM:
                /* Not support: CD-ROM device */
                USB_PRINTF1("### Unit %d CD-ROM device.(not support)\n", j);
                break;
            case USB_PDT_OPTICAL:
                /* Not support: Optivasl device */
                USB_PRINTF1("### Unit %d optivasl device.(not support)\n", j);
                break;
            case USB_PDT_UNKNOWN:
                /* Not support: Unknown device */
                USB_PRINTF1("### Unit %d unknown device.(not support)\n", j);
                break;
            default:
                /* Not support: Not direct access device */
                USB_PRINTF1("### Unit %d is not direct access device.(not support)\n", j);
                break;
            }
        }
        usb_shmsc_StrgDriveSearchSeq[mess->ip]++;
        if( result == 0 )
        {
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);

        }   
        break;

    case USB_SEQ_9:
        addr = usb_ghmsc_RootDevaddr[mess->ip];
        new_count = USB_ERROR;
        for( j = (uint32_t)0; j <= usb_ghmsc_MaxLUN[mess->ip]; j++ )
        {
            /* Set sector size & block address */
            if( usb_shmsc_DeviceReady[j] == USB_PDT_DIRECT )
            {
                new_count = mess->result;
            }
        }
        if( new_count == USB_DONE )
        {
            usb_ghmsc_StrgCount++;
        }
        (usb_shmsc_command_result[mess->ip])( (USB_UTR_t *)mess, addr, 0 );

        usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_0;
        usb_shmsc_StrgProcess[mess->ip] = USB_NONE;
        break;
    default:
        usb_shmsc_StrgProcess[mess->ip] = USB_NONE;
        usb_shmsc_StrgDriveSearchSeq[mess->ip] = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchCount[mess->ip] = USB_SEQ_0;
        usb_shmsc_StrgDriveSearchErrCount[mess->ip] = USB_SEQ_0;
        break;
    }
}   /* eof usb_hmsc_StrgDriveSearchAct() */

/******************************************************************************
Function Name   : usb_hmsc_strg_user_command_result
Description     : Storage drive search
Arguments       : USB_CLSINFO_t *mess   : Message
Return value    : none
******************************************************************************/
void usb_hmsc_strg_user_command_result(USB_CLSINFO_t *mess)
{
    (usb_shmsc_command_result[mess->ip])( (USB_UTR_t *)mess, 0, 0 );
}   /* eof usb_hmsc_strg_user_command_result() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartition
Description     : Searches drive SndMsg
Arguments       : uint16_t unit        : Unit
                : uint32_t trans_byte  : Trans byte
Return value    : uint16_t
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartition(USB_UTR_t *ptr, uint16_t unit, uint32_t trans_byte)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    usb_shmsc_DevReadPartitionSeq[ptr->ip] = USB_SEQ_0;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HSTRG_MPL, &p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = USB_MSG_HMSC_DEV_READ_PARTITION;
        cp->keyword = unit;
        usb_ghmsc_TransSize[ptr->ip] = trans_byte;

        cp->ip      = ptr->ip;
        cp->ipp     = ptr->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### DevReadSectorSize function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### DevReadSectorSize function pget_blk error\n");
    }   
    return (USB_DONE);
}   /* eof usb_hmsc_SmpDevReadPartition() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevReadPartitionAct
Description     : Drive read partition
Arguments       : USB_CLSINFO_t *mess : Message
Return value    : uint16_t            : [USB_DONE/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevReadPartitionAct(USB_CLSINFO_t *mess)
{
    uint32_t    i;
    uint16_t    result;
    uint16_t    new_drive, parcount = 0;
    uint16_t    unit;
    uint8_t     partition_info[USB_BOOTPARTNUM];
    uint32_t    partition_lba[USB_BOOTPARTNUM + 1u];
    uint32_t    trans_byte;

    trans_byte = usb_ghmsc_TransSize[mess->ip];
    new_drive = usb_shmsc_NewDrive[mess->ip];

    switch( usb_shmsc_DevReadPartitionSeq[mess->ip] )
    {
    case USB_SEQ_0:
        if( usb_shmsc_LoopCont[mess->ip] == USB_SEQ_0 )
        {
            usb_shmsc_Unit[mess->ip] = mess->keyword;
            usb_ghmsc_PartTransSize[mess->ip] = trans_byte;
        }
        else
        {
            trans_byte = usb_ghmsc_PartTransSize[mess->ip];
        }
        partition_lba[0]                = (uint32_t)0;
        partition_lba[USB_BOOTPARTNUM]  = (uint32_t)0;
    
        /* set drive number */
        new_drive = usb_hmsc_SmpDevNextDriveSearch(mess);
        usb_shmsc_NewDrive[mess->ip] = new_drive;

        /* Read10 */
        result = R_usb_hmsc_Read10(
                    mess, USB_MAXDRIVE, (uint8_t*)&usb_ghmsc_Data[mess->ip], partition_lba[0], (uint16_t)1, trans_byte);
        usb_shmsc_DevReadPartitionSeq[mess->ip]++;
        usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_DEV_READ_PARTITION;
        break;
    case USB_SEQ_1:
        unit = usb_shmsc_Unit[mess->ip];
        usb_shmsc_DevReadPartitionSeq[mess->ip] = USB_SEQ_0;

        if( mess -> result == USB_HMSC_OK )
        {
            /* Check boot record */
            result = usb_hmsc_SmpDevCheckBootRecord((uint8_t*)&usb_ghmsc_Data[mess->ip],
                        (uint32_t*)&partition_lba, (uint8_t*)&partition_info, (uint16_t)0 );
            /* Display partition information */
            if( result != (uint16_t)USB_BOOT_ERROR )
            {
                result = USB_DONE;

                for( i = (uint32_t)0; i < (uint32_t)USB_BOOTPARTNUM; i++ )
                {
                    switch( partition_info[i] )
                    {
                    case USB_PT_FAT12:
                    case USB_PT_FAT16:
                    case USB_PT_FAT32:
                        USB_PRINTF2("    Partition %d open. SIDE %d !\n", i, new_drive);
                        /* Drive yes */
                        usb_ghmsc_DriveChk[new_drive][0] = USB_YES;
                        /* Unit Number */
                        usb_ghmsc_DriveChk[new_drive][1] = unit;
                        /* Partition Number */
                        usb_ghmsc_DriveChk[new_drive][2] = parcount;
                        /* Device address */
                        usb_ghmsc_DriveChk[new_drive][3] = usb_ghmsc_DriveChk[USB_MAXDRIVE][3];
                        /* Endpoint table offset */
                        usb_ghmsc_DriveChk[new_drive][4] = usb_ghmsc_DriveChk[USB_MAXDRIVE][4];
                        /* USB IP No. */
                        usb_ghmsc_DriveChk[new_drive][5] = usb_ghmsc_DriveChk[USB_MAXDRIVE][5];

                        usb_ghmsc_MaxDrive++;
                        if( usb_ghmsc_MaxDrive == USB_MAXDRIVE )
                        {
                            USB_PRINTF1("    Max drive over %d .\n", usb_ghmsc_MaxDrive );
                            i = (uint32_t)USB_BOOTPARTNUM;
                        }
                        else
                        {
                            /* Next drive search */
                            new_drive = usb_hmsc_SmpDevNextDriveSearch(mess);
                            usb_shmsc_NewDrive[mess->ip] = new_drive;
                        }
                        parcount++;
                        break;
                    case USB_PT_EPRT:
                        USB_PRINTF1("    Extended partition %d. !\n", i);
                        if( partition_lba[USB_BOOTPARTNUM] == (uint32_t)0 )
                        {
                            /* Master Boot */
                            partition_lba[USB_BOOTPARTNUM] = partition_lba[i];
                            /* Next EMBR sector */
                            partition_lba[0] = partition_lba[i];
                        }
                        else
                        {
                            /* Next EBMR sector */
                            partition_lba[0] = partition_lba[i] + partition_lba[USB_BOOTPARTNUM];
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                /* Drive read error */
                USB_PRINTF2("### %d drive read error  ( %d times ).\n", new_drive, 0);
                result = USB_ERROR;
            }
        }
        else
        {
            /* Drive read error */
            USB_PRINTF2("### %d drive read error  ( %d times ).\n", new_drive, 0);
            usb_shmsc_LoopCont[mess->ip]++;
            result = (uint16_t)USB_EMBR_ADDR;
            if( usb_shmsc_LoopCont[mess->ip] == (uint32_t)10 )
            {
                result = USB_ERROR;
                usb_shmsc_LoopCont[mess->ip] = USB_SEQ_0;
            }
        }

        if( result != (uint16_t)USB_EMBR_ADDR )
        {
            usb_shmsc_StrgProcess[mess->ip] = USB_MSG_HMSC_STRG_DRIVE_SEARCH;
            mess->result = result;
            usb_shmsc_LoopCont[mess->ip] = USB_SEQ_0;
        }
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    default:
        usb_shmsc_DevReadPartitionSeq[mess->ip] = USB_SEQ_0;
        mess->result = USB_ERROR;
        usb_hmsc_StrgSpecifiedPath((USB_CLSINFO_t *)mess);
        break;
    }
    return 0;
}   /* eof usb_hmsc_SmpDevReadPartitionAct() */


/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hmsc_GetStringDesc(ptr, devaddr, (uint16_t)0, complete);

    return  USB_DONE;
}   /* eof usb_hmsc_GetStringDescriptor1() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2
Description     : Get String descriptor
Arguments       : uint16_t devaddr          : device address
                : uint16_t index            : descriptor index
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2(USB_UTR_t *ptr, uint16_t devaddr, uint16_t index, USB_CB_t complete)
{
    usb_hmsc_GetStringDesc(ptr, devaddr, index, complete);

    return  USB_DONE;
}   /* eof usb_hmsc_GetStringDescriptor2() */

/******************************************************************************
Function Name   : usb_hmsc_StrgSpecifiedPath
Description     : Next Process Selector
Arguments       : USB_CLSINFO_t *mess  : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgSpecifiedPath(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess[mess->ip];
        cp->keyword = mess->keyword;
        cp->result  = mess->result;

        cp->ip      = mess->ip;
        cp->ipp     = mess->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = R_USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
}   /* eof usb_hmsc_StrgSpecifiedPath() */

/******************************************************************************
Function Name   : usb_hmsc_StrgCheckResult
Description     : Hub class check result
Arguments       : USB_UTR_t *mess    : Message
Return value    : none
******************************************************************************/
void usb_hmsc_StrgCheckResult(USB_UTR_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;
    
    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_HSTRG_MPL,&p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = usb_shmsc_StrgProcess[mess->ip];
        cp->keyword = mess->keyword;
        cp->result  = mess->status;

        cp->ip      = mess->ip;
        cp->ipp     = mess->ipp;

        /* Send message */
        err = R_USB_SND_MSG( USB_HSTRG_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            err = USB_REL_BLK(USB_HSTRG_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### CheckResult function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### CheckResult function pget_blk error\n");
    }
}   /* eof usb_hmsc_StrgCheckResult() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDesc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : USB_CB_t complete       : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string
            , USB_CB_t complete)
{
    uint16_t        i;

    if( string == 0 )
    {
        usb_shmsc_ClassRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_shmsc_ClassRequest[ptr->ip][3] = (uint16_t)0x0004;
    }
    else
    {
        /* Set LanguageID */
        usb_shmsc_ClassRequest[ptr->ip][2] = (uint16_t)(usb_ghmsc_ClassData[ptr->ip][2]);
        usb_shmsc_ClassRequest[ptr->ip][2] |= (uint16_t)((uint16_t)(usb_ghmsc_ClassData[ptr->ip][3]) << 8);
        usb_shmsc_ClassRequest[ptr->ip][3] = (uint16_t)USB_HMSC_CLSDATASIZE;
    }
    usb_shmsc_ClassRequest[ptr->ip][0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_shmsc_ClassRequest[ptr->ip][1]   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
    usb_shmsc_ClassRequest[ptr->ip][4]   = addr;

    for( i = 0; i < usb_shmsc_ClassRequest[ptr->ip][3]; i++ )
    {
        usb_ghmsc_ClassData[ptr->ip][i]  = (uint8_t)0xFF;
    }

    return usb_hmsc_CmdSubmit(ptr, complete);
}   /* eof usb_hmsc_GetStringDesc() */

/******************************************************************************
Function Name   : usb_hmsc_CmdSubmit
Description     : command submit
Arguments       : USB_CB_t complete         : callback info
Return value    : uint16_t                  : USB_DONE
******************************************************************************/
uint16_t usb_hmsc_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete)
{
    usb_shmsc_ClassControl[ptr->ip].tranadr  = (void *)usb_ghmsc_ClassData[ptr->ip];
    usb_shmsc_ClassControl[ptr->ip].complete = complete;
    usb_shmsc_ClassControl[ptr->ip].tranlen  = (uint32_t)usb_shmsc_ClassRequest[ptr->ip][3];
    usb_shmsc_ClassControl[ptr->ip].keyword  = USB_PIPE0;
    usb_shmsc_ClassControl[ptr->ip].setup    = usb_shmsc_ClassRequest[ptr->ip];
    usb_shmsc_ClassControl[ptr->ip].segment  = USB_TRAN_END;

    usb_shmsc_ClassControl[ptr->ip].ip   = ptr->ip;
    usb_shmsc_ClassControl[ptr->ip].ipp  = ptr->ipp;

    usb_hstd_TransferStart(&usb_shmsc_ClassControl[ptr->ip]);
    
    return USB_DONE;
}   /* eof usb_hmsc_CmdSubmit() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDevCheckBootRecord
Description     : Device check boot record
Arguments       : uint8_t  *data      : Data
                : uint32_t *par_lba   : Par LBA
                : uint8_t  *par_info   : Par info
                : uint16_t flag       : Flag
Return value    : uint16_t            : Error code [USB_DONE/USB_ERROR]
******************************************************************************/
uint16_t usb_hmsc_SmpDevCheckBootRecord(uint8_t *data, uint32_t *par_lba, uint8_t *par_info, uint16_t flag)
{
    USB_MBR_t       *mbr_data;
    USB_PBR_t       *pbr_data;
    USB_FAT1216_t   *fat1216;
    USB_PTBL_t      *partition;
    uint16_t        fat_sector, i, embr_flag;
    uint32_t        total_sector32, dummy;

    mbr_data = (USB_MBR_t*)data;
    pbr_data = (USB_PBR_t*)data;

    /* BOOT Recorder ? */
    dummy   = (uint32_t)(pbr_data->Signature[0]);
    dummy   |= (uint32_t)((uint16_t)(pbr_data->Signature[1]) << 8);
    if( dummy != (uint32_t)USB_BOOTRECORD_SIG )
    {
        par_info[0] = USB_PT_NONE;
        USB_PRINTF1("    USB_BOOTRECORD_SIG error 0x%04x\n", dummy);
        return USB_BOOT_ERROR;
    }

    embr_flag = USB_PT_NONE;

    /* MBR check (Partition n) */
    for( i = 0; i < USB_BOOTPARTNUM; i++ )
    {
        partition   = (USB_PTBL_t*)&(mbr_data->PartitionTable[i * 16]);
        par_info[i] = USB_PT_NONE;
        par_lba[i]  = (uint32_t)(partition->StartSectorNum[0]);
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[1]) << 8;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[2]) << 16;
        par_lba[i]  |= (uint32_t)(partition->StartSectorNum[3]) << 24;
        switch( partition->PartitionType )
        {
        case USB_PT_NONE:
            break;
        case USB_PT_EPRTA:
        case USB_PT_EPRTB:
            embr_flag   = USB_EMBR_ADDR;
            par_info[i] = USB_PT_EPRT;
            break;
        case USB_PT_FAT12A:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT12;
            break;
        case USB_PT_FAT16A:
        case USB_PT_FAT16B:
        case USB_PT_FAT16X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT16;
            break;
        case USB_PT_FAT32A:
        case USB_PT_FAT32X:
            if( embr_flag == USB_PT_NONE )
            {
                embr_flag = USB_MBR_ADDR;
            }
            par_info[i] = USB_PT_FAT32;
            break;
        default:
            if( flag != 0 )
            {
                USB_PRINTF1("    Partition type not support 0x%02x\n", partition->PartitionType);
            }
            break;
        }
    }

    switch( embr_flag )
    {
    case USB_MBR_ADDR:
    case USB_EMBR_ADDR:
        return embr_flag;
        break;
    default:
        break;
    }

    /* PBR check */
    fat1216     = (USB_FAT1216_t*)&(pbr_data->FATSigData);

    fat_sector      =  (uint16_t)(pbr_data->FATSector[0]);
    fat_sector      |= (uint16_t)((uint16_t)(pbr_data->FATSector[1]) << 8);
    total_sector32  =  (uint32_t)(pbr_data->TotalSector1[0]);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[1]) << 8);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[2]) << 16);
    total_sector32  |= ((uint32_t)(pbr_data->TotalSector1[3]) << 24);

    if( ((pbr_data->JMPcode == USB_JMPCODE1) &&
        (pbr_data->NOPcode == USB_NOPCODE)) ||
        (pbr_data->JMPcode == USB_JMPCODE2) )
    {
        if( fat_sector == 0 )
        {
            if( total_sector32 != (uint32_t)0 )
            {
                par_info[0] = USB_PT_FAT32; /* FAT32 spec */
            }
        }
        else
        {
            if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x32) )
            {
                par_info[0] = USB_PT_FAT12; /* FAT12 spec */
            }
            else if( (fat1216->FileSystemType[3] == 0x31) && (fat1216->FileSystemType[4] == 0x36) )
            {
                par_info[0] = USB_PT_FAT16; /* FAT16 spec */
            }
            else
            {
            }
        }
    }

    if( par_info[0] == USB_PT_NONE )
    {
        USB_PRINTF0("    Partition error\n");
        return USB_BOOT_ERROR;
    }
    else
    {
        return USB_PBR_ADDR;
    }
}   /* eof usb_hmsc_SmpDevCheckBootRecord() */

/******************************************************************************
Function Name   : usb_hmsc_SmpFsiDriveClear
Description     : Device check boot record
Arguments       : uint16_t addr       : Address
Return value    : none
******************************************************************************/
void usb_hmsc_SmpFsiDriveClear(USB_UTR_t *ptr, uint16_t addr)
{
    uint16_t    i, offset, msgnum;
    uint16_t    find = USB_NO;
    uint16_t    ip;

    ip = ptr->ip;
    for( i = 0; i < USB_MAXDRIVE; i++ )
    {
        if( (usb_ghmsc_DriveChk[i][3] == addr) && (usb_ghmsc_DriveChk[i][5] == ip) )
        {
            msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, i);
            if( USB_ERROR != msgnum )
            {
                offset = (uint16_t)(2u * USB_EPL * msgnum);

                usb_ghmsc_PipeTable[ip][offset +1]  &= (USB_BFREON | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON); /* PIPECFG */
                usb_ghmsc_PipeTable[ip][offset +3]  = USB_NONE;     /* PIPEMAXP */
                usb_ghmsc_PipeTable[ip][offset +4]  = USB_NONE;     /* PIPEPERI */
                offset += USB_EPL;

                usb_ghmsc_PipeTable[ip][offset +1]  &= (USB_BFREON | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON); /* PIPECFG */
                usb_ghmsc_PipeTable[ip][offset +3]  = USB_NONE;     /* PIPEMAXP */
                usb_ghmsc_PipeTable[ip][offset +4]  = USB_NONE;     /* PIPEPERI */

                usb_ghmsc_DriveChk[i][0] = USB_NO;                  /* Yes/No */
                usb_ghmsc_DriveChk[i][1] = 0;                       /* Unit Number */
                usb_ghmsc_DriveChk[i][2] = 0;                       /* Partition Number */
                usb_ghmsc_DriveChk[i][3] = 0;                       /* Device address */
                usb_ghmsc_DriveChk[i][4] = 0;                       /* Device number */
                usb_ghmsc_DriveChk[i][5] = 0;                       /* USB IP number */
                usb_ghmsc_MaxDrive--;
                find = USB_YES;
            }
        }
    }

    if( find == USB_NO )
    {
        if( (usb_ghmsc_DriveChk[USB_MAXDRIVE][3] == addr) && (usb_ghmsc_DriveChk[i][5] == ip) )
        {
            msgnum = usb_hmsc_SmpDrive2Msgnum(ptr, USB_MAXDRIVE);
            if( USB_ERROR != msgnum )
            {
                offset = (uint16_t)(2u * USB_EPL * msgnum);

                usb_ghmsc_PipeTable[ip][offset +1]  &= (USB_BFREON | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON); /* PIPECFG */
                usb_ghmsc_PipeTable[ip][offset +3]  = USB_NONE;     /* PIPEMAXP */
                usb_ghmsc_PipeTable[ip][offset +4]  = USB_NONE;     /* PIPEPERI */
                offset += USB_EPL;

                usb_ghmsc_PipeTable[ip][offset +1]  &= (USB_BFREON | USB_DBLBON | USB_CNTMDON | USB_SHTNAKON); /* PIPECFG */
                usb_ghmsc_PipeTable[ip][offset +3]  = USB_NONE;     /* PIPEMAXP */
                usb_ghmsc_PipeTable[ip][offset +4]  = USB_NONE;     /* PIPEPERI */

                usb_ghmsc_DriveChk[USB_MAXDRIVE][0] = USB_NO;       /* Yes/No */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][1] = 0;            /* Unit Number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][2] = 0;            /* Partition Number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][3] = 0;            /* Device address */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][4] = 0;            /* Device number */
                usb_ghmsc_DriveChk[USB_MAXDRIVE][5] = 0;            /* USB IP number */
            }
        }
    }

    if( usb_ghmsc_StrgCount != 0 )
    {
        usb_ghmsc_StrgCount--;
    }

    usb_ghmsc_OutPipe[ip][msgnum][0]    = USB_NOPORT;           /* Pipe initial */
    usb_ghmsc_OutPipe[ip][msgnum][1]    = 0;                    /* Toggle clear */
    usb_ghmsc_InPipe[ip][msgnum][0]     = USB_NOPORT;
    usb_ghmsc_InPipe[ip][msgnum][1]     = 0;

    if( usb_ghmsc_StrgCount == 0 )
    {
        R_usb_hmsc_TaskClose(ptr);
    }
}   /* eof usb_hmsc_SmpFsiDriveClear() */


/******************************************************************************
Function Name   : usb_hmsc_SmpTotalDrive
Description     : Total drive information
Arguments       : none
Return value    : uint16_t             : Max drive
******************************************************************************/
uint16_t usb_hmsc_SmpTotalDrive(void)
{
    return usb_ghmsc_MaxDrive;
}   /* eof usb_hmsc_SmpTotalDrive() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Unit
Description     : Total drive information
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Unit number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Unit(USB_UTR_t *ptr, uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        USB_PRINTF3("### Drive %d is not opened.   Unit=%d, Partition=%d !\n",
                side, usb_ghmsc_DriveChk[side][1], usb_ghmsc_DriveChk[side][2]);
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][1]);           /* Unit Number */
}   /* eof usb_hmsc_SmpDrive2Unit() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Part
Description     : Retrieves partition number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Partition number
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Part(USB_UTR_t *ptr, uint16_t side)
{
    if( usb_ghmsc_DriveChk[side][0] != USB_YES )
    {
        USB_PRINTF3("### Drive %d is not opened.   Unit=%d, Partition=%d !\n"
            , side, usb_ghmsc_DriveChk[side][1], usb_ghmsc_DriveChk[side][2]);
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][2]);   /* Parttition Number */
}   /* eof usb_hmsc_SmpDrive2Part() */

/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Addr
Description     : Retrieves device address
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Device address
******************************************************************************/
void usb_hmsc_SmpDrive2Addr(uint16_t side, USB_UTR_t *devadr)
{
    devadr->keyword = usb_ghmsc_DriveChk[side][3];   /* Device Address */
    devadr->ip     = usb_ghmsc_DriveChk[side][5];   /* USB IP No.     */
    devadr->ipp    = R_usb_cstd_GetUsbIpAdr(devadr->ip);
}   /* eof usb_hmsc_SmpDrive2Addr() */


/******************************************************************************
Function Name   : usb_hmsc_SmpDrive2Msgnum
Description     : Checks drive number
Arguments       : uint16_t side        : Side
Return value    : uint16_t             : Drive address
******************************************************************************/
uint16_t usb_hmsc_SmpDrive2Msgnum(USB_UTR_t *ptr, uint16_t side)
{
    if( USB_NO == usb_ghmsc_DriveChk[side][0] )
    {
        return USB_ERROR;
    }
    return (usb_ghmsc_DriveChk[side][4]);
}   /* eof usb_hmsc_SmpDrive2Msgnum() */

/******************************************************************************
Function Name   : usb_hmsc_ClassWait
Description     : HMSC Class Wait
Arguments       : USB_UTR_t *mess : 
Return value    : none
******************************************************************************/
void usb_hmsc_ClassWait(USB_ID_t id, USB_UTR_t *mess)
{
    USB_ER_t        err;                    /* Error code */
    uint16_t        mode, tbl[10];

    R_usb_hstd_DeviceInformation(mess, 0, (uint16_t *)&tbl);

    if( mess->keyword == 0 )
    {
        mode = tbl[9];      /* PORT1 */
    }
    else
    {
        mode = tbl[8];      /* PORT0 */
    }
    if( mode != USB_DEFAULT )
    {
        mess->msginfo = USB_MSG_MGR_AORDETACH;
        err = R_USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Strg enuwait snd_msg error\n");
        }
    }
    else
    {
        err = R_USB_SND_MSG(id, (USB_MSG_t*)mess);
        if( err != USB_E_OK )
        {
            USB_PRINTF0("### USB Strg enuwait snd_msg error\n");
        }
    }
}   /* eof usb_hmsc_ClassWait() */

/******************************************************************************
Function Name   : usb_hmsc_StdReqCheck
Description     : Sample Standard Request Check
Arguments       : uint16_t errcheck        : error
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hmsc_StdReqCheck(uint16_t errcheck)
{
    if( errcheck == USB_DATA_TMO )
    {
        USB_PRINTF0("*** Standard Request Timeout error !\n");
        return  USB_ERROR;
    }
    else if( errcheck == USB_DATA_STALL )
    {
        USB_PRINTF0("*** Standard Request STALL !\n");
        return  USB_ERROR;
    }
    else if( errcheck != USB_CTRL_END )
    {
        USB_PRINTF0("*** Standard Request error !\n");
        return  USB_ERROR;
    }
    else
    {
    }
    return  USB_DONE;
}   /* eof usb_hmsc_StdReqCheck() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor1Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor1Check(USB_UTR_t *ptr, uint16_t errcheck)
{
    if( errcheck == (USB_ER_t)USB_DATA_STALL )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else if( errcheck != (USB_ER_t)USB_CTRL_END )
    {
        USB_PRINTF0("*** LanguageID  not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return  USB_DONE;
}   /* eof usb_hmsc_GetStringDescriptor1Check() */

/******************************************************************************
Function Name   : usb_hmsc_GetStringDescriptor2Check
Description     : Get String descriptor Check
Arguments       : uint16_t errcheck         : errcheck
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hmsc_GetStringDescriptor2Check(USB_UTR_t *ptr, uint16_t errcheck)
{
    if( errcheck == (USB_ER_t)USB_DATA_STALL )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else if( errcheck != (USB_ER_t)USB_CTRL_END )
    {
        USB_PRINTF0("*** SerialNumber not support !\n");
        return USB_ERROR;
    }
    else
    {
    }

    return USB_DONE;
}   /* eof usb_hmsc_GetStringDescriptor2Check() */


/******************************************************************************
Function Name   : usb_hmsc_SmpFsiSectorInitialized
Description     : Initialized global area
Arguments       : int side : drive number
                : uint32_t offset : buffer address
                : uint16_t size : sector size
Return value    : none
******************************************************************************/
void usb_hmsc_SmpFsiSectorInitialized(uint16_t side, uint32_t offset,
     uint16_t size)
{
    USB_MEDIA_INITIALIZE(side);
}   /* eof usb_hmsc_SmpFsiSectorInitialized() */

/******************************************************************************
Function Name   : usb_hmsc_SmpFsiOffsetSectorRead
Description     : Offset Sector Read
Arguments       : int side : drive number
                : uint16_t size : sector size
Return value    : uint32_t offset_sector[side]
******************************************************************************/
uint32_t usb_hmsc_SmpFsiOffsetSectorRead(uint16_t side)
{
    return (uint32_t)0uL;
}   /* eof usb_hmsc_SmpFsiOffsetSectorRead() */

/******************************************************************************
Function Name   : usb_hmsc_SmpFsiFileSystemInitialized
Description     : Initialized global area
Arguments       : uint16_t side    : drive number
                : uint8_t *Data    : partition table
                : uint32_t Offset  : offset address
Return value    : uint16_t DONE
******************************************************************************/
uint16_t usb_hmsc_SmpFsiFileSystemInitialized(uint16_t side, uint8_t *Data,
     uint32_t Offset)
{
    return USB_DONE;
}   /* eof usb_hmsc_SmpFsiFileSystemInitialized() */


/******************************************************************************
End  Of File
******************************************************************************/
