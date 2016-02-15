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


struct gcodeParserSingleton {	 	  // struct to manage globals
	uint8_t modals[MODAL_GROUP_COUNT];// collects modal groups in a block
};

extern struct gcodeParserSingleton gp;

#define SET_MODAL_MACRO(m,parm,val) cm.gn.parm=val; cm.gf.parm=1; gp.modals[m]+=1;
#define SET_NON_MODAL_MACRO(parm,val) cm.gn.parm=val; cm.gf.parm=1;

void M3_Macro(void)
{
	/* 3- ZERA O EIXO Z COM G28.3 Z0 (NÃO PRECISA MAIS COMPENSAR O SENSOR, MINHA MAQUINA USARÁ SISTEMA OHMICO, OFFSET=0) */
	SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
	SET_NON_MODAL_MACRO(target[AXIS_Z], 0);
	_execute_gcode_block();

	/*4- SOBE O EIXO Z PARA "ALTURA DE PERFURAÇÃO" */
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
	SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_PERFURACAO]);
	_execute_gcode_block();

	/*6 -DISPARA O RELE DA TOCHA */
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_CW);
	_execute_gcode_block();

	/*8- DEIXA CORRER O TEMPO DE PERFURAÇÃO "TEMPO DE PERFURAÇÃO" */
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
	SET_NON_MODAL_MACRO (parameter, configVar[TEMPO_PERFURACAO]*1000);
	_execute_gcode_block();

	/*9- DESCE A TOCHA USANDO G01 F800 PARA "ALTURA DE CORTE" */
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
	SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_CORTE]);
	SET_NON_MODAL_MACRO (feed_rate, 800);
	_execute_gcode_block();

	/*10- SETA O SISTEMA COM FEEDRATE DE CORTE F "VELOC. DE CORTE” PARA OS PROXIMOS G01, G02 E G03*/
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	SET_NON_MODAL_MACRO (feed_rate, 3000);
}

void M5_Macro(void)
{
	SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_OFF);
	_execute_gcode_block();

	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
	SET_NON_MODAL_MACRO(target[AXIS_Z], configVar[ALTURA_DESLOCAMENTO]);
}
