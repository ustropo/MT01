/*
 * network.c - tinyg networking protocol
 * Part of TinyG project
 *
 * Copyright (c) 2010 - 2015 Alden S. Hart Jr.
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
/* 	This module is really nothing mre than a placeholder at this time.
 * 	"Networking" refers to a planned RS485 broadcast network to support
 *	multi-board configs and external RS485 devices such as extruders.
 *	Basic operation of RS485 on the TinyG hardware has been verified
 *	using what's in this file, but you won;t find much more.
 */


#include "tinyg.h"
#include "network.h"
#include "controller.h"
#include "gpio.h"
#include "hardware.h"
#include "xio.h"

/*
 * Local Scope Functions and Data
 */

/*
 * network_init()
 */
void network_init()
{

}

void net_forward(unsigned char c)
{

}

/*
 * net_test_rxtx() - test transmission from master to slave
 * net_test_loopback() - test transmission from master to slave and looping back
 */

uint8_t net_test_rxtx(uint8_t c)
{

	return (c);
}

uint8_t net_test_loopback(uint8_t c)
{

	return (c);
}

