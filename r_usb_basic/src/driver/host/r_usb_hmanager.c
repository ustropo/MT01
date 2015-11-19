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
* File Name    : r_usb_hmanager.c
* Description  : USB Host Control Manager
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
#include "r_usb_api.h"
#include "r_usb_reg_access.h"


#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)

/******************************************************************************
Constant macro definitions
******************************************************************************/
#define CLSDATASIZE     512


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Manager */
uint16_t                usb_ghstd_EnumSeq[USB_NUM_USBIP];   /* Enumeration request */
uint16_t                usb_ghstd_DeviceDescriptor[USB_NUM_USBIP][USB_DEVICESIZE / 2u];
uint16_t                usb_ghstd_ConfigurationDescriptor[USB_NUM_USBIP][USB_CONFIGSIZE / 2u];
uint16_t                usb_ghstd_SuspendPipe[USB_NUM_USBIP][USB_MAX_PIPE_NO + 1u];

uint16_t                usb_ghstd_CheckEnuResult[USB_NUM_USBIP];
uint8_t                 usb_ghstd_EnuWait[USB_NUM_USBIP + USB_NUM_USBIP%2u];

void usb_hstd_MgrRelMpl(USB_UTR_t *ptr,uint16_t n);

/******************************************************************************
Static variables and functions
******************************************************************************/
uint16_t         usb_shstd_StdRequest[USB_NUM_USBIP][5];
static uint16_t         usb_shstd_DummyData;
USB_UTR_t        usb_shstd_StdReqMsg[USB_NUM_USBIP];
    /* Condition compilation by the difference of the operating system */
static uint16_t         usb_shstd_RegPointer[USB_NUM_USBIP];
static USB_MGRINFO_t    *usb_shstd_MgrMsg[USB_NUM_USBIP];
static uint16_t         usb_shstd_mgr_msginfo[USB_NUM_USBIP] = {0,0};
static USB_CB_t         usb_shstd_mgr_callback[USB_NUM_USBIP];
static uint16_t         usb_shstd_SuspendSeq[USB_NUM_USBIP] = {0,0};
static uint16_t         usb_shstd_ResumeSeq[USB_NUM_USBIP] = {0,0};
static  void usb_hstd_SuspCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport);
static  void usb_hstd_ResuCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport);

static  uint16_t usb_hstd_ChkDeviceClass(USB_UTR_t *ptr, USB_HCDREG_t *driver, uint16_t port);
static  void usb_hstd_EnumerationErr(uint16_t Rnum);
        uint16_t usb_hstd_SetFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t completeC);
        uint16_t usb_hstd_GetConfigDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t length, USB_CB_t complete);
        uint16_t usb_hstd_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete);
        uint16_t usb_hstd_StdReqCheck(uint16_t errcheck);
static  uint16_t usb_hstd_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete);
static  uint16_t usb_hstd_ChkRemote(USB_UTR_t *ptr);


/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Enumeration Table */
void (*usb_ghstd_EnumarationProcess[8])(USB_UTR_t *, uint16_t, uint16_t) =
{
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumSetAddress,
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumGetDescriptor,
    usb_hstd_EnumGetDescriptor,     usb_hstd_EnumGetDescriptor,
    usb_hstd_EnumSetConfiguration,  usb_hstd_EnumDummyRequest,
};

uint8_t         usb_ghstd_ClassData[USB_NUM_USBIP][CLSDATASIZE];
USB_UTR_t       usb_ghstd_ClassControl[USB_NUM_USBIP];
uint16_t        usb_ghstd_ClassRequest[USB_NUM_USBIP][5];

/******************************************************************************
External variables and functions
******************************************************************************/
extern  uint16_t    usb_hstd_ChkRemote(USB_UTR_t *ptr);
extern void (*usb_ghstd_EnumarationProcess[8])(USB_UTR_t *,uint16_t, uint16_t); /* Enumeration Table */


/******************************************************************************
Function Name   : usb_hstd_MgrRelMpl
Description     : Release a memory block.
Argument        : uint16_t n    : Error no.
Return          : none
******************************************************************************/
void usb_hstd_MgrRelMpl(USB_UTR_t *ptr,uint16_t n)
{
    USB_ER_t        err;

    err = USB_REL_BLK(USB_MGR_MPL, (USB_MH_t)usb_shstd_MgrMsg[ptr->ip]);
    if( err != USB_E_OK )
    {
        USB_PRINTF1("### USB MGR rel_blk error: %d\n", n);
    }
}
/******************************************************************************
End of function usb_hstd_MgrRelMpl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_mgr_chgdevst_cb
Description     : Call the callback function specified by the argument given to
                : the API R_usb_hstd_ChangeDeviceState.
Argument        : uint16_t rootport : Port no.
Return          : none
******************************************************************************/
static void usb_hstd_mgr_chgdevst_cb(USB_UTR_t *ptr, uint16_t rootport)
{
    if( usb_shstd_mgr_msginfo[ptr->ip] != 0 )
    {
        (*usb_shstd_mgr_callback[ptr->ip])(ptr, rootport, usb_shstd_mgr_msginfo[ptr->ip]);
        usb_shstd_mgr_msginfo[ptr->ip] = 0;
    }
}

