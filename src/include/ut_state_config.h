/*
 * ut_state_config.h
 *
 *  Created on: Jan 5, 2016
 *      Author: Fernando
 */

#ifndef INCLUDE_UT_STATE_CONFIG_H_
#define INCLUDE_UT_STATE_CONFIG_H_

#include "FreeRTOS.h"
#include "task.h"
#include "ut_state_config_var.h"

/**
 * Enum to indicate configuration variable type
 */


/**
 * Which configs are available
 */
typedef enum
{
	CONFIG_JOG_LENTO = 0,    //!< CONFIG_JOG_LENTO
	CONFIG_JOG_RAPIDO,   //!< CONFIG_JOG_RAPIDO
	CONFIG_TESTE_DISPARO,//!< CONFIG_TESTE_DISPARO
	CONFIG_MAX           //!< CONFIG_MAX
} ut_config_name;

/* Array with all config variables */
extern ut_config_var configs[CONFIG_MAX];

#endif /* INCLUDE_UT_STATE_CONFIG_H_ */
