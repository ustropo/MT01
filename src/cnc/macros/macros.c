#include "tinyg.h"			// #1
#include "config.h"			// #2
#include "controller.h"
#include "gcode_parser.h"
#include "canonical_machine.h"
#include "planner.h"
#include "spindle.h"
#include "util.h"
#include "xio.h"			// for char definitions
#include "macros.h"
#include "eeprom.h"
#include "plasma.h"

#include "lcd.h"
#include "keyboard.h"

static float altura_perfuracao;
static float altura_deslocamento;
static float altura_corte;
static float vel_corte;
static float tempo_perfuracao;
static float tempo_aquecimento;

float *velocidadeJog;
float Xcord,Ycord;
ut_config_name_ox tempoDwell;
uint8_t jogAxis;
float jogMaxDistance;
uint8_t state = 0;
uint32_t linenumMacro;
extern bool sim;
extern bool intepreterRunning;
extern float zeroPiecebuffer[3];

stat_t (*macro_func_ptr)(void);

struct gcodeParserSingleton {	 	  // struct to manage globals
	uint8_t modals[MODAL_GROUP_COUNT];// collects modal groups in a block
};

extern struct gcodeParserSingleton gp;

#define SET_MODAL_MACRO(m,parm,val) cm.gn.parm=val; cm.gf.parm=1; gp.modals[m]+=1;
#define SET_NON_MODAL_MACRO(parm,val) cm.gn.parm=val; cm.gf.parm=1;

