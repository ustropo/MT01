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
* File Name    : r_usb_cdefusbip.h
* Description  : USB definition for IP
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 04.01.2014 1.00 First Release
*         : 30.01.2015 1.01    Support Multi device.
***********************************************************************************************************************/


#ifndef __R_USB_CDEFUSBIP_H__
#define __R_USB_CDEFUSBIP_H__


/*****************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
USB specification define
******************************************************************************/

/* Standard Device Descriptor Define */
#define USB_DEV_B_LENGTH                    0u      /* Size of descriptor */
#define USB_DEV_B_DESCRIPTOR_TYPE           1u      /* Descriptor type */
#define USB_DEV_BCD_USB_L                   2u      /* USB Specification Release Number */
#define USB_DEV_BCD_USB_H                   3u      /* USB Specification Release Number */
#define USB_DEV_B_DEVICE_CLASS              4u      /* Class code */
#define USB_DEV_B_DEVICE_SUBCLASS           5u      /* Subclass code */
#define USB_DEV_B_DEVICE_PROTOCOL           6u      /* Protocol code */
#define USB_DEV_B_MAX_PACKET_SIZE_0         7u      /* Max packet size for EP0(only 8,16,32,64 are valid) */
#define USB_DEV_ID_VENDOR_L                 8u      /* Vendor ID */
#define USB_DEV_ID_VENDOR_H                 9u      /* Vendor ID */
#define USB_DEV_ID_PRODUCT_L                10u     /* Product ID */
#define USB_DEV_ID_PRODUCT_H                11u     /* Product ID */
#define USB_DEV_BCD_DEVICE_L                12u     /* Device relese number */
#define USB_DEV_BCD_DEVICE_H                13u     /* Device relese number */
#define USB_DEV_I_MANUFACTURER              14u     /* Index of string descriptor describing manufacturer */
#define USB_DEV_I_PRODUCT                   15u     /* Index of string descriptor describing product */
#define USB_DEV_I_SERIAL_NUMBER             16u     /* Device serial number */
#define USB_DEV_B_NUM_CONFIGURATION         17u     /* Number of possible configuration */

/* Standard Configuration Descriptor Define */
//#define   USB_DEV_B_LENGTH                    0u      /* Size of descriptor */
//#define   USB_DEV_B_DESCRIPTOR_TYPE           1u      /* Descriptor type */
#define USB_DEV_W_TOTAL_LENGTH_L            2       /* Total length of data returned for this configuration */
#define USB_DEV_W_TOTAL_LENGTH_H            3       /* Total length of data returned for this configuration */
#define USB_DEV_B_NUM_INTERFACES            4       /* Number of interfaces supported by this configuration */
#define USB_DEV_B_CONFIGURATION_VALUE       5       /* Configuration value */
#define USB_DEV_I_CONFIGURATION             6       /* Index of string descriptor describing this configuration */
#define USB_DEV_BM_ATTRIBUTES               7       /* Configuration characteristics */
#define USB_DEV_B_MAX_POWER                 8       /* Max power consumption of the USB device from the bus */

/* Descriptor type  Define */
#define USB_DT_DEVICE                       0x01u   /* Configuration Descriptor */
#define USB_DT_CONFIGURATION                0x02u   /* Configuration Descriptor */
#define USB_DT_STRING                       0x03u   /* Configuration Descriptor */
#define USB_DT_INTERFACE                    0x04u   /* Interface Descriptor */
#define USB_DT_ENDPOINT                     0x05u   /* Endpoint Descriptor */
#define USB_DT_DEVICE_QUALIFIER             0x06u   /* Device Qualifier Descriptor */
#define USB_DT_OTHER_SPEED_CONF             0x07u   /* Other Speed Configuration Descriptor */
#define USB_DT_INTERFACE_POWER              0x08u   /* Interface Power Descriptor */
#define USB_DT_OTGDESCRIPTOR                0x09u   /* OTG Descriptor */
#define USB_DT_HUBDESCRIPTOR                0x29u   /* HUB descriptor */

/* Device class Define  */
#define USB_DEVCLS_INTF                     0x00u   /* Class information at interface */
#define USB_DEVCLS_COMM                     0x02u   /* Communication Device */
#define USB_DEVCLS_HUB                      0x90u   /* HUB Device */
#define USB_DEVCLS_DIAG                     0xDCu   /* Diagnostic Device */
#define USB_DEVCLS_WIRE                     0xE0u   /* Wireless Controller */
#define USB_DEVCLS_APL                      0xFEu   /* Application-Specific */
#define USB_DEVCLS_VEN                      0xFFu   /* Vendor-Specific */

