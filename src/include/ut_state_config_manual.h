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
	CONFIG_MODO_MANUAL  = 0,    //!<
	CONFIG_ZERAR_EIXOS,   //!<
	CONFIG_DESLOCAR_ZERO,//!<
	CONFIG_JOG_RAP_LENTO,//!<
	CONFIG_MANUAL_MAX           //!< CONFIG_MAX
} ut_config_name;


#endif /* INCLUDE_UT_STATE_CONFIG_H_ */
