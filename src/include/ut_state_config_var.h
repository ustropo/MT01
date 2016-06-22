/*
 * ut_state_config_var.h
 *
 *  Created on: Jan 5, 2016
 *      Author: Fernando
 */

#ifndef INCLUDE_UT_STATE_CONFIG_VAR_H_
#define INCLUDE_UT_STATE_CONFIG_VAR_H_

#include "platform.h"
#include "ut_state.h"

#define THC_VMAX 300
#define THC_VMIN 20
#define MOTOR_VMAX 10000
#define MOTOR_VMIN 10

/**
 * Enum to indicate configuration variable type
 */
typedef enum
{
	UT_CONFIG_INT = 0, //!< UT_CONFIG_INT
	UT_CONFIG_BOOL,//!< UT_CONFIG_BOOL
	UT_CONFIG_NULL,
	/* This should be the last one! */
	UT_CONFIG_MAX  //!< UT_CONFIG_MAX
} ut_config_type;

typedef void (*var_func)(void *);

/**
 * Struct to hold a configuration variable
 * and how it is handled.
 */
typedef struct
{
	ut_config_type type; //!< Type of configuration variable
	void *value;		 //!< Value of variable
	float valueMin;		 //!< valueMin of variable
	float valueMax;		 //!< valueMax of variable
	float step;		 //!< valueMax of variable
	uint8_t point;		 //!< valueMax of variable
	const char* unit;    //!< unit of the variable
	const char* name;    //!< Name of the variable
	ut_state currentState;
	uint8_t currentItem;
	var_func func_var;
} ut_config_var;

extern ut_config_var* configsVar;

#endif /* INCLUDE_UT_STATE_CONFIG_VAR_H_ */
