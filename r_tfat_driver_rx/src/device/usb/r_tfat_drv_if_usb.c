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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_tfat_drv_if_usb.c
* Description  : TFAT driver Interface for USB.
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 01.12.2014 1.00     First Release
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include "r_tfat_drv_if_dev.h"
#include "r_tfat_driver_rx_config.h"
#include "r_tfat_rx_config.h"

#if (TFAT_USB_DRIVE_NUM > 0)

#include "r_usb_basic_if.h"
#include "r_usb_hatapi_define.h"    /* Peripheral ATAPI Device extern */
#include "r_tfat_lib.h"             /* TFAT define */

#include "r_usb_hmsc_extern.h"
#include "r_usb_hmsc_api.h"
#include "r_usb_hmsc_config.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
*******************************************************************************/
//static void R_usb_hmsc_WaitLoop(void);
static uint16_t usb_ghmsc_tfatSecSize = 512;

USB_UTR_t tfat_ptr;

/******************************************************************************
* Function Name : R_tfat_usb_disk_initialize
* Description   : This function initializes the memory medium
*               :    for file operations
* Arguments     : uint8_t  drive        : Physical drive number
* Return value  : Status of the memory medium
******************************************************************************/
DSTATUS R_tfat_usb_disk_initialize(uint8_t pdrv)
{
    return  TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_usb_disk_read
* Description   : This function reads data from the specified location
*               :    of the memory medium
* Arguments     : uint8_t  drive        : Physical drive number
*               : uint8_t* buffer       : Pointer to the read data buffer
*               : uint32_t sector_number : uint32_t SectorNumber
*               : uint8_t sector_count   : Number of sectors to read
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_usb_disk_read (
    uint8_t drive,              /* Physical drive number            */
    uint8_t* buffer,            /* Pointer to the read data buffer  */
    uint32_t sector_number,     /* Start sector number              */
    uint8_t sector_count        /* Number of sectors to read        */
)
{
    uint16_t        res[10];
    USB_UTR_t       *mess;
    uint16_t        err;
    uint32_t        tran_byte;

    /* set transfer length */
    tran_byte = (uint32_t)sector_count * usb_ghmsc_tfatSecSize;

    usb_hmsc_SmpDrive2Addr( drive, &tfat_ptr );                 /* Drive no. -> USB IP no. and IO Reg Base address */

    R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res);         /* Get device connect state */
    if ( USB_DETACHED == res[1] )    /* Check detach */
    {
        return TFAT_RES_ERROR;
    }

    /* read function */
    err = R_usb_hmsc_StrgReadSector(&tfat_ptr, (uint16_t)drive, buffer, sector_number
                                    , (uint16_t)sector_count, tran_byte);
    if ( USB_E_OK == err )
    {
#ifndef FREE_RTOS_PP
        do  /* Wait for complete R_usb_hmsc_StrgReadSector() */
        {
            R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res); /* Get device connect state */
            R_usb_hmsc_WaitLoop();                                                      /* Task Schedule */
            err = R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive read complete msg */
        }
        while ( (err != USB_OK) && (res[1] != USB_DETACHED) );
#else /* FREE_RTOS_PP */
            R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res); /* Get device connect state */
            if (USB_DETACHED == res[1])
            {
            	return TFAT_RES_ERROR;
            }
            err = R_USB_TRCV_MSG(USB_TFAT_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive read complete msg */
#endif /* FREE_RTOS_PP */
        if ( err == USB_OK )
        {   /* Complete R_usb_hmsc_StrgReadSector() */
            err = mess->result;                                        /* Set result for R_usb_hmsc_StrgReadSector() */
#ifdef FREE_RTOS_PP
        R_USB_REL_BLK(USB_TFAT_MPL,(USB_MH_t)mess);
#else
            R_USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)mess);
