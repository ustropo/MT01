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
* File Name    : rx_rsk.c
* Description  : RX MCU processing
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Added RX71M.
***********************************************************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <machine.h>
#include "r_usb_basic_if.h"

#ifdef USB_DTC_ENABLE
#include "r_dtc_rx_if.h"
#endif /* USB_DTC_ENABLE */


/******************************************************************************
Constant macro definitions
******************************************************************************/
/* Select Use IP */
#define USB_PswIntDisable               (uint32_t)(7 << 24) /* Processer Status Word - IPL(Level7) */
#define USB_PswIntSleep                 (uint32_t)(1 << 24) /* Processer Status Word - IPL(Level1) */

/******************************************************************************
External variables and functions
******************************************************************************/
extern void     R_usb_pstd_DeviceInformation(USB_UTR_t *ptr, uint16_t *tbl);

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
#ifdef USB_DTC_ENABLE
dtc_transfer_data_cfg_t    usb_td_cfg[2];
dtc_transfer_data_t        usb_dtc_transfer_data[2];
#endif  /* USB_DTC_ENABLE */


/******************************************************************************
Private global variables and functions
******************************************************************************/
uint16_t usb_gcstd_D0fifo[2u] = {0,0};      /* D0fifo0 Interrupt Request enable */
uint16_t usb_gcstd_D1fifo[2u] = {0,0};      /* D1fifo0 Interrupt Request enable */

static bool usb_gstd_is_opened[2] = { false, false };

/*=== SYSTEM ================================================================*/
usb_err_t   usb_cpu_module_start( usb_ip_t ip_type );
usb_err_t   usb_cpu_module_stop( usb_ip_t ip_type );
void        usb_cpu_target_init(void);
/*=== Interrupt =============================================================*/
void        usb_cpu_usbint_init(void);
void        usb_cpu_usb_int_hand(void);
void        usb2_cpu_usb_int_hand(void);
void        usb_cpu_d0fifo_int_hand(void);
void        usb2_cpu_d0fifo_int_hand(void);
void        usb_cpu_int_enable(USB_UTR_t *ptr);
void        usb_cpu_int_disable(USB_UTR_t *ptr);
/*=== TIMER =================================================================*/
void        usb_cpu_Delay1us(uint16_t time);
void        usb_cpu_DelayXms(uint16_t time);

#ifdef USB_DTC_ENABLE
/*=== DMA ===================================================================*/
void        usb_cpu_d0fifo2buf_start_dma(USB_UTR_t *ptr, uint32_t SourceAddr);
void        usb_cpu_buf2d0fifo_start_dma(USB_UTR_t *ptr, uint32_t DistAdr);
void        usb_cpu_d0fifo_restart_dma(USB_UTR_t *ptr);
void        usb_cpu_d0fifo_stop_dma(USB_UTR_t *ptr);
uint16_t    usb_cpu_get_dtc_block_count(USB_UTR_t *ptr);
void        usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr );
void        usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr );
#endif  /* USB_DTC_ENABLE */

