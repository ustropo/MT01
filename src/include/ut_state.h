/*
 * ut_state.h
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */

#ifndef STATES_UT_STATE_H_
#define STATES_UT_STATE_H_

#include "ut_context.h"

/**
 * Enum to indicate available states
 */
typedef enum
{
	STATE_SPLASH = 0,                         //!< Fun�ao da tela de entrada
	STATE_WARNING,                            //!< Fun�ao da tela de warnings
	STATE_MAIN_MENU,                          //!< Fun�ao da tela de principal
	STATE_CHOOSE_FILE,                        //!< Fun�ao da tela de escolha de arquivos
	STATE_CONFIG_MENU_OX,                     //!< Fun�ao da tela de configura��o de corte - Oxicorte
	STATE_CONFIG_MENU_PL,                     //!< Fun�ao da tela de configura��o de corte - Plasma
	STATE_CONFIG_MANUAL_MODE,                 //!< Fun�ao da tela do menu de corte manual
	STATE_CONFIG_JOG,                 		  //!< Fun�ao da tela da configura�ao de jog
	STATE_CONFIG_AUTO_MODE,                   //!< Fun�ao da tela do menu de corte automatico
	STATE_CONFIG_MAQUINA,                     //!< Fun�ao da tela de da configura�ao da maquina
	STATE_CONFIG_PARAMETROS_MAQ,              //!< Fun�ao da tela de parametros da maquina
	STATE_CONFIG_MAQUINA_THC,              	  //!< Fun�ao da tela de parametros do thc
	STATE_CONFIG_VAR,                         //!< Fun�ao da tela de manipula��o de variaveis
	STATE_MANUAL_MODE,                        //!< Fun�ao da tela de corte manual
	STATE_DESLOCAZERO_MODE,                   //!< Fun�ao da tela de deslocar para zero
	STATE_AUTO_MODE,                          //!< Fun�ao da tela de corte automatico
	STATE_LINE_SELECTION,                     //!< Fun�ao da tela de selecionar linhas
	STATE_MAQ_MODEL_SELECTION,                //!< Fun�ao da tela de selecionar modelo de maquina
	/* This should be always the last one! */
	STATE_NUMBER
} ut_state;

typedef enum
{
	CONFIG_AUTO_RODAR_PROG  = 0,
	CONFIG_AUTO_MODO_SIM,
	CONFIG_AUTO_DESLOCAR_ZERO,
	CONFIG_AUTO_SELECIONAR_LINHA,
	CONFIG_AUTO_TESTAR_TAMANHO_PECA,
	CONFIG_AUTO_MAX,
	/*Melhorar esta implementa��o */
	CONFIG_AUTO_MODO_SIM_RUN
} ut_config_auto_name;

typedef enum
{
	CONFIG_MANUAL_MODO_MANUAL  = 0,
	CONFIG_MANUAL_JOG_RAP_LENTO,
	CONFIG_MANUAL_ZERAR_PECA,
	CONFIG_MANUAL_DESLOCAR_ZERO,
	CONFIG_MANUAL_ZERAR_MAQUINA,
	CONFIG_MANUAL_MAX
} ut_config_manual_name;

/**
 *
 */
typedef enum
{
	MAIN_MENU_FILE = 0,
	MAIN_MENU_CONFIG_MANUAL,
	MAIN_MENU_AUTO,
	MAIN_MENU_CONFIG,
	MAIN_MENU_MODOMAQUINA,
	MAIN_MENU_NUMBER
} main_menu_options;

typedef enum
{
	CONFIG_JOG_RAPIDO = 0,
	CONFIG_JOG_LENTO,
	CONFIG_JOG_MAX
} ut_config_jog_name;

typedef enum
{
	OX_CONFIG_ALTURA_PERFURACAO,   		//!< Altura de perfura��o
	OX_CONFIG_ALTURA_CORTE, 			//!< Altura de corte
	OX_CONFIG_VELOC_CORTE, 				//!< Velocidade de corte
	OX_CONFIG_TEMPO_AQUECIMENTO,		//!< Tempo de aquecimento
	OX_CONFIG_TEMPO_PERFURACAO,			//!< Tempo de Perfura��o
	OX_CONFIG_MAX          				//!< CONFIG_MAX
} ut_config_name_ox;

typedef enum
{
	PL_CONFIG_ALTURA_PERFURACAO,      //!< Altura de perfura��o
	PL_CONFIG_ALTURA_CORTE,           //!< Altura de corte
	PL_CONFIG_VELOC_CORTE,            //!< Velocidade de corte
	PL_CONFIG_TEMPO_PERFURACAO,       //!< Tempo de Perfura��o
	PL_CONFIG_TENSAO_THC,             //!< Tensao do THC
	PL_CONFIG_MAX        			  //!< CONFIG_MAX
} ut_config_name_pl;

