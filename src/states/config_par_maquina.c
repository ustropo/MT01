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
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR,
	STATE_CONFIG_VAR
};

uint32_t pm_init_values[CFG_PAR_MAQ_MAX] =
{
		20,                             	  //!< Altura de deslocamento
		20,                                    //!< Modo maquina
		20,
		400,
		400,
		6000,
		10000,
		10000,
		900,
		75.3982236862,
		75.3982236862,
		3,
		0.2,
		160000
};

/* Initial values for each config variable */
ut_config_type pm_init_types[CFG_PAR_MAQ_MAX] =
{
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
	UT_CONFIG_INT,
};

char* pm_init_names[CFG_PAR_MAQ_MAX] =
{
	" EIXO X1",                 //!< Altura de deslocamento
	" EIXO X2",                   	  //!< Modo maquina
	" EIXO Y",                 //!< Parametros maquina
	" JERK X",
	" JERK Y",
	" JERK Z",
	" VEL. MAX X",
	" VEL. MAX Y",
	" VEL. MAX Z",
	" MM POR REV. X",
	" MM POR REV Y",
	" MM POR REV Z",
	" DESVIO DE JUNTA",
	" ACEL. DE JUNTA",
};

float pm_init_max[CFG_PAR_MAQ_MAX] =
{
	20,                             	  //!< Altura de deslocamento
	20,                                    //!< Modo maquina
	20,
	10000,
	10000,
	10000,
	20000,
	20000,
	20000,
	1000,
	1000,
	1000,
	1,
	2000000,
};

float pm_init_min[CFG_PAR_MAQ_MAX] =
{
	-20,                                    //!< Altura de deslocamento
	-20,                                    //!< Modo maquina
	-20,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,

};

uint8_t pm_init_point[CFG_PAR_MAQ_MAX] =
{
	1,                                   //!< Altura de deslocamento
	1,                                   //!< Modo maquina
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	0,
};

float pm_init_step[CFG_PAR_MAQ_MAX] =
{
	0.1,                                  //!< Altura de deslocamento
	0.1,                                 //!< Modo maquina
	0.1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	0.01,
	1,
};

char* pm_init_unit[CFG_PAR_MAQ_MAX] =
{
	"mm",                                 //!< Altura de deslocamento
	"mm",                                 //!< Modo maquina
	"mm",
	"mm/min^3",
	"mm/min^3",
	"mm/min^3",
	"mm/min",
	"mm/min",
	"mm/min",
	"mm/rev",
	"mm/rev",
	"mm/rev",
	"mm/min^3",
	"mm/min^3",
};
