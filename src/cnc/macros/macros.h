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

stat_t M5_Macro(void);
stat_t M3_Macro(void);
stat_t ZerarEixos_Macro(void);
stat_t homming_Macro(void);

#endif /* CNC_MACROS_MACROS_H_ */