#endif
        }
        else
        {   /* Device detach */
#ifndef FREE_RTOS_PP
            R_usb_hmsc_WaitLoop();
            err = R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive read complete msg */
            if ( USB_OK == err )
            {
                R_USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)mess);
            }
            err = USB_ERROR;
#else
            err = R_USB_TRCV_MSG(USB_TFAT_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive read complete msg */
            if ( USB_OK == err )
            {
                R_USB_REL_BLK(USB_TFAT_MPL, (USB_MH_t)mess);
            }
            err = USB_ERROR;
#endif

        }
    }

    if ( err != USB_OK )
    {
        return TFAT_RES_ERROR;
    }
    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_usb_disk_write
* Description   : This function writes data to a specified location
*               :    of the memory medium
* Arguments     : uint8_t Drive : Physical drive number
*               : const uint8_t* buffer       : Pointer to the write data
*               : uint32_t       sector_number : Sector number to write
*               : uint8_t        sector_count  : Number of sectors to write
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_usb_disk_write (
    uint8_t drive,               /* Physical drive number           */
    const uint8_t* buffer,       /* Pointer to the write data       */
    uint32_t sector_number,      /* Sector number to write          */
    uint8_t sector_count         /* Number of sectors to write      */
)
{
    uint16_t        res[10];
    USB_UTR_t       *mess;
    uint16_t        err;
    uint32_t        tran_byte;

    /* set transfer length */
    tran_byte = (uint32_t)sector_count * usb_ghmsc_tfatSecSize;
    usb_hmsc_SmpDrive2Addr(drive, &tfat_ptr);                   /* Drive no. -> USB IP no. and IO Reg Base address */

    R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res);         /* Get device connect state */
    if ( USB_DETACHED == res[1] )    /* Check detach */
    {
        return TFAT_RES_ERROR;
    }

    /* write function */
    err = R_usb_hmsc_StrgWriteSector(&tfat_ptr, (uint16_t)drive, (uint8_t *)buffer
                                     , sector_number, (uint16_t)sector_count, tran_byte);
    if ( USB_E_OK == err )
    {
 #ifndef FREE_RTOS_PP
        do  /* Wait for complete R_usb_hmsc_StrgWriteSector() */
        {
            R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res); /* Get device connect state */
            R_usb_hmsc_WaitLoop();                                                      /* Task Schedule */
            err = R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );    /* Receive write complete msg */
        }
        while ( ( err != USB_OK ) && ( res[1] != USB_DETACHED ) );
#else /* FREE_RTOS_PP */
		R_usb_hstd_DeviceInformation(&tfat_ptr, tfat_ptr.keyword, (uint16_t *)res); /* Get device connect state */
		if( USB_DETACHED == res[1] )
		{
			return TFAT_RES_ERROR;
		}
		err = R_USB_TRCV_MSG(USB_TFAT_MBX, (USB_MSG_t**)&mess, (uint16_t)0 );    /* Receive write complete msg */
#endif
        if ( USB_OK == err )
        {   /* Complete R_usb_hmsc_StrgWriteSector() */
            err = mess->result;                                     /* Set result for R_usb_hmsc_StrgWriteSector() */
#ifdef FREE_RTOS_PP
            R_USB_REL_BLK(USB_TFAT_MPL,(USB_MH_t)mess);
#else
            R_USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)mess);
#endif
        }
        else
        {   /* Device detach */
#ifndef FREE_RTOS_PP
            R_usb_hmsc_WaitLoop();
            err = R_USB_TRCV_MSG(USB_HSTRG_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive write complete msg */
             if ( err == USB_OK )
             {
                 R_USB_REL_BLK(USB_HSTRG_MPL, (USB_MH_t)mess);
             }
             err = USB_ERROR;
#endif
             err = R_USB_TRCV_MSG(USB_TFAT_MBX, (USB_MSG_t**)&mess, (uint16_t)0);     /* Receive write complete msg */
			  if ( err == USB_OK )
			  {
				  R_USB_REL_BLK(USB_TFAT_MPL, (USB_MH_t)mess);
			  }
			  err = USB_ERROR;
        }
    }

    if ( err != USB_OK )
    {
        return TFAT_RES_ERROR;
    }
    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_usb_disk_ioctl