#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
#pragma interrupt usb_cpu_usb_int_hand(vect = VECT(USB0, USBI0))
#pragma interrupt usb2_cpu_usb_int_hand(vect = VECT(USBA, USBAR))
#pragma interrupt usb_cpu_d0fifo_int_hand(vect = VECT(USB0, D0FIFO0))
#pragma interrupt usb2_cpu_d0fifo_int_hand(vect = VECT(USBA, D0FIFO2))
#else
#pragma interrupt usb_cpu_usb_int_hand(vect = VECT(USB0, USBI0))
#pragma interrupt usb_cpu_d0fifo_int_hand(vect = VECT(USB0, D0FIFO0))
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
/******************************************************************************
Renesas Abstracted RSK functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_module_start
Description     : USB module start
Arguments       : usb_ip_t ip_type  : USB_IP0/USB_IP1
Return value    : none
******************************************************************************/
usb_err_t usb_cpu_module_start( usb_ip_t ip_type )
{
    if( USB_IP0 == ip_type )
    {
        if(R_BSP_HardwareLock(BSP_LOCK_USB0) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(usb_gstd_is_opened[ip_type] == true)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB0);
            return USB_ERR_OPENED;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Enable power for USB0 */
        SYSTEM.MSTPCRB.BIT.MSTPB19 = 0u;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USB0);
    }
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    else if( USB_IP1 == ip_type )
    {
        if(R_BSP_HardwareLock(BSP_LOCK_USBA) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(usb_gstd_is_opened[ip_type] == true)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USBA);
            return USB_ERR_OPENED;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Enable power for USBA */
        SYSTEM.MSTPCRB.BIT.MSTPB12 = 0u;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USBA);
    }
    else
    {
    }
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    usb_gstd_is_opened[ip_type] = true;

    return USB_SUCCESS;
}
/******************************************************************************
End of function usb_cpu_McuInitialize
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_module_stop
Description     : USB module stop
Arguments       : usb_ip_t ip_type  : USB_IP0/USB_IP1
Return value    : none
******************************************************************************/
usb_err_t usb_cpu_module_stop( usb_ip_t ip_type )
{
    if( USB_IP0 == ip_type )
    {
        if(R_BSP_HardwareLock(BSP_LOCK_USB0) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(usb_gstd_is_opened[ip_type] == false)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USB0);
            return USB_ERR_NOT_OPEN;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Disable power for USB0 */
        SYSTEM.MSTPCRB.BIT.MSTPB19 = 1u;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USB0);
    }
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    else if( USB_IP1 == ip_type )
    {
        if(R_BSP_HardwareLock(BSP_LOCK_USBA) == false)
        {
            /* Lock has already been acquired by another task. Need to try again later */
            return USB_ERR_BUSY;
        }
        if(usb_gstd_is_opened[ip_type] == false)
        {
            R_BSP_HardwareUnlock(BSP_LOCK_USBA);
            return USB_ERR_NOT_OPEN;
        }

        /* Enable writing to MSTP registers */
        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
        /* Disable power for USBA */
        SYSTEM.MSTPCRB.BIT.MSTPB12 = 1u;
        /* Disable writing to MSTP registers */
        R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

        R_BSP_HardwareUnlock(BSP_LOCK_USBA);
    }
    else
    {
    }
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
    usb_gstd_is_opened[ip_type] = false;

    return USB_SUCCESS;
}
/******************************************************************************
End of function usb_cpu_McuInitialize
******************************************************************************/

/******************************************************************************
Function Name   : usb_cpu_target_init
Description     : Target System Initialize
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_target_init( void )
{
    /* Enable Interrupt */
    usb_cpu_usbint_init();      /* Initialized USB interrupt  */
}
/******************************************************************************
End of function usb_cpu_target_init
******************************************************************************/

/******************************************************************************
Interrupt function
******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_usbint_init
Description     : USB interrupt Initialize
Arguments       : void
Return value    : void
******************************************************************************/
void usb_cpu_usbint_init( void )
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
    /* Deep standby USB monitor register
    b0      SRPC0    USB0 single end control
    b3-b1   Reserved 0
    b4      FIXPHY0  USB0 transceiver output fix
    b7-b5   Reserved 0
    b8      SRPC1    USB1 single end control
    b11-b9  Reserved 0
    b12     FIXPHY1  USB1 transceiver output fix
    b15-b13 Reserved 0
    b16     DP0      USB0 DP input
    b17     DM0      USB0 DM input
    b19-b18 Reserved 0
    b20     DOVCA0   USB0 OVRCURA input
    b21     DOVCB0   USB0 OVRCURB input
    b22     Reserved 0
    b23     DVBSTS0  USB1 VBUS input
    b24     DP1      USB1 DP input
    b25     DM1      USB1 DM input
    b27-b26 Reserved 0
    b28     DOVCA1   USB1 OVRCURA input
    b29     DOVCB1   USB1 OVRCURB input
    b30     Reserved 0
    b31     DVBSTS1  USB1 VBUS input
    */
    USB.DPUSR0R.BIT.FIXPHY0 = 0u;   /* USB0 Transceiver Output fixed */

    /* Interrupt enable register
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
    */
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    IEN( USB0, D0FIFO0 ) = 0u;    /* D0FIFO0 disable */
    IEN( USB0, D1FIFO0 ) = 0u;    /* D1FIFO0 disable */
    IEN( USB0, USBR0 )   = 1u;    /* USBR0 enable */

    /* Interrupt priority register
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
    */
    IPR( USB0, D0FIFO0 ) = 0x00;  /* D0FIFO0 */
    IPR( USB0, D1FIFO0 ) = 0x00;  /* D1FIFO0 */
    IPR( USB0, USBR0 )   = 0x00;  /* USBR0 */