/******************************************************************************
End of function usb_hstd_mgr_chgdevst_cb
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_Enumeration
Description     : Execute enumeration on the connected USB device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
Return          : uint16_t       : Enumeration status.
******************************************************************************/
uint16_t usb_hstd_Enumeration(USB_UTR_t *ptr)
{
#ifdef USB_HOST_COMPLIANCE_MODE
    uint16_t        vendor_id;
    uint16_t        product_id;
#endif  /* USB_HOST_COMPLIANCE_MODE */

    uint16_t        md, flg;
    USB_HCDREG_t    *driver;
    uint16_t        enume_mode;     /* Enumeration mode (device state) */
    uint8_t         *descriptor_table;
    uint16_t        rootport, pipenum, devsel;

    /* Attach Detect Mode */
    enume_mode = USB_NONDEVICE;

    pipenum     = usb_shstd_MgrMsg[ptr->ip]->keyword;
    /* Agreement device address */
    devsel      = usb_cstd_GetDevsel(ptr, pipenum);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);

    /* Manager Mode Change */
    switch( usb_shstd_MgrMsg[ptr->ip]->result )
    {
    case USB_CTRL_END:
        enume_mode = USB_DEVICEENUMERATION;
        switch( usb_ghstd_EnumSeq[ptr->ip] )
        {
        /* Receive Device Descriptor */
        case 0:
            break;
        /* Set Address */
        case 1:
            descriptor_table = (uint8_t*)usb_ghstd_DeviceDescriptor[ptr->ip];
            devsel = (uint16_t)(usb_ghstd_DeviceAddr[ptr->ip] << USB_DEVADDRBIT);
            /* Set device speed */
            usb_hstd_SetDevAddr(ptr, devsel, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
            usb_ghstd_DcpRegister[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]] 
                = (uint16_t)((uint16_t)(descriptor_table[7] & USB_MAXPFIELD) | devsel);
            break;
        /* Receive Device Descriptor(18) */
        case 2:
            break;
        /* Receive Configuration Descriptor(9) */
        case 3:
            break;
        /* Receive Configuration Descriptor(xx) */
        case 4:
#ifdef USB_HOST_COMPLIANCE_MODE
            descriptor_table = (uint8_t*)usb_ghstd_DeviceDescriptor[ptr->ip];
            /* If 'vendor_id' and 'product_id' value is defined by PET, */
            /*    system works for compliance test mode. */
            /* Values ??defined by PET is the following. */
            /* vendor_id : 0x1A0A  */
            /* product_id : 0x0101 - 0x0108 , 0x0200 */

            vendor_id = (uint16_t)(descriptor_table[USB_DEV_ID_VENDOR_L]
                      + ((uint16_t)descriptor_table[USB_DEV_ID_VENDOR_H] << 8));

            if(vendor_id == 0x1A0A)
            {
                product_id = (uint16_t)(descriptor_table[USB_DEV_ID_PRODUCT_L]
                           + ((uint16_t)descriptor_table[USB_DEV_ID_PRODUCT_H] << 8));

                descriptor_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor[ptr->ip];
#ifdef USB_HS_EL_TEST
                if((product_id >= 0x0101) && (product_id <= 0x0108))
                {
                    usb_hstd_ElectricalTestMode(ptr, product_id, rootport);
                    enume_mode = USB_NOTTPL;
                    break;
                }
#endif  /* USB_HS_EL_TEST */
                if(product_id == 0x0200)
                {
                    usb_ghstd_EnumSeq[ptr->ip]++;
                    break;
                }
            }
#endif  /* USB_HOST_COMPLIANCE_MODE */


            /* Device enumeration function */
            switch( usb_hstd_EnumFunction1() )
            {
            /* Driver open */
            case USB_DONE:
                for( flg = 0u, md = 0; (md < usb_ghstd_DeviceNum[ptr->ip]) && (flg == 0u); md++ ) 
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devstate == USB_DETACHED ) 
                    {
                        uint16_t retval;
                        retval = usb_hstd_ChkDeviceClass(ptr, driver, rootport);
                        usb_ghstd_CheckEnuResult[ptr->ip] = USB_DONE;
                        /* In this function, check device class of       */
                        /*              enumeration flow move to class   */
                        /* "R_usb_hstd_ReturnEnuMGR()" is used to return */
                        if( retval == USB_DONE )
                        {
                            usb_shstd_RegPointer[ptr->ip] = md;
                            flg = 1;    /* break; */
                        }
                    }
                }
                if( flg != 1 )
                {
#ifdef USB_HOST_COMPLIANCE_MODE
                    usb_ghstd_EnumSeq[ptr->ip] = usb_ghstd_EnumSeq[ptr->ip] + 2;
#else /* USB_HOST_COMPLIANCE_MODE */
                    usb_ghstd_EnumSeq[ptr->ip]++;
#endif /* USB_HOST_COMPLIANCE_MODE */
                }
                break;
            /* OTG CV test */
            case USB_OTG_DONE:
                for( flg = 0u, md = 0u; (md < usb_ghstd_DeviceNum[ptr->ip]) && (flg == 0); md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devstate == USB_DETACHED )
                    {
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][0] = rootport; /* root port */
                        driver->rootport    = rootport;
                        driver->devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
                        flg = 1;    /* break; */
                    }
                }
                break;
            /* Descriptor error */
            case USB_ERROR:
                USB_PRINTF0("### Enumeration is stoped(ClassCode-ERROR)\n");
                /* Attach Detect Mode */
                enume_mode = USB_NOTTPL;
                break;
            default:
                /* Attach Detect Mode */
                enume_mode = USB_NONDEVICE;
                break;
            }
            break;
        /* Class Check Result */
        case 5:
            switch(usb_ghstd_CheckEnuResult[ptr->ip])
            {
            case    USB_DONE:
                driver  = &usb_ghstd_DeviceDrv[ptr->ip][usb_shstd_RegPointer[ptr->ip]];
                usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][0] = rootport; /* Root port */
                driver->rootport    = rootport;
                driver->devaddr     = usb_ghstd_DeviceAddr[ptr->ip];
                break;
            case    USB_ERROR:
                enume_mode = USB_NOTTPL;
                break;
            default:
                enume_mode = USB_NONDEVICE;
                break;
            }
            break;
        /* Set Configuration */
        case 6:
            /* Device enumeration function */
            if( usb_hstd_EnumFunction2(&enume_mode) == USB_YES )
            {
                USB_PRINTF0(" Configured Device\n");
                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( usb_ghstd_DeviceAddr[ptr->ip] == driver->devaddr )
                    {
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][1] = USB_CONFIGURED;
                        /* Device speed */
                        usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][4] = usb_ghstd_DeviceSpeed[ptr->ip];
                        /* Device state */
                        driver->devstate        = USB_CONFIGURED;
                        (*driver->devconfig)(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)USB_NO_ARG); /* Call Back */
                        return (USB_COMPLETEPIPESET);
                    }
                }
                enume_mode = USB_COMPLETEPIPESET;
            }
            break;

        default:
            break;
        }
        usb_ghstd_EnumSeq[ptr->ip]++;
        /* Device Enumeration */
        if( enume_mode == USB_DEVICEENUMERATION )
        {
            switch( usb_ghstd_EnumSeq[ptr->ip] )
            {
            case 1:
                (*usb_ghstd_EnumarationProcess[1])(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceAddr[ptr->ip]);
                break;
            case 5:
                break;
            case 6:
                descriptor_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor[ptr->ip];
                /* Device state */
                usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][2] = descriptor_table[5];
                (*usb_ghstd_EnumarationProcess[6])(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)(descriptor_table[5]));
                break;
#ifdef USB_HOST_COMPLIANCE_MODE
            case 7:
                enume_mode = USB_NOTTPL;
            break;
#endif /* USB_HOST_COMPLIANCE_MODE */
            default:
                (*usb_ghstd_EnumarationProcess[usb_ghstd_EnumSeq[ptr->ip]])(ptr, 
                    usb_ghstd_DeviceAddr[ptr->ip], usb_ghstd_EnumSeq[ptr->ip]);
                break;
            }
        }
        break;
    case USB_DATA_ERR:
        USB_PRINTF0("### Enumeration is stoped(SETUP or DATA-ERROR)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    case USB_DATA_OVR:
        USB_PRINTF0("### Enumeration is stoped(receive data over)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    case USB_DATA_STALL:
        USB_PRINTF0("### Enumeration is stoped(SETUP or DATA-STALL)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        /* Device enumeration function */
        usb_hstd_EnumFunction4(&usb_ghstd_EnumSeq[ptr->ip], &enume_mode, usb_ghstd_DeviceAddr[ptr->ip]);
        break;
    default:
        USB_PRINTF0("### Enumeration is stoped(result error)\n");
        usb_hstd_EnumerationErr(usb_ghstd_EnumSeq[ptr->ip]);
        break;
    }
    return (enume_mode);
}
/******************************************************************************
End of function usb_hstd_Enumeration
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumerationErr
Description     : Output error information when enumeration error occurred.
Argument        : uint16_t Rnum             : enumeration sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumerationErr(uint16_t Rnum)
{
/* Condition compilation by the difference of useful function */
#ifdef USB_DEBUGPRINT_PP
    switch( Rnum ) 
    {
    case 0:     USB_PRINTF0(" Get_DeviceDescrip(8)\n");     break;
    case 1:     USB_PRINTF0(" Set_Address\n");              break;
    case 2:     USB_PRINTF0(" Get_DeviceDescrip(18)\n");    break;
    case 3:     USB_PRINTF0(" Get_ConfigDescrip(9)\n");     break;
    case 4:     USB_PRINTF0(" Get_ConfigDescrip(xx)\n");    break;
    /* Device enumeration function */
    case 5:     usb_hstd_EnumFunction5();                   break;
    case 6:     USB_PRINTF0(" Set_Configuration\n");        break;
    default:    break;
    }
#endif  /* USB_DEBUGPRINT_PP */
}
/******************************************************************************
End of function usb_hstd_EnumerationErr
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ChkDeviceClass
Description     : Interface class search
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : USB_HCDREG_t *driver      : Class driver
                : uint16_t port             : Port no.
Return          : uint16_t                  : USB_DONE / USB_ERROR
******************************************************************************/
uint16_t usb_hstd_ChkDeviceClass(USB_UTR_t *ptr, USB_HCDREG_t *driver, uint16_t port)
{
    uint8_t     *descriptor_table;
    uint16_t    total_length1;
    uint16_t    total_length2;
    uint16_t    result;
    uint16_t    hub_device;
    uint16_t    *table[9];
    uint16_t    tmp4;
    uint16_t    tmp5;
    uint16_t    tmp6;
#ifdef USB_HOST_COMPLIANCE_MODE
    uint16_t    vendor_id;
    uint16_t    product_id;
    uint16_t    id_check;
    uint16_t    i;
#endif /* USB_HOST_COMPLIANCE_MODE */
    
    
    descriptor_table = (uint8_t*)usb_ghstd_DeviceDescriptor[ptr->ip];
    
    /* Device class check */
    tmp4 = descriptor_table[USB_DEV_B_DEVICE_CLASS];
    tmp5 = descriptor_table[USB_DEV_B_DEVICE_SUBCLASS];
    tmp6 = descriptor_table[USB_DEV_B_DEVICE_PROTOCOL];
    hub_device = USB_DONE;
    if( ((tmp4 == 0xFF) && (tmp5 == 0xFF)) && (tmp6 == 0xFF) )
    {
        USB_PRINTF0("*** Vendor specific device.\n\n");
    }
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x00) )
    {
        USB_PRINTF0("*** Full-Speed HUB device.\n\n");
        hub_device = USB_FSHUB;
    } 
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x01) )
    {
        USB_PRINTF0("*** High-Speed single TT device.\n\n");
        hub_device = USB_HSHUBS;
    }
    else if( ((tmp4 == USB_IFCLS_HUB) && (tmp5 == 0x00)) && (tmp6 == 0x02) )
    {
        USB_PRINTF0("*** High-Speed multiple TT device.\n\n");
        hub_device = USB_HSHUBM;
    }
    else if( ((tmp4 != 0) || (tmp5 != 0)) || (tmp6 != 0) )
    {
        USB_PRINTF0("### Device class information error!\n\n");
    }
    else
    {
    }
    
    