/* Interface class Define */
#define USB_IFCLS_NOT                       0x00u   /* Un corresponding Class */
#define USB_IFCLS_AUD                       0x01u   /* Audio Class */
#define USB_IFCLS_CDC                       0x02u   /* CDC Class */
#define USB_IFCLS_CDCC                      0x02u   /* CDC-Control Class */
#define USB_IFCLS_HID                       0x03u   /* HID Class */
#define USB_IFCLS_PHY                       0x05u   /* Physical Class */
#define USB_IFCLS_IMG                       0x06u   /* Image Class */
#define USB_IFCLS_PRN                       0x07u   /* Printer Class */
#define USB_IFCLS_MAS                       0x08u   /* Mass Storage Class */
#define USB_IFCLS_HUB                       0x09u   /* HUB Class */
#define USB_IFCLS_CDCD                      0x0Au   /* CDC-Data Class */
#define USB_IFCLS_CHIP                      0x0Bu   /* Chip/Smart Card Class */
#define USB_IFCLS_CNT                       0x0Cu   /* Content-Security Class */
#define USB_IFCLS_VID                       0x0Du   /* Video Class */
#define USB_IFCLS_DIAG                      0xDCu   /* Diagnostic Device */
#define USB_IFCLS_WIRE                      0xE0u   /* Wireless Controller */
#define USB_IFCLS_APL                       0xFEu   /* Application-Specific */
#define USB_IFCLS_VEN                       0xFFu   /* Vendor-Specific Class */

/* Endpoint Descriptor  Define */
#define USB_EP_DIRMASK                      0x80u   /* Endpoint direction mask [2] */
#define USB_EP_IN                           0x80u   /* In Endpoint */
#define USB_EP_OUT                          0x00u   /* Out Endpoint */
#define USB_EP_NUMMASK                      0x0Fu   /* Endpoint number mask [2] */
#define USB_EP_USGMASK                      0x30u   /* Usage type mask [2] */
#define USB_EP_SYNCMASK                     0x0Cu   /* Synchronization type mask [2] */
#define USB_EP_TRNSMASK                     0x03u   /* Transfer type mask [2] */
#define USB_EP_CNTRL                        0x00u   /* Control Transfer */
#define USB_EP_ISO                          0x01u   /* Isochronous Transfer */
#define USB_EP_BULK                         0x02u   /* Bulk Transfer */
#define USB_EP_INT                          0x03u   /* Interrupt Transfer */

/* Configuration descriptor bit define */
#define USB_CF_RESERVED                     0x80u   /* Reserved(set to 1) */
#define USB_CF_SELFP                        0x40u   /* Self Powered */
#define USB_CF_BUSP                         0x00u   /* Bus Powered */
#define USB_CF_RWUPON                       0x20u   /* Remote Wakeup ON */
#define USB_CF_RWUPOFF                      0x00u   /* Remote Wakeup OFF */

/* OTG descriptor bit define */
#define USB_OTG_HNP                         0x02u   /* HNP support */

/* SRP support */
#define USB_OTG_SRP                         0x01u

/* USB Standard request */
/* USB_BREQUEST         0xFF00u(b15-8) */
#define USB_GET_STATUS                      0x0000u
#define USB_CLEAR_FEATURE                   0x0100u
#define USB_REQRESERVED                     0x0200u
#define USB_SET_FEATURE                     0x0300u
#define USB_REQRESERVED1                    0x0400u
#define USB_SET_ADDRESS                     0x0500u
#define USB_GET_DESCRIPTOR                  0x0600u
#define USB_SET_DESCRIPTOR                  0x0700u
#define USB_GET_CONFIGURATION               0x0800u
#define USB_SET_CONFIGURATION               0x0900u
#define USB_GET_INTERFACE                   0x0A00u
#define USB_SET_INTERFACE                   0x0B00u
#define USB_SYNCH_FRAME                     0x0C00u

/* USB_BMREQUESTTYPEDIR 0x0080u(b7) */
#define USB_HOST_TO_DEV                     0x0000u
#define USB_DEV_TO_HOST                     0x0080u

/* USB_BMREQUESTTYPETYPE    0x0060u(b6-5) */
#define USB_STANDARD                        0x0000u
#define USB_CLASS                           0x0020u
#define USB_VENDOR                          0x0040u

/* USB_BMREQUESTTYPERECIP   0x001Fu(b4-0) */
#define USB_DEVICE                          0x0000u
#define USB_INTERFACE                       0x0001u
#define USB_ENDPOINT                        0x0002u
#define USB_OTHER                           0x0003u

/* GET_STATUS request information */
/* Standard Device status */
#define USB_GS_BUSPOWERD                    0x0000u
#define USB_GS_SELFPOWERD                   0x0001u
#define USB_GS_REMOTEWAKEUP                 0x0002u