#else
    IEN( USB0, D0FIFO0 ) = 0u;    /* D0FIFO0 disable */
    IEN( USB0, D1FIFO0 ) = 0u;    /* D1FIFO0 disable */

    /* Interrupt priority register
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
    */
    IPR( USB0, D0FIFO0 ) = 0x00;  /* D0FIFO0 */
    IPR( USB0, D1FIFO0 ) = 0x00;  /* D1FIFO0 */
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */

    IPR( USB0, USBI0 )   = 0x03;  /* USBI0 in vector 128 */
    IEN( USB0, USBI0 )   = 1u;    /* USBI0 enable in vector 128 */

#endif  /* USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP */

#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    /* Interrupt enable register
    b0 IEN0 Interrupt enable bit
    b1 IEN1 Interrupt enable bit
    b2 IEN2 Interrupt enable bit
    b3 IEN3 Interrupt enable bit
    b4 IEN4 Interrupt enable bit
    b5 IEN5 Interrupt enable bit
    b6 IEN6 Interrupt enable bit
    b7 IEN7 Interrupt enable bit
    */
    IEN( USBA, D0FIFO2 ) = 0u;   /* Disable D0FIF2 interrupt */
    IEN( USBA, D1FIFO2 ) = 0u;   /* Disable D1FIF2 interrupt */
    IEN( USBA, USBAR )   = 1u;   /* Enable  USBA  interrupt */

    /* Priority D0FIFO0=0(Disable)
    b3-b0 IPR      Interrupt priority
    b7-b4 Reserved 0
    */
    IPR( USBA, D0FIFO2 ) = 0x00; /* D0FIFO2 */
    IPR( USBA, D1FIFO2 ) = 0x00; /* D0FIFO2 */
    IPR( USBA, USBAR )   = 0x03; /* USBA */
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#endif  /* USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP */
}
/******************************************************************************
End of function usb_cpu_usbint_init
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_usb_int_hand
Description     : USB interrupt Handler
Arguments       : void
Return value    : void
******************************************************************************/
void usb_cpu_usb_int_hand(void)
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
    /* Call USB interrupt routine */
    usb_cstd_UsbHandler();      /* Call interrupt routine */
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    ICU.PIBR7.BYTE |= 0x40;   /* Flag clear */
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
#endif /* USB_FUNCSEL_USBIP0_PP */
}
/******************************************************************************
End of function usb_cpu_usb_int_hand
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d0fifo_int_hand
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_d0fifo_int_hand(void)
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP0_PP != USB_NOUSE_PP
    usb_cstd_DmaHandler();      /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP0_PP */
}
/******************************************************************************
End of function usb_cpu_d0fifo_int_hand
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d1fifo_int_hand
Description     : D1FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb_cpu_d1fifo_int_hand(void)
{
    /* Please add the processing for the system. */
}
/******************************************************************************
End of function usb_cpu_d1fifo_int_hand
******************************************************************************/


/******************************************************************************
Function Name   : usb2_cpu_usb_int_hand
Description     : USB interrupt Handler
Arguments       : void
Return value    : void
******************************************************************************/
void usb2_cpu_usb_int_hand(void)
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
    usb2_cstd_UsbHandler();     /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP1_PP */
}
/******************************************************************************
End of function usb2_cpu_usb_int_hand
******************************************************************************/


/******************************************************************************
Function Name   : usb2_cpu_d0fifo_int_hand
Description     : D0FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb2_cpu_d0fifo_int_hand(void)
{
/* Condition compilation by the difference of USB function */
#if USB_FUNCSEL_USBIP1_PP != USB_NOUSE_PP
    usb2_cstd_DmaHandler();     /* Call interrupt routine */
#endif /* USB_FUNCSEL_USBIP1_PP */
}
/******************************************************************************
End of function usb2_cpu_d0fifo_int_hand
******************************************************************************/


/******************************************************************************
Function Name   : usb2_cpu_d1fifo_int_hand
Description     : D1FIFO interrupt Handler
Arguments       : none
Return value    : none
******************************************************************************/
void usb2_cpu_d1fifo_int_hand(void)
{
    /* Please add the processing for the system. */
}
/******************************************************************************
End of function usb2_cpu_d1fifo_int_hand
******************************************************************************/


