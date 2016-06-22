/*
 * config_menu_ox.h
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */

#ifndef INCLUDE_CONFIG_MENU_OX_H_
#define INCLUDE_CONFIG_MENU_OX_H_

#include "ut_state_config_var.h"

extern ut_config_var configsOx[OX_CONFIG_MAX];
extern ut_config_type ox_init_types[OX_CONFIG_MAX];
extern char* ox_init_names[OX_CONFIG_MAX];
extern float ox_init_max[OX_CONFIG_MAX];
extern float ox_init_min[OX_CONFIG_MAX];
extern float ox_init_step[OX_CONFIG_MAX];
extern uint8_t ox_init_point[OX_CONFIG_MAX];
extern char* ox_init_unit[OX_CONFIG_MAX];
extern void initOx(void);

#endif /* INCLUDE_CONFIG_MENU_OX_H_ */