/* Endpoint status */
#define USB_GS_NOTHALT                      0x0000u
#define USB_GS_HALT                         0x0001u

/* CLEAR_FEATURE/GET_FEATURE/SET_FEATURE request information */
/* Standard Feature Selector */
#define USB_ENDPOINT_HALT                   0x0000u
#define USB_DEV_REMOTE_WAKEUP               0x0001u
#define USB_TEST_MODE                       0x0002u

/* GET_DESCRIPTOR/SET_DESCRIPTOR request information */
/* Standard Descriptor type */
#define USB_HUB_DESCRIPTOR                  0x0000u
#define USB_DEV_DESCRIPTOR                  0x0100u
#define USB_CONF_DESCRIPTOR                 0x0200u
#define USB_STRING_DESCRIPTOR               0x0300u
#define USB_INTERFACE_DESCRIPTOR            0x0400u
#define USB_ENDPOINT_DESCRIPTOR             0x0500u
#define USB_DEV_QUALIFIER_DESCRIPTOR        0x0600u
#define USB_OTHER_SPEED_CONF_DESCRIPTOR     0x0700u
#define USB_INTERFACE_POWER_DESCRIPTOR      0x0800u

/* HUB CLASS REQUEST */
#define USB_HUB_CLEAR_TT_BUFFER             0x0800u
#define USB_HUB_RESET_TT                    0x0900u
#define USB_HUB_GET_TT_STATE                0x0A00u
#define USB_HUB_STOP_TT                     0x0B00u

/* HUB CLASS FEATURE SELECTER */
#define USB_HUB_C_HUB_LOCAL_POWER           0x0000u
#define USB_HUB_C_HUB_OVER_CURRENT          0x0001u
#define USB_HUB_PORT_CONNECTION             0x0000u
#define USB_HUB_PORT_ENABLE                 0x0001u
#define USB_HUB_PORT_SUSPEND                0x0002u
#define USB_HUB_PORT_OVER_CURRENT           0x0003u
#define USB_HUB_PORT_RESET                  0x0004u
#define USB_HUB_PORT_POWER                  0x0008u
#define USB_HUB_PORT_LOW_SPEED              0x0009u
#define USB_HUB_PORT_HIGH_SPEED             0x000Au
#define USB_HUB_C_PORT_CONNECTION           0x0010u
#define USB_HUB_C_PORT_ENABLE               0x0011u
#define USB_HUB_C_PORT_SUSPEND              0x0012u
#define USB_HUB_C_PORT_OVER_CURRENT         0x0013u
#define USB_HUB_C_PORT_RESET                0x0014u
#define USB_HUB_PORT_TEST                   0x0015u
#define USB_HUB_PORT_INDICATOR              0x0016u

/* HUB PORT STAUS */
#define USB_HUB_STS_PORT_CONNECT            0x0001u
#define USB_HUB_STS_PORT_ENABLE             0x0002u
#define USB_HUB_STS_PORT_SUSPEND            0x0004u
#define USB_HUB_STS_PORT_OVRCURRNET         0x0008u
#define USB_HUB_STS_PORT_RESET              0x0010u
#define USB_HUB_STS_PORT_POWER              0x0100u
#define USB_HUB_STS_PORT_LOWSPEED           0x0200u
#define USB_HUB_STS_PORT_FULLSPEED          0x0000u
#define USB_HUB_STS_PORT_HIGHSPEED          0x0400u
#define USB_HUB_STS_PORT_TEST               0x0800u
#define USB_HUB_STS_PORT_INDICATOR          0x1000u

/* HUB PORT CHANGE */
#define USB_HUB_CHG_PORT_CONNECT            0x0001u
#define USB_HUB_CHG_PORT_ENABLE             0x0002u
#define USB_HUB_CHG_PORT_SUSPEND            0x0004u
#define USB_HUB_CHG_PORT_OVRCURRNET         0x0008u
#define USB_HUB_CHG_PORT_RESET              0x0010u


/******************************************************************************
USB-H/W register define
******************************************************************************/

/* Root port */
#define USB_PORT0                           0u
#define USB_PORT1                           1u

/* Device connect information */
#define USB_ATTACH                          0x0040u
#define USB_ATTACHL                         0x0041u
#define USB_ATTACHF                         0x0042u
#define USB_DETACH                          0x0043u

/* Reset Handshake result */
#define USB_NOCONNECT                       0x0000u /* Speed undecidedness */
#define USB_HSCONNECT                       0x00C0u /* Hi-Speed connect */
#define USB_FSCONNECT                       0x0080u /* Full-Speed connect */
#define USB_LSCONNECT                       0x0040u /* Low-Speed connect */