stat_t M3_Macro(void)
{
	float tempo;

	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarPl[PL_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
		switch (state)
		{
				/*   1- Procura chapa. G38.2 -50 COM FEEDRATE DE 800MM/MIN  */
			case 0: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_STRAIGHT_PROBE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], -50);
					SET_NON_MODAL_MACRO (feed_rate, 800);
					state++; break;

				/*  2- Zera o eixo Z com G28.3 Z0*/
			case 1: if(configFlags[MODOMAQUINA] == MODO_PLASMA){

						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
						SET_NON_MODAL_MACRO(target[AXIS_Z], 0);

					}
					state++; break;

					/* 3- Posiciona o eixo Z para "ALTURA DE PERFURAÇÃO" */
			case 2: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_perfuracao);
					state++; break;

					/* 4- CHECA SE O ESTÁ EM MODO SIMULAÇÃO, SE SIM, PULAR PARA PASSO 8. SE ESTIVER EM MODO OXICORTE, CONTINUA.
					   4 -Dispara a tocha */
			case 3:	SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_CW);
					state++;
					break;

					/*  5 -Espera o arco OK */
			case 4: if(!sim)
					{
						uint32_t lRet;
						pl_arcook_start();
						lRet = xSemaphoreTake( xArcoOkSync, pdMS_TO_TICKS(3000) );
						if (lRet == pdFALSE)
						{
							uint32_t qSend = ARCO_OK_FAILED;
							xQueueSend( qKeyboard, &qSend, 0 );
							return (STAT_OK);
						}
						else
						{
							isCuttingSet(true);
						}
					}
					state++; break;

					/*6- Dwell do "TEMPO DE PERFURAÇÃO" */
			case 5:	if (tempo_perfuracao > 0){
						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
						SET_NON_MODAL_MACRO (parameter, tempo_perfuracao*1000);
					}
					else
					{
						delay_thcStartStop(true);
					}
					state++; break;


					/*7- Desce para a "ALTURA DE CORTE" com feedrate de 800*/
			case 6: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_corte);
					SET_NON_MODAL_MACRO (feed_rate, 800);
					state++; break;

					/*8- Seta o sistema com o feedrate de corte "VELOC. DE CORTE" */
			case 7: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (feed_rate, vel_corte);
					state++; break;

			default: state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
		}
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarOx[OX_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarOx[OX_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarOx[OX_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
		switch (state)
		{
					/* 1- Posiciona o eixo Z para "ALTURA DE AQUECIMENTO = ALTURA DE CORTE" */
			case 0: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_corte);
					state++; break;

					/*2- Dwell do "TEMPO DE AQUECIMENTO". Pula se estiver em simulação */
			case 1: if(!sim)
					{
						tempo = tempo_aquecimento;
					}
					else
					{
						tempo = 0;
					}

				    if (tempo > 0){
						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
						SET_NON_MODAL_MACRO (parameter, tempo*1000);
					}
					else
					{
						delay_thcStartStop(true);
					}
					state++; break;

					/*3- Sobe para "ALTURA DE PERFURAÇÃO" */
			case 2:	SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_perfuracao);
					state++; break;

					/*4- Liga a tocha */
			case 3: if(configFlags[MODOMAQUINA] == MODO_OXICORTE){
						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_CW);
					}
					state++;
					break;

					/*5- Dwell do "TEMPO DE PERFURAÇÃO" */
			case 4: if(configFlags[MODOMAQUINA] == MODO_OXICORTE){
						if(tempo_perfuracao > 0){
							SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
							SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
							SET_NON_MODAL_MACRO (parameter, tempo_perfuracao*1000);
						}
						else
						{
							delay_thcStartStop(true);
						}
					}
					state++; break;

					/*6- Desce para a "ALTURA DE CORTE" com feedrate de 800*/
			case 5: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_corte);
					SET_NON_MODAL_MACRO (feed_rate, 800);
					state++; break;

					/*7- Seta o sistema com o feedrate de corte "VELOC. DE CORTE" */
			case 6: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (feed_rate, vel_corte);
					state++; break;

			default: state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
		}
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t M5_Macro(void)
{
	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarPl[PL_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarOx[OX_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarOx[OX_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarOx[OX_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
	}

	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
			    SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_OFF);
				state++; break;
		case 1: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
				SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Z], altura_deslocamento);
				state++; break;
		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t ZerarMaquina_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	intepreterRunning = true;
	switch (state)
	{
		case 0: SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Z], 0);
				state++; break;
		default:state = 0; 	intepreterRunning = false; macro_func_ptr = command_idle; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t ZerarPeca_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	intepreterRunning = true;
	switch (state)
	{
		case 0: SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
				SET_NON_MODAL_MACRO(target[AXIS_X], zeroPiece[AXIS_X]);
				SET_NON_MODAL_MACRO(target[AXIS_Y], zeroPiece[AXIS_Y]);
				SET_NON_MODAL_MACRO(target[AXIS_Z], zeroPiece[AXIS_Z]);
				state++; break;
		default:state = 0; 	intepreterRunning = false; macro_func_ptr = command_idle; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t homming_Macro(void)
{
	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarPl[PL_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarOx[OX_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarOx[OX_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarOx[OX_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
	}

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
				SET_NON_MODAL_MACRO(target[AXIS_Z], altura_deslocamento);
				state++; break;

		case 4: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_X], zeroPiece[AXIS_X]);
				SET_NON_MODAL_MACRO(target[AXIS_Y], zeroPiece[AXIS_Y]);
				state++; break;

		case 5: SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				zeroPiecebuffer[AXIS_X] = zeroPiece[AXIS_X];
				zeroPiecebuffer[AXIS_Y] = zeroPiece[AXIS_Y];
				zeroPiece[AXIS_X] = 0;
				zeroPiece[AXIS_Y] = 0;
//				SET_NON_MODAL_MACRO(target[AXIS_Z], zeroPiece[AXIS_Z]);
				state++; break;

		case 6: SET_MODAL_MACRO (MODAL_GROUP_M4, program_flow, PROGRAM_END);
				state++; break;

		default:state = 0; macro_func_ptr = command_idle; return (STAT_OK);
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
		default:state = 0; macro_func_ptr = command_idle; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t RunningInicial_Macro(void)
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

		case 1: SET_NON_MODAL_MACRO (absolute_override, true);
				state++; break;

		case 2: SET_MODAL_MACRO (MODAL_GROUP_G3, distance_mode, ABSOLUTE_MODE);
				state++; break;

		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t G10_Macro(void)
{
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: SET_MODAL_MACRO (MODAL_GROUP_G0, next_action, NEXT_ACTION_SET_COORD_DATA);
				SET_NON_MODAL_MACRO (parameter, 1);
				SET_NON_MODAL_MACRO (target[AXIS_X], mp_get_runtime_absolute_position(AXIS_X));
				SET_NON_MODAL_MACRO (target[AXIS_Y], mp_get_runtime_absolute_position(AXIS_Y));
				SET_NON_MODAL_MACRO (target[AXIS_Z], 0);
				state++; break;

		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t feedRateOverride_Macro(void)
{
	switch (state)
	{
		case 0: cm_request_feedhold();
				state++; break;
		case 1: cm_request_cycle_start();
				state++; break;
		default:state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t limit_test(void)
{
	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarPl[PL_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarOx[OX_CONFIG_ALTURA_DESLOCAMENTO];
		altura_corte		= 	configVarOx[OX_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarOx[OX_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
	}

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
				SET_NON_MODAL_MACRO(target[AXIS_Z], altura_deslocamento);
				state++; break;

		case 4: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_X], Xcord);
				state++; break;

		case 5: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Y], Ycord);
				state++; break;

		case 6: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				state++; break;

		case 7: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				state++; break;

		case 8: SET_MODAL_MACRO (MODAL_GROUP_M4, program_flow, PROGRAM_END);
				state++; break;

		default:state = 0; macro_func_ptr = command_idle; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

void macroInitVar(void)
{
	velocidadeJog = &configVarJog[JOG_RAPIDO];
}
