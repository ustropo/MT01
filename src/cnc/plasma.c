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
#include "eeprom.h"

#include "platform.h"
#include "r_s12ad_rx_if.h"
#include "r_mtu_rx_if.h"

#define DEBOUNCE_COUNT 15
#define ARCOOK_DELAY_COUNT 33

#define THC_VMIN 20
#define KI 0.0000001
#define KP 0.0004

void thc_interrupt(void *p_args);
void timer_motorPower_callback(void *pdata);
void emergencia_task(void);

static uint32_t timerch;
static bool isCutting = false;
static bool arcoOk = false;
TaskHandle_t xPlasmaTaskHandle;
TaskHandle_t xEmergenciaTaskHandle;
extern TaskHandle_t xCncTaskHandle;
extern bool simTorch;
volatile uint16_t    data;
float THC_real;
float THC_err;
float THC_integral;

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

	xTaskCreate((pdTASK_CODE)plasma_task, "Plasma task", 512, NULL, 3, &xPlasmaTaskHandle );
#endif
}

void pl_thc_init(void)
{
    adc_cfg_t config;
    adc_ch_cfg_t ch_cfg;
    /*
    mtu_timer_chnl_settings_t plasma_timer_cfg;
    mtu_err_t result;

    plasma_timer_cfg.clock_src.source  = MTU_CLK_SRC_INTERNAL;
    plasma_timer_cfg.clock_src.clock_edge  = MTU_CLK_RISING_EDGE;
    plasma_timer_cfg.clear_src  = MTU_CLR_TIMER_A;
    plasma_timer_cfg.timer_a.actions.do_action = (mtu_actions_t)(MTU_ACTION_TRIGGER_ADC | MTU_ACTION_INTERRUPT | MTU_ACTION_REPEAT) ;
    plasma_timer_cfg.timer_a.actions.output  = MTU_PIN_NO_OUTPUT;
    plasma_timer_cfg.timer_a.freq  = 5000;
    plasma_timer_cfg.timer_b.actions.do_action = MTU_ACTION_NONE;
    plasma_timer_cfg.timer_b.actions.output  = MTU_PIN_NO_OUTPUT;
    plasma_timer_cfg.timer_c.actions.do_action = MTU_ACTION_NONE;
    plasma_timer_cfg.timer_d.actions.do_action = MTU_ACTION_NONE;
    result = R_MTU_Timer_Open(MTU_CHANNEL_0, &plasma_timer_cfg, FIT_NO_FUNC);

    config.trigger = ADC_TRIG_SYNC_TRG0AN_0;
    config.priority = 3;
    config.add_cnt = ADC_ADD_OFF;
    config.alignment = ADC_ALIGN_RIGHT;
    config.clearing = ADC_CLEAR_AFTER_READ_ON;
    config.conv_speed = ADC_CONVERT_SPEED_PCLK;
    R_ADC_Open(ADC_MODE_SS_ONE_CH, &config, thc_interrupt);

    ch_cfg.chan_mask = ADC_MASK_CH3;
    R_ADC_Control(ADC_CMD_ENABLE_CHANS, &ch_cfg);
    R_ADC_Control(ADC_CMD_ENABLE_INT, NULL);
    R_ADC_Control(ADC_CMD_ENABLE_TRIG, NULL);

    result = R_MTU_Control(MTU_CHANNEL_0, MTU_CMD_START, FIT_NO_PTR);
    */
    config.trigger = ADC_TRIG_SOFTWARE;
    config.priority = 0;
    config.add_cnt = ADC_ADD_OFF;
    config.alignment = ADC_ALIGN_RIGHT;
    config.clearing = ADC_CLEAR_AFTER_READ_ON;
    config.conv_speed = ADC_CONVERT_SPEED_PCLK;
    R_ADC_Open(ADC_MODE_SS_ONE_CH, &config, FIT_NO_FUNC);

    ch_cfg.chan_mask = ADC_MASK_CH3;
    R_ADC_Control(ADC_CMD_ENABLE_CHANS, &ch_cfg);
}

void pl_thc_read(float *thcValue)
{
	uint16_t u16thc_read;
	uint16_t u16thc_sum = 0;
	uint16_t u16thc_value;
	for(uint8_t i = 0; i < 8;i++){
		/* CAUSE SOFTWARE TRIGGER */
		R_ADC_Control(ADC_CMD_SCAN_NOW, NULL);
		/* WAIT FOR SCAN TO COMPLETE */
		while (R_ADC_Control(ADC_CMD_CHECK_SCAN_DONE, NULL) == ADC_ERR_SCAN_NOT_DONE)
		nop();
		/* READ RESULT */
		R_ADC_Read(ADC_REG_CH3, &u16thc_read);
		u16thc_sum += u16thc_read;
	}
	u16thc_value = u16thc_sum/8;
	*thcValue = (float)(((float)300/4095)*u16thc_value);
}

float pl_thc_pid(void)
{
	float result = 0;
	pl_thc_read(&THC_real);
	if(THC_real > THC_VMIN)
	{
		THC_err = configVar[TENSAO_THC] - THC_real;
		THC_integral += THC_err;
		result = (KP * THC_err) + (KI * THC_integral);
		if (fabs(result) > 0.02)
		{
			if(result > 0)
				result = 0.02;
			else if(result < 0)
				result = -0.02;
		}
	}
	return result;
}

void pl_emergencia_init(void)
{
    xTaskCreate((pdTASK_CODE)emergencia_task, "Emergencia task", 512, NULL, 6, &xEmergenciaTaskHandle );
    R_CMT_CreatePeriodic(10000,timer_motorPower_callback,&timerch);
}

void pl_arcook_start(void)
{
#ifndef MODULO
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ0) = 1;            // Enable interrupt
#else
    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ9) = 1;            // Enable interrupt
	arcoOkSet(false);
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
	arcoOkSet(false);
#endif
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
		arcoOkSet(false);
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
            debounce = 0;
        	do
        	{
				if(ARCO_OK)
				{
					debounce++;
					arcoOkSet(false);
				}
				else
				{
					debounce = 0;
		        	arcoOkSet(true);
				}
				vTaskDelay(pdMS_TO_TICKS(10));
        	}while(debounce != ARCOOK_DELAY_COUNT && isCutting);
        	if (isCutting)
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

void thc_interrupt(void *p_args)
{
    R_ADC_Read(ADC_REG_CH3, &data);
}

void isCuttingSet(bool state)
{
	isCutting = state;
}

bool isCuttingGet(void)
{
	return isCutting;
}

void arcoOkSet(bool state)
{
	arcoOk = state;
}

bool arcoOkGet(void)
{
	return arcoOk;
}

float THC_realGet(void)
{
	return THC_real;
}
