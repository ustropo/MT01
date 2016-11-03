/*
 * config_maquina.c
 *
 *  Created on: Oct 7, 2016
 *      Author: LAfonso01
 */
#include "ut_context.h"
#include "ut_state.h"
#include "config_par_maquina.h"
#include "eeprom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ut_config_var configsParMaq[CFG_PAR_MAQ_MAX];
bool reset_flag;

const ut_state geNextStatePar[CFG_PAR_MAQ_MAX] =
{
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR
};

uint32_t pm_init_values[CFG_PAR_MAQ_MAX] =
{
	0,
	0,
	0
};

/* Initial values for each config variable */
ut_config_type pm_init_types[CFG_PAR_MAQ_MAX] =
{
	UT_CONFIG_INT,                        //!< Altura de deslocamento
	UT_CONFIG_INT,                       //!< Modo maquina
	UT_CONFIG_INT
};

char* pm_init_names[CFG_PAR_MAQ_MAX] =
{
	" EIXO X1",                 //!< Altura de deslocamento
	" EIXO X2",                   	  //!< Modo maquina
	" EIXO Y"                 //!< Parametros maquina
};

float pm_init_max[CFG_PAR_MAQ_MAX] =
{
	20,                             	  //!< Altura de deslocamento
	20,                                    //!< Modo maquina
	20
};

float pm_init_min[CFG_PAR_MAQ_MAX] =
{
	-20,                                    //!< Altura de deslocamento
	-20,                                    //!< Modo maquina
	-20
};

uint8_t pm_init_point[CFG_PAR_MAQ_MAX] =
{
	1,                                   //!< Altura de deslocamento
	1,                                   //!< Modo maquina
	1
};

float pm_init_step[CFG_PAR_MAQ_MAX] =
{
	0.1,                                  //!< Altura de deslocamento
	0.1,                                 //!< Modo maquina
	0.1
};

char* pm_init_unit[CFG_PAR_MAQ_MAX] =
{
	"mm",                                 //!< Altura de deslocamento
	"mm",                                 //!< Modo maquina
	"mm"
};
