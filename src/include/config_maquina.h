/*
 * config_menu_ox.h
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */

#ifndef INCLUDE_CONFIG_MAQUINA_H_
#define INCLUDE_CONFIG_MAQUINA_H_

#include "ut_state_config_var.h"

extern ut_config_var configsMaq[CFG_MAQUINA_MAX];
extern ut_config_type mq_init_types[CFG_MAQUINA_MAX];
extern char* mq_init_names[CFG_MAQUINA_MAX];
extern float mq_init_max[CFG_MAQUINA_MAX];
extern float mq_init_min[CFG_MAQUINA_MAX];
extern float mq_init_step[CFG_MAQUINA_MAX];
extern uint8_t mq_init_point[CFG_MAQUINA_MAX];
extern char* mq_init_unit[CFG_MAQUINA_MAX];
extern const ut_state geNextStateMaq[CFG_MAQUINA_MAX];
extern uint32_t mq_init_values[CFG_MAQUINA_MAX];

#endif /* INCLUDE_CONFIG_MAQUINA_H_ */
