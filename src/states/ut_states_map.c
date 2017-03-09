/*
 * ut_states_map.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */


#include "ut_context.h"
#include "ut_state.h"

/**
 * State table
 */
const state_func_ptr states_table[STATE_NUMBER] =
{
		&ut_state_splash,						//!< Funçao da tela de entrada
		&ut_state_warning,                      //!< Funçao da tela de warnings
		&ut_state_main_menu,                    //!< Funçao da tela de principal
		&ut_state_choose_file,                  //!< Funçao da tela de escolha de arquivos
		&ut_state_config_menu_ox,               //!< Funçao da tela de configuração de corte - Oxicorte
		&ut_state_config_menu_pl,               //!< Funçao da tela de configuração de corte - Plasma
		&ut_state_config_manual_menu,           //!< Funçao da tela do menu de corte manual
		&ut_state_config_jog,          			//!< Funçao da tela da configuraçao de jog
		&ut_state_config_auto_menu,             //!< Funçao da tela do menu de corte automatico
		&ut_state_config_maquina,				//!< Funçao da tela de da configuraçao da maquina
		&ut_state_config_par_maq,				//!< Funçao da tela de parametros da maquina
		&ut_state_config_maq_thc,				//!< Funçao da tela de parametros do thc
		&ut_state_config_var,                   //!< Funçao da tela de manipulação de variaveis
		&ut_state_manual_mode,                  //!< Funçao da tela de corte manual
		&ut_state_deslocaZero_mode,             //!< Funçao da tela de deslocar para zero
		&ut_state_auto_mode,                    //!< Funçao da tela de corte automatico
		&ut_state_line_selection,               //!< Funçao da tela de selecionar linhas
		&ut_state_config_maq_model,				//!< Funçao da tela de selecionar modelo de maquina
};
