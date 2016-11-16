/*
 * config_menu_pl.c
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */
#include "ut_context.h"
#include "ut_state.h"
#include "config_menu_pl.h"
#include "eeprom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ut_config_var configsPl[PL_CONFIG_MAX];
/* Initial values for each config variable */
ut_config_type pl_init_types[PL_CONFIG_MAX] =
{
	UT_CONFIG_INT,                           //!< Altura de perfuração
	UT_CONFIG_INT,                           //!< Altura de corte
	UT_CONFIG_INT,                           //!< Velocidade de corte
	UT_CONFIG_INT,                           //!< Tempo de Perfuração
	UT_CONFIG_INT                            //!< Tensao do THC
};

char* pl_init_names[PL_CONFIG_MAX] =
{
	" ALT. PERFURAÇÃO",                      //!< Altura de perfuração
	" ALTURA DE CORTE",                      //!< Altura de corte
	" VELOC. CORTE",                  		 //!< Velocidade de corte
	" TEMPO PERFURAÇÃO",                     //!< Tempo de Perfuração
	" TENSÃO THC"                            //!< Tensao do THC
};

float pl_init_max[PL_CONFIG_MAX] =
{
	50,                                      //!< Altura de perfuração
	50,                                      //!< Altura de corte
	MOTOR_VMAX,                              //!< Velocidade de corte
	60,                              		 //!< Tempo de Perfuração
	THC_VMAX,                                //!< Tensao do THC
};

float pl_init_min[PL_CONFIG_MAX] =
{
	1,                                       //!< Altura de perfuração
	1,                                       //!< Altura de corte
	MOTOR_VMIN,                              //!< Velocidade de corte
	0,                              		 //!< Tempo de Perfuração
	THC_VMIN                                 //!< Tensao do THC
};

float pl_init_step[PL_CONFIG_MAX] =
{
	0.1,                                     //!< Altura de perfuração
	0.1,                                     //!< Altura de corte
	1,                                       //!< Velocidade de corte
	0.1,                                     //!< Tempo de Perfuração
	1                                        //!< Tensao do THC
};

uint8_t pl_init_point[PL_CONFIG_MAX] =
{
	1,                                       //!< Altura de perfuração
	1,                                       //!< Altura de corte
	0,                                       //!< Velocidade de corte
	1,                                       //!< Tempo de Perfuração
	0                                        //!< Tensao do THC
};

char* pl_init_unit[PL_CONFIG_MAX] =
{
	"mm",                                   //!< Altura de perfuração    //!< Altura de perfuração
	"mm",                                   //!< Altura de corte         //!< Altura de corte
	"mm/min",                               //!< Velocidade de corte     //!< Velocidade de corte
	"s",                                    //!< Tempo de Perfuração     //!< Tempo de Perfuração
	"V"                                     //!< Tensao do THC           //!< Tensao do THC
};

void initPl(void)
{
	uint8_t i;

	/* Zero all values */
	memset(configsPl, 0, sizeof(configsPl));
	eepromReadConfig(CONFIGVAR_PL);
	/* Initialize all variables */
	for(i = 0; i < PL_CONFIG_MAX; i++)
	{
		configsPl[i].type = pl_init_types[i];
		configsPl[i].value = &configVarPl[i];
		configsPl[i].valueMax = pl_init_max[i];
		configsPl[i].valueMin = pl_init_min[i];
		configsPl[i].name = pl_init_names[i];
		configsPl[i].unit = pl_init_unit[i];
		configsPl[i].step = pl_init_step[i];
		configsPl[i].point = pl_init_point[i];
		configsPl[i].currentState = STATE_CONFIG_MENU_PL;
		configsPl[i].currentItem = i;
	}
}
