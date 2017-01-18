/*
 * cycle_probing.c - probing cycle extension to canonical_machine.c
 * Part of TinyG project
 *
 * Copyright (c) 2010 - 2015 Alden S Hart, Jr.
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, you may use this file as part of a software library without
 * restriction. Specifically, if other files instantiate templates or use macros or
 * inline functions from this file, or you compile this file and link it with  other
 * files to produce an executable, this file does not by itself cause the resulting
 * executable to be covered by the GNU General Public License. This exception does not
 * however invalidate any other reasons why the executable file might be covered by the
 * GNU General Public License.
 *
 * THE SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
 * WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "tinyg.h"
#include "config.h"
#include "json_parser.h"
#include "text_parser.h"
#include "canonical_machine.h"
#include "spindle.h"
#include "report.h"
#include "switch.h"
#include "util.h"
#include "planner.h"
#include "hardware.h"
#include "switch.h"
#include "macros.h"
#include "plasma.h"
#include "controller.h"
#include "keyboard.h"
#include "config_SwTimers.h"
#include "lcd.h"


extern bool lstop;
extern void warm_stop(uint8_t flag);
/**** Probe singleton structure ****/

#define MINIMUM_PROBE_TRAVEL 0.254

struct pbWaitingSwitchSingleton {						// persistent probing runtime variables
	stat_t (*func)();							// binding for callback function state machine

	// switch configuration
	uint8_t waiting_switch;						// which switch should we check?
	uint8_t saved_switch_type;					// saved switch type NO/NC
	uint8_t saved_switch_mode;	                // save the probe switch's original settings
};
static struct pbWaitingSwitchSingleton ws;

/**** NOTE: global prototypes and other .h info is located in canonical_machine.h ****/

static stat_t _waiting_init();
static stat_t _waiting_start();
static stat_t _waiting_finish();
static stat_t _waiting_finalize_exit();
static stat_t _waiting_error_exit(int8_t axis);


/**** HELPERS ***************************************************************************
 * _set_pb_func() - a convenience for setting the next dispatch vector and exiting
 */

uint8_t _set_ws_func(uint8_t (*func)())
{
	ws.func = func;
	return (STAT_EAGAIN);
}

/****************************************************************************************
 * cm_probing_cycle_start()	- G38.2 homing cycle using limit switches
 * cm_probing_callback() 	- main loop callback for running the homing cycle
 *
 *	--- Some further details ---
 *
 *	All cm_probe_cycle_start does is prevent any new commands from queueing to the
 *	planner so that the planner can move to a sop and report MACHINE_PROGRAM_STOP.
 *	OK, it also queues the function that's called once motion has stopped.
 *
 *	Note: When coding a cycle (like this one) you get to perform one queued move per
 *	entry into the continuation, then you must exit.
 *
 *	Another Note: When coding a cycle (like this one) you must wait until
 *	the last move has actually been queued (or has finished) before declaring
 *	the cycle to be done. Otherwise there is a nasty race condition in the
 *	tg_controller() that will accept the next command before the position of
 *	the final move has been recorded in the Gcode model. That's what the call
 *	to cm_get_runtime_busy() is about.
 */

uint8_t cm_straight_wait(float time)
{
	cm.wait_state = WS_WAITING;		// wait until planner queue empties before completing initialization
	ws.func = _waiting_init; 			// bind probing initialization function
	return (STAT_OK);
}

uint8_t cm_wait_callback(void)
{
	if ((cm.cycle_state != CYCLE_WAITINGSWITCH) && (cm.wait_state != WS_WAITING)) {
		return (STAT_NOOP);				// exit if not in a probe cycle or waiting for one
	}
	if (cm_get_runtime_busy() == true) { return (STAT_EAGAIN);}	// sync to planner move ends
	if (lstop) { return (STAT_EAGAIN);}
    return (ws.func());                                         // execute the current homing move
}

/*
 * _probing_init()	- G38.2 homing cycle using limit switches
 *
 *	These initializations are required before starting the probing cycle.
 *	They must be done after the planner has exhasted all current CYCLE moves as
 *	they affect the runtime (specifically the switch modes). Side effects would
 *	include limit switches initiating probe actions instead of just killing movement
 */

static uint8_t _waiting_init()
{
	// so optimistic... ;)
	// NOTE: it is *not* an error condition for the probe not to trigger.
	// it is an error for the limit or homing switches to fire, or for some other configuration error.
	cm.probe_state = WS_FAILED;
	cm.cycle_state = CYCLE_WAITINGSWITCH;


	return (_set_ws_func(_waiting_start));							// start the move
}

/*
 * _probing_start()
 */

static stat_t _waiting_start()
{
	uint32_t lRet = pdFALSE;
	pl_arcook_start();
	xQueueReset((xQueueHandle)xArcoOkSync);
	lRet = xSemaphoreTake( xArcoOkSync, pdMS_TO_TICKS(3000) );
	if (lRet == pdFALSE)
	{
		uint32_t qSend = ARCO_OK_INIT_FAILED;
		stopDuringCut_Set(true);
		xQueueSend( qKeyboard, &qSend, 0 );
		macro_func_ptr = command_idle;
		cm.wait_state = WS_FAILED;
	//	return (STAT_OK);
	}
	else
	{
		cm.wait_state = WS_SUCCEEDED;
	}
	return (_set_ws_func(_waiting_finish));
}

/*
 * _probing_finish()
 */

static stat_t _waiting_finish()
{
	return (_set_ws_func(_waiting_finalize_exit));
}

/*
 * _probe_restore_settings()
 * _probing_finalize_exit()
 * _probing_error_exit()
 */

void _wait_restore_settings()
{
	mp_flush_planner(); 						// we should be stopped now, but in case of switch closure

	cm_cycle_end();
	cm.cycle_state = CYCLE_OFF;
}

static stat_t _waiting_finalize_exit()
{
	_wait_restore_settings();
	return (STAT_OK);
}

static stat_t _waiting_error_exit(int8_t axis)
{
	// clean up and exit
	_probe_restore_settings();
	return (STAT_PROBE_CYCLE_FAILED);
}