/******************************************************************************
Renesas Abstracted RSK functions
******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_int_enable
Description     : USB Interrupt Enable
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : void
******************************************************************************/
void usb_cpu_int_enable(USB_UTR_t *ptr)
{
    if( ptr->ip == USB_USBIP_0 )
    {
        /* Interrupt enable register (USB0 USBIO enable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USB0, USBI0 )   = 1;                              /* Enable USB0 interrupt */
        IEN( USB0, D0FIFO0 ) = usb_gcstd_D0fifo[ptr->ip];
        IEN( USB0, D1FIFO0 ) = usb_gcstd_D1fifo[ptr->ip];
    }
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    if( ptr->ip == USB_USBIP_1 )
    {
        /* Interrupt enable register (USB1 USBIO enable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USBA, USBAR )   = 1u;                            /* Enable USBA interrupt */
        IEN( USBA, D0FIFO2 ) = usb_gcstd_D0fifo[ptr->ip];
        IEN( USBA, D1FIFO2 ) = usb_gcstd_D1fifo[ptr->ip];
    }
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
}
/******************************************************************************
End of function usb_cpu_int_enable
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_int_disable
Description     : USB Interrupt disable
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : void
******************************************************************************/
void usb_cpu_int_disable(USB_UTR_t *ptr)
{
    if( ptr->ip == USB_USBIP_0 )
    {
        /* Interrupt enable register (USB0 USBIO disable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USB0, USBI0 ) = 0;    /* Disnable USB0 interrupt */
        usb_gcstd_D0fifo[ptr->ip] = IEN( USB0, D0FIFO0 );
        IEN( USB0, D0FIFO0 ) = 0;
        usb_gcstd_D1fifo[ptr->ip] = IEN( USB0, D1FIFO0 );
        IEN( USB0, D1FIFO0 ) = 0;
    }
#if defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX71M)
    if ( ptr->ip == USB_USBIP_1 )
    {
        /* Interrupt enable register (USB1 USBIO disable)
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USBA, USBAR ) = 0u;   /* Disnable USBA interrupt */
        usb_gcstd_D0fifo[ptr->ip] = IEN( USBA, D0FIFO2 );
        IEN( USBA, D0FIFO2 ) = 0;
        usb_gcstd_D1fifo[ptr->ip] = IEN( USBA, D1FIFO2 );
        IEN( USBA, D1FIFO2 ) = 0;
    }
#endif /* #if defined(BSP_MCU_RX64M) || (BSP_MCU_RX71M) */
}
/******************************************************************************
End of function usb_cpu_int_disable
******************************************************************************/


/******************************************************************************
TIMER function
******************************************************************************/
/******************************************************************************
Function Name   : usb_cpu_Delay1us
Description     : 1us Delay timer
Arguments       : uint16_t  time        ; Delay time(*1us)
Return value    : none
Note            : Please change for your MCU
******************************************************************************/
void usb_cpu_Delay1us(uint16_t time)
{
    volatile register uint16_t  i;

    /* Wait 1us (Please change for your MCU) */
#if defined(BSP_MCU_RX63N)
    for( i = 0; i < (7 * time); ++i )
#endif /* defined(BSP_MCU_RX63N) */

#if defined(BSP_MCU_RX64M)
    for( i = 0; i < (9 * time); ++i )
#endif /* defined(BSP_MCU_RX64M) */

#if defined(BSP_MCU_RX71M)
    for( i = 0; i < (20 * time); ++i )
#endif /* defined(BSP_MCU_RX71M) */

    {
    };
}
/******************************************************************************
End of function usb_cpu_Delay1us
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_DelayXms
Description     : xms Delay timer
Arguments       : uint16_t  time        ; Delay time(*1ms)
Return value    : void
Note            : Please change for your MCU
******************************************************************************/
void usb_cpu_DelayXms(uint16_t time)
{
    /* Wait xms (Please change for your MCU) */
    volatile register uint32_t  i;

    /* Wait 1ms */
#if defined(BSP_MCU_RX63N)
    for( i = 0; i < (7600 * time); ++i )
#endif /* defined(BSP_MCU_RX63N) */

#if defined(BSP_MCU_RX64M)
    for( i = 0; i < (9500 * time); ++i )
#endif /* defined(BSP_MCU_RX64M) */

#if defined(BSP_MCU_RX71M)
    for( i = 0; i < (20000 * time); ++i )
#endif /* defined(BSP_MCU_RX71M) */
    {
    };
        /* When "ICLK=120MHz" is set, this code is waiting for 1ms.
          Please change this code with CPU Clock mode. */
}
/******************************************************************************
End of function usb_cpu_DelayXms
******************************************************************************/


