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
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_dtc_rx_private.h
* Description  : Functions for using DTC on RX devices.
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2014 1.00    Initial revision
*         : 17.07.2014 2.00    Second  revision
*         : 12.11.2014 2.01    Added RX113.
*         : 30.01.2015 2.02    Added RX71M.
*******************************************************************************/
#ifndef DTC_RX_PRIVATE_H
#define DTC_RX_PRIVATE_H

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
/* Fixed width integer support. */
#include <stdint.h>
/* bool support */
#include <stdbool.h>

#if defined(BSP_MCU_RX113)
    #include ".\src\targets\rx113\r_dtc_rx_target.h"
    #if (DTC_CFG_USE_DMAC_FIT_MODULE == DTC_ENABLE)
        #error "This MCU does not have DMAC module."
        #error "Change to DTC_CFG_USE_DMAC_FIT_MODULE (DTC_DISABLE) in r_dtc_rx_config.h."
    #endif
#elif defined(BSP_MCU_RX111)
    #include ".\src\targets\rx111\r_dtc_rx_target.h"
    #if (DTC_CFG_USE_DMAC_FIT_MODULE == DTC_ENABLE)
        #error "This MCU does not have DMAC module."
        #error "Change to DTC_CFG_USE_DMAC_FIT_MODULE (DTC_DISABLE) in r_dtc_rx_config.h."
    #endif
#elif defined(BSP_MCU_RX110)
    #include ".\src\targets\rx110\r_dtc_rx_target.h"
    #if (DTC_CFG_USE_DMAC_FIT_MODULE == DTC_ENABLE)
        #error "This MCU does not have DMAC module."
        #error "Change to DTC_CFG_USE_DMAC_FIT_MODULE (DTC_DISABLE) in r_dtc_rx_config.h."
    #endif
#elif defined(BSP_MCU_RX63N)
    #include ".\src\targets\rx63n\r_dtc_rx_target.h"
#elif defined(BSP_MCU_RX64M)
    #include ".\src\targets\rx64m\r_dtc_rx_target.h"
#elif defined(BSP_MCU_RX71M)
    #include ".\src\targets\rx71m\r_dtc_rx_target.h"
#else
    #error "This MCU is not supported by the current r_dtc_rx module."
#endif

/*****************************************************************************
Macro definitions
******************************************************************************/
#define DTC_BIG_ENDIAN        (defined(__BIG) || defined(__BIG_ENDIAN__) || defined(__RX_BIG_ENDIAN__))


/*****************************************************************************
Typedef definitions
******************************************************************************/
/* The DTC Mode Register A (MRA) structure */
#pragma bit_order left
#pragma unpack
typedef union dtc_mra {
    uint8_t BYTE;
    struct {
        uint8_t MD:2; /* b7,b6: DTC Transfer Mode Select */
        uint8_t SZ:2; /* DTC Data Transfer Size */
        uint8_t SM:2; /* Transfer Source Address Addressing Mode */
        uint8_t rs:2; /* reserved */
    } BIT;

} dtc_mra_t;

/* The DTC Mode Register B (MRB) structure */
typedef union dtc_mrb {
    uint8_t BYTE;
    struct {
        uint8_t CHNE :1;  /* b7: DTC Chain Transfer Enable */
        uint8_t CHNS :1;  /* DTC Chain Transfer Select */
        uint8_t DISEL:1;  /* DTC Interrupt Select */
        uint8_t DTS  :1;  /* DTC Transfer Mode Select */
        uint8_t DM   :2;  /* Transfer Destination Address Addressing Mode */
        uint8_t rs   :2;  /* reserved */
    } BIT;

} dtc_mrb_t;

/* The DTC Transfer Count Register A (CRA) structure */
typedef union dtc_cra {
    uint16_t WORD;
    struct {
#if (DTC_BIG_ENDIAN)
        uint8_t CRA_H;
        uint8_t CRA_L;
#else /* little endian */
        uint8_t CRA_L;
        uint8_t CRA_H;
#endif
    } BYTE;
} dtc_cra_t;

/* The DTC Transfer Count Register B (CRB) structure */
typedef union dtc_crb {
    uint16_t WORD;
} dtc_crb_t;

#if (DTC_ENABLE == DTC_CFG_SHORT_ADDRRESS_MODE) /* Transfer data in short-address mode */
typedef struct st_dtc_short_transfer_data {
    union {
        uint32_t LWORD;
        struct {
#if (DTC_BIG_ENDIAN) /* Big-Endian */
            dtc_mra_t     MRA;
            uint8_t SAR[3];
#else /* Little-Endian */
            uint8_t SAR[3];
            dtc_mra_t     MRA;
#endif /* (DTC_BIG_ENDIAN) */

        } REG;

    } FIRST_LWORD;
    union {
        uint32_t LWORD;
        struct {
#if (DTC_BIG_ENDIAN) /* Big-Endian */
            dtc_mrb_t       MRB;
            uint8_t   DAR[3];
#else /* Little-Endian */
            uint8_t SAR[3];
            dtc_mrb_t     MRB;
#endif /* (DTC_BIG_ENDIAN) */

        } REG;
    } SECOND_LWORD;
    union {
        uint32_t LWORD;
        struct {
#if (DTC_BIG_ENDIAN) /* Big-Endian */
            dtc_cra_t CRA;
            dtc_crb_t CRB;
#else /* Little-Endian */
            dtc_crb_t CRB;
            dtc_cra_t CRA;
#endif /* (DTC_BIG_ENDIAN) */
        } REG;
    } THIRD_LWORD;
} dtc_internal_registers_t;

#else /* Transfer data in full-address mode */
typedef struct st_dtc_full_transfer_data {
    union {
        uint32_t LWORD;
        struct {
#if (DTC_BIG_ENDIAN) /* Big-Endian */
            dtc_mra_t MRA;
            dtc_mrb_t MRB;
            uint16_t  reserver; /* reserve area */
#else /* Little-Endian */
            uint16_t  reserver; /* reserve area */
            dtc_mrb_t MRB;
            dtc_mra_t MRA;
#endif /* (DTC_BIG_ENDIAN) */
        } REG;
    } FIRST_LWORD;
    union {
        uint32_t SAR;
    } SECOND_LWORD;
    union {
        uint32_t DAR;
    } THIRD_LWORD;
    union {
        uint32_t LWORD;
        struct {
#if (DTC_BIG_ENDIAN) /* Big-Endian */
            dtc_cra_t CRA;
            dtc_crb_t CRB;
#else /* Little-Endian */
            dtc_crb_t CRB;
            dtc_cra_t CRA;
#endif /* (DTC_BIG_ENDIAN) */
        } REG;
    } FOURTH_LWORD;
} dtc_internal_registers_t;

#endif /* DTC_CFG_SHORT_ADDRRESS_MODE */

#pragma bit_order
#pragma packoption


/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
void r_dtc_module_enable(void);
void r_dtc_module_disable(void);
#if ((0 != BSP_CFG_USER_LOCKING_ENABLED) || (bsp_lock_t != BSP_CFG_USER_LOCKING_TYPE) \
      || (DTC_ENABLE != DTC_CFG_USE_DMAC_FIT_MODULE))
bool r_dtc_check_DMAC_locking_byUSER(void);
#endif


#endif /* DTC_RX_PRIVATE_H */

/* End of File */