/* Pipe define */
#define USB_USEPIPE                         0x00FEu
#define USB_PERIPIPE                        0x00FDu
#define USB_CLRPIPE                         0x00FCu /* Clear Pipe registration */
#define USB_PIPE0                           0x0000u /* PIPE 0 */
#define USB_PIPE1                           0x0001u /* PIPE 1 */
#define USB_PIPE2                           0x0002u /* PIPE 2 */
#define USB_PIPE3                           0x0003u /* PIPE 3 */
#define USB_PIPE4                           0x0004u /* PIPE 4 */
#define USB_PIPE5                           0x0005u /* PIPE 5 */
#define USB_PIPE6                           0x0006u /* PIPE 6 */
#define USB_PIPE7                           0x0007u /* PIPE 7 */
#define USB_PIPE8                           0x0008u /* PIPE 8 */
#define USB_PIPE9                           0x0009u /* PIPE 9 */
#define USB_MAX_PIPE_NO                     9u      /* PIPE0 ... PIPE9 */

/* Pipe configuration table define */
#define USB_EPL                             6u      /* Pipe configuration table length */
#define USB_TYPFIELD                        0xC000u /* Transfer type */
#define   USB_PERIODIC                      0x8000u /* Periodic pipe */
#define   USB_ISO                           0xC000u /* Isochronous */
#define   USB_INT                           0x8000u /* Interrupt */
#define   USB_BULK                          0x4000u /* Bulk */
#define   USB_NOUSE                         0x0000u /* Not configuration */
#define USB_BFREFIELD                       0x0400u /* Buffer ready interrupt mode select */
#define   USB_BFREON                        0x0400u
#define   USB_BFREOFF                       0x0000u
#define USB_DBLBFIELD                       0x0200u /* Double buffer mode select */
#define   USB_DBLBON                        0x0200u
#define   USB_DBLBOFF                       0x0000u
#define USB_CNTMDFIELD                      0x0100u /* Continuous transfer mode select */
#define   USB_CNTMDON                       0x0100u
#define   USB_CNTMDOFF                      0x0000u
#define USB_SHTNAKFIELD                     0x0080u /* Transfer end NAK */
#define   USB_SHTNAKON                      0x0080u
#define   USB_SHTNAKOFF                     0x0000u
#define USB_DIRFIELD                        0x0010u /* Transfer direction select */
#define   USB_DIR_H_OUT                     0x0010u /* HOST OUT */
#define   USB_DIR_P_IN                      0x0010u /* PERI IN */
#define   USB_DIR_H_IN                      0x0000u /* HOST IN */
#define   USB_DIR_P_OUT                     0x0000u /* PERI OUT */
#define   USB_BUF2FIFO                      0x0010u /* Buffer --> FIFO */
#define   USB_FIFO2BUF                      0x0000u /* FIFO --> buffer */
#define USB_EPNUMFIELD                      0x000Fu /* Endpoint number select */
#define USB_MAX_EP_NO                       15u     /* EP0 EP1 ... EP15 */
#define   USB_EP0                           0x0000u
#define   USB_EP1                           0x0001u
#define   USB_EP2                           0x0002u
#define   USB_EP3                           0x0003u
#define   USB_EP4                           0x0004u
#define   USB_EP5                           0x0005u
#define   USB_EP6                           0x0006u
#define   USB_EP7                           0x0007u
#define   USB_EP8                           0x0008u
#define   USB_EP9                           0x0009u
#define   USB_EP10                          0x000Au
#define   USB_EP11                          0x000Bu
#define   USB_EP12                          0x000Cu
#define   USB_EP13                          0x000Du
#define   USB_EP14                          0x000Eu
#define   USB_EP15                          0x000Fu

#define USB_BUF_SIZE(x)                     ((uint16_t)(((x) / 64u) - 1u) << 10u)
#define USB_BUF_NUMB(x)                     (x)

#define USB_IFISFIELD                       0x1000u /* Isochronous in-buf flash mode */
#define   USB_IFISON                        0x1000u
#define   USB_IFISOFF                       0x0000u
#define USB_IITVFIELD                       0x0007u /* Isochronous interval */
#define USB_IITV_TIME(x)                    (x)

/* FIFO port & access define */
#define USB_CUSE                            0u  /* CFIFO CPU trans */
#define USB_D0USE                           1u  /* D0FIFO CPU trans */
#define USB_D0DMA                           2u  /* D0FIFO DMA trans */
#define USB_D1USE                           3u  /* D1FIFO CPU trans */
#define USB_D1DMA                           4u  /* D1FIFO DMA trans */
#define USB_CUSE2                           5u  /* CFIFO CPU trans (not trans count) */


