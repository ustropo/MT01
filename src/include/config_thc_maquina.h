/*
 * config_menu_ox.h
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */

#ifndef INCLUDE_CONFIG_THC_H_
#define INCLUDE_CONFIG_THC_H_

#include "ut_state_config_var.h"

extern ut_config_var configsTh[CFG_THC_MAX];
extern ut_config_type th_init_types[CFG_THC_MAX];
extern char* th_init_names[CFG_THC_MAX];
extern float th_init_max[CFG_THC_MAX];
extern float th_init_min[CFG_THC_MAX];
extern float th_init_step[CFG_THC_MAX];
extern uint8_t th_init_point[CFG_THC_MAX];
extern char* th_init_unit[CFG_THC_MAX];
extern const ut_state geNextStateTh[CFG_THC_MAX];
extern uint32_t th_init_values[CFG_THC_MAX];

#endif /* INCLUDE_CONFIG_TH_H_ */
