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
#include "lcd.h"

#define DEBOUNCE_COUNT 15

void timer_motorPower_callback(void *pdata);
void emergencia_task(void);

static uint32_t timerch;
static bool isCutting = false;
TaskHandle_t xPlasmaTaskHandle;
TaskHandle_t xEmergenciaTaskHandle;
extern TaskHandle_t xCncTaskHandle;
extern bool simTorch;

void pl_arcook_init(void)
{
#ifndef MODULO
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    PORTD.PMR.BYTE  = 0x41 ;
    MPC.PD0PFS.BYTE = 0x40 ;    /* PD0 is a IRQ - ARCO_OK*/
    ICU.IRQCR[0].BIT.IRQMD = 3;
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IPR(ICU, IRQ0) = 3;            //Set interrupt priority
    IEN(ICU, IRQ0) = 0;            // Enable interrupt
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
#else
    ICU.IRQCR[9].BIT.IRQMD = 3;
    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
    IPR(ICU, IRQ9) = 3;            //Set interrupt priority
    IEN(ICU, IRQ9) = 0;            // Enable interrupt
#endif
}

void pl_emergencia_init(void)
{
    xTaskCreate((pdTASK_CODE)emergencia_task, "Emergencia task", 512, NULL, 6, &xEmergenciaTaskHandle );
    R_CMT_CreatePeriodic(10000,timer_motorPower_callback,&timerch);
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
#ifndef MODULO
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ0) = 1;            // Enable interrupt
#else
    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ9) = 1;            // Enable interrupt
#endif
}

void pl_arcook_stop(void)
{
#ifndef MODULO
    IEN(ICU, IRQ0) = 0;            // Disable interrupt
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
#else
    IEN(ICU, IRQ9) = 0;            // Disable interrupt
    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
#endif
}

void pl_arcook_check(void)
{
//	uint32_t qSend;
//
//	if (TORCH && isCutting){
//		isCutting = false;
//		qSend = ARCO_OK_FAILED;
//		xQueueSend( qKeyboard, &qSend, 0 );
//	}
}


#ifndef MODULO
#pragma interrupt IRQ0_isr(vect=VECT(ICU, IRQ0))
static void IRQ0_isr (void) {
#else
#pragma interrupt IRQ9_isr(vect=VECT(ICU, IRQ9))
static void IRQ9_isr (void) {
#endif
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
				if (!simTorch)
				{
					xTaskNotifyGive(xCncTaskHandle);
				}
			}
			else
			{
				qSend = ARCO_OK_FAILED;
				xQueueSend( qKeyboard, &qSend, 0 );
			}
        }
	}
}

void emergencia_task(void)
{
	while(1)
	{
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        TORCH = FALSE;
        while(EMERGENCIA)
        {
            ut_lcd_output_warning("SISTEMA INOPERANTE\nMODO DE EMERGÊNCIA\n");
			IWDT.IWDTRR = 0x00u;
			IWDT.IWDTRR = 0xFFu;
        }
    	R_BSP_InterruptsDisable();
        while(1){}
	}
}

void timer_motorPower_callback(void *pdata)
{
    BaseType_t xHigherPriorityTaskWoken;
	PWMCH ^= 1;
	if(EMERGENCIA)
	{
		xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR( xEmergenciaTaskHandle, &xHigherPriorityTaskWoken );
		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	}
}