/******************************************************************************
Another define
******************************************************************************/
/* FIFO read / write result */
#define USB_FIFOERROR                       USB_ERROR   /* FIFO not ready */
#define USB_WRITEEND                        0x0000u     /* End of write (but packet may not be outputting) */
#define USB_WRITESHRT                       0x0001u     /* End of write (send short packet) */
#define USB_WRITING                         0x0002u     /* Write continues */
#define USB_READEND                         0x0000u     /* End of read */
#define USB_READSHRT                        0x0001u     /* Insufficient (receive short packet) */
#define USB_READING                         0x0002u     /* Read continues */
#define USB_READOVER                        0x0003u     /* Buffer size over */

/* Pipe define table end code */
#define USB_PDTBLEND                        0xFFFFu /* End of table */

/* Transfer status Type */
#define USB_CTRL_END                        0u
#define USB_DATA_NONE                       1u
#define USB_DATA_WAIT                       2u
#define USB_DATA_OK                         3u
#define USB_DATA_SHT                        4u
#define USB_DATA_OVR                        5u
#define USB_DATA_STALL                      6u
#define USB_DATA_ERR                        7u
#define USB_DATA_STOP                       8u
#define USB_DATA_TMO                        9u
#define USB_CTRL_READING                    17u
#define USB_CTRL_WRITING                    18u
#define USB_DATA_READING                    19u
#define USB_DATA_WRITING                    20u

/* Utr member (segment) */
#define USB_TRAN_CONT                       0x00u
#define USB_TRAN_END                        0x80u

/* Callback argument */
#define USB_NO_ARG                          0u

/* USB interrupt type (common)*/
#define USB_INT_UNKNOWN                     0x0000u
#define USB_INT_BRDY                        0x0001u
#define USB_INT_BEMP                        0x0002u
#define USB_INT_NRDY                        0x0003u

/* USB interrupt type (PERI)*/
#define USB_INT_VBINT                       0x0011u
#define USB_INT_RESM                        0x0012u
#define USB_INT_SOFR                        0x0013u
#define USB_INT_DVST                        0x0014u
#define USB_INT_CTRT                        0x0015u
#define USB_INT_ATTACH                      0x0016u
#define USB_INT_DETACH                      0x0017u

/* USB interrupt type (HOST)*/
#define USB_INT_OVRCR0                      0x0041u
#define USB_INT_BCHG0                       0x0042u
#define USB_INT_DTCH0                       0x0043u
#define USB_INT_ATTCH0                      0x0044u
#define USB_INT_EOFERR0                     0x0045u
#define USB_INT_PDDETINT0                   0x0046u
#define USB_INT_OVRCR1                      0x0051u
#define USB_INT_BCHG1                       0x0052u
#define USB_INT_ATTCH1                      0x0053u
#define USB_INT_DTCH1                       0x0054u
#define USB_INT_EOFERR1                     0x0055u
#define USB_INT_SACK                        0x0061u
#define USB_INT_SIGN                        0x0062u

/******************************************************************************
HCD driver specification define
******************************************************************************/

/******************************************************************************
HCD driver common define
******************************************************************************/
/* Global macro Define  */
#define USB_UACTON                          (1u)
#define USB_UACTOFF                         (0u)
#define USB_VBON                            (0u)
#define USB_VBOFF                           (1u)


/******************************************************************************
USB specification define
******************************************************************************/
/* Device class Define  */
#define USB_NOVENDOR                        0xFFFFu /* Vendor ID nocheck */
#define USB_NOPRODUCT                       0xFFFFu /* Product ID nocheck */

/* Interface class Define */
#define USB_INTFCLSHET                      0xAAu   /* Host electrical test class */

/******************************************************************************
USB-H/W register define
******************************************************************************/
/* Root port */
#define USB_NOPORT                          0xFFFFu /* Not connect */

/* Max device */
#define USB_MAXPIPE_BULK                    5
#define     USB_MAXPIPE_ISO                 2
#define     USB_MAXPIPE_NUM                 9

 /* Condition compilation by the difference of IP */
#if USB_IP_DEVADD_PP == USB_IP_DEVADD_A_PP
#define USB_MAXDEVADDR                      10u
#else /* USB_IP_DEVADD_PP = USB_IP_DEVADD_A_PP */
#define USB_MAXDEVADDR                      5u
#endif /* USB_IP_DEVADD_PP = USB_IP_DEVADD_A_PP */

