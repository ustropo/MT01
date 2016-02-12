/*
 * eeprom.h
 *
 *  Created on: 31/01/2016
 *      Author: leocafonso
 */

#ifndef INCLUDE_EEPROM_H_
#define INCLUDE_EEPROM_H_

typedef enum
{
	ALTURA_DESLOCAMENTO = 0,    //!< CONFIG_JOG_LENTO
	ALTURA_PERFURACAO,   //!< CONFIG_JOG_RAPIDO
	TEMPO_PERFURACAO,//!< CONFIG_TESTE_DISPARO
	ALTURA_CORTE, // ALTURA DE CORTE
	VELOC_CORTE, // VELOC. DE CORTE
	VELOC_JOG_LENTO, // VELOC. DE JOG LENTO
	VELOC_JOG_RAPIDO, // VELOC. DE JOG R�PIDO
	CANCELAR_IHS, // CANCELAR IHS
	TESTE_DISPARO, //" TESTE DE DISPARO",
	TESTE_SENSOR_CHAPA, //" TESTE SENSOR DE CHAPA",
	TESTE_SENSOR_ARCOOK, //" TESTE SENSOR ARCO OK",
	TESTE_EMERGENCIA, //" TESTE BOT�O DE EMERG�NCIA"
	VAR_MAX           //!< CONFIG_MAX
} var_name;

extern float configVar[VAR_MAX];

void eepromInit(void);
void eepromInitVar(void);
void eepromWriteConfig(void);
void eepromReadConfig(void);

#endif /* INCLUDE_EEPROM_H_ */
