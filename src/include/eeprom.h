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

typedef enum
{
	MODOMAQUINA = 0,	//!< Flag do modo de funcinamento da maquina
	VEL_THC,			//!< Flag da velocidade do THC
	FLAG_MAX            //!< CONFIG_MAX
} flag_name;

enum{
	CONFIGVAR_OX = 0,    //!< Indice da variavel EEPROM para oxicorte (configVarOx)
	CONFIGVAR_PL,        //!< Indice da variavel EEPROM para plasma   (configVarPl)
	CONFIGVAR_JOG,       //!< Indice da variavel EEPROM para jog      (configVarJog)
	CONFIGFLAG,          //!< Indice da variavel EEPROM para flags    (configFlags)
	ZEROPIECE			 //!< Indice da variavel EEPROM para zero peça  (zeroPiece)
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

extern float configVarOx[OX_CONFIG_MAX];
extern float configVarPl[PL_CONFIG_MAX];
extern float configVarJog[JOG_MAX];
extern float zeroPiece[3];
extern uint32_t configFlags[FLAG_MAX];
extern uint32_t configTHCVel;


void eepromInit(void);
void eepromWriteConfig(uint8_t varType);
void eepromReadConfig(uint8_t varType);

#endif /* INCLUDE_EEPROM_H_ */