#define   USB_DEVICE_0                      0x0000u /* Device address 0 */
#define   USB_DEVICE_1                      0x1000u /* Device address 1 */
#define   USB_DEVICE_2                      0x2000u /* Device address 2 */
#define   USB_DEVICE_3                      0x3000u /* Device address 3 */
#define   USB_DEVICE_4                      0x4000u /* Device address 4 */
#define   USB_DEVICE_5                      0x5000u /* Device address 5 */
#define   USB_DEVICE_6                      0x6000u /* Device address 6 */
#define   USB_DEVICE_7                      0x7000u /* Device address 7 */
#define   USB_DEVICE_8                      0x8000u /* Device address 8 */
#define   USB_DEVICE_9                      0x9000u /* Device address 9 */
#define   USB_DEVICE_A                      0xA000u /* Device address A */
#define   USB_NODEVICE                      0xF000u /* No device */
#define   USB_DEVADDRBIT                    12u

/* DCP Max packetsize */
#define USB_MAXPFIELD                       0x007Fu /* Maxpacket size of DCP */

/******************************************************************************
Another define
******************************************************************************/
/* ControlPipe Max Packet size */
#define USB_DEFPACKET                       0x0040u /* Default DCP Max packet size */

/* Device state define */
#define USB_NONDEVICE                       0u
#define USB_NOTTPL                          1u
#define USB_ATTACHDEVICE                    2u
#define USB_DEVICEENUMERATION               3u
#define USB_DEVICEADDRESSED                 4u
#define USB_DEVICECONFIGURED                5u
#define USB_COMPLETEPIPESET                 10u
#define USB_DEVICESUSPENDED                 20u
#define USB_ELECTRICALTEST                  30u

/* Control Transfer Stage */
#define USB_IDLEST                          0u  /* Idle */
#define USB_SETUPNDC                        1u  /* Setup Stage No Data Control */
#define USB_SETUPWR                         2u  /* Setup Stage Control Write */
#define USB_SETUPRD                         3u  /* Setup Stage Control Read */
#define USB_DATAWR                          4u  /* Data Stage Control Write */
#define USB_DATARD                          5u  /* Data Stage Control Read */
#define USB_STATUSRD                        6u  /* Status stage */
#define USB_STATUSWR                        7u  /* Status stage */
#define USB_SETUPWRCNT                      17u /* Setup Stage Control Write */
#define USB_SETUPRDCNT                      18u /* Setup Stage Control Read */
#define USB_DATAWRCNT                       19u /* Data Stage Control Write */
#define USB_DATARDCNT                       20u /* Data Stage Control Read */

/******************************************************************************
HUB define
******************************************************************************/
/* HUB spec */
#define USB_FSHUB                           1u
#define USB_HSHUBS                          2u
#define USB_HSHUBM                          3u

/* Interface number */
#define USB_HUB_INTNUMFS                    1u
#define USB_HUB_INTNUMHSS                   1u
#define USB_HUB_INTNUMHSM                   1u


/******************************************************************************
OTG define
******************************************************************************/
#define USB_ADEVICE                         0x0000u
#define USB_BDEVICE                         0x0004u

#define USB_UNDER05                         0x0000u
#define USB_MID05TO14                       0x4000u
#define USB_MID14TO45                       0x8000u
#define USB_OVER45                          0xC000u


/*  USB Manager mode    */
#define USB_PORTOFF                         0u      /* Disconnect(VBUSoff) */
#define USB_DETACHED                        10u     /* Disconnect(VBUSon) */
#define USB_ATTACHED                        20u     /* Disconnect(HUBdiconfigured) */
#define USB_POWERED                         30u     /* Start reset handshake */
#define USB_DEFAULT                         40u     /* Set device address */
#define USB_ADDRESS                         50u     /* Enumeration start */
#define USB_ENUMERATION                     60u     /* Wait device enumeration */
#define USB_CONFIGURED                      70u     /* Detach detected */
#define USB_SUSPENDED                       80u     /* Device suspended */
#define USB_DETACH_PROCESS                  101u    /* Wait device detach */
#define USB_SUSPENDED_PROCESS               102u    /* Wait device suspend */
#define USB_RESUME_PROCESS                  103u    /* Wait device resume */
/* Condition compilation by the difference of IP */
#if USB_IPSEL_PP == USB_596IP_PP
#define USB_DISABLEDET                      90u /* Disable detach detected */
#endif  /* USB_596IP_PP */


/******************************************************************************
Task request message type
******************************************************************************/
/* HCD common task message command */
#define USB_MSG_HCD_ATTACH                  0x0101u
#define USB_MSG_HCD_DETACH                  0x0102u
#define USB_MSG_HCD_USBRESET                0x0103u
#define USB_MSG_HCD_SUSPEND                 0x0104u
#define USB_MSG_HCD_RESUME                  0x0105u
#define USB_MSG_HCD_REMOTE                  0x0106u
#define USB_MSG_HCD_VBON                    0x0107u
#define USB_MSG_HCD_VBOFF                   0x0108u
#define USB_MSG_HCD_CLR_STALL               0x0109u
#define USB_MSG_HCD_DETACH_MGR              0x010Au
#define USB_MSG_HCD_ATTACH_MGR              0x010Bu

