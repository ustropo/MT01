#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

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
#include "interpreter_if.h"

#include "lcd.h"
#include "keyboard.h"

#define FEEDRATE_Z 900

static float altura_perfuracao;
static float altura_deslocamento;
static float altura_corte;
static float vel_corte;
static float tempo_perfuracao;
static float tempo_aquecimento;

float *velocidadeJog;
float Xcord,Ycord;
ut_config_name_ox tempoDwell;
float jogMaxDistance[3];
uint8_t state = 0;
uint32_t linenumMacro;
extern bool sim;
extern bool intepreterRunning;
extern float zeroPiecebuffer[3];
extern bool lstop;
bool xMacroArcoOkSync = false;

stat_t (*macro_func_ptr)(void);
stat_t (*macro_buffer)(void);

struct gcodeParserSingleton {	 	  // struct to manage globals
	uint8_t modals[MODAL_GROUP_COUNT];// collects modal groups in a block
};

extern struct gcodeParserSingleton gp;

#define SET_MODAL_MACRO(m,parm,val) cm.gn.parm=val; cm.gf.parm=1; gp.modals[m]+=1;
#define SET_NON_MODAL_MACRO(parm,val) cm.gn.parm=val; cm.gf.parm=1;

stat_t M3_Macro(void)
{
	float tempo;
	macro_buffer = M3_Macro;

	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block
	printf("Macro M3 - state %d\nlinumacro - %d\n",state,linenumMacro);
	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
		switch (state)
		{
				/*   1- Procura chapa. G38.2 -50 COM FEEDRATE DE 900MM/MIN  */
			case 0: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_STRAIGHT_PROBE);
					SET_NON_MODAL_MACRO(target[AXIS_Z], -50);
					SET_NON_MODAL_MACRO (feed_rate, FEEDRATE_Z);
					state++; break;

				/*  2- Zera o eixo Z com G28.3 Z0*/
			case 1: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO(next_action, NEXT_ACTION_SET_ABSOLUTE_ORIGIN);
					SET_NON_MODAL_MACRO(target[AXIS_Z], 0);
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
						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_WAIT_SWITCH);
						SET_NON_MODAL_MACRO (parameter, 3000);
					}
					state++; break;

					/*6- Dwell do "TEMPO DE PERFURAÇÃO" */
			case 5:	if (tempo_perfuracao > 0.09){
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
					SET_NON_MODAL_MACRO (feed_rate, FEEDRATE_Z);
					state++; break;

					/*8- Seta o sistema com o feedrate de corte "VELOC. DE CORTE" */
			case 7: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (feed_rate, vel_corte);
					state++; break;

			default: state = 0; macro_buffer = _command_dispatch ; macro_func_ptr = _command_dispatch; return (STAT_OK);
		}
	}
	else
	{
		if (cm_get_runtime_busy() == true  || lstop == true) { return (STAT_EAGAIN);}	// sync to planner move ends
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
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

				    if (tempo > 0.09){
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
				case 4: if(!sim)
						{
							tempo = tempo_perfuracao;
						}
						else
						{
							tempo = 0;
						}

						if(tempo > 0.09){
							SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
							SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
							SET_NON_MODAL_MACRO (parameter, tempo*1000);
						}
						else
						{
							delay_thcStartStop(true);
						}
					state++; break;

					/*6- Desce para a "ALTURA DE CORTE" com feedrate de 800*/
			case 5: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
					SET_NON_MODAL_MACRO(target[AXIS_Z], altura_corte);
					SET_NON_MODAL_MACRO (feed_rate, FEEDRATE_Z);
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

stat_t M4_Macro(void)
{
	float tempo;
	macro_buffer = M4_Macro;

	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
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
					SET_NON_MODAL_MACRO (feed_rate, FEEDRATE_Z);
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
						SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
						SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_WAIT_SWITCH);
						SET_NON_MODAL_MACRO (parameter, 3000);
					}
					state++; break;

					/*6- Dwell do "TEMPO DE PERFURAÇÃO" */
			case 5:	delay_thcStartStop(true);
					state++; break;

					/*8- Seta o sistema com o feedrate de corte "VELOC. DE CORTE" */
			case 6: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
					SET_NON_MODAL_MACRO (feed_rate, vel_corte);
					state++; break;

			default: state = 0; macro_func_ptr = M5_Macro; return (STAT_OK);
		}
	}
	else
	{
		state = 0; macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

stat_t M5_Macro(void)
{
	macro_buffer = M5_Macro;
	if(configFlags[MODOMAQUINA] == MODO_PLASMA)
	{
		/* A macro não pode acorrer até que o buffer seja esvaziado, para que ações durante o corte tenham efeito imediato*/
	//	if (cm_get_runtime_busy() == true ) { return (STAT_EAGAIN);}	// sync to planner move ends
		altura_perfuracao 	= 	configVarPl[PL_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		/* A macro não pode acorrer até que o buffer seja esvaziado, para que ações durante o corte tenham efeito imediato*/
		if (cm_get_runtime_busy() == true  || lstop == true) { return (STAT_EAGAIN);}	// sync to planner move ends
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		altura_corte		= 	configVarOx[OX_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarOx[OX_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
	}
	printf("Macro M5 - state %d\nlinumacro - %d\n",state,linenumMacro);
	// set initial state for new move
	memset(&gp, 0, sizeof(gp));						// clear all parser values
	memset(&cm.gf, 0, sizeof(GCodeInput_t));		// clear all next-state flags
	memset(&cm.gn, 0, sizeof(GCodeInput_t));		// clear all next-state values
	cm.gn.motion_mode = cm_get_motion_mode(MODEL);	// get motion mode from previous block

	switch (state)
	{
		case 0: stopDuringCut_Set(false);
				SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
			    SET_MODAL_MACRO (MODAL_GROUP_M7, spindle_mode, SPINDLE_OFF);
				state++; break;
		case 1: SET_NON_MODAL_MACRO (linenum,(uint32_t)linenumMacro);
				SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
				SET_NON_MODAL_MACRO(target[AXIS_Z], altura_deslocamento);
				state++; break;
		default:state = 0; macro_buffer = _command_dispatch; macro_func_ptr = _command_dispatch; return (STAT_OK);
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
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
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
	bool diagonalX = false;
	bool diagonalY = false;
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
				if ((JogkeyPressed & KEY_RIGHT ) == KEY_RIGHT || (JogkeyPressed & KEY_LEFT ) == KEY_LEFT)
				{
						SET_NON_MODAL_MACRO(target[AXIS_X], jogMaxDistance[AXIS_X]);
						diagonalX = true;
				}
				if ((JogkeyPressed & KEY_UP ) == KEY_UP || (JogkeyPressed & KEY_DOWN ) == KEY_DOWN)
				{
						SET_NON_MODAL_MACRO(target[AXIS_Y], jogMaxDistance[AXIS_Y]);
						diagonalY = true;
				}
				if ((JogkeyPressed & KEY_Z_DOWN ) == KEY_Z_DOWN || (JogkeyPressed & KEY_Z_UP ) == KEY_Z_UP)
				{
						SET_NON_MODAL_MACRO(target[AXIS_Z], jogMaxDistance[AXIS_Z]);
				}
				if(diagonalX && diagonalY)
				{
					SET_NON_MODAL_MACRO (feed_rate, *velocidadeJog*1.41);
				}
				else
				{
					SET_NON_MODAL_MACRO (feed_rate, *velocidadeJog);
				}
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

stat_t arcoOK_Macro(void)
{
	uint32_t lRet = pdFALSE;

	if (xMacroArcoOkSync == true)
	{
		pl_arcook_start();
		xQueueReset((xQueueHandle)xArcoOkSync);
		lRet = xSemaphoreTake( xArcoOkSync, pdMS_TO_TICKS(3000) );
		if (lRet == pdFALSE)
		{
			uint32_t qSend;
			stopDuringCut_Set(true);
			qSend = ARCO_OK_INIT_FAILED;
			xQueueSend( qKeyboard, &qSend, 0 );
			macro_func_ptr = command_idle;
			xMacroArcoOkSync = false;
		}
		else
		{
			cm_request_cycle_start();
			stopDuringCut_Set(false);
			delay_thcStartStop(true);
			xMacroArcoOkSync = false;
			//state = 0;
			//macro_func_ptr = _command_dispatch;
			macro_func_ptr = macro_buffer;
		}
	}
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
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		altura_corte		= 	configVarPl[PL_CONFIG_ALTURA_CORTE];
		vel_corte			= 	configVarPl[PL_CONFIG_VELOC_CORTE];
		tempo_perfuracao	= 	configVarPl[PL_CONFIG_TEMPO_PERFURACAO];
		tempo_aquecimento	= 	0;
	}
	else
	{
		altura_perfuracao 	= 	configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		altura_deslocamento	= 	configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
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

//		case 3: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_TRAVERSE);
//				SET_NON_MODAL_MACRO(target[AXIS_Z], altura_deslocamento);
//				state++; break;

		case 3: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				SET_NON_MODAL_MACRO (feed_rate, 6000);
				state++; break;

		case 4: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				state++; break;

//		case 4: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
//				SET_NON_MODAL_MACRO (parameter, 1000);
//				state++; break;

		case 5: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_Y], Ycord);
				state++; break;

//		case 6: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
//				SET_NON_MODAL_MACRO (parameter, 1000);
//				state++; break;

		case 6: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_X], Xcord);
				state++; break;

//		case 8: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
//				SET_NON_MODAL_MACRO (parameter, 1000);
//				state++; break;

		case 7: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_Y], 0);
				state++; break;

//		case 10: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
//				SET_NON_MODAL_MACRO (parameter, 1000);
//				state++; break;

		case 8: SET_MODAL_MACRO (MODAL_GROUP_G1, motion_mode, MOTION_MODE_STRAIGHT_FEED);
				SET_NON_MODAL_MACRO(target[AXIS_X], 0);
				state++; break;

//		case 12: SET_NON_MODAL_MACRO (next_action, NEXT_ACTION_DWELL);
//				SET_NON_MODAL_MACRO (parameter, 1000);
//				state++; break;

		case 9: SET_MODAL_MACRO (MODAL_GROUP_M4, program_flow, PROGRAM_END);
				state++; break;

		default:state = 0;  macro_func_ptr = _command_dispatch; return (STAT_OK);
	}
	_execute_gcode_block();
	return (STAT_OK);
}

void macroInitVar(void)
{
	velocidadeJog = &configVarJog[JOG_RAPIDO];
}
