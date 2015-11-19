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
* File Name    : r_usb_hintfifo.c
* Description  : USB Host and FIFO interrupt code
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
#include "r_usb_reg_access.h"


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Constant macro definitions
******************************************************************************/


/******************************************************************************
External variables and functions
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
Renesas Abstracted Host FIFO Interrupt functions
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_BrdyPipe
Description     : BRDY Interrupt
Arguments       : USB_UTR_t *ptr
                : uint16_t bitsts           : BRDYSTS Reg & BRDYENB Reg
Return value    : none
******************************************************************************/
//void usb_hstd_BrdyPipe(uint16_t bitsts)
void usb_hstd_BrdyPipe(USB_UTR_t *ptr)
{
    uint16_t    bitsts;

    bitsts = ptr->status;

    /* When operating by the host function, usb_cstd_brdy_pipe() is executed without fail because */
    /* only one BRDY message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BRDY0) == USB_BRDY0 )
    {
        /* Branch  by the Control transfer stage management */
        switch( usb_ghstd_Ctsq[ptr->ip] )
        {
        /* Data stage of Control read transfer */
        case USB_DATARD:
            switch( usb_cstd_read_data( ptr, USB_PIPE0, USB_CUSE ) )
            {
            /* End of data read */
            case USB_READEND:
                /* continue */
            /* End of data read */
            case USB_READSHRT:
                usb_hstd_StatusStart(ptr);
                break;
            /* Continue of data read */
            case USB_READING:
                break;
            /* FIFO access error */
            case USB_READOVER:
                USB_PRINTF0("### Receive data over PIPE0 \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_OVR);
                break;
            /* FIFO access error */
            case USB_FIFOERROR:
                USB_PRINTF0("### FIFO access error \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                break;
            default:
                break;
            }
            break;
        /* Data stage of Control read transfer */
        case USB_DATARDCNT:
            switch( usb_cstd_read_data( ptr, USB_PIPE0, USB_CUSE ) )
            {
            /* End of data read */
            case USB_READEND:
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_CTRL_READING);
                break;
            /* End of data read */
            case USB_READSHRT:
                /* Control Read/Write Status */
                usb_hstd_StatusStart(ptr);
                break;
            /* Continue of data read */
            case USB_READING:
                break;
            /* FIFO access error */
            case USB_READOVER:
                USB_PRINTF0("### Receive data over PIPE0 \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_OVR);
                break;
            /* FIFO access error */
            case USB_FIFOERROR:
                USB_PRINTF0("### FIFO access error \n");
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                break;
            default:
                break;
            }
            break;

        /* Status stage of Control write (NoData control) transfer */
        case USB_STATUSWR:
            /* Control Read/Write End */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_CTRL_END);
            break;
        default:
            break;
        }
    }
    /* BRDY interrupt */
    usb_cstd_BrdyPipe(ptr, bitsts);
}
/******************************************************************************
End of function usb_hstd_BrdyPipe
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_NrdyPipe
Description     : NRDY Interrupt
Arguments       : USB_UTR_t *ptr
                : uint16_t bitsts           : NRDYSTS Reg & NRDYENB Reg
Return value    : none
******************************************************************************/
void usb_hstd_NrdyPipe(USB_UTR_t *ptr)
{
    uint16_t        buffer;
    uint16_t        bitsts;

    bitsts = ptr->status;

    /* When operating by the host function, usb_cstd_nrdy_pipe() is executed without fail because */
    /* only one NRDY message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_NRDY0) == USB_NRDY0 )
    {
        /* Get Pipe PID from pipe number */
        buffer = usb_cstd_GetPid(ptr, (uint16_t)USB_PIPE0);
        /* STALL ? */
        if( (buffer & USB_PID_STALL) == USB_PID_STALL )
        {           
            USB_PRINTF0("### STALL Pipe 0\n");
            /* PIPE0 STALL call back */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_STALL);
        }
        else
        {
            /* Ignore count */
            usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0]++;
            USB_PRINTF2("### IGNORE Pipe %d is %d times \n", USB_PIPE0, usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0]);
            /* Pipe error check */
            if( usb_ghstd_IgnoreCnt[ptr->ip][USB_PIPE0] == USB_PIPEERROR )
            {
                /* Control Data Stage Device Ignore X 3 call back */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
            }
            else
            {
                /* Control Data Stage Retry */
                /* 5ms wait */
                usb_cpu_DelayXms((uint16_t)5);
                /* PIPE0 Send IN or OUT token */
                usb_cstd_SetBuf(ptr, (uint16_t)USB_PIPE0);
            }
        }
    }
    /* Nrdy Pipe interrupt */
    usb_cstd_NrdyPipe(ptr, bitsts);
}
/******************************************************************************
End of function usb_hstd_NrdyPipe
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_BempPipe
Description     : BEMP interrupt
Arguments       : USB_UTR_t *ptr
                : uint16_t bitsts           : BEMPSTS Reg & BEMPENB Reg
Return value    : none
******************************************************************************/
void usb_hstd_BempPipe(USB_UTR_t *ptr)
{
    uint16_t        buffer;
    uint16_t        bitsts;

    bitsts = ptr->status;

    /* When operating by the host function, usb_cstd_bemp_pipe() is executed without fail because */
    /* only one BEMP message is issued even when the demand of PIPE0 and PIPEx has been generated at the same time. */
    if( (bitsts & USB_BEMP0) == USB_BEMP0 )
    {
        /* Get Pipe PID from pipe number */
        buffer = usb_cstd_GetPid(ptr, (uint16_t)USB_PIPE0);
        /* MAX packet size error ? */
        if( (buffer & USB_PID_STALL) == USB_PID_STALL )
        {
            USB_PRINTF0("### STALL Pipe 0\n");
            /* PIPE0 STALL call back */
            usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_STALL);
        } 
        else
        {
            /* Branch  by the Control transfer stage management */
            switch( usb_ghstd_Ctsq[ptr->ip] )
            {
            /* Continuas of data stage (Control write) */
            case USB_DATAWR:
                /* Buffer to CFIFO data write */
                switch( usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE ) )
                {
                /* End of data write */
                case USB_WRITESHRT:
                    /* Next stage is Control write status stage */
                    usb_ghstd_Ctsq[ptr->ip] = USB_STATUSWR;
                    /* Enable Empty Interrupt */
                    usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
                    /* Enable Not Ready Interrupt */
                    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
                    break;
                /* End of data write (not null) */
                case USB_WRITEEND:
                    /* continue */
                /* Continue of data write */
                case USB_WRITING:
                    /* Enable Empty Interrupt */
                    usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
                    /* Enable Not Ready Interrupt */
                    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
                    break;
                /* FIFO access error */
                case USB_FIFOERROR:
                    USB_PRINTF0("### FIFO access error \n");
                    /* Control Read/Write End */
                    usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                    break;
                default:
                    break;
                }
                break;
            /* Next stage to Control write data */
            case USB_DATAWRCNT:
                /* Buffer to CFIFO data write */
                switch( usb_cstd_write_data( ptr, USB_PIPE0, USB_CUSE ) )
                {
                /* End of data write */
                case USB_WRITESHRT:
                    /* Next stage is Control write status stage */
                    usb_ghstd_Ctsq[ptr->ip] = USB_STATUSWR;
                    /* Enable Empty Interrupt */
                    usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
                    /* Enable Not Ready Interrupt */
                    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
                    break;
                /* End of data write (not null) */
                case USB_WRITEEND:
                    /* Control Read/Write End */
                    usb_hstd_ControlEnd(ptr, (uint16_t)USB_CTRL_WRITING);
                    break;
                /* Continue of data write */
                case USB_WRITING:
                    /* Enable Empty Interrupt */
                    usb_creg_set_bempenb(ptr, (uint16_t)USB_PIPE0);
                    /* Enable Not Ready Interrupt */
                    usb_cstd_NrdyEnable(ptr, (uint16_t)USB_PIPE0);
                    break;
                /* FIFO access error */
                case USB_FIFOERROR:
                    USB_PRINTF0("### FIFO access error \n");
                    /* Control Read/Write End */
                    usb_hstd_ControlEnd(ptr, (uint16_t)USB_DATA_ERR);
                    break;
                default:
                    break;
                }
                break;
            /* End of data stage (Control write) */
            case USB_STATUSWR:
                usb_hstd_StatusStart(ptr);
                break;
            /* Status stage of Control read transfer */
            case USB_STATUSRD:
                /* Control Read/Write End */
                usb_hstd_ControlEnd(ptr, (uint16_t)USB_CTRL_END);
                break;
            default:
                break;
            }
        }
    }
    /* BEMP interrupt */
    usb_cstd_BempPipe(ptr, bitsts);
}
/******************************************************************************
End of function usb_hstd_BempPipe
******************************************************************************/

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
End  Of File
******************************************************************************/
