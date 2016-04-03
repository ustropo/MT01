/*
 * macros.h
 *
 *  Created on: 13/02/2016
 *      Author: leocafonso
 */
#ifndef CNC_MACROS_MACROS_H_
#define CNC_MACROS_MACROS_H_

extern stat_t (*macro_func_ptr)(void);

extern int8_t macro;
extern uint8_t jogAxis;
extern float jogMaxDistance;
extern uint8_t state;
extern uint32_t linenumMacro;

stat_t M5_Macro(void);
stat_t M3_Macro(void);
stat_t G10_Macro(void);
stat_t ZerarMaquina_Macro(void);
stat_t ZerarPeca_Macro(void);
stat_t homming_Macro(void);
stat_t jog_Macro(void);
stat_t RunningInicial_Macro(void);
stat_t feedRateOverride_Macro(void);

#endif /* CNC_MACROS_MACROS_H_ */
