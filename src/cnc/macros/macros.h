/*
 * macros.h
 *
 *  Created on: 13/02/2016
 *      Author: leocafonso
 */
#ifndef CNC_MACROS_MACROS_H_
#define CNC_MACROS_MACROS_H_

#include "tinyg.h"			// #1
#include "config.h"			// #2
#include "eeprom.h"

extern stat_t (*macro_func_ptr)(void);
extern stat_t (*macro_buffer)(void);
extern int8_t macro;
extern float jogMaxDistance[3];
extern uint8_t state;
extern uint32_t linenumMacro;
extern float *velocidadeJog;
extern ut_config_name_ox tempoDwell;
extern bool xMacroArcoOkSync;
extern float Xcord,Ycord;

stat_t M5_Macro(void);
stat_t M4_Macro(void);
stat_t M3_Macro(void);
stat_t G10_Macro(void);
stat_t ZerarMaquina_Macro(void);
stat_t ZerarPeca_Macro(void);
stat_t homming_Macro(void);
stat_t jog_Macro(void);
stat_t RunningInicial_Macro(void);
stat_t feedRateOverride_Macro(void);
stat_t arcoOK_Macro(void);
stat_t limit_test(void);
void macroInitVar(void);

#endif /* CNC_MACROS_MACROS_H_ */
