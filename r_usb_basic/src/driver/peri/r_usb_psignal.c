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
* File Name    : r_usb_psignal.c
* Description  : USB Peripheral signal control code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_reg_access.h"


#if (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP)

/******************************************************************************
External variables and functions
******************************************************************************/
extern  uint16_t    usb_gpstd_intsts0;

uint16_t usb_pstd_InitFunction(USB_UTR_t *ptr);

/******************************************************************************
Renesas Abstracted Peripheral signal control functions
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DpEnable
Description     : D+ Line Pull-up Enable
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_DpEnable(USB_UTR_t *ptr)
{

    usb_preg_set_dprpu( ptr );

}
/******************************************************************************
End of function usb_pstd_DpEnable
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DpDisable
Description     : D+ Line Pull-up Disable
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
**************************************************************************/
void usb_pstd_DpDisable(USB_UTR_t *ptr)
{

    usb_preg_clr_dprpu( ptr );

}
/******************************************************************************
End of function usb_pstd_DpDisable
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_BusReset
Description     : A USB bus reset was issued by the host. Execute relevant pro-
                : cessing.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_BusReset(USB_UTR_t *ptr)
{
    uint16_t    connect_info;

    /* Bus Reset */
    usb_pstd_BusresetFunction(ptr);

    /* Memory clear */
    usb_pstd_ClearMem();
    connect_info = usb_cstd_PortSpeed(ptr, (uint16_t)USB_PORT0);
    /* Callback */
#ifdef USB_PERI_BC_ENABLE
    (*usb_gpstd_Driver.devdefault)(ptr, connect_info, (uint16_t)g_usb_bc_detect);
#else
    (*usb_gpstd_Driver.devdefault)(ptr, connect_info, (uint16_t)USB_NO_ARG);
#endif
    /* DCP configuration register  (0x5C) */
    usb_creg_write_dcpcfg( ptr, 0 );
    /* DCP maxpacket size register (0x5E) */
    usb_creg_write_dcpmxps( ptr, usb_gpstd_Driver.devicetbl[USB_DEV_MAX_PKT_SIZE]);
}
/******************************************************************************
 End of function usb_pstd_BusReset
 ******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_InitConnect
Description     : Set up interrupts and initialize.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_InitConnect(USB_UTR_t *ptr)
{
    uint16_t        connect_info;

    /* Interrupt enable */
    usb_pstd_InterruptEnable(ptr);
    usb_cstd_SetHse(ptr, (uint16_t)USB_PORT0, usb_gcstd_HsEnable[ptr->ip]);

    usb_creg_set_cnen( ptr );
    connect_info = usb_pstd_InitFunction( ptr );
    
    switch( connect_info )
    {
    /* Attach host controller */
    case USB_ATTACH:
        usb_pstd_AttachProcess(ptr);
        break;
    /* Detach host controller */
    case USB_DETACH:
        usb_pstd_DetachProcess(ptr);
        break;
    default:
        break;
    }
}
/******************************************************************************
End of function usb_pstd_InitConnect
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_AttachProcess
Description     : USB attach setting.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_AttachProcess(USB_UTR_t *ptr)
{
    usb_pstd_AttachFunction( ptr );
    usb_cpu_DelayXms((uint16_t)10);
#ifdef USB_PERI_BC_ENABLE
    usb_pstd_bc_detect_process(ptr);
#endif /* USB_PERI_BC_ENABLE */
    usb_preg_set_dprpu( ptr );

}
/******************************************************************************
End of function usb_pstd_AttachProcess
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_DetachProcess
Description     : Initialize USB registers for detaching, and call the callback
                : function that completes the detach.
Arguments       : USB_UTR_t *ptr    : USB system internal structure.
Return value    : none
******************************************************************************/
void usb_pstd_DetachProcess(USB_UTR_t *ptr)
{
    uint16_t        i, conf;
    uint16_t        *tbl;

    usb_creg_clr_cnen( ptr );
    /* Pull-up disable */
    usb_preg_clr_dprpu( ptr );
    usb_cpu_Delay1us((uint16_t)2);
    usb_creg_set_dcfm( ptr );
    usb_cpu_Delay1us((uint16_t)1);
    usb_creg_clr_dcfm( ptr );

    conf = usb_gpstd_ConfigNum;
    if( conf < (uint16_t)1 )
    {
        /* Address state */
        conf = (uint16_t)1;
    }

    /* Configuration number */
    usb_gpstd_ConfigNum = 0;
    /* Remote wakeup enable flag */
    usb_gpstd_RemoteWakeup = USB_NO;

    /* INTSTS0 clear */
    usb_gpstd_intsts0 = 0;

    tbl = usb_gpstd_Driver.pipetbl[conf - 1];
    for( i = 0; tbl[i] != USB_PDTBLEND; i += USB_EPL )
    {
        usb_cstd_ForcedTermination(ptr, tbl[i], (uint16_t)USB_DATA_STOP);
        usb_cstd_ClrPipeCnfg(ptr, tbl[i]);
    }
    /* Callback */
    (*usb_gpstd_Driver.devdetach)(ptr, (uint16_t)USB_NO_ARG, (uint16_t)USB_NO_ARG);
    usb_cstd_StopClock(ptr);
}
/******************************************************************************
End of function usb_pstd_DetachProcess
******************************************************************************/

/******************************************************************************
Function Name   : usb_pstd_SuspendProcess
Description     : Perform a USB peripheral suspend.
Arguments       : none
Return value    : none
******************************************************************************/
void usb_pstd_SuspendProcess(USB_UTR_t *ptr)
{
    uint16_t    intsts0, buf;

    /* Resume interrupt enable */
    usb_preg_set_enb_rsme( ptr );

    intsts0 = usb_creg_read_intsts( ptr );
    buf = usb_creg_read_syssts( ptr, USB_PORT0 );
    if(((intsts0 & USB_DS_SUSP) != (uint16_t)0) && ((buf & USB_LNST) == USB_FS_JSTS))
    {
        /* Suspend */
        usb_cstd_StopClock(ptr);
        usb_pstd_SuspendFunction(ptr);
        /* Callback */
        (*usb_gpstd_Driver.devsuspend)(ptr, (uint16_t)usb_gpstd_RemoteWakeup, (uint16_t)USB_NO_ARG);
    }
    /* --- SUSPEND -> RESUME --- */
    else
    {
        /* RESM status clear */
        usb_preg_clr_sts_resm( ptr );
        /* RESM interrupt disable */
        usb_preg_clr_enb_rsme( ptr );
    }
}
/******************************************************************************
End of function usb_pstd_SuspendProcess
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_PERI_PP) || (USB_FUNCSEL_USBIP1_PP == USB_PERI_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
