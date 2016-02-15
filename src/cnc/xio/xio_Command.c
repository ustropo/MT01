/*
 *  xio_file.c	- device driver for program memory "files"
 * 				- works with avr-gcc stdio library
 *
 * Part of TinyG project
 *
 * Copyright (c) 2011 - 2015 Alden S. Hart Jr.
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * THE SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
 * WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <stdio.h>				// precursor for xio.h
#include <stdbool.h>			// true and false
#include <string.h>				// for memset
#include <stdint.h>				//
//#include "platform.h"
#include "xio.h"
#include "ut_state.h"


/******************************************************************************
 * FILE CONFIGURATION RECORDS
 ******************************************************************************/

xioCommand_t	    command[XIO_DEV_COMMAND_COUNT];

struct cfgFILE {
	x_open_t x_open;			// see xio.h for typedefs
	x_ctrl_t x_ctrl;
	x_close_t x_close;
	x_gets_t x_gets;
	x_getc_t x_getc;
	x_putc_t x_putc;
	x_flow_t x_flow;
};

static struct cfgFILE const cfgFile[] = {
{	// PGM config
	xio_open_command,				// open function
	xio_ctrl_generic, 			// ctrl function
	xio_close_command, 			// close function
	xio_gets_command,				// get string function
	xio_getc_command,				// stdio getc function
	xio_putc_command,				// stdio putc function
	xio_fc_null,				// flow control callback
}
};
/******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

/*
 *	xio_init_file() - initialize and set controls for file IO
 *
 *	Need to bind the open function or a subsequent opens will fail
 */

void xio_init_command()
{
	for (uint8_t i=0; i<XIO_DEV_COMMAND_COUNT; i++) {
		xio_open_generic(XIO_DEV_COMMAND_OFFSET + i,
						(x_open_t)(cfgFile[i].x_open),
						(x_ctrl_t)(cfgFile[i].x_ctrl),
						(x_close_t)(cfgFile[i].x_close),
						(x_gets_t)(cfgFile[i].x_gets),
						(x_getc_t)(cfgFile[i].x_getc),
						(x_putc_t)(cfgFile[i].x_putc),
						(x_flow_t)(cfgFile[i].x_flow));
	}
}

/*
 *	xio_open_file() - open the program memory device to a specific string address
 *
 *	OK, so this is not really a UNIX open() except for its moral equivalent
 *  Returns a pointer to the stdio FILE struct or -1 on error
 */
FILE * xio_open_command(const uint8_t dev, const char *addr, const flags_t flags)
{
	xioDev_t *d = (xioDev_t *)&ds[dev];
	d->x = &command[dev - XIO_DEV_COMMAND_OFFSET];			// bind extended struct to device
	xioCommand_t *dx = (xioCommand_t *)d->x;
	dx->filebase_P = addr;

	return(&d->file);								// return pointer to the FILE stream
}

int xio_gets_command(xioDev_t *d, char *buf, const int size)
{
	xioCommand_t *dx = (xioCommand_t *)d->x;
	const char *buffin = dx->filebase_P ;

	if (buffin == NULL)
	{
		return(XIO_EAGAIN);
	}

	while ( *buffin != '\n')
	{
		*buf++ = *buffin++;
		if(*buffin == '\0')
		{
			dx->filebase_P = NULL;
			return(XIO_OK);
		}
	}
	*buf++ = '\n';
	dx->filebase_P = ++buffin;
	*buf = '\0';
	return(XIO_OK);
}

void xio_close_command (xioDev_t *d)
{

}

int xio_getc_command(FILE *stream)
{
	return(0);
}

int xio_putc_command(const char c, FILE *stream)
{
	return(-1);
}
