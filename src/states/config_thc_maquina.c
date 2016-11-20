/*
 * config_maquina.c
 *
 *  Created on: Oct 7, 2016
 *      Author: LAfonso01
 */
#include "ut_context.h"
#include "ut_state.h"
#include "config_thc_maquina.h"
#include "eeprom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ut_config_var configsTh[CFG_THC_MAX];

const ut_state geNextStateTh[CFG_THC_MAX] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
};

uint32_t th_init_values[CFG_THC_MAX] =
{
	0,
	0,
};

/* Initial values for each config variable */
ut_config_type th_init_types[CFG_THC_MAX] =
{
	UT_CONFIG_BOOL,                        //!< Kerf
	UT_CONFIG_BOOL,                       //!< Mergulho
};

char* th_init_names[CFG_THC_MAX] =
{
	" DETECTOR DE KERF",                 //!< Kerf
	" ANTI MERGULHO",                   //!< Mergulho
};

float th_init_max[CFG_THC_MAX] =
{
	NULL,
	NULL
};

float th_init_min[CFG_THC_MAX] =
{
	NULL,
	NULL
};

uint8_t th_init_point[CFG_THC_MAX] =
{
	NULL,
	NULL
};

float th_init_step[CFG_THC_MAX] =
{
	NULL,
	NULL
};

char* th_init_unit[CFG_THC_MAX] =
{
	NULL,
	NULL
};
