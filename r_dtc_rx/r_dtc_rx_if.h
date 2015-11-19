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
* File Name    : r_dtc_rx_if.h
* Description  : Functions for DTC driver
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 17.03.2014 1.00    Initial revision
*         : 17.07.2014 2.00    Second  revision
*         : 12.11.2014 2.01    Added RX113.
*         : 30.01.2015 2.02    Added RX71M.
*******************************************************************************/
#ifndef DTC_RX_IF_H
#define DTC_RX_IF_H

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
/* Includes board and MCU related header files. */
#include "platform.h"
/* Configuration for this package. */
#include "r_dtc_rx_config.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* Version Number of API. */
#define DTC_VERSION_MAJOR  (2)
#define DTC_VERSION_MINOR  (2)

/*******************************************************************************
Typedef definitions
*******************************************************************************/
/* Configurable options for DTC Transfer mode */
typedef enum e_dtc_transfer_mode
{
    DTC_TRANSFER_MODE_NORMAL = (0),      /* = (0 << 6): Normal mode */
    DTC_TRANSFER_MODE_REPEAT = (1 << 6), /* Repeat mode */
    DTC_TRANSFER_MODE_BLOCK  = (2 << 6)  /* Block mode */
} dtc_transfer_mode_t;

/* Configurable options for DTC Data transfer size */
typedef enum e_dtc_data_size
{
    DTC_DATA_SIZE_BYTE  = (0),      /* = (0 << 4): 8-bit (byte) data */
    DTC_DATA_SIZE_WORD  = (1 << 4), /* 16-bit (word) data */
    DTC_DATA_SIZE_LWORD = (2 << 4)  /* 32-bit (long word) data */
} dtc_data_size_t;

/* Configurable options for Source address addressing mode */
typedef enum e_dtc_src_addr_mode
{
    DTC_SRC_ADDR_FIXED =  (0),      /* = (0 << 2): Source address is fixed. */
    DTC_SRC_ADDR_INCR  =  (2 << 2), /* Source address is incremented after each transfer. */
    DTC_SRC_ADDR_DECR  =  (3 << 2)  /* Source address is decremented after each transfer. */
} dtc_src_addr_mode_t;

/* Configurable options for Chain transfer */
typedef enum e_dtc_chain_transfer
{
    DTC_CHAIN_TRANSFER_DISABLE      = (0),      /* Disable Chain transfer. */
    DTC_CHAIN_TRANSFER_ENABLE       = (1 << 7)  /* Enable Chain transfer. */
} dtc_chain_transfer_t;

/* Configurable options for how chain transfer is performed. */
typedef enum e_dtc_chain_transfer_mode
{
    DTC_CHAIN_TRANSFER_CONTINUOUSLY = (0),     /* = (0 << 6): Chain transfer is performed continuously. */
    DTC_CHAIN_TRANSFER_NORMAL       = (1 << 6) /* Chain transfer is performed only when the counter */
                                               /* is changed to 0 or CRAH. */
} dtc_chain_transfer_mode_t;

/* Configurable options for Interrupt */
typedef enum e_dtc_interrupt
{
    DTC_INTERRUPT_AFTER_ALL_COMPLETE  = (0),     /* Interrupt is generated when specified data transfer is completed. */
    DTC_INTERRUPT_PER_SINGLE_TRANSFER = (1 << 5) /* Interrupt is generated when each transfer time is completed. */
} dtc_interrupt_t;

/* Configurable options for Side to be repeat or block */
typedef enum e_dtc_repeat_block_side
{
    DTC_REPEAT_BLOCK_DESTINATION = (0),     /* = (0 << 4): Destination is repeat or block area. */
    DTC_REPEAT_BLOCK_SOURCE      = (1 << 4) /* Source is repeat or block area. */
} dtc_repeat_block_side_t;

/* Configurable options for Destination address addressing mode */
typedef enum e_dtc_dest_addr_mode
{
    DTC_DES_ADDR_FIXED = (1 << 2), /* Destination address is fixed. */
    DTC_DES_ADDR_INCR  = (2 << 2), /* Destination address is incremented after each transfer. */
    DTC_DES_ADDR_DECR  = (3 << 2)  /* Destination address is decremented after each transfer. */
} dtc_dest_addr_mode_t;

/* Enumerate list that can be selected as DTC activation source
 * enum enum_dtce: is included from iodefine.h
 */
typedef enum enum_dtce dtc_activation_source_t;

