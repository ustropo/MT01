/*
 * state_functions.h
 *
 *  Created on: 30/04/2016
 *      Author: leocafonso
 */

#ifndef INCLUDE_STATE_FUNCTIONS_H_
#define INCLUDE_STATE_FUNCTIONS_H_

#include "eeprom.h"

extern uint32_t LineM5;
extern float selecionarLinhas;

uint32_t selecionarlinhasMax(void);
void selecionarlinhas(void);
char** selecionarLinhatexto(void);
void linhaSelecionada(uint32_t flag);

void zerar_maquina(void *var);
void zerar_peca(void *var);
void homming_eixos(void *var);
void testar_peca(void *var);
void mem_format(void *var);
uint32_t delay_esc(uint32_t timems);
uint32_t delay_esc_enter(uint32_t timems);
uint8_t get_dec_digits(float fnum);
uint8_t get_decimal_digits(float fnum);
void idle(void *var);

#endif /* INCLUDE_STATE_FUNCTIONS_H_ */
