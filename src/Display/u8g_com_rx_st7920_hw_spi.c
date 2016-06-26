/*
  
  u8g_com_rx_st7920_hw_spi.c
  
  Additional COM device, initially introduced for 3D Printer community
  Implements a fast SW SPI com subsystem

  Universal 8bit Graphics Library
  
  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

  A special SPI interface for ST7920 controller

  Update for ATOMIC operation done (01 Jun 2013)
    U8G_ATOMIC_OR(ptr, val)
    U8G_ATOMIC_AND(ptr, val)
    U8G_ATOMIC_START();
    U8G_ATOMIC_END();


*/

#include "platform.h"
#include "u8g.h"
#include "r_rspi_rx_if.h"
#include "FreeRTOS.h"
#include "task.h"

static rspi_handle_t handle;
volatile uint8_t g_rspi_callbackend = 0;
extern xTaskHandle task_main_handle;

static void rspi_callback(void *p_arg);

static uint8_t u8g_rx_st7920_hw_spi_shift_out(u8g_t *u8g, uint8_t val)
{
	rspi_command_word_t command;
	command.cpha = RSPI_SPCMD_CPHA_SAMPLE_EVEN;
	command.cpol = RSPI_SPCMD_CPOL_IDLE_HI;
	command.bit_order = RSPI_SPCMD_ORDER_MSB_FIRST;
	command.bit_length = RSPI_SPCMD_BIT_LENGTH_8;
	command.br_div = RSPI_SPCMD_BR_DIV_1;
	R_RSPI_Write(handle,command,&val,sizeof(val));
	while(!g_rspi_callbackend);
	g_rspi_callbackend = 0;
//	ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

  return  0;
}


static void u8g_com_rx_st7920_write_byte_hw_spi_seq(u8g_t *u8g, uint8_t rs, uint8_t *ptr, uint8_t len)
{
  uint8_t i;

  if ( rs == 0 )
  {
    /* command */
    u8g_rx_st7920_hw_spi_shift_out(u8g, 0x0f8);
  }
  else if ( rs == 1 )
  {
    /* data */
    u8g_rx_st7920_hw_spi_shift_out(u8g, 0x0fa);
  }

  while( len > 0 )
  {
    u8g_rx_st7920_hw_spi_shift_out(u8g, *ptr & 0x0f0);
    u8g_rx_st7920_hw_spi_shift_out(u8g, *ptr << 4);
    ptr++;
    len--;
    u8g_10MicroDelay();
  }

  for( i = 0; i < 1; i++ )
    u8g_10MicroDelay();
}

static void u8g_com_rx_st7920_write_byte_hw_spi(u8g_t *u8g, uint8_t rs, uint8_t val)
{
  uint8_t i;

  if ( rs == 0 )
  {
    /* command */
    u8g_rx_st7920_hw_spi_shift_out(u8g, 0x0f8);
  }
  else if ( rs == 1 )
  {
    /* data */
    u8g_rx_st7920_hw_spi_shift_out(u8g, 0x0fa);
  }
  else
  {
    /* do nothing, keep same state */
  }

  u8g_rx_st7920_hw_spi_shift_out(u8g, val & 0x0f0);
  u8g_rx_st7920_hw_spi_shift_out(u8g, val << 4);

  for( i = 0; i < 1; i++ )
    u8g_10MicroDelay();
}

uint8_t u8g_com_rx_hw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
	/* Conditions: Channel not yet open. */
	uint8_t chan = 0;
	rspi_chnl_settings_t my_config;
	rspi_cmd_baud_t my_setbaud_struct;
	rspi_err_t rspi_result;


	  switch(msg)
	  {
	    case U8G_COM_MSG_INIT:
	    	my_config.gpio_ssl = RSPI_IF_MODE_3WIRE;
	    	my_config.master_slave_mode = RSPI_MS_MODE_MASTER;
	    	my_config.bps_target = 1000000; // Bit rate in bits-per-second.
	    	rspi_result = R_RSPI_Open(chan, &my_config, rspi_callback, &handle );
	    	if (RSPI_SUCCESS != rspi_result)
	    	{
	    		while(1);
	    	}
	    	my_setbaud_struct.bps_target = 1000000;
	    	rspi_result = R_RSPI_Control(handle, RSPI_CMD_SET_BAUD, &my_setbaud_struct);
	    	if (RSPI_SUCCESS != rspi_result)
	    	{
	    		while(1);
	    	}

	      break;
	    case U8G_COM_MSG_STOP:
	      break;

	    case U8G_COM_MSG_CHIP_SELECT:
	        if ( arg_val == 0 )
	        {
	          /* disable, note: the st7920 has an active high chip select */
	        	LCD_CS = 0;
	        }
	        else
	        {
	          /* enable */
	        	LCD_CS = 1;
	        }
	      break;


	    case U8G_COM_MSG_WRITE_BYTE:
	    	 u8g_com_rx_st7920_write_byte_hw_spi(u8g,  u8g->pin_list[U8G_PI_A0_STATE], arg_val);
	      break;
	    case U8G_COM_MSG_WRITE_SEQ:
	        u8g_com_rx_st7920_write_byte_hw_spi_seq(u8g, u8g->pin_list[U8G_PI_A0_STATE], (uint8_t *)arg_ptr, arg_val);
	      break;
	      case U8G_COM_MSG_WRITE_SEQ_P:
	      {
	        register uint8_t *ptr = arg_ptr;
	        while( arg_val > 0 )
	        {
	          u8g_com_rx_st7920_write_byte_hw_spi(u8g, u8g->pin_list[U8G_PI_A0_STATE], u8g_pgm_read(ptr) );
	          // u8g->pin_list[U8G_PI_A0_STATE] = 2;
	          ptr++;
	          arg_val--;
	        }
	      }
	      break;

	    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
	      u8g->pin_list[U8G_PI_A0_STATE] = arg_val;
	      break;
	  }
	  return 1;
}

void rspi_callback(void *p_arg)
{
	g_rspi_callbackend = 1;
//    BaseType_t xHigherPriorityTaskWoken;
//
//    xHigherPriorityTaskWoken = pdFALSE;
//
//    vTaskNotifyGiveFromISR( task_main_handle, &xHigherPriorityTaskWoken );
//
//    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}