#define USB_MSG_HCD_CLR_STALL_RESULT        0x010Cu
#define USB_MSG_HCD_CLR_STALLBIT            0x010Du
#define USB_MSG_HCD_SQTGLBIT                0x010Fu

/* HCD task message command */
#define USB_MSG_HCD_SETDEVICEINFO           0x0111u
#define USB_MSG_HCD_SUBMITUTR               0x0112u
#define USB_MSG_HCD_TRANSEND1               0x0113u
#define USB_MSG_HCD_TRANSEND2               0x0114u
#define USB_MSG_HCD_CLRSEQBIT               0x0115u
#define USB_MSG_HCD_SETSEQBIT               0x0116u
#define USB_MSG_HCD_INT                     0x0117u
#define USB_MSG_HCD_PCUTINT                 0x0118u
#define USB_MSG_HCD_DMAINT                  0x0119u

#define USB_MSG_HCD_D0FIFO_INT              0x0141u
#define USB_MSG_HCD_D1FIFO_INT              0x0142u
#define USB_MSG_HCD_RESM_INT                0x0143u
#define USB_MSG_PCD_D0FIFO_INT              0x0144u
#define USB_MSG_PCD_D1FIFO_INT              0x0145u
#define USB_MSG_PCD_RESM_INT                0x0146u

/* USB Manager task message command */
#define USB_MSG_MGR_AORDETACH               0x0121u
#define USB_MSG_MGR_OVERCURRENT             0x0122u
#define USB_MSG_MGR_STATUSRESULT            0x0123u
#define USB_MSG_MGR_SUBMITRESULT            0x0124u
#define USB_MSG_MGR_TRANSENDRESULT          0x0125u

#define USB_MSG_MGR_SETCONFIGURATION        0x0300
#define USB_MSG_MGR_SETCONFIGURATION_RESULT 0x0301

#define USB_MSG_MGR_GETDESCRIPTOR           0x0400
#define USB_MSG_MGR_GETDESCRIPTOR_RESULT    0x0401

/* USB HUB task message command */
#define USB_MSG_HUB_HUB2HUBSTART            0x0131u
#define USB_MSG_HUB_START                   0x0132u
#define USB_MSG_HUB_STOP                    0x0133u
#define USB_MSG_HUB_SUBMITRESULT            0x0134u
#define USB_MSG_HUB_EVENT                   0x0135u /* nonOS */
#define USB_MSG_HUB_ATTACH                  0x0136u /* nonOS */
#define USB_MSG_HUB_RESET                   0x0137u /* nonOS */

/* CLS task message command */
#define USB_MSG_CLS_CHECKREQUEST            0x0201u /* nonOS */
#define USB_MSG_CLS_INIT                    0x0202u /* nonOS */
#define USB_MSG_CLS_TASK                    0x0203u /* nonOS */
#define USB_MSG_CLS_WAIT                    0x0204u /* nonOS */
#define USB_MSG_CLS_PROCESSRESULT           0x0205u /* nonOS */

/* HET task message command */
#define USB_MSG_HET_UACTOFF                 0x0171u
#define USB_MSG_HET_UACTON                  0x0172u
#define USB_MSG_HET_VBUSOFF                 0x0173u
#define USB_MSG_HET_VBUSON                  0x0174u
#define USB_MSG_HET_RESET                   0x0175u
#define USB_MSG_HET_SUSPEND                 0x0176u
#define USB_MSG_HET_RESUME                  0x0177u
#define USB_MSG_HET_ENUMERATION             0x0178u
#define USB_MSG_HET_TESTNONE                0x0181u
#define USB_MSG_HET_TESTPACKET              0x0182u
#define USB_MSG_HET_TESTJ                   0x0183u
#define USB_MSG_HET_TESTK                   0x0184u
#define USB_MSG_HET_TESTSE0                 0x0185u
#define USB_MSG_HET_TESTSIGSTOP             0x0186u
#define USB_MSG_HET_SINGLESETUP             0x0187u
#define USB_MSG_HET_SINGLEDATA              0x0188u

/******************************************************************************
USB driver specification define
******************************************************************************/

/******************************************************************************
Another define
******************************************************************************/
/* Descriptor index */
#define USB_DEV_MAX_PKT_SIZE                7u  /* Index of bMAXPacketSize */
#define USB_DEV_NUM_CONFIG                  17u /* Index of bNumConfigurations */
#define USB_ALT_NO                          255u
#define USB_SOFT_CHANGE                     0u

