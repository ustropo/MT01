/*
 * state_functions.h
 *
 *  Created on: 30/04/2016
 *      Author: leocafonso
 */

#ifndef INCLUDE_STATE_FUNCTIONS_H_
#define INCLUDE_STATE_FUNCTIONS_H_

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
void idle(void *var);

#endif /* INCLUDE_STATE_FUNCTIONS_H_ */