* Description   : This function is used to execute memory operations
*               :    other than read\write
* Arguments     : uint8_t drive   : Drive number
*               : uint8_t command : Control command code
*               : void*   buffer  : Data transfer buffer
* Return value  : Result of function execution
******************************************************************************/
DRESULT R_tfat_usb_disk_ioctl (
    uint8_t drive,               /* Drive number             */
    uint8_t command,             /* Control command code     */
    void* buffer                 /* Data transfer buffer     */
)
{

    /*  Please put the code for R_tfat_disk_ioctl driver interface
         function over here.  */
    /*  Please refer the application note for details.  */
    return TFAT_RES_OK;
}

/******************************************************************************
* Function Name : R_tfat_usb_disk_status
* Description   : This function is used to retrieve the current status
*               :    of the disk
* Arguments     : uint8_t drive : Physical drive number
* Return value  : Status of the disk
******************************************************************************/
DSTATUS R_tfat_usb_disk_status (
    uint8_t drive                 /* Physical drive number    */
)
{

    /*  Please put the code for R_tfat_disk_status driver interface
         function over here.  */
    /*  Please refer the application note for details.  */
    return TFAT_RES_OK;
}

/******************************************************************************
Function Name   : R_usb_hmsc_WaitLoop
Description     : Hmsc wait loop function
Arguments       : none
Return value    : none
******************************************************************************/
//void R_usb_hmsc_WaitLoop(void)
//{
//#ifdef FREE_RTOS_PP
//    /* empty */
//    /* This function is not used.
//       Nothing is processed in this function. */
//#else
//    if ( R_usb_cstd_CheckSchedule() == USB_FLGSET )
//    {
//        R_usb_hstd_HcdTask((USB_VP_INT)0);
//        R_usb_hstd_MgrTask((USB_VP_INT)0);
//        R_usb_hhub_Task((USB_VP_INT)0);
//        usb_hmsc_Task();
//    }
//    R_usb_cstd_Scheduler();
//#endif
//}

#ifdef FREE_RTOS_PP
/******************************************************************************
* Function Name : R_tfat_disk_read_write_complete
* Include       :
* Declaration   : void R_tfat_disk_read_write_complete(USB_CLSINFO_t *mess)
* Description   : tfat disk read/write completion notice
* Arguments     : USB_CLSINFO_t *
* Return value  : none
* NOTE          :
******************************************************************************/
void R_tfat_disk_read_write_complete(USB_CLSINFO_t *mess)
{
    USB_MH_t            p_blf;
    USB_ER_t            err;
    USB_CLSINFO_t       *cp;

    /* Get mem pool blk */
    if( R_USB_PGET_BLK(USB_TFAT_MPL, &p_blf) == USB_E_OK )
    {
        cp = (USB_CLSINFO_t*)p_blf;
        cp->msginfo = 0;
        cp->result  = mess->result;

        /* Send message */
        err = R_USB_SND_MSG( USB_TFAT_MBX, (USB_MSG_t*)p_blf );
        if( err != USB_E_OK )
        {
            R_USB_REL_BLK(USB_TFAT_MPL,(USB_MH_t)p_blf);
            USB_PRINTF0("### R_tfat_disk_read_write_complete function snd_msg error\n");
        }
    }
    else
    {
        USB_PRINTF0("### R_tfat_disk_read_write_complete function pget_blk error\n");
    }
}   /* eof R_tfat_disk_read_write_complete() */
#endif /* FREE_RTOS_PP */

#endif // (TFAT_USB_DRIVE_NUM > 0)
/******************************************************************************
End  of file
******************************************************************************/
