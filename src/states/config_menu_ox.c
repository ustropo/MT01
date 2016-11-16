/*
 * config_menu_ox.c
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */
#include "ut_context.h"
#include "ut_state.h"
#include "config_menu_ox.h"
#include "eeprom.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ut_config_var configsOx[OX_CONFIG_MAX];
/* Initial values for each config variable */
ut_config_type ox_init_types[OX_CONFIG_MAX] =
{
	UT_CONFIG_INT,                        //!< Altura de perfuração
	UT_CONFIG_INT,                        //!< Altura de corte
	UT_CONFIG_INT,                        //!< Velocidade de corte
	UT_CONFIG_INT,                        //!< Tempo de aquecimento
	UT_CONFIG_INT,                        //!< Tempo de Perfuração
};

char* ox_init_names[OX_CONFIG_MAX] =
{
	" ALT. PERFURAÇÃO",                   //!< Altura de perfuração
	" ALTURA DE CORTE",			          //!< Altura de corte
	" VELOC. CORTE",                      //!< Velocidade de corte
	" TEMPO AQUECIMENTO",                 //!< Tempo de aquecimento
	" TEMPO PERFURAÇÃO",                  //!< Tempo de Perfuração
};

float ox_init_max[OX_CONFIG_MAX] =
{
	50,                                   //!< Altura de perfuração
	50,                                   //!< Altura de corte
	MOTOR_VMAX,                           //!< Velocidade de corte
	300,                                   //!< Tempo de aquecimento
	60,                                   //!< Tempo de Perfuração
};

float ox_init_min[OX_CONFIG_MAX] =
{
	1,                                    //!< Altura de perfuração
	1,                                    //!< Altura de corte
	MOTOR_VMIN,                           //!< Velocidade de corte
	0,                                    //!< Tempo de aquecimento
	0,                                    //!< Tempo de Perfuração
};

uint8_t ox_init_point[OX_CONFIG_MAX] =
{
	1,                                   //!< Altura de perfuração
	1,                                   //!< Altura de corte
	0,                                   //!< Velocidade de corte
	1,                                   //!< Tempo de aquecimento
	1,                                   //!< Tempo de Perfuração
};

float ox_init_step[OX_CONFIG_MAX] =
{
	0.1,                                  //!< Altura de perfuração
	0.1,                                  //!< Altura de corte
	1,                                    //!< Velocidade de corte
	0.1,                                  //!< Tempo de aquecimento
	0.1,                                  //!< Tempo de Perfuração
};

char* ox_init_unit[OX_CONFIG_MAX] =
{
	"mm",                                 //!< Altura de perfuração
	"mm",                                 //!< Altura de corte
	"mm/min",                             //!< Velocidade de corte
	"s",                                  //!< Tempo de aquecimento
	"s",                                  //!< Tempo de Perfuração
};

void initOx(void)
{
	uint8_t i;

	/* Zero all values */
	memset(configsOx, 0, sizeof(configsOx));
	eepromReadConfig(CONFIGVAR_OX);
	/* Initialize all variables */
	for(i = 0; i < OX_CONFIG_MAX; i++)
	{
		configsOx[i].type = ox_init_types[i];
		configsOx[i].value = &configVarOx[i];
		configsOx[i].valueMax = ox_init_max[i];
		configsOx[i].valueMin = ox_init_min[i];
		configsOx[i].name = ox_init_names[i];
		configsOx[i].unit = ox_init_unit[i];
		configsOx[i].step = ox_init_step[i];
		configsOx[i].point = ox_init_point[i];
		configsOx[i].currentState = STATE_CONFIG_MENU_OX;
		configsOx[i].currentItem = i;
	}
}
