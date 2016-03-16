/*
 * plasma.c
 *
 *  Created on: 14/03/2016
 *      Author: leocafonso
 */
#include "FreeRTOS.h"
#include "task.h"
#include "tinyg.h"
#include "config.h"
#include "plasma.h"
#include "hardware.h"
#include "canonical_machine.h"
#include "text_parser.h"
#include "keyboard.h"

#define DEBOUNCE_COUNT 3

static bool isCutting = false;
TaskHandle_t xPlasmaTaskHandle;
extern TaskHandle_t xCncTaskHandle;

void pl_arcook_init(void)
{
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    PORTD.PMR.BYTE  = 0x41 ;
    MPC.PD0PFS.BYTE = 0x40 ;    /* PD0 is a IRQ - ARCO_OK*/
    ICU.IRQCR[0].BIT.IRQMD = 3;
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IPR(ICU, IRQ0) = 3;            //Set interrupt priority
    IEN(ICU, IRQ0) = 0;            // Enable interrupt
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}


void isCuttingSet(bool state)
{
	isCutting = state;
}

bool isCuttingGet(void)
{
	return isCutting;
}

void pl_arcook_start(void)
{
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ0) = 1;            // Enable interrupt
}

void pl_arcook_stop(void)
{
    IEN(ICU, IRQ0) = 0;            // Disable interrupt
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
}

void pl_arcook_check(void)
{
	uint32_t qSend;

	if (!TORCH && isCutting){
		isCutting = false;
		qSend = ARCO_OK_FAILED;
		xQueueSend( qKeyboard, &qSend, 0 );
	}
}



#pragma interrupt IRQ0_isr(vect=VECT(ICU, IRQ0))
static void IRQ0_isr (void) {
	    BaseType_t xHigherPriorityTaskWoken;

	    xHigherPriorityTaskWoken = pdFALSE;

	    vTaskNotifyGiveFromISR( xPlasmaTaskHandle, &xHigherPriorityTaskWoken );

	    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void plasma_task(void)
{
	uint8_t debounce;
	uint32_t qSend;
    bool pinState;

	while(1)
	{
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        debounce = 0;
        for(uint8_t i = 0; i < DEBOUNCE_COUNT; i++){
        	pinState = ARCO_OK;
            vTaskDelay(pdMS_TO_TICKS(10));
            if(pinState == ARCO_OK)
            {
            	debounce++;
            }
        }
        if(debounce == DEBOUNCE_COUNT)
        {
			if (!ARCO_OK)
			{
				xTaskNotifyGive(xCncTaskHandle);
			}
			else
			{
				qSend = ARCO_OK_FAILED;
				xQueueSend( qKeyboard, &qSend, 0 );
			}
        }
	}
}
