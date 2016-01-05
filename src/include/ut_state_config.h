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

/**
 * Enum to indicate configuration variable type
 */
typedef enum
{
	UT_CONFIG_INT = 0, //!< UT_CONFIG_INT
	UT_CONFIG_BOOL,//!< UT_CONFIG_BOOL
	/* This should be the last one! */
	UT_CONFIG_MAX  //!< UT_CONFIG_MAX
} ut_config_type;

/**
 * Struct to hold a configuration variable
 * and how it is handled.
 */
typedef struct
{
	ut_config_type type; //!< Type of configuration variable
	uint32_t value;		 //!< Value of variable
	const char* name;    //!< Name of the variable
} ut_config_var;

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