typedef enum
{
	CFG_MAQUINA_ALT_DESLOCAMENTO,     //!< Altura de deslocamento
	CFG_MAQUINA_MODOMAQUINA,          //!< Modo da maquina
	CFG_MAQUINA_PARAMETROS,			  //!< Parametros da maquina
	CFG_MAQUINA_PARAMETROS_THC,		  //!< Parametros de THC
	CFG_MAQUINA_MAX        			  //!< CONFIG_MAX
} ut_config_maquina;

typedef enum
{
	CFG_THC_KERF,     			//!< Kerf
	CFG_THC_MERGULHO,          //!< mergulho
	CFG_THC_MAX        			  //!< CONFIG_MAX
} ut_config_maquina_thc;

typedef enum
{
	CFG_PAR_MAQ_EIXO_X1,     			//!< EIXO_X1
	CFG_PAR_MAQ_EIXO_X2,          		//!< EIXO_X2
	CFG_PAR_MAQ_EIXO_Y,			  		//!< EIXO_Y
	CFG_PAR_MAQ_EIXO_Z,			  		//!< EIXO_Z
	CFG_PAR_MAQ_JERK_X,                 //!< JERK X
	CFG_PAR_MAQ_JERK_Y,                 //!< JERK Y
	CFG_PAR_MAQ_JERK_Z,                 //!< JERK Z
	CFG_PAR_MAQ_VEL_X,                  //!< VEL X
	CFG_PAR_MAQ_VEL_Y,                  //!< VEL Y
	CFG_PAR_MAQ_VEL_Z,                  //!< VEL Z
	CFG_PAR_MAQ_JUNCTION_DEV,           //!< JUNCTION DEV
	CFG_PAR_MAQ_JUNCTION_ACEL,          //!< JUNCTION ACCEL
	CFG_PAR_MAQ_CHORDAL_TOL,            //!< CHORDAL TOLERANCE
	CFG_FORMAT,                         //!< FORMAT MEM
	CFG_PAR_MAQ_MAX        			    //!< CFG PAR MAX
} ut_config_par_maquina;

typedef enum
{
	CFG_MODEL_EASYMAK = 0,
	CFG_MODEL_COMPACTA,
	CFG_MODEL_MOBILE,
	CFG_MODEL_UNIMAQ,
	CFG_MODEL_MAX
} ut_config_model;

/**
 * Function pointer to a state execution.
 * Any state must have exactly this signature
 *
 * @param ut_context_ptr pointer to context structure
 * @return next state
 */
typedef ut_state (*state_func_ptr)(ut_context*);

/**
 * States declaration.
 *
 * Any additional state goes here!
 */
extern ut_state ut_state_splash(ut_context* pContext);
extern ut_state ut_state_warning(ut_context* pContext);
extern ut_state ut_state_main_menu(ut_context* pContext);
extern ut_state ut_state_choose_file(ut_context* pContext);
extern ut_state ut_state_config_menu_ox(ut_context* pContext);
extern ut_state ut_state_config_menu_pl(ut_context* pContext);
extern ut_state ut_state_config_manual_menu(ut_context* pContext);
extern ut_state ut_state_config_jog(ut_context* pContext);
extern ut_state ut_state_config_var(ut_context* pContext);
extern ut_state ut_state_manual_mode(ut_context* pContext);
extern ut_state ut_state_auto_mode(ut_context* pContext);
extern ut_state ut_state_config_maquina(ut_context* pContext);
extern ut_state ut_state_config_par_maq(ut_context* pContext);
extern ut_state ut_state_deslocaZero_mode(ut_context* pContext);
extern ut_state ut_state_config_auto_menu(ut_context* pContext);
extern ut_state ut_state_line_selection(ut_context* pContext);
extern ut_state ut_state_config_maq_thc(ut_context* pContext);
extern ut_state ut_state_config_maq_model(ut_context* pContext);


/**
 * State map definition
 * Any state relationship must goes here:
 * 	enum -> function
 * In exactly same order as the enum declaration
 */
extern const state_func_ptr states_table[STATE_NUMBER];

// ***********************************************************************
// Defines
// ***********************************************************************
#define MAX_FILE_PATH_SIZE 256
#define USB_ROOT	""
#define DEFAULT_FILE_EXTENSION	".txt"
// ***********************************************************************
// Global variables
// ***********************************************************************
extern char gszCurFile[MAX_FILE_PATH_SIZE];
extern ut_state currentState;

#endif /* STATES_UT_STATE_H_ */