typedef enum e_dtc_command {
    DTC_CMD_DTC_START,               /* DTC will can accept activation requests. */
    DTC_CMD_DTC_STOP,                /* DTC will not accept new activation request. */
    DTC_CMD_ACT_SRC_ENABLE,          /* Enable an activation source specified by vector number. */
    DTC_CMD_ACT_SRC_DISABLE,         /* Disable an activation source specified by vector number. */
    DTC_CMD_DATA_READ_SKIP_ENABLE,   /* Enable Transfer Data Read Skip. */
    DTC_CMD_DATA_READ_SKIP_DISABLE,  /* Disable Transfer Data Read Skip. */
    DTC_CMD_STATUS_GET,              /* Get the current status of DTC. */
    DTC_CMD_CHAIN_TRANSFER_ABORT     /* Abort the current Chain transfer process. */
} dtc_command_t;

typedef enum e_dtc_err       /* DTC API error codes */
{
    DTC_SUCCESS_DMAC_BUSY = 0,  /* One or some DMAC resources are locked by another process. */
    DTC_SUCCESS,
    DTC_ERR_OPENED,             /* DTC was initialized already. */
    DTC_ERR_NOT_OPEN,           /* DTC module is not initialized yet. */
    DTC_ERR_INVALID_ARG,        /* Arguments are invalid. */
    DTC_ERR_INVALID_COMMAND,    /* Command parameters are invalid. */
    DTC_ERR_NULL_PTR,           /* Argument pointers are NULL. */
    DTC_ERR_BUSY                /* The DTC resources are locked by another process. */

} dtc_err_t;

/* Transfer data type */
#if (DTC_ENABLE == DTC_CFG_SHORT_ADDRRESS_MODE) /* Short-address mode */

typedef struct st_transfer_data { /* 3 long-words */
    uint32_t lw1;
    uint32_t lw2;
    uint32_t lw3;
} dtc_transfer_data_t;

#else /* Full-address mode */
typedef struct st_transfer_data { /* 4 long-words */
    uint32_t lw1;
    uint32_t lw2;
    uint32_t lw3;
    uint32_t lw4;
} dtc_transfer_data_t;
#endif

/* Transfer data configuration */
typedef struct st_dtc_transfer_data_cfg {
    dtc_transfer_mode_t       transfer_mode;         /* DTC transfer mode */
    dtc_data_size_t           data_size;             /* Size of data */
    dtc_src_addr_mode_t       src_addr_mode;         /* Address mode of source */
    dtc_chain_transfer_t      chain_transfer_enable; /* Chain transfer is enabled or not. */
    dtc_chain_transfer_mode_t chain_transfer_mode;   /* How chain transfer is performed. */
    dtc_interrupt_t           response_interrupt;    /* How response interrupt is raised */
    dtc_repeat_block_side_t   repeat_block_side;     /* The side being repeat or block in */
                                                     /*  repeat / block transfer mode. */
    dtc_dest_addr_mode_t      dest_addr_mode;        /* Address mode of destination */
    uint32_t                  source_addr;           /* Start address of source */
    uint32_t                  dest_addr;             /* Start address of destination */
    uint32_t                  transfer_count;        /* Transfer count */
    uint16_t                  block_size;            /* Size of a block in block transfer mode */
} dtc_transfer_data_cfg_t;


typedef struct st_dtc_stat {
    uint8_t    vect_nr;                              /* the current vector number */
    bool       in_progress;                          /* Active flag of DTC module */
} dtc_stat_t;

typedef struct st_dtc_cmd_arg {
    dtc_activation_source_t act_src;                 /* The activation source will be controlled */
    uint32_t                chain_transfer_nr;       /* Number of chain transfer when command */
                                                     /* is DTC_CMD_CHAIN_TRANSFER_ABORT. */
} dtc_cmd_arg_t;


/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
dtc_err_t R_DTC_Open(void);
dtc_err_t R_DTC_Create(dtc_activation_source_t act_source, dtc_transfer_data_t *p_transfer_data,
                        dtc_transfer_data_cfg_t *p_data_cfg, uint32_t chain_transfer_nr);
dtc_err_t R_DTC_Close(void);
dtc_err_t R_DTC_Control(dtc_command_t command, dtc_stat_t *p_stat, dtc_cmd_arg_t *p_args);
uint32_t  R_DTC_GetVersion(void);


#endif /* DTC_RX_IF_H */

/* End of File */

