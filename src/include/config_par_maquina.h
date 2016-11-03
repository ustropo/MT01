/*
 * config_menu_ox.h
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */

#ifndef INCLUDE_CONFIG_PAR_MAQUINA_H_
#define INCLUDE_CONFIG_PAR_MAQUINA_H_

#include "ut_state_config_var.h"

extern bool reset_flag;

extern ut_config_var configsParMaq[CFG_PAR_MAQ_MAX];
extern ut_config_type pm_init_types[CFG_PAR_MAQ_MAX];
extern char* pm_init_names[CFG_PAR_MAQ_MAX];
extern float pm_init_max[CFG_PAR_MAQ_MAX];
extern float pm_init_min[CFG_PAR_MAQ_MAX];
extern float pm_init_step[CFG_PAR_MAQ_MAX];
extern uint8_t pm_init_point[CFG_PAR_MAQ_MAX];
extern char* pm_init_unit[CFG_PAR_MAQ_MAX];
extern const ut_state geNextStatePar[CFG_PAR_MAQ_MAX];
extern uint32_t pm_init_values[CFG_PAR_MAQ_MAX];

#endif /* INCLUDE_CONFIG_PAR_MAQUINA_H_ */
