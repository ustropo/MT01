/*
 * eeprom.h
 *
 *  Created on: 31/01/2016
 *      Author: leocafonso
 */

#ifndef INCLUDE_EEPROM_H_
#define INCLUDE_EEPROM_H_

#include "config_menu_pl.h"
#include "config_menu_ox.h"
#include "config_par_maquina.h"
#include "state_functions.h"

typedef enum
{
	MODOMAQUINA = 0,	//!< Flag do modo de funcinamento da maquina
	VEL_THC,			//!< Flag da velocidade do THC
	KERF,				//!< Kerf
	MERGULHO,			//!< Mergulho
	FLAG_MAX            //!< CONFIG_MAX
} flag_name;

typedef enum
{
	UNDEFINED_MAQ = 0,		//!< Undefined
	EASYMAK_MAQ,				//!< Easymak
	COMPACTA_MAQ,			//!< COMPACTA
	MOBILE_MAQ,				//!< MOBILE
	TYPE_MAX_MAQ        //!< MAQ_TYPE_MAX
} maq_name;

enum
{
	MODEL_RETA = 0,
	MODEL_HELI,
	MODEL_MAX
};

typedef struct
{
	maq_name model;
	bool crem;
}maq_st;

enum{
	CONFIGVAR_OX = 0,    //!< Indice da variavel EEPROM para oxicorte (configVarOx)
	CONFIGVAR_PL,        //!< Indice da variavel EEPROM para plasma   (configVarPl)
	CONFIGVAR_JOG,       //!< Indice da variavel EEPROM para jog      (configVarJog)
	CONFIGFLAG,          //!< Indice da variavel EEPROM para flags    (configFlags)
	ZEROPIECE,			 //!< Indice da variavel EEPROM para zero peça  (zeroPiece)
	CONFIGVAR_MAQ,       //!< Indice da variavel EEPROM para config maquina      (configVarMaq)
	CONFIGVAR_PAR_MAQ,   //!< Indice da variavel EEPROM para config parametros maquina      (configVarParMaq)
	CONFIGVAR_MAX,   	//!<
};

enum{
	JOG_RAPIDO = 0,      //!< Indice para jog rapido
	JOG_LENTO,           //!< Flag da velocidade do THC
	JOG_MAX              //!< CONFIG_MAX
};

enum{
	MODO_PLASMA = 0,      //!< modo plasma
	MODO_OXICORTE,        //!< modo Oxicorte
};

enum{
	DESABILITADO = 0,      //!<
	HABILITADO,        //!<
};

typedef enum
{
	MEM_OK = 0,
	MEM_FAIL
} mem_check;

extern float configVarOx[OX_CONFIG_MAX];
extern float configVarPl[PL_CONFIG_MAX];
extern float configVarJog[JOG_MAX];
extern float configVarMaq[CFG_MAQUINA_MAX - 1]; // retirado o modo maquina
extern float configVarParMaq[CFG_PAR_MAQ_MAX];
extern float zeroPiece[3];
extern uint32_t configFlags[FLAG_MAX];
extern uint32_t configTHCVel;
extern maq_st g_maq;


void eepromInit(void);
void eepromWriteConfig(uint8_t varType);
void eepromReadConfig(uint8_t varType);
void eepromConsistencyCheck(void);
void eepromFormat(void);
mem_check eepromIntegrityCheck(void);
void machine_type_write(const char * p_str_model,const char * p_str_crem);
maq_st check_machine_type(void);

#endif /* INCLUDE_EEPROM_H_ */