#ifdef USB_HOST_COMPLIANCE_MODE
    id_check = USB_ERROR;
    for( i = 0; i < driver->tpl[0]; i++ )
    {
        vendor_id = (uint16_t)(descriptor_table[USB_DEV_ID_VENDOR_L]
                  + ((uint16_t)descriptor_table[USB_DEV_ID_VENDOR_H] << 8));
        
        if( (driver->tpl[(i * 2) + 2] == USB_NOVENDOR) || (driver->tpl[(i * 2) + 2] == vendor_id) )
        {
            product_id = (uint16_t)(descriptor_table[USB_DEV_ID_PRODUCT_L]
                       + ((uint16_t)descriptor_table[USB_DEV_ID_PRODUCT_H] << 8));
            
            if( (driver->tpl[(i * 2) + 3] == USB_NOPRODUCT) || (driver->tpl[(i * 2) + 3] == product_id) )
            {
                id_check = USB_DONE;
                USB_COMPLIANCE_DISP(ptr, USB_COMP_TPL, USB_NO_ARG);
                USB_COMPLIANCE_DISP(ptr, USB_COMP_VID, vendor_id);
                USB_COMPLIANCE_DISP(ptr, USB_COMP_PID, product_id);
            }
        }
    }
    
    if( id_check == USB_ERROR )
    {
        USB_PRINTF0("### Not support device\n");
        if( descriptor_table[4] == USB_IFCLS_HUB )
        {
            USB_COMPLIANCE_DISP(ptr, USB_COMP_HUB, vendor_id);
        }
        else
        {
            USB_COMPLIANCE_DISP(ptr, USB_COMP_NOTTPL, vendor_id);
        }
        
        return USB_ERROR;
    }
