/*
 * nextion.c
 *
 *  Created on: Feb 28, 2017
 *      Author: LAfonso01
 */
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "platform.h"
#include "r_sci_async_rx_if.h"
#include "nextion.h"

SemaphoreHandle_t xSemSCI;

#define NEX_RET_CMD_FINISHED            (0x01)
#define NEX_RET_EVENT_LAUNCHED          (0x88)
#define NEX_RET_EVENT_UPGRADED          (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_INVALID_CMD             (0x00)
#define NEX_RET_INVALID_COMPONENT_ID    (0x02)
#define NEX_RET_INVALID_PAGE_ID         (0x03)
#define NEX_RET_INVALID_PICTURE_ID      (0x04)
#define NEX_RET_INVALID_FONT_ID         (0x05)
#define NEX_RET_INVALID_BAUD            (0x11)
#define NEX_RET_INVALID_VARIABLE        (0x1A)
#define NEX_RET_INVALID_OPERATION       (0x1B)

#define NEX_RECEIVED_BYTES       (0x4)


const sci_uart_t config = {
	    .baud_rate = 9600,     // ie 9600, 19200, 115200
		.clk_src = SCI_CLK_INT,
		.data_size = SCI_DATA_8BIT,
		.parity_en = SCI_PARITY_OFF,
		.parity_type = SCI_EVEN_PARITY,
		.stop_bits =SCI_STOPBITS_1,
	    .int_priority = 4,   // txi, tei, rxi INT priority; 1=low, 15=high
};

sci_hdl_t console;
uint8_t rx_bytes_size = 0;

void sci9_callback(void *p_args)
{
	static uint8_t rx_bytes_received = 0;
	BaseType_t xHigherPriorityTaskWoken;
	sci_cb_args_t *args;
	args = (sci_cb_args_t *)p_args;
	if (args->event == SCI_EVT_RX_CHAR)
	{

		rx_bytes_received++;
		if (rx_bytes_received == NEX_RECEIVED_BYTES)
		{
			xSemaphoreGiveFromISR( xSemSCI, &xHigherPriorityTaskWoken );
			rx_bytes_received = 0;
		}
	}
}

void sendCommand(const char* cmd)
{
	uint8_t p = 0xFF;
//    while (nexSerial.available())
//    {
//        nexSerial.read();
//    }
	R_SCI_Send(console,cmd,strlen(cmd));
	R_SCI_Send(console,&p,1);
	R_SCI_Send(console,&p,1);
	R_SCI_Send(console,&p,1);
}

bool recvRetCommandFinished(uint32_t timeout)
{
	uint32_t lRet = pdFALSE;
    bool ret = false;
    uint8_t temp[4] = {0};
    R_SCI_Receive(console, temp, sizeof(temp));
    lRet = xSemaphoreTake(xSemSCI,timeout);
    if(lRet == true)
    {
        if (temp[0] == NEX_RET_CMD_FINISHED
            && temp[1] == 0xFF
            && temp[2] == 0xFF
            && temp[3] == 0xFF
            )
        {
            ret = true;
        }
    }
    return ret;
}


bool nexInit(void)
{
    bool ret1 = false;
    xSemSCI = xSemaphoreCreateBinary();
	R_SCI_Open(SCI_CH9,SCI_MODE_ASYNC,(void *)&config,sci9_callback,&console);
    sendCommand("");
    sendCommand("bkcmd=1");
    recvRetCommandFinished(100);
    sendCommand("page 0");
    ret1 = recvRetCommandFinished(100);
    return ret1;
}

bool NexPage_Show(const char *name)
{
	char cmd[20];
	snprintf(cmd,sizeof(cmd),"page %s",name);
    sendCommand(cmd);
    return recvRetCommandFinished(100);
}

bool NexPage_Picture(uint16_t x,uint16_t y,nt_img_t img_number)
{
	char cmd[20];
	snprintf(cmd,sizeof(cmd),"pic %d,%d,%d",x,y,img_number);
    sendCommand(cmd);
    return recvRetCommandFinished(100);
}

bool NexPage_Str(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t fontID, uint16_t fcolor,uint16_t bcolor,
		uint8_t xcenter,uint8_t ycenter, uint8_t sta, const char *str)
{
	char cmd[60];
	snprintf(cmd,sizeof(cmd),"xstr %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\"",x,y,w,h,fontID,fcolor,bcolor,xcenter,ycenter,sta,str);
    sendCommand(cmd);
    return recvRetCommandFinished(100);
}

