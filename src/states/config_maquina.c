/*
 * config_maquina.c
 *
 *  Created on: Oct 7, 2016
 *      Author: LAfonso01
 */
#include "ut_context.h"
#include "ut_state.h"
#include "config_maquina.h"
#include "eeprom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ut_config_var configsMaq[CFG_MAQUINA_MAX];

const ut_state geNextStateMaq[CFG_MAQUINA_MAX] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_PARAMETROS_MAQ
};

uint32_t mq_init_values[CFG_MAQUINA_MAX] =
{
	0,
	0,
	0
};

/* Initial values for each config variable */
ut_config_type mq_init_types[CFG_MAQUINA_MAX] =
{
	UT_CONFIG_INT,                        //!< Altura de deslocamento
	UT_CONFIG_BOOL,                       //!< Modo maquina
	NULL
};

char* mq_init_names[CFG_MAQUINA_MAX] =
{
	" ALT. DESLOCAMENTO",                 //!< Altura de deslocamento
	" MODO MÁQUINA",                   	  //!< Modo maquina
	" PARAMETROS EIXOS"                   //!< Parametros maquina
};

float mq_init_max[CFG_MAQUINA_MAX] =
{
	50,                             	  //!< Altura de deslocamento
	0,                                    //!< Modo maquina
	NULL
};

float mq_init_min[CFG_MAQUINA_MAX] =
{
	1,                                    //!< Altura de deslocamento
	0,                                    //!< Modo maquina
	NULL
};

uint8_t mq_init_point[CFG_MAQUINA_MAX] =
{
	1,                                   //!< Altura de deslocamento
	0,                                   //!< Modo maquina
	NULL
};

float mq_init_step[CFG_MAQUINA_MAX] =
{
	0.1,                                  //!< Altura de deslocamento
	0,                                 //!< Modo maquina
	NULL
};

char* mq_init_unit[CFG_MAQUINA_MAX] =
{
	"mm",                                 //!< Altura de deslocamento
	NULL,                                 //!< Modo maquina
	NULL
};