#endif /* USB_HOST_COMPLIANCE_MODE */
    
    
    descriptor_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor[ptr->ip];
    total_length1 = 0;
    total_length2 = (uint16_t)(descriptor_table[USB_DEV_W_TOTAL_LENGTH_L]
                  + ((uint16_t)descriptor_table[USB_DEV_W_TOTAL_LENGTH_H] << 8));
    
    if( total_length2 > USB_CONFIGSIZE )
    {
        total_length2 = USB_CONFIGSIZE;
    }

    /* Search within configuration total-length */
    while( total_length1 < total_length2 )
    {
        switch( descriptor_table[total_length1 + 1] )
        {
        /* Configuration Descriptor ? */
        case USB_DT_CONFIGURATION:
            table[1] = (uint16_t*)&descriptor_table[total_length1];
            break;
        /* Interface Descriptor ? */
        case USB_DT_INTERFACE:
            if( driver->ifclass == (uint16_t)descriptor_table[total_length1 + 5] )
            {
                    result = USB_ERROR;
                    table[0]    = (uint16_t*)&usb_ghstd_DeviceDescriptor[ptr->ip];
                    table[2]    = (uint16_t*)&descriptor_table[total_length1];
                    table[3]    = &result;
                    table[4]    = &hub_device;
                    table[5]    = &port;
                    table[6]    = &usb_ghstd_DeviceSpeed[ptr->ip];
                    table[7]    = &usb_ghstd_DeviceAddr[ptr->ip];
                    table[8]    = (uint16_t*)driver->pipetbl;
                    (*driver->classcheck)(ptr, (uint16_t**)&table);
                    /* Interface Class */
                    usb_ghstd_DeviceInfo[ptr->ip][usb_ghstd_DeviceAddr[ptr->ip]][3]
                        = descriptor_table[total_length1 + 5];
                    return result;
            }
/*          USB_PRINTF2("*** Interface class is 0x%02x (not 0x%02x)\n", 
                            descriptor_table[total_length1 + 5], driver->ifclass);*/
            break;
        default:
            break;
        }
        total_length1 += descriptor_table[total_length1];
        if( descriptor_table[total_length1] == 0 )
        {
            break;
        }
    }
    return USB_ERROR;
}
/******************************************************************************
End of function usb_hstd_ChkDeviceClass
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_NotifAtorDetach
Description     : Notify MGR (manager) task that attach or detach occurred.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t result   : Result.
                : uint16_t port     : Port no.
Return          : none
******************************************************************************/
void usb_hstd_NotifAtorDetach(USB_UTR_t *ptr, uint16_t result, uint16_t port)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_AORDETACH, port, result);
}
/******************************************************************************
End of function usb_hstd_NotifAtorDetach
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_OvcrNotifiation
Description     : Notify MGR (manager) task that overcurrent was generated
Argument        : uint16_t port     : Port no.
Return          : none
******************************************************************************/
void usb_hstd_OvcrNotifiation(USB_UTR_t *ptr, uint16_t port)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_OVERCURRENT, port, (uint16_t)0u);
}
/******************************************************************************
End of function usb_hstd_OvcrNotifiation
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_StatusResult
Description     : This is a callback as a result of calling 
                : R_usb_hstd_ChangeDeviceState. This notifies the MGR (manager) 
                : task that the change of USB Device status completed.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t port     : Port no.
                : uint16_t result   : Result.
Return          : none
******************************************************************************/
void usb_hstd_StatusResult(USB_UTR_t *ptr, uint16_t port, uint16_t result)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_STATUSRESULT, port, result);
}
/******************************************************************************
End of function usb_hstd_StatusResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SubmitResult
Description     : Callback after completion of a standard request.
Argument        : uint16_t *utr_table   : Message.
Return          : none
******************************************************************************/
void usb_hstd_SubmitResult(USB_UTR_t *ptr, uint16_t data1, uint16_t data2)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_SUBMITRESULT, ptr->keyword, ptr->status);
}
/******************************************************************************
End of function usb_hstd_SubmitResult
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_TransferEndResult
Description     : Notify the MGR (manager) task that force-termination of data 
                : transfer completed.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t Result   : Transfer result
                : uint16_t Pipe             : Pipe No
Return          : none
******************************************************************************/
void usb_hstd_TransferEndResult(USB_UTR_t *ptr, uint16_t result, uint16_t pipe)
{
    usb_hstd_MgrSndMbx(ptr, (uint16_t)USB_MSG_MGR_TRANSENDRESULT, pipe, result);
}
/******************************************************************************
End of function usb_hstd_TransferEndResult
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumGetDescriptor
Description     : Send GetDescriptor to the connected USB device.
Argument        : uint16_t addr             : Device Address
                : uint16_t cnt_value        : Enumeration sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumGetDescriptor(USB_UTR_t *ptr, uint16_t addr, uint16_t cnt_value)
{
    uint8_t         *data_table;

    switch( cnt_value )
    {
    case 0:
        /* continue */
    case 1:
        /* continue */
    case 5:
        usb_shstd_StdRequest[ptr->ip][0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[ptr->ip][1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)0x0040;
        if( usb_shstd_StdRequest[ptr->ip][3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[ptr->ip][3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg[ptr->ip].tranadr = usb_ghstd_DeviceDescriptor[ptr->ip];
        break;
    case 2:
        usb_shstd_StdRequest[ptr->ip][0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[ptr->ip][1] = (uint16_t)USB_DEV_DESCRIPTOR;
        usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)0x0012;
        if( usb_shstd_StdRequest[ptr->ip][3] > USB_DEVICESIZE )
        {
            usb_shstd_StdRequest[ptr->ip][3] = USB_DEVICESIZE;
        }
        usb_shstd_StdReqMsg[ptr->ip].tranadr = usb_ghstd_DeviceDescriptor[ptr->ip];
        break;
    case 3:
        usb_shstd_StdRequest[ptr->ip][0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[ptr->ip][1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)0x0009;
        usb_shstd_StdReqMsg[ptr->ip].tranadr = usb_ghstd_ConfigurationDescriptor[ptr->ip];
        break;
    case 4:
        data_table = (uint8_t*)usb_ghstd_ConfigurationDescriptor[ptr->ip];
        usb_shstd_StdRequest[ptr->ip][0] = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
        usb_shstd_StdRequest[ptr->ip][1] = (uint16_t)USB_CONF_DESCRIPTOR;
        usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)(((uint16_t)data_table[3] << 8) + (uint16_t)data_table[2]);
        if( usb_shstd_StdRequest[ptr->ip][3] > USB_CONFIGSIZE )
        {
            usb_shstd_StdRequest[ptr->ip][3] = USB_CONFIGSIZE;
            USB_PRINTF0("***WARNING Descriptor size over !\n");
        }
        usb_shstd_StdReqMsg[ptr->ip].tranadr = usb_ghstd_ConfigurationDescriptor[ptr->ip];
        break;
    default:
        return;
        break;
    }
    usb_shstd_StdRequest[ptr->ip][4] = addr;
    usb_shstd_StdReqMsg[ptr->ip].keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg[ptr->ip].tranlen     = (uint32_t)usb_shstd_StdRequest[ptr->ip][3];
    usb_shstd_StdReqMsg[ptr->ip].setup       = usb_shstd_StdRequest[ptr->ip];
    usb_shstd_StdReqMsg[ptr->ip].status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg[ptr->ip].complete    = (USB_CB_t)&usb_hstd_SubmitResult;
    usb_shstd_StdReqMsg[ptr->ip].segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg[ptr->ip].ipp         = ptr->ipp;
    usb_shstd_StdReqMsg[ptr->ip].ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg[ptr->ip]);
}
/******************************************************************************
End of function usb_hstd_EnumGetDescriptor
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumSetAddress
Description     : Send SetAddress to the connected USB device.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t addr     : Device Address.
                : uint16_t setaddr  : New address.
Return          : none
******************************************************************************/
void usb_hstd_EnumSetAddress(USB_UTR_t *ptr, uint16_t addr, uint16_t setaddr)
{
    usb_shstd_StdRequest[ptr->ip][0] = USB_SET_ADDRESS | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
    usb_shstd_StdRequest[ptr->ip][1] = setaddr;
    usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
    usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)0x0000;
    usb_shstd_StdRequest[ptr->ip][4] = addr;
    usb_shstd_StdReqMsg[ptr->ip].keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg[ptr->ip].tranadr     = (void *)&usb_shstd_DummyData;
    usb_shstd_StdReqMsg[ptr->ip].tranlen     = (uint32_t)usb_shstd_StdRequest[ptr->ip][3];
    usb_shstd_StdReqMsg[ptr->ip].setup       = usb_shstd_StdRequest[ptr->ip];
    usb_shstd_StdReqMsg[ptr->ip].status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg[ptr->ip].complete    = (USB_CB_t)&usb_hstd_SubmitResult;
    usb_shstd_StdReqMsg[ptr->ip].segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg[ptr->ip].ipp         = ptr->ipp;
    usb_shstd_StdReqMsg[ptr->ip].ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg[ptr->ip]);
}
/******************************************************************************
End of function usb_hstd_EnumSetAddress
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumSetConfiguration
Description     : Send SetConfiguration to the connected USB device.
Argument        : uint16_t addr     : Device Address.
                : uint16_t confnum  : Configuration number.
Return          : none
******************************************************************************/
void usb_hstd_EnumSetConfiguration(USB_UTR_t *ptr, uint16_t addr, uint16_t confnum)
{
    usb_shstd_StdRequest[ptr->ip][0] = USB_SET_CONFIGURATION | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
    usb_shstd_StdRequest[ptr->ip][1] = confnum;
    usb_shstd_StdRequest[ptr->ip][2] = (uint16_t)0x0000;
    usb_shstd_StdRequest[ptr->ip][3] = (uint16_t)0x0000;
    usb_shstd_StdRequest[ptr->ip][4] = addr;
    usb_shstd_StdReqMsg[ptr->ip].keyword     = (uint16_t)USB_PIPE0;
    usb_shstd_StdReqMsg[ptr->ip].tranadr     = (void *)&usb_shstd_DummyData;
    usb_shstd_StdReqMsg[ptr->ip].tranlen     = (uint32_t)usb_shstd_StdRequest[ptr->ip][3];
    usb_shstd_StdReqMsg[ptr->ip].setup       = usb_shstd_StdRequest[ptr->ip];
    usb_shstd_StdReqMsg[ptr->ip].status      = USB_DATA_NONE;
    usb_shstd_StdReqMsg[ptr->ip].complete    = (USB_CB_t)&usb_hstd_SubmitResult;
    usb_shstd_StdReqMsg[ptr->ip].segment     = USB_TRAN_END;

    usb_shstd_StdReqMsg[ptr->ip].ipp         = ptr->ipp;
    usb_shstd_StdReqMsg[ptr->ip].ip          = ptr->ip;

    usb_hstd_TransferStart(&usb_shstd_StdReqMsg[ptr->ip]);
}
/******************************************************************************
End of function usb_hstd_EnumSetConfiguration
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_EnumDummyRequest
Description     : Dummy function.
Argument        : uint16_t addr             : Device Address
                : uint16_t cnt_value        : Enumeration Sequence
Return          : none
******************************************************************************/
void usb_hstd_EnumDummyRequest(USB_UTR_t *ptr, uint16_t addr, uint16_t cnt_value)
{
}
/******************************************************************************
End of function usb_hstd_EnumDummyRequest
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrSuspend
Description     : Suspend request
Argument        : uint16_t info     : Info for release of memory block.
Return          : none
******************************************************************************/
void usb_hstd_MgrSuspend(USB_UTR_t *ptr, uint16_t info)
{
    uint16_t        rootport, devaddr, devsel;
    uint16_t        j;

    devaddr     = usb_shstd_MgrMsg[ptr->ip]->keyword;
    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);

    if( usb_hstd_ChkDevAddr(ptr, devsel, rootport) != USB_NOCONNECT )
    {
        /* PIPE suspend */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            /* Agreement device address */
            if( usb_cstd_GetDevsel(ptr, j) == devsel )
            {
                /* PID=BUF ? */
                if( usb_cstd_GetPid(ptr, j) == USB_PID_BUF )
                {
                    usb_cstd_SetNak(ptr, j);
                    usb_ghstd_SuspendPipe[ptr->ip][j] = USB_SUSPENDED;
                }
            }
        }
        usb_shstd_SuspendSeq[ptr->ip]=0;
        usb_hstd_SuspCont(ptr, (uint16_t)devaddr, (uint16_t)rootport);
        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_SUSPENDED_PROCESS;
    }
    usb_hstd_MgrRelMpl(ptr,info);
}
/******************************************************************************
End of function usb_hstd_MgrSuspend
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DeviceStateControl
Description     : Setup a call to the function usb_hstd_ChangeDeviceState to re-
                : quest the connected USB Device to change status.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t devaddr      : Device address.
                : uint16_t msginfo      : Request type.
Return          : none
******************************************************************************/
void usb_hstd_DeviceStateControl(USB_UTR_t *ptr, uint16_t devaddr, uint16_t msginfo)
{
    switch( devaddr )
    {
    case 0:
        USB_PRINTF0("### usbd_message device address error\n");
        break;
    case USB_DEVICEADDR:
        usb_hstd_ChangeDeviceState(ptr, &usb_hstd_StatusResult, msginfo, (uint16_t)USB_PORT0);
        break;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    case (USB_DEVICEADDR + 1u):
        usb_hstd_ChangeDeviceState(ptr, &usb_hstd_StatusResult, msginfo, (uint16_t)USB_PORT1);
        break;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    default:
        if( USB_HUBDPADDR <= devaddr )
        {
        }
        break;
    }
}
/******************************************************************************
End of function usb_hstd_DeviceStateControl
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_DeviceStateControl2
Description     : Setup a call to the function usb_hstd_ChangeDeviceState to re-
                : quest the connected USB Device to change status.
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : USB_CB_t complete         : Callback function Pointer
                : uint16_t devaddr          : Device address
                : uint16_t msginfo          : Request type for HCD
                : uint16_t mgr_msginfo      : Request type for MGR
Return          : none
******************************************************************************/
void usb_hstd_DeviceStateControl2(  USB_UTR_t *ptr, USB_CB_t complete, 
                                    uint16_t devaddr, uint16_t msginfo,
                                    uint16_t mgr_msginfo)
{
    usb_shstd_mgr_callback[ptr->ip] = complete;
    usb_shstd_mgr_msginfo[ptr->ip] = mgr_msginfo;
    usb_hstd_DeviceStateControl(ptr, devaddr, msginfo);
}
/******************************************************************************
End of function usb_hstd_DeviceStateControl2
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrReset
Description     : Request HCD (Host Control Driver) to do a USB bus reset.
Argument        : uint16_t devaddr      : Device address.
Return          : none
******************************************************************************/
void usb_hstd_MgrReset(USB_UTR_t *ptr, uint16_t addr)
{
    usb_hstd_DeviceStateControl(ptr, addr, (uint16_t)USB_MSG_HCD_USBRESET);
    if( addr == USB_DEVICEADDR )
    {
        usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DEFAULT;
    }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    else if( addr == (USB_DEVICEADDR + 1u) )
    {
        usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DEFAULT;
    }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
    else
    {
    }
}
/******************************************************************************
End of function usb_hstd_MgrReset
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_MgrResume
Description     : Request HCD (Host Control Device) to send RESUME signal.
Argument        : uint16_t info : Information.
Return          : none
******************************************************************************/
void usb_hstd_MgrResume(USB_UTR_t *ptr, uint16_t info)
{
    uint16_t        rootport, devaddr, devsel;

    devaddr     = usb_shstd_MgrMsg[ptr->ip]->keyword;
    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    /* Get root port number from device addr */
    rootport    = usb_hstd_GetRootport(ptr, devsel);
    if( usb_hstd_ChkDevAddr(ptr, devsel, rootport) != USB_NOCONNECT )
    {
        /* Device resume */
        usb_hstd_DeviceStateControl(ptr, devaddr, usb_shstd_MgrMsg[ptr->ip]->msginfo);
        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_RESUME_PROCESS;
        usb_shstd_ResumeSeq[ptr->ip] = 0;
    }
    usb_hstd_MgrRelMpl(ptr,info);
}
/******************************************************************************
End of function usb_hstd_MgrResume
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_SuspCont
Description     : Suspend the connected USB Device (Function for nonOS)
Arguments       : USB_UTR_t *ptr : USB system internal structure.
                : uint16_t devaddr          : Device Address
                : uint16_t rootport         : Port no.
Return          : none
******************************************************************************/
void usb_hstd_SuspCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport)
{
    uint16_t        checkerr;
    
    checkerr    = usb_shstd_MgrMsg[ptr->ip]->result;

    switch(usb_shstd_SuspendSeq[ptr->ip])
    {
    case    0:
        usb_hstd_GetConfigDesc(ptr, devaddr, (uint16_t)0x09, (USB_CB_t)&usb_hstd_SubmitResult);
        usb_shstd_SuspendSeq[ptr->ip]++;
        break;
    case    1:
        if (usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            if( usb_hstd_ChkRemote(ptr) == USB_YES )
            {
                usb_hstd_SetFeature(ptr, devaddr, (uint16_t)0xFF, (USB_CB_t)&usb_hstd_SubmitResult);
                usb_shstd_SuspendSeq[ptr->ip]++;
            }
            else
            {
                USB_PRINTF0("### Remote wakeup disable\n");
                usb_hstd_DeviceStateControl(ptr, devaddr, (uint16_t)USB_MSG_HCD_REMOTE);
                usb_ghstd_MgrMode[ptr->ip][rootport]    = USB_SUSPENDED;
            }
        }
        break;
    case    2:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            usb_hstd_DeviceStateControl(ptr, devaddr, (uint16_t)USB_MSG_HCD_REMOTE);
            usb_ghstd_MgrMode[ptr->ip][rootport]    = USB_SUSPENDED;
        }
        break;
    default:
        break;
    }
}
/******************************************************************************
End of function usb_hstd_SuspCont
******************************************************************************/