#ifdef USB_DTC_ENABLE
/******************************************************************************
Function Name   : usb_cpu_d0fifo2buf_start_dma
Description     : FIFO to Buffer data read DMA start
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
                : uint32_t SourceAddr   : Source address
Return value    : void
******************************************************************************/
void usb_cpu_d0fifo2buf_start_dma(USB_UTR_t *ptr, uint32_t SourceAddr)
{
    uint16_t    size;
    uint32_t    tmp;
    dtc_activation_source_t    act_src;    /* activation source is Software Interrupt */


    /* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
    */
    R_DTC_Control(DTC_CMD_DTC_START, NULL, 0);

    /* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
    */
    R_DTC_Control(DTC_CMD_DATA_READ_SKIP_DISABLE, NULL, 0);

    /* DTC mode register A (Block Transfer Set)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
    */
    usb_td_cfg[ptr->ip].transfer_mode = DTC_TRANSFER_MODE_BLOCK;

    if(ptr->ip == USB_USBIP_0)
    {
        act_src = DTCE_USB0_D0FIFO0;
        tmp = ((usb_gcstd_Dma0Fifo[ptr->ip] - 1) / 2) + 1;

        /* DTC mode register A (Word Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
        */
        usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_WORD;
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        act_src = DTCE_USBA_D0FIFO2;
        tmp = ((usb_gcstd_Dma0Fifo[ptr->ip] - 1) / 4) + 1;

        /* DTC mode register A (Long Size)
        b1-b0 Reserved 0
        b3-b2 SM       source address mode bit
        b5-b4 SZ       DTC data transfer size bit
        b7-b6 MD       DTC mode bit
        */
        usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_LWORD;
    }

    /* DTC mode register A (Source Address fixed)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
    */
    usb_td_cfg[ptr->ip].src_addr_mode = DTC_SRC_ADDR_FIXED;

    /* DTC mode register B (Chain Transfer disable)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].chain_transfer_enable = DTC_CHAIN_TRANSFER_DISABLE;

    /* DTC mode register B (Select Data Transfer End Interrupt)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].response_interrupt = DTC_INTERRUPT_AFTER_ALL_COMPLETE;

    /* DTC mode register B (Source Side Block Area)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].repeat_block_side = DTC_REPEAT_BLOCK_SOURCE;

    /* DTC mode register B (Destination Address Increment)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].dest_addr_mode = DTC_DES_ADDR_INCR;

    /* DTC source address register (FIFO port address)
    b31-b0 SAR Destination address
    */
    usb_td_cfg[ptr->ip].source_addr = SourceAddr;

    /* DTC source address register (Table address)
    b31-b0 SAR Source address
    */
    usb_td_cfg[ptr->ip].dest_addr = (uint32_t)(usb_gcstd_DataPtr[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]]);

    size = (uint8_t)(tmp);

    /* DTC transfer count registerA
    b15-b0 CRA Transfer count
    */
    usb_td_cfg[ptr->ip].block_size = (uint16_t)(size);

    /* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
    */
    usb_td_cfg[ptr->ip].transfer_count =
        (uint16_t)((usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] -1) / usb_gcstd_Dma0Fifo[ptr->ip]) +1;

    /* DTC address mode register (Full Address Mode)
    b0    SHORT    Short address mode bit
    b7-b1 Reserved 0
    */

    /* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
    */
    R_DTC_Control(DTC_CMD_DATA_READ_SKIP_ENABLE, NULL, 0);

    if( ptr->ip == USB_USBIP_0 )
    {
        /* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USB0, D0FIFO0 ) = 0x00;

        /* Interrupt enable register (USB0 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USB0, D0FIFO0 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USB0, D0FIFO0 ) = 1;

        /* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USB0, D0FIFO0 ) = 1;
    }
    else
    {
        /* Priority D0FIFO2=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USBA, D0FIFO2 ) = 0x00;

        /* Interrupt enable register (USBA D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USBA, D0FIFO2 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USBA, D0FIFO2 ) = 1;


        /* DTC start enable register (USBA D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USBA, D0FIFO2 ) = 1;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_buf2d0fifo_start_dma
Description     : Buffer to FIFO data write DMA start
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
                : uint32_t DistAdr      : Destination address
Return value    : void
******************************************************************************/
void usb_cpu_buf2d0fifo_start_dma(USB_UTR_t *ptr, uint32_t DistAdr)
{
    uint16_t    size;
    uint32_t    tmp;
    dtc_activation_source_t    act_src;    /* activation source is Software Interrupt */


    /* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
    */
    R_DTC_Control( DTC_CMD_DTC_START, NULL, 0 );

    /* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
    */
    R_DTC_Control( DTC_CMD_DATA_READ_SKIP_DISABLE, NULL, 0 );

    /* DTC mode register A (Block Transfer Set)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
    */
    usb_td_cfg[ptr->ip].transfer_mode = DTC_TRANSFER_MODE_BLOCK;

    if(ptr->ip == USB_USBIP_0)
    {
        act_src = DTCE_USB0_D0FIFO0;

        if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001) != 0 )
        {
            /* if count == odd */
            tmp = usb_gcstd_Dma0Size[ptr->ip];

            /* DTC mode register A (Byte Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_BYTE;
        }
        else
        {
            tmp = usb_gcstd_Dma0Size[ptr->ip] / 2;

            /* DTC mode register A (Word Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_WORD;
        }
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        act_src = DTCE_USBA_D0FIFO2;

        if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0003) != 0 )
        {
            /* if count == odd */
            tmp = usb_gcstd_Dma0Size[ptr->ip];

            /* DTC mode register A (Byte Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_BYTE;
        }
        else
        {
            tmp = usb_gcstd_Dma0Size[ptr->ip] / 4;

            /* DTC mode register A (Word Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_LWORD;
        }
    }

    /* DTC mode register A (Source Address Increment)
    b1-b0 Reserved 0
    b3-b2 SM       source address mode bit
    b5-b4 SZ       DTC data transfer size bit
    b7-b6 MD       DTC mode bit
    */
    usb_td_cfg[ptr->ip].src_addr_mode = DTC_SRC_ADDR_INCR;

    /* DTC mode register B (Chain Transfer disable)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].chain_transfer_enable = DTC_CHAIN_TRANSFER_DISABLE;

    /* DTC mode register B (Select Data Transfer End Interrupt)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].response_interrupt = DTC_INTERRUPT_AFTER_ALL_COMPLETE;

    /* DTC mode register B (Destination Side Block Area)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].repeat_block_side = DTC_REPEAT_BLOCK_DESTINATION;

    /* DTC mode register B (Destination Address fixed)
    b1-b0 Reserved 0
    b3-b2 DM       Destination address mode bit
    b4    DTS      DTC transfer mode select bit
    b5    DISEL    DTC interrupt select bit
    b6    CHNS     DTC chain transfer select bit
    b7    CHNE     DTC chain transfer enable bit
    */
    usb_td_cfg[ptr->ip].dest_addr_mode = DTC_DES_ADDR_FIXED;

    /* DTC source address register (Table address)
    b31-b0 SAR Destination address
    */
    usb_td_cfg[ptr->ip].source_addr = (uint32_t)(usb_gcstd_DataPtr[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]]);

    /* DTC source address register (FIFO port address)
    b31-b0 SAR Source address
    */
    usb_td_cfg[ptr->ip].dest_addr = (uint32_t)(DistAdr);

    size = (uint8_t )(tmp);

    /* DTC transfer count registerA
    b15-b0 CRA Transfer count
    */
    usb_td_cfg[ptr->ip].block_size = (uint16_t)(size);

    /* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
    */
    usb_td_cfg[ptr->ip].transfer_count =
        (uint16_t)(usb_gcstd_DataCnt[ptr->ip][usb_gcstd_Dma0Pipe[ptr->ip]] / usb_gcstd_Dma0Size[ptr->ip]);

    /* DTC address mode register (Full Address Mode)
    b0    SHORT    Short address mode bit
    b7-b1 Reserved 0
    */

    /* DTC control register (Transfer Information Read No Skip)
    b2-b0 Reserved 0
    b3    Reserved 0
    b4    RRS      DTC Transfer Information Read Skip enable bit
    b7-b5 Reserved 0
    */
    R_DTC_Control(DTC_CMD_DATA_READ_SKIP_ENABLE, NULL, 0);

    if( ptr->ip == USB_USBIP_0 )
    {
        /* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USB0, D0FIFO0 ) = 0x00;

        /* Interrupt enable register (USB0 D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USB0, D0FIFO0 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USB0, D0FIFO0 ) = 1;

        /* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USB0, D0FIFO0 ) = 1;
    }
    else
    {
        /* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USBA, D0FIFO2 ) = 0x00;

        /* Interrupt enable register (USBA D0FIFO enable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USBA, D0FIFO2 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USBA, D0FIFO2 ) = 1;

        /* DTC start enable register (USBA D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USBA, D0FIFO2 ) = 1;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d0fifo_stop_dma
Description     : DMA stop
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
Return value    : void
******************************************************************************/
void usb_cpu_d0fifo_stop_dma(USB_UTR_t *ptr)
{
    if( ptr->ip == USB_USBIP_0 )
    {
        /* Interrupt request register
        b0    IR       Interrupt status flag
        b7-b1 Reserved 0
        */
        IR( USB0, D0FIFO0 ) = 0;

        /* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USB0, D0FIFO0 ) = 0x00;

        /* Interrupt enable register (USB0 D0FIFO disable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USB0, D0FIFO0 ) = 0;

        /* DTC start enable register (USB0 D0FIFO transfer disable)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USB0, D0FIFO0 ) = 0;
    }
    else
    {
        /* Interrupt request register
        b0    IR       Interrupt status flag
        b7-b1 Reserved 0
        */
        IR( USBA, D0FIFO2 ) = 0;

        /* Priority D0FIFO0=0
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USBA, D0FIFO2 ) = 0x00;

        /* Interrupt enable register (USBA D0FIFO disable(IEN4))
        b0 IEN0 Interrupt enable bit
        b1 IEN1 Interrupt enable bit
        b2 IEN2 Interrupt enable bit
        b3 IEN3 Interrupt enable bit
        b4 IEN4 Interrupt enable bit
        b5 IEN5 Interrupt enable bit
        b6 IEN6 Interrupt enable bit
        b7 IEN7 Interrupt enable bit
        */
        IEN( USBA, D0FIFO2 ) = 0;

        /* DTC start enable register (USBA D0FIFO transfer disable)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USBA, D0FIFO2 ) = 0;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d0fifo_restart_dma
Description     : DMA Restart
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void usb_cpu_d0fifo_restart_dma( USB_UTR_t *ptr )
{
    uint16_t    size;
    uint32_t    tmp;
    dtc_activation_source_t    act_src;    /* activation source is Software Interrupt */


    if(ptr->ip == USB_USBIP_0)
    {
        act_src = DTCE_USB0_D0FIFO0;

        if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0001u) != 0u )
        {
            /* if count == odd */
            tmp = usb_gcstd_Dma0Size[ptr->ip];

            /* DTC mode register A (Byte Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_BYTE;

            /* DTC source address register (Table address)
            b31-b0 SAR Source address
            */
            usb_td_cfg[ptr->ip].dest_addr = usb_cstd_GetD0fifo8Adr(ptr);
        }
        else
        {
            tmp = usb_gcstd_Dma0Size[ptr->ip] / 2;

            /* DTC mode register A (Word Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_WORD;
        }
    }
    else if(ptr->ip == USB_USBIP_1)
    {
        act_src = DTCE_USBA_D0FIFO2;

        if( (usb_gcstd_Dma0Size[ptr->ip] & 0x0003u) != 0u )
        {
            /* if count == odd */
            tmp = usb_gcstd_Dma0Size[ptr->ip];

            /* DTC mode register A (Byte Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_BYTE;

            /* DTC source address register (Table address)
            b31-b0 SAR Source address
            */
            usb_td_cfg[ptr->ip].dest_addr = usb_cstd_GetD0fifo8Adr(ptr);
        }
        else
        {
            tmp = usb_gcstd_Dma0Size[ptr->ip] / 4;

            /* DTC mode register A (Word Size)
            b1-b0 Reserved 0
            b3-b2 SM       source address mode bit
            b5-b4 SZ       DTC data transfer size bit
            b7-b6 MD       DTC mode bit
            */
            usb_td_cfg[ptr->ip].data_size = DTC_DATA_SIZE_LWORD;
        }
    }

    /* DTC source address register (Table address)
    b31-b0 SAR Destination address
    */
    usb_td_cfg[ptr->ip].source_addr = usb_dtc_transfer_data[ptr->ip].lw2;

    size = (uint8_t)(tmp);

    /* DTC transfer count registerA
    b15-b0 CRA Transfer count
    */
    usb_td_cfg[ptr->ip].block_size = (uint16_t)(size);

    /* DTC transfer count registerB (Block count)
    b15-b0 CRB Transfer count
    */
    usb_td_cfg[ptr->ip].transfer_count = (uint16_t)(1);
    
    if( ptr->ip == USB_USBIP_0 )
    {
        IEN( USB0, D0FIFO0 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USB0, D0FIFO0 ) = 1;
    }
    else
    {
        IEN( USBA, D0FIFO2 ) = 0;
        R_DTC_Create( act_src, &usb_dtc_transfer_data[ptr->ip], &usb_td_cfg[ptr->ip], 0 );
        IEN( USBA, D0FIFO2 ) = 1;
    }

    /* DTC Transfer enable
    b0    DTCST    DTC module start bit
    b7-b1 Reserved 0
    */
    R_DTC_Control( DTC_CMD_DTC_START, NULL, 0 );

    if( ptr->ip == USB_USBIP_0 )
    {
        /* DTC start enable register (USB0 D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USB0, D0FIFO0 ) = 1;
    }
    else
    {
        /* DTC start enable register (USBA D0FIFO transfer)
        b0    DTCE     DTC start enable bit
        b7-b1 Reserved 0
        */
        DTCE( USBA, D0FIFO2 ) = 1;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d0fifo_enable_dma
Description     : DTC(D0FIFO) interrupt enable (Interrupt priority 5 set)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void usb_cpu_d0fifo_enable_dma(USB_UTR_t *ptr)
{
    if( ptr->ip == USB_USBIP_0 )
    {
        /* Priority D0FIFO0 = 0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USB0, D0FIFO0 ) = 0x05;
    }
    else
    {
        /* Priority D0FIFO2 = 0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USBA, D0FIFO2 ) = 0x05;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_d0fifo_disable_dma
Description     : D0FIFO interrupt disable (Interrupt priority 0 set)
Arguments       : USB_UTR_t *ptr  : USB internal structure. Selects USB channel. 
Return value    : none
******************************************************************************/
void usb_cpu_d0fifo_disable_dma(USB_UTR_t *ptr)
{
    if( ptr->ip == USB_USBIP_0 )
    {
        /* Priority D0FIFO0 = 0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USB0, D0FIFO0 ) = 0x00;
    }
    else
    {
        /* Priority D0FIFO2 = 0(Disable)
        b3-b0 IPR      Interrupt priority
        b7-b4 Reserved 0
        */
        IPR( USBA, D0FIFO2 ) = 0x00;
    }
}
/******************************************************************************
End of function
******************************************************************************/


/******************************************************************************
Function Name   : usb_cpu_get_dtc_block_count
Description     : Get DTC Transfer count reg B(CRB).
Arguments       : USB_UTR_t *ptr        : USB internal structure. Selects USB channel. 
Return value    : DTC Transfer count reg B(CRB)
******************************************************************************/
uint16_t usb_cpu_get_dtc_block_count(USB_UTR_t *ptr)
{
    uint16_t value;
    uint16_t status_reg = 0;
    
    /* Wait Complete DTC Transfer */
    do
    {
        status_reg = DTC.DTCSTS.WORD;
    }
    while( 0 != ( status_reg & 0x8000 ) );  /* DTC is not active */
 
    /* Read DTC transfer count (CRB) */
    value = (uint16_t)(usb_dtc_transfer_data[ptr->ip].lw4 & 0xffff);
    
    return value;
}
/******************************************************************************
End of function
******************************************************************************/
#endif /* USB_DTC_ENABLE */


/******************************************************************************
End  Of File
******************************************************************************/
