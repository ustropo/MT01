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
	STATE_CONFIG_VAR,                //!< EIXO_X1
	STATE_CONFIG_VAR,                //!< EIXO_X2
	STATE_CONFIG_VAR,                //!< EIXO_Y
	STATE_CONFIG_VAR,                //!< JERK X
	STATE_CONFIG_VAR,                //!< JERK Y
	STATE_CONFIG_VAR,                //!< VEL X
	STATE_CONFIG_VAR,                //!< VEL Y
	STATE_CONFIG_VAR,                //!< VEL Z
	STATE_CONFIG_VAR,                //!< JUNCTION DEV
	STATE_CONFIG_VAR,                //!< JUNCTION ACCEL
	STATE_CONFIG_VAR,         		 //!< CHORDAL TOLERANCE
	STATE_CONFIG_VAR,                //!< FORMAT MEM
};

//uint32_t pm_init_values[CFG_PAR_MAQ_MAX] =
//{
//		20,                         //!< EIXO_X1
//		20,                         //!< EIXO_X2
//		20,                         //!< EIXO_Y
//		400,                        //!< JERK X
//		400,                        //!< JERK Y
//		10000,                      //!< VEL X
//		10000,                      //!< VEL Y
//		900,                        //!< VEL Z
//		0.2,                        //!< JUNCTION DEV
//		160000,                     //!< JUNCTION ACCEL
//		CFG_PAR_MAQ_CHORDAL_TOL,            //!< CHORDAL TOLERANCE
//		0,                          //!< FORMAT MEM
//};

/* Initial values for each config variable */
ut_config_type pm_init_types[CFG_PAR_MAQ_MAX] =
{
	UT_CONFIG_INT,                 //!< EIXO_X1
	UT_CONFIG_INT,                 //!< EIXO_X2
	UT_CONFIG_INT,                 //!< EIXO_Y
	UT_CONFIG_INT,                 //!< JERK X
	UT_CONFIG_INT,                 //!< JERK Y
	UT_CONFIG_INT,                 //!< VEL X
	UT_CONFIG_INT,                 //!< VEL Y
	UT_CONFIG_INT,                 //!< VEL Z
	UT_CONFIG_INT,                 //!< JUNCTION DEV
	UT_CONFIG_INT,                 //!< JUNCTION ACCEL
	UT_CONFIG_INT,                 //!< CHORDAL TOLERANCE
	UT_CONFIG_BOOL,                //!< FORMAT MEM
};

char* pm_init_names[CFG_PAR_MAQ_MAX] =
{
	" EIXO X1",                    //!< EIXO_X1
	" EIXO X2",                    //!< EIXO_X2
	" EIXO Y",                     //!< EIXO_Y
	" JERK X",                     //!< JERK X
	" JERK Y",                     //!< JERK Y
	" VEL. MAX X",                 //!< VEL X
	" VEL. MAX Y",                 //!< VEL Y
	" VEL. MAX Z",                 //!< VEL Z
	" DESVIO DE JUNTA",            //!< JUNCTION DEV
	" ACEL. DE JUNTA",             //!< JUNCTION ACCEL
	" TOLERÂNCIA DE ARCO",         //!< CHORDAL TOLERANCE
	" FORMATA MEM"                 //!< FORMAT MEM
};

float pm_init_max[CFG_PAR_MAQ_MAX] =
{
	200,                         //!< EIXO_X1
	200,                         //!< EIXO_X2
	200,                         //!< EIXO_Y
	10000,                       //!< JERK X
	10000,                       //!< JERK Y
	20000,                       //!< VEL X
	20000,                       //!< VEL Y
	20000,                       //!< VEL Z
	10,                           //!< JUNCTION DEV
	2000000,                     //!< JUNCTION ACCEL
	10,            				 //!< CHORDAL TOLERANCE
	1,                           //!< FORMAT MEM
};

float pm_init_min[CFG_PAR_MAQ_MAX] =
{
	0,                        	 //!< EIXO_X1
	0,                        	 //!< EIXO_X2
	0,                        	 //!< EIXO_Y
	0,                           //!< JERK X
	0,                           //!< JERK Y
	0,                           //!< VEL X
	0,                           //!< VEL Y
	0,                           //!< VEL Z
	0,                           //!< JUNCTION DEV
	0,                           //!< JUNCTION ACCEL
	0,            //!< CHORDAL TOLERANCE
	0,                           //!< FORMAT MEM
};

uint8_t pm_init_point[CFG_PAR_MAQ_MAX] =
{
	1,                           //!< EIXO_X1
	1,                           //!< EIXO_X2
	1,                           //!< EIXO_Y
	0,                           //!< JERK X
	0,                           //!< JERK Y
	0,                           //!< VEL X
	0,                           //!< VEL Y
	0,                           //!< VEL Z
	0,                           //!< JUNCTION DEV
	0,                           //!< JUNCTION ACCEL
	0,            //!< CHORDAL TOLERANCE
	0,                           //!< FORMAT MEM
};

float pm_init_step[CFG_PAR_MAQ_MAX] =
{
	0.001,                         //!< EIXO_X1
	0.001,                         //!< EIXO_X2
	0.001,                         //!< EIXO_Y
	1,                           //!< JERK X
	1,                           //!< JERK Y
	1,                           //!< VEL X
	1,                           //!< VEL Y
	1,                           //!< VEL Z
	0.01,                        //!< JUNCTION DEV
	1,                           //!< JUNCTION ACCEL
	0.01,           			 //!< CHORDAL TOLERANCE
	1,                           //!< FORMAT MEM
};

char* pm_init_unit[CFG_PAR_MAQ_MAX] =
{
	"mm",                        //!< EIXO_X1
	"mm",                        //!< EIXO_X2
	"mm",                        //!< EIXO_Y
	"mm/min^3",                  //!< JERK X
	"mm/min^3",                  //!< JERK Y
	"mm/min",                    //!< VEL X
	"mm/min",                    //!< VEL Y
	"mm/min",                    //!< VEL Z
	"mm",                        //!< JUNCTION DEV
	"mm/min^3",                  //!< JUNCTION ACCEL
	"mm",            //!< CHORDAL TOLERANCE
	 NULL,                       //!< FORMAT MEM
};