/******************************************************************************
Function Name   : usb_hstd_ResuCont
Description     : Resume the connected USB Device (Function for nonOS)
Argument        : uint16_t devaddr          : Device Address
                : uint16_t rootport         : Port no.
Return          : none
******************************************************************************/
void usb_hstd_ResuCont(USB_UTR_t *ptr, uint16_t devaddr, uint16_t rootport)
{
    uint16_t            devsel;
    uint16_t            j,md;
    USB_HCDREG_t        *driver;
    uint16_t            checkerr;

    devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
    checkerr    = usb_shstd_MgrMsg[ptr->ip]->result;
    
    switch(usb_shstd_ResumeSeq[ptr->ip])
    {
    case    0:
        usb_hstd_GetConfigDesc(ptr, devaddr, (uint16_t)0x09, (USB_CB_t)&usb_hstd_SubmitResult);
        usb_shstd_ResumeSeq[ptr->ip]++;
        break;
    case    1:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            if( usb_hstd_ChkRemote(ptr) == USB_YES )
            {
                usb_hstd_ClearFeature(ptr, devaddr, (uint16_t)0xFF, (USB_CB_t)&usb_hstd_SubmitResult);
                usb_shstd_ResumeSeq[ptr->ip]++;
            }
            else
            {
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;
            }
        }
        break;
    case    2:
        if(usb_hstd_StdReqCheck(checkerr) == USB_DONE)
        {
            usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;
        }
        break;
    default:
        break;
    }
    
    if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_CONFIGURED )
    {
        /* PIPE resume */
        for( j = USB_MIN_PIPE_NO; j <= USB_MAX_PIPE_NO; j++ )
        {
            /* Agreement device address */
            if(usb_cstd_GetDeviceAddress(ptr, j) == devsel)
            {
                if(usb_ghstd_SuspendPipe[ptr->ip][j] == USB_SUSPENDED)
                {
                    usb_cstd_SetBuf(ptr, j);
                    usb_ghstd_SuspendPipe[ptr->ip][j] = USB_DONE;
                }
            }
        }

        for(md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++)
        {
            driver  = &usb_ghstd_DeviceDrv[ptr->ip][md];
            if(driver->devaddr == (rootport+USB_DEVICEADDR))
            {
                (*driver->devresume)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);
                /* Device state */
                usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_CONFIGURED;

                if( usb_shstd_mgr_msginfo[ptr->ip] == USB_DO_GLOBAL_RESUME )
                {
                    usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                }

                /* Device state */
                driver->devstate = USB_CONFIGURED;
            }
        }
    }
}
/******************************************************************************
End of function usb_hstd_ResuCont
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_ChkRemote
Description     : check remote
Arguments       : none
Return value    : uint16_t                  : error info
******************************************************************************/
uint16_t usb_hstd_ChkRemote(USB_UTR_t *ptr)
{
    if( (usb_ghstd_ClassData[ptr->ip][7] & USB_CF_RWUPON) != (uint8_t)0 )
    {
        return USB_YES;
    }
    return USB_NO;
}
/******************************************************************************
End of function usb_hstd_ChkRemote
******************************************************************************/


