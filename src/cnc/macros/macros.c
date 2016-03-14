#include "tinyg.h"			// #1
#include "config.h"			// #2
#include "controller.h"
#include "gcode_parser.h"
#include "canonical_machine.h"
#include "spindle.h"
#include "util.h"
#include "xio.h"			// for char definitions
#include "macros.h"
#include "eeprom.h"

extern float *velocidadeJog;

uint8_t jogAxis;
float jogMaxDistance;
uint8_t state = 0;

stat_t (*macro_func_ptr)(void);

struct gcodeParserSingleton {	 	  // struct to manage globals
	uint8_t modals[MODAL_GROUP_COUNT];// collects modal groups in a block
};

extern struct gcodeParserSingleton gp;

#define SET_MODAL_MACRO(m,parm,val) cm.gn.parm=val; cm.gf.parm=1; gp.modals[m]+=1;
#define SET_NON_MODAL_MACRO(parm,val) cm.gn.parm=val; cm.gf.parm=1;

stat_t M3_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
			/* 1- CHECA SE O USUARIO CANCELOU O IHS (MODO OXICORTE), OU SE ESTÁ EM MODO SIMULAÇÃO. SE SIM, PULAR PARA PASSO 3
			   2- PROCURA A CHAPA USANDO O G38.2 -50 COM FEEDRATE DE 800MM/MIN  */
		case 0: if(configFlags == 0){
					SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_STRAIGHT_PROBE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], -50);
					SET_NON_MODAL_MACRO (feed_rate, 800);
				}
				state++; break;

				/* 3- ZERA O EIXO Z COM G28.3 Z0 (NÃO PRECISA MAIS COMPENSAR O SENSOR, MINHA MAQUINA USARÁ SISTEMA OHMICO, OFFSET=0) */
		case 1: if(configFlags == 0){
					SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
					SET_NON_MODAL_MACRO(target[AXIS_Z], 0);
				}
				state++; break;

				/*4- SOBE O EIXO Z PARA "ALTURA DE PERFURAÇÃO" */
		case 2: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_PERFURACAO]);
				state++; break;

				/*6 -DISPARA O RELE DA TOCHA */
		case 3: SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_CW);
				state++; break;

				/*8- DEIXA CORRER O TEMPO DE PERFURAÇÃO "TEMPO DE PERFURAÇÃO" */
		case 4: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
				SET_NON_MODAL_MACRO (parameter, configVar[TEMPO_PERFURACAO]*1000);
				state++; break;

				/*9- DESCE A TOCHA USANDO G01 F800 PARA "ALTURA DE CORTE" */
		case 5: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_CORTE]);
				SET_NON_MODAL_MACRO (feed_rate, 800);
				state++; break;

				/*10- SETA O SISTEMA COM FEEDRATE DE CORTE F "VELOC. DE CORTE” PARA OS PROXIMOS G01, G02 E G03*/
		case 6: SET_NON_MODAL_MACRO (feed_rate, configVar[VELOC_CORTE]);
				state++; break;

		default: state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t M5_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_OFF);
				state++; break;
		case 1: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_DESLOCAMENTO]);
				state++; break;
		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t ZerarEixos_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Z], 0);
				state++; break;
		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t homming_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_MODAL_MACRO (MODAL_GROUP_G6, units_mode, MILLIMETERS);
				state++; break;

		case 1: SET_MODAL_MACRO (MODAL_GROUP_G3, distance_mode, ABSOLUTE_MODE);
				state++; break;

		case 2: SET_NON_MODAL_MACRO (absolute_override, true);
				state++; break;

		case 3: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_DESLOCAMENTO]);
				state++; break;

		case 4: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				state++; break;

		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t jog_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_MODAL_MACRO (MODAL_GROUP_G3, distance_mode, INCREMENTAL_MODE);
				state++; break;

		case 1: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[jogAxis], jogMaxDistance);
				SET_NON_MODAL_MACRO (feed_rate, *velocidadeJog);
				state++; break;
		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}