/******************************************************************************
Task request message type
******************************************************************************/
/* USB Peripheral task message command */
#define USB_MSG_PCD_INT                     0x0151u
#define USB_MSG_PCD_SUBMITUTR               0x0152u
#define USB_MSG_PCD_TRANSEND1               0x0153u
#define USB_MSG_PCD_TRANSEND2               0x0154u
#define USB_MSG_PCD_REMOTEWAKEUP            0x0155u
#define USB_MSG_PCD_DETACH                  0x0161u
#define USB_MSG_PCD_ATTACH                  0x0162u
#define USB_MSG_PCD_CLRSEQBIT               0x0163u
#define USB_MSG_PCD_SETSTALL                0x0164u
#define USB_MSG_PCD_PCUTINT                 0x0156u

#define USB_MSG_PCD_DP_ENABLE               0x0157u
#define USB_MSG_PCD_DP_DISABLE              0x0158u
#define USB_MSG_PCD_DM_ENABLE               0x0159u
#define USB_MSG_PCD_DM_DISABLE              0x015Au

#define USB_MSG_PCD_DMAINT                  0x015bu

#define USB_DO_REMOTEWAKEUP                 USB_MSG_PCD_REMOTEWAKEUP
#define USB_DP_ENABLE                       USB_MSG_PCD_DP_ENABLE
#define USB_DP_DISABLE                      USB_MSG_PCD_DP_DISABLE
#define USB_DM_ENABLE                       USB_MSG_PCD_DM_ENABLE
#define USB_DM_DISABLE                      USB_MSG_PCD_DM_DISABLE

#define USB_DO_STALL                        USB_MSG_PCD_SETSTALL

#define USB_DO_RESET_AND_ENUMERATION        0x0202u /* USB_MSG_HCD_ATTACH */
#define USB_PORT_ENABLE                     0x0203u /* USB_MSG_HCD_VBON */
#define USB_PORT_DISABLE                    0x0204u /* USB_MSG_HCD_VBOFF */
#define USB_DO_GLOBAL_SUSPEND               0x0205u /* USB_MSG_HCD_SUSPEND */
#define USB_DO_SELECTIVE_SUSPEND            0x0206u /* USB_MSG_HCD_SUSPEND */
#define USB_DO_GLOBAL_RESUME                0x0207u /* USB_MSG_HCD_RESUME */
#define USB_DO_SELECTIVE_RESUME             0x0208u /* USB_MSG_HCD_RESUME */
#define USB_DO_CLR_STALL                    0x0209u /* USB_MSG_HCD_CLR_STALL */
#define USB_DO_SET_SQTGL                    0x020Au /* USB_MSG_HCD_SQTGLBIT */
#define USB_DO_CLR_SQTGL                    0x020Bu /* USB_MSG_HCD_CLRSEQBIT */

/******************************************************************************
Host Battery Charging Define
******************************************************************************/
#ifdef USB_HOST_BC_ENABLE
/* BC State Define */
#define USB_BC_STATE_INIT   (0x00u)
#define USB_BC_STATE_DET    (0x01u)
#define USB_BC_STATE_CDP    (0x02u)
#define USB_BC_STATE_SDP    (0x03u)
#define USB_BC_STATE_DCP    (0x04u)
#define USB_BC_STATE_MAX    (0x05u)

/* BC State Change Event Define */
#define USB_BC_EVENT_VB     (0x00u)
#define USB_BC_EVENT_AT     (0x01u)
#define USB_BC_EVENT_DT     (0x02u)
#define USB_BC_EVENT_MAX    (0x03u)

/* DCP Mode Setting Define */
#ifdef USB_BC_DCP_ENABLE
#define USB_BC_DCPMODE      (0x01u)
#else   /* USB_BC_DCP_ENABLE */
#define USB_BC_DCPMODE      (0x00u)
#endif  /* USB_BC_DCP_ENABLE */

#define USB_NOT_BC          (0xFFFEu)

#endif /*  USB_HOST_BC_ENABLE */

/******************************************************************************
Host Compliance Define
******************************************************************************/
#ifdef USB_HOST_COMPLIANCE_MODE
#define USB_COMP_ATTACH     (0x00u)
#define USB_COMP_DETACH     (0x01u)
#define USB_COMP_TPL        (0x02u)
#define USB_COMP_NOTTPL     (0x03u)
#define USB_COMP_HUB        (0x04u)
#define USB_COMP_STOP       (0x05u)
#define USB_COMP_NOTRESP    (0x06u)
#define USB_COMP_VID        (0x07u)
#define USB_COMP_PID        (0x08u)
#define USB_COMP_ERR        (0x09u)
#endif /* USB_HOST_COMPLIANCE_MODE */


#endif  /* __R_USB_CDEFUSBIP_H__ */
/******************************************************************************
End  Of File
******************************************************************************/