/* Condition compilation by the difference of IP */
/******************************************************************************
Function Name   : usb_hstd_CmdSubmit
Description     : command submit
Arguments       : USB_CB_t complete         : callback info
Return value    : uint16_t                  : USB_DONE
******************************************************************************/
uint16_t usb_hstd_CmdSubmit(USB_UTR_t *ptr, USB_CB_t complete)
{
    usb_ghstd_ClassControl[ptr->ip].tranadr = (void*)usb_ghstd_ClassData[ptr->ip];
    usb_ghstd_ClassControl[ptr->ip].complete    = complete;
    usb_ghstd_ClassControl[ptr->ip].tranlen = (uint32_t)usb_ghstd_ClassRequest[ptr->ip][3];
    usb_ghstd_ClassControl[ptr->ip].keyword = USB_PIPE0;
    usb_ghstd_ClassControl[ptr->ip].setup   = usb_ghstd_ClassRequest[ptr->ip];
    usb_ghstd_ClassControl[ptr->ip].segment = USB_TRAN_END;

    usb_ghstd_ClassControl[ptr->ip].ip = ptr->ip;
    usb_ghstd_ClassControl[ptr->ip].ipp = ptr->ipp;

    usb_hstd_TransferStart(&usb_ghstd_ClassControl[ptr->ip]);
    
    return USB_DONE;
}
/******************************************************************************
End of function usb_hstd_CmdSubmit
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_SetFeature
Description     : Set SetFeature
Arguments       : uint16_t addr            : device address
                : uint16_t epnum           : endpoint number
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_SetFeature(USB_UTR_t *ptr, uint16_t addr, uint16_t epnum, USB_CB_t complete)
{
    if( epnum == 0xFF )
    {
        /* SetFeature(Device) */
        usb_ghstd_ClassRequest[ptr->ip][0]   = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_DEVICE;
        usb_ghstd_ClassRequest[ptr->ip][1]   = USB_DEV_REMOTE_WAKEUP;
        usb_ghstd_ClassRequest[ptr->ip][2]   = (uint16_t)0x0000;
    }
    else
    {
        /* SetFeature(endpoint) */
        usb_ghstd_ClassRequest[ptr->ip][0]   = USB_SET_FEATURE | USB_HOST_TO_DEV | USB_STANDARD | USB_ENDPOINT;
        usb_ghstd_ClassRequest[ptr->ip][1]   = USB_ENDPOINT_HALT;
        usb_ghstd_ClassRequest[ptr->ip][2]   = epnum;
    }
    usb_ghstd_ClassRequest[ptr->ip][3]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[ptr->ip][4]   = addr;

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_SetFeature
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetConfigDesc
Description     : Set GetConfigurationDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t length          : descriptor length
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_GetConfigDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t length, USB_CB_t complete)
{
    uint16_t        i;

    /* Get Configuration Descriptor */
    usb_ghstd_ClassRequest[ptr->ip][0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_ghstd_ClassRequest[ptr->ip][1]   = USB_CONF_DESCRIPTOR;
    usb_ghstd_ClassRequest[ptr->ip][2]   = (uint16_t)0x0000;
    usb_ghstd_ClassRequest[ptr->ip][3]   = length;
    if( usb_ghstd_ClassRequest[ptr->ip][3] > CLSDATASIZE )
    {
        usb_ghstd_ClassRequest[ptr->ip][3] = (uint16_t)CLSDATASIZE;
        USB_PRINTF0("***WARNING Descriptor size over !\n");
    }
    usb_ghstd_ClassRequest[ptr->ip][4]   = addr;

    for( i = 0; i < usb_ghstd_ClassRequest[ptr->ip][3]; i++ )
    {
        usb_ghstd_ClassData[ptr->ip][i] = (uint8_t)0xFF;
    }

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_GetConfigDesc
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_StdReqCheck
Description     : Sample Standard Request Check
Arguments       : uint16_t errcheck        : error
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_StdReqCheck(uint16_t errcheck)
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
}
/******************************************************************************
End of function usb_hstd_StdReqCheck
******************************************************************************/


/******************************************************************************
Function Name   : usb_hstd_GetStringDesc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : USB_CB_t complete        : callback function
Return value    : uint16_t                 : error info
******************************************************************************/
uint16_t usb_hstd_GetStringDesc(USB_UTR_t *ptr, uint16_t addr, uint16_t string, USB_CB_t complete)
{
    uint16_t        i;

    if( string == 0 )
    {
        usb_ghstd_ClassRequest[ptr->ip][2] = (uint16_t)0x0000;
        usb_ghstd_ClassRequest[ptr->ip][3] = (uint16_t)0x0004;
    }
    else
    {
        /* Set LanguageID */
        usb_ghstd_ClassRequest[ptr->ip][2] = (uint16_t)(usb_ghstd_ClassData[ptr->ip][2]);
        usb_ghstd_ClassRequest[ptr->ip][2] |= (uint16_t)((uint16_t)(usb_ghstd_ClassData[ptr->ip][3]) << 8);
        usb_ghstd_ClassRequest[ptr->ip][3] = (uint16_t)CLSDATASIZE;
    }
    usb_ghstd_ClassRequest[ptr->ip][0]   = USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE;
    usb_ghstd_ClassRequest[ptr->ip][1]   = (uint16_t)(USB_STRING_DESCRIPTOR + string);
    usb_ghstd_ClassRequest[ptr->ip][4]   = addr;

    for( i = 0; i < usb_ghstd_ClassRequest[ptr->ip][3]; i++ )
    {
        usb_ghstd_ClassData[ptr->ip][i] = (uint8_t)0xFF;
    }

    return usb_hstd_CmdSubmit(ptr, complete);
}
/******************************************************************************
End of function usb_hstd_GetStringDesc
******************************************************************************/


#ifdef USB_HOST_COMPLIANCE_MODE
#ifdef USB_HS_EL_TEST
/******************************************************************************
Function Name   : usb_hstd_ElectricalTestMode
Description     : Host electrical test mode function
Argument        : product_id          : Task Start Code
                : port                : rootport number
Return          : none
******************************************************************************/
void usb_hstd_ElectricalTestMode(USB_UTR_t *ptr, uint16_t product_id, uint16_t port)
{
    uint16_t    brdysts;


    switch(product_id)
    {
    case 0x0101:    /* Test_SE0_NAK */
        usb_hstd_TestSignal(ptr, port, 3);
        while(1);   /* This loops infinitely until it's reset. */
        break;
    case 0x0102:    /* Test_J */
        usb_hstd_TestSignal(ptr, port, 1);
        while(1);   /* This loops infinitely until it's reset. */
        break;
    case 0x0103:    /* Test_K */
        usb_hstd_TestSignal(ptr, port, 2);
        while(1);   /* This loops infinitely until it's reset. */
        break;
    case 0x0104:    /* Test_Packet */
        usb_hstd_TestSignal(ptr, port, 4);
        while(1);   /* This loops infinitely until it's reset. */
        break;
    case 0x0105:    /* Reserved */
        break;
    case 0x0106:    /* HS_HOST_PORT_SUSPEND_RESUME */
        usb_cpu_DelayXms(15000);  /* wait 15sec */
        usb_hstd_TestSuspend(ptr, port);
        usb_cpu_DelayXms(15000);  /* wait 15sec */
        usb_hstd_TestResume(ptr, port);
        break;
    case 0x0107:    /* SINGLE_STEP_GET_DEV_DESC */
        usb_cpu_DelayXms(15000);  /* wait 15sec */
        usb_hreg_write_usbreq(ptr, (USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE));
        usb_hreg_set_usbval(ptr, USB_DEV_DESCRIPTOR);
        usb_hreg_set_usbleng(ptr, 0x0012);
        usb_hreg_set_sureq(ptr);
        break;
    case 0x0108:    /* SINGLE_STEP_GET_DEV_DESC_DATA */
        usb_hreg_write_usbreq(ptr, (USB_GET_DESCRIPTOR | USB_DEV_TO_HOST | USB_STANDARD | USB_DEVICE));
        usb_hreg_set_usbval(ptr, USB_DEV_DESCRIPTOR);
        usb_hreg_set_usbindx(ptr, 0x0000);
        usb_hreg_set_usbleng(ptr, 0x0012);
        usb_hreg_set_sureq(ptr);
        usb_cpu_DelayXms(15000);  /* wait 15sec */

        usb_cstd_SetNak(ptr, USB_PIPE0);
        usb_creg_write_dcpcfg(ptr, 0);
        usb_creg_rmw_fifosel(ptr, USB_CUSE, (USB_RCNT|USB_PIPE0), (USB_RCNT|USB_ISEL|USB_CURPIPE));
        usb_creg_set_bclr(ptr, USB_CUSE);
        usb_cstd_SetBuf(ptr,USB_PIPE0);
        do
        {
            brdysts = usb_creg_read_brdysts(ptr);
        }while( !(brdysts&USB_BRDY0));
        usb_cstd_SetNak(ptr, USB_PIPE0);
        usb_creg_set_sqclr(ptr, USB_PIPE0);
        usb_creg_set_bclr(ptr, USB_CUSE);
        break;
    default:
        break;
    }
}
/******************************************************************************
End of function usb_hstd_ElectricalTestMode
******************************************************************************/
#endif /* USB_HS_EL_TEST */
#endif /* USB_HOST_COMPLIANCE_MODE */

#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */

/******************************************************************************
Function Name   : usb_hstd_MgrTask
Description     : The host manager (MGR) task.
Argument        : USB_VP_INT stacd          : Task Start Code
Return          : none
******************************************************************************/
void usb_hstd_MgrTask(USB_VP_INT stacd)
{
#if (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP)
    USB_UTR_t       *mess, *ptr;
    USB_ER_t        err;
    USB_HCDREG_t    *driver;
    USB_HCDINFO_t   *hp;
    uint16_t        rootport, devaddr, devsel, pipenum, msginfo;
    uint16_t        md;
    uint16_t        enume_mode; /* Enumeration mode (device state) */
    uint16_t        connect_speed;
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
    uint16_t        elseport;
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
#ifdef FREE_RTOS_PP
    for( ;; )
    {
#endif
    /* Receive message */
    err = USB_TRCV_MSG(USB_MGR_MBX, (USB_MSG_t**)&mess, (USB_TM_t)10000);
    if ( (err != USB_E_OK) && (err != USB_E_TMOUT) )
    {
#ifdef FREE_RTOS_PP
        continue;
#else
        return;
#endif
    }
    else
    {
        usb_shstd_MgrMsg[mess->ip]    = (USB_MGRINFO_t*)mess;
        rootport    = usb_shstd_MgrMsg[mess->ip]->keyword;
        devaddr     = usb_shstd_MgrMsg[mess->ip]->keyword;
        pipenum     = usb_shstd_MgrMsg[mess->ip]->keyword;
        devsel      = (uint16_t)(devaddr << USB_DEVADDRBIT);
        hp          = (USB_HCDINFO_t*)mess;
        ptr         = mess;

        /* Detach is all device */
        msginfo = usb_shstd_MgrMsg[ptr->ip]->msginfo;
        switch( usb_shstd_MgrMsg[ptr->ip]->msginfo )
        {
        /* USB-bus control (change device state) */
        case USB_MSG_MGR_STATUSRESULT:
            switch( usb_ghstd_MgrMode[ptr->ip][rootport] )
            {
            /* End of reset signal */
            case USB_DEFAULT:
                usb_ghstd_DeviceSpeed[ptr->ip] = usb_shstd_MgrMsg[ptr->ip]->result;
                /* Set device speed */
                usb_hstd_SetDevAddr(ptr, (uint16_t)USB_DEVICE_0, usb_ghstd_DeviceSpeed[ptr->ip], rootport);
                usb_ghstd_DcpRegister[ptr->ip][0] = (uint16_t)(USB_DEFPACKET + USB_DEVICE_0);
                usb_ghstd_EnumSeq[ptr->ip] = 0;
                switch( usb_ghstd_DeviceSpeed[ptr->ip] )
                {
                case USB_HSCONNECT: /* Hi Speed Device Connect */
                    USB_PRINTF0(" Hi-Speed Device\n");
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    break;
                case USB_FSCONNECT: /* Full Speed Device Connect */
                    USB_PRINTF0(" Full-Speed Device\n");
                    (*usb_ghstd_EnumarationProcess[0])(ptr, (uint16_t)USB_DEVICE_0, (uint16_t)0);
                    break;
                case USB_LSCONNECT: /* Low Speed Device Connect */
                    USB_PRINTF0(" Low-Speed Device\n");
                    usb_hstd_LsConnectFunction(ptr);
                    break;
                default:
                    USB_PRINTF0(" Device/Detached\n");
                    usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;
                    break;
                }
                break;
            /* End of resume signal */
            case USB_CONFIGURED:
                /* This Resume Sorce is moved to usb_hResuCont() by nonOS */
                break;
            /* Start of suspended state */
            case USB_SUSPENDED:
                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devaddr == (rootport + USB_DEVICEADDR) )
                    {
                        (*driver->devsuspend)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);

                        if( usb_shstd_mgr_msginfo[ptr->ip] == USB_DO_GLOBAL_SUSPEND )
                        {
                            usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                        }
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_SUSPENDED;
                        /* Device state */
                        driver->devstate = USB_SUSPENDED;
                    }
                }
                break;
            /* Continue of resume signal */
            case    USB_RESUME_PROCESS:
                /* Resume Sequence Number is 0 */
                usb_hstd_ResuCont(ptr, (uint16_t)(USB_DEVICEADDR + rootport), (uint16_t)rootport);
                break;

            case    USB_DETACHED:
                switch( usb_shstd_mgr_msginfo[ptr->ip] )
                {
                case    USB_PORT_DISABLE:
                    usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;

        case USB_MSG_MGR_SUBMITRESULT:
            /* Agreement device address */
            devsel      = usb_cstd_GetDevsel(ptr, pipenum);
            /* Get root port number from device addr */
            rootport    = usb_hstd_GetRootport(ptr, devsel);
            switch(usb_ghstd_MgrMode[ptr->ip][rootport])
            {
            /* Resume */
            case    USB_RESUME_PROCESS:
                /* Resume Sequence Number is 1 to 2 */
                usb_hstd_ResuCont(ptr, (uint16_t)(devsel >> USB_DEVADDRBIT), (uint16_t)rootport);
                break;
            /* Suspend */
            case    USB_SUSPENDED_PROCESS:
                usb_hstd_SuspCont(ptr, (uint16_t)(devsel >> USB_DEVADDRBIT), (uint16_t)rootport);
                break;
            /* Enumeration */
            case    USB_DEFAULT:
                /* Peripheral Device Speed support check */
                connect_speed = usb_hstd_support_speed_check( ptr, rootport );
                if( connect_speed != USB_NOCONNECT )
                {
                    enume_mode = usb_hstd_Enumeration(ptr);
                    switch( enume_mode )
                    {
                    /* Detach Mode */
                    case USB_NONDEVICE:
                        USB_PRINTF1("### Enumeration error (address%d)\n", usb_ghstd_DeviceAddr[ptr->ip]);
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;

                        if( ( usb_shstd_mgr_msginfo[ptr->ip] == USB_DO_RESET_AND_ENUMERATION )
                            || ( usb_shstd_mgr_msginfo[ptr->ip] == USB_PORT_ENABLE ) )
                        {
                            usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                        }
                        break;
                    /* Detach Mode */
                    case USB_NOTTPL:
                        USB_PRINTF1("### Not support device (address%d)\n", usb_ghstd_DeviceAddr[ptr->ip]);
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;

                        if( ( usb_shstd_mgr_msginfo[ptr->ip] == USB_DO_RESET_AND_ENUMERATION )
                            || ( usb_shstd_mgr_msginfo[ptr->ip] == USB_PORT_ENABLE ) )
                        {
                            usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                        }
                        break;
                    case USB_COMPLETEPIPESET:
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_CONFIGURED;

                        if( ( usb_shstd_mgr_msginfo[ptr->ip] == USB_DO_RESET_AND_ENUMERATION )
                            || ( usb_shstd_mgr_msginfo[ptr->ip] == USB_PORT_ENABLE ) )
                        {
                            usb_hstd_mgr_chgdevst_cb(ptr, rootport);
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            default:
                break;
            }
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;
        case USB_MSG_MGR_AORDETACH:
            switch( usb_shstd_MgrMsg[ptr->ip]->result )
            {
            case USB_DETACH:
#ifdef USB_HOST_COMPLIANCE_MODE
                USB_COMPLIANCE_DISP(ptr, USB_COMP_DETACH, USB_NO_ARG);
#endif /* USB_HOST_COMPLIANCE_MODE */
                USB_PRINTF1(" [Detach Device port%d] \n", rootport);
                usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DETACHED;
                usb_ghstd_DeviceSpeed[ptr->ip] = USB_NOCONNECT;

                for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
                {
                    driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                    if( driver->devaddr == (rootport + USB_DEVICEADDR) )
                    {
                        (*driver->devdetach)(ptr, driver->devaddr, (uint16_t)USB_NO_ARG);
                        /* Root port */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
                        /* Device state */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;
                        /* Not configured */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;
                        /* Interface Class : NO class */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
                        /* No connect */
                        usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;
                        
                        /* Root port */
                        driver->rootport    = USB_NOPORT;
                        /* Device address */
                        driver->devaddr     = USB_NODEVICE;
                        /* Device state */
                        driver->devstate    = USB_DETACHED;
                    }
                }
                usb_hstd_MgrRelMpl(ptr,msginfo);
                break;
            case USB_ATTACHL:
                /* continue */
            case USB_ATTACHF:
#ifdef USB_HOST_COMPLIANCE_MODE
                USB_COMPLIANCE_DISP(ptr, USB_COMP_ATTACH, USB_NO_ARG);
#endif /* USB_HOST_COMPLIANCE_MODE */
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
                elseport = 0;
                if( rootport == 0 )
                {
                    elseport = 1;
                }
                if( usb_ghstd_MgrMode[ptr->ip][elseport] == USB_DEFAULT )
                {
                    /* 2ms wait */
                    usb_cpu_DelayXms((uint16_t)2);
/* Enumeration wait setting--------------- */
                    if(usb_ghstd_EnuWait[ptr->ip] != (uint8_t)USB_MGR_TSK)
                    {
                        usb_shstd_MgrMsg[ptr->ip]->msginfo = USB_MSG_CLS_WAIT;
                        usb_shstd_MgrMsg[ptr->ip]->keyword = rootport;
                        err = USB_SND_MSG( usb_ghstd_EnuWait[ptr->ip], (USB_MSG_t*)usb_shstd_MgrMsg[ptr->ip] );
                    }
                    else
                    {
                        err = USB_SND_MSG(USB_MGR_MBX, (USB_MSG_t*)usb_shstd_MgrMsg[ptr->ip]);
                    }
/* Enumeration wait setting end------------ */
                    if( err != USB_E_OK )
                    {
                        USB_PRINTF1("### hMgrTask snd_msg error (%ld)\n", err);
                    }
                }
                else if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_DETACHED )
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;
                    usb_ghstd_DeviceAddr[ptr->ip] = (uint16_t)(rootport + USB_DEVICEADDR);
                    if( USB_MAXDEVADDR < usb_ghstd_DeviceAddr[ptr->ip] )
                    {
                        /* For port1 */
                        USB_PRINTF0("Device address error\n");
                    }
                    else
                    {
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
#ifdef USB_HOST_BC_ENABLE
                        if(USB_BC_SUPPORT_IP == ptr->ip)
                        {
                            /* Call Back */
                            USB_BC_ATTACH(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)g_usb_hstd_bc[ptr->ip].state);
                        }
                        else
                        {
                            USB_BC_ATTACH(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)USB_NOT_BC);
                        }
#endif /* USB_HOST_BC_ENABLE */
                        usb_hstd_AttachFunction();
                        usb_hstd_MgrReset(ptr, usb_ghstd_DeviceAddr[ptr->ip]);
                    }
                    usb_hstd_MgrRelMpl(ptr,msginfo);
                }
#else   /* USB_PORTSEL_PP == USB_2PORT_PP */
                if( usb_ghstd_MgrMode[ptr->ip][rootport] == USB_DETACHED )
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;
                    usb_ghstd_DeviceAddr[ptr->ip] = (uint16_t)(rootport + USB_DEVICEADDR);
                    if( USB_MAXDEVADDR < usb_ghstd_DeviceAddr[ptr->ip] )
                    {
                        /* For port1 */
                        USB_PRINTF0("Device address error\n");
                    }
                    else
                    {
                        usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;

#ifdef USB_HOST_BC_ENABLE
                         /* Call Back */
                         USB_BC_ATTACH(ptr, usb_ghstd_DeviceAddr[ptr->ip], (uint16_t)g_usb_hstd_bc[ptr->ip].state);
#endif /* USB_HOST_BC_ENABLE */
                        usb_hstd_AttachFunction();
                        usb_hstd_MgrReset(ptr, usb_ghstd_DeviceAddr[ptr->ip]);
                    }
                    usb_hstd_MgrRelMpl(ptr,msginfo);
                }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
                else
                {
                    /* enumeration wait setting */
                    usb_ghstd_EnuWait[ptr->ip] = (uint8_t)USB_MGR_TSK;  
                    usb_hstd_MgrRelMpl(ptr,msginfo);
                }
                break;
            default:
                usb_hstd_MgrRelMpl(ptr,msginfo);
                break;
            }
            break;
        case USB_MSG_MGR_OVERCURRENT:
            USB_PRINTF0(" Please detach device \n ");
            USB_PRINTF1("VBUS off port%d\n", rootport);
            usb_hstd_VbusControl(ptr, rootport, (uint16_t)USB_VBOFF);
            usb_ghstd_MgrMode[ptr->ip][rootport] = USB_DEFAULT;
            for( md = 0; md < usb_ghstd_DeviceNum[ptr->ip]; md++ )
            {
                driver = &usb_ghstd_DeviceDrv[ptr->ip][md];
                if( driver->rootport == rootport )
                {
                    USB_OVERCURRENT(ptr, rootport, (uint16_t)USB_NO_ARG);
                    /* Root port */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][0] = USB_NOPORT;
                    /* Device state */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][1] = USB_DETACHED;   
                    /* Not configured */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][2] = (uint16_t)0;    
                    /* Interface Class : NO class */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][3] = (uint16_t)USB_IFCLS_NOT;
                    /* No connect */
                    usb_ghstd_DeviceInfo[ptr->ip][driver->devaddr][4] = (uint16_t)USB_NOCONNECT;
                    
                    /* Root port */
                    driver->rootport    = USB_NOPORT;
                    /* Device address */
                    driver->devaddr     = USB_NODEVICE;
                    /* Device state */
                    driver->devstate    = USB_DETACHED;
                }
            }
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;

        /* USB_MSG_HCD_ATTACH */
        case USB_DO_RESET_AND_ENUMERATION:
            ptr->msginfo = USB_MSG_HCD_ATTACH_MGR;

            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }

            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;

        /* USB_MSG_HCD_VBON */
        case USB_PORT_ENABLE:
            ptr->msginfo = USB_MSG_HCD_VBON;
            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;

        /* USB_MSG_HCD_VBOFF */
        case USB_PORT_DISABLE:
            /* VBUS is off at the time of the abnormalities in a device */
            ptr->msginfo = USB_MSG_HCD_VBOFF;
            if( devaddr == USB_DEVICEADDR )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT0] = USB_DETACHED;
            }
/* Condition compilation by the difference of the devices */
#if USB_PORTSEL_PP == USB_2PORT_PP
            else if( devaddr == (USB_DEVICEADDR + 1u) )
            {
                usb_ghstd_MgrMode[ptr->ip][USB_PORT1] = USB_DETACHED;
            }
#endif  /* USB_PORTSEL_PP == USB_2PORT_PP */
            else
            {
            }
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;

        /* USB_MSG_HCD_SUSPEND */
        case USB_DO_GLOBAL_SUSPEND:
            ptr->msginfo = USB_MSG_HCD_REMOTE;
            usb_shstd_mgr_callback[ptr->ip] = hp->complete;
            usb_shstd_mgr_msginfo[ptr->ip] = msginfo;
            usb_hstd_MgrSuspend(ptr, msginfo);
            break;

        /* USB_MSG_HCD_SUSPEND */
        case USB_DO_SELECTIVE_SUSPEND:
            ptr->msginfo = USB_MSG_HCD_REMOTE;
            usb_hstd_MgrSuspend(ptr, msginfo);
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_DO_GLOBAL_RESUME:
            ptr->msginfo = USB_MSG_HCD_RESUME;
            usb_shstd_mgr_callback[ptr->ip] = hp->complete;
            usb_shstd_mgr_msginfo[ptr->ip] = msginfo;
            usb_hstd_MgrResume(ptr, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_MSG_HCD_RESUME:
            usb_shstd_mgr_msginfo[ptr->ip] = msginfo;
            usb_hstd_MgrResume(ptr, msginfo);
            break;

        /* USB_MSG_HCD_RESUME */
        case USB_DO_SELECTIVE_RESUME:
            ptr->msginfo = USB_MSG_HCD_RESUME;
            usb_hstd_MgrResume(ptr, msginfo);
            usb_hstd_DeviceStateControl2(ptr, hp->complete, devaddr, ptr->msginfo, msginfo);
            break;

        default:
            usb_hstd_MgrRelMpl(ptr,msginfo);
            break;
        }
    }
#ifdef FREE_RTOS_PP
    }
#endif
#endif  /* (USB_FUNCSEL_USBIP0_PP == USB_HOST_PP) || (USB_FUNCSEL_USBIP1_PP == USB_HOST_PP) */
}
/******************************************************************************
End of function usb_hstd_MgrTask
******************************************************************************/


/******************************************************************************
End  Of File
******************************************************************************/
