/*
 * plasma.c
 *
 *  Created on: 14/03/2016
 *      Author: leocafonso
 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "tinyg.h"
#include "config.h"
#include "controller.h"
#include "plasma.h"
#include "spindle.h"
#include "hardware.h"
#include "switch.h"
#include "canonical_machine.h"
#include "text_parser.h"
#include "keyboard.h"
#include "lcd.h"
#include "eeprom.h"
#include "config_SwTimers.h"
#include "macros.h"

#include "platform.h"
#include "r_s12ad_rx_if.h"
#include "r_mtu_rx_if.h"

#define DEBOUNCE_COUNT 15
#define ARCOOK_DELAY_COUNT 33

#define THC_VMIN 20
#define THC_HISTERESE 2
#define THC_PORCENTAGE 0.2
#define THC_RAPIDO 0.005
#define THC_LENTO  0.001

/* Lento */
#define KI 0.000000125
#define KP 0.00025
/* Rapido */
//#define KI 0.0000005
//#define KP 0.001

void timer_motorPower_callback(void *pdata);
void emergencia_task(void);
extern void warm_stop(void);

extern bool simTorch;
extern bool lstop;

static bool isCutting = false;
static bool arcoOk = false;
static uint16_t delay_thc = 0;
static uint16_t delay_thcStart = false;
TaskHandle_t xPlasmaTaskHandle;
TaskHandle_t xEmergenciaTaskHandle;
bool emergenciaFlag = false;
SemaphoreHandle_t xArcoOkSync;
bool ArcoOktaskIdle = false;
volatile uint16_t    data;
float THC_real;
float THC_err;
float THC_integral;
static char Str[50];

#pragma section B NOINIT
uint32_t currentLine;
#pragma section

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
//    ICU.IRQCR[9].BIT.IRQMD = 1;    // Borda de descida
//    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
//    IPR(ICU, IRQ9) = 3;            //Set interrupt priority
//    IEN(ICU, IRQ9) = 0;            // Enable interrupt

	xTaskCreate((pdTASK_CODE)plasma_task, "Plasma task", 512, NULL, 3, &xPlasmaTaskHandle );
    /* Attempt to create a semaphore. */
	xArcoOkSync = xSemaphoreCreateBinary();
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
	float THCVel = 0;
	if(configTHCVel)
	{
		THCVel = THC_RAPIDO;
	}
	else
	{
		THCVel = THC_LENTO;
	}
	pl_thc_read(&THC_real);
	if(delay_thcGet() > (uint16_t)(configVar[DELAY_THC]*10000)){
		if(THC_real > THC_VMIN)
		{/*
			THC_err = configVar[TENSAO_THC] - THC_real;
			THC_integral += THC_err;
			result = (KP * THC_err) + (KI * THC_integral);
			if (fabs(result) > 0.02)
			{
				if(result > 0)
				{
					result = 0.02;
					THC_integral = 0;
				}
				else if(result < 0)
				{
					result = -0.02;
					THC_integral = 0;
				}
			}
			*/
			THC_err = configVar[TENSAO_THC] - THC_real;
			if(THC_err > THC_HISTERESE)
			{
				result = THCVel;
			}
			if(THC_err < -THC_HISTERESE)
			{
				result = -THCVel;
			}
			if(THC_err >= -THC_HISTERESE && THC_err <= THC_HISTERESE)
			{
				result = 0;
			}
			if(fabs(THC_err) > configVar[TENSAO_THC]*THC_PORCENTAGE)
			{
				result = 0;
			}
		}
	}
	return result;
}

void pl_emergencia_init(void)
{
    ICU.IRQCR[8].BIT.IRQMD = 2;
    IR(ICU, IRQ8)  = 0;            //Clear any previously pending interrupts
    IPR(ICU, IRQ8) = 3;            //Set interrupt priority
    IEN(ICU, IRQ8) = 0;            // Enable interrupt
    IR(ICU, IRQ8)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ8) = 1;            // Enable interrupt
    xTaskCreate((pdTASK_CODE)emergencia_task, "Emergencia task", 256, NULL, 3, &xEmergenciaTaskHandle );
    R_CMT_CreatePeriodic(10000,timer_motorPower_callback,&timerMotorPower);
}

void pl_arcook_start(void)
{
#ifndef MODULO
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
    IEN(ICU, IRQ0) = 1;            // Enable interrupt
#else

//	IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
//	IEN(ICU, IRQ9) = 1;            // Enable interrupt
//	xSemaphoreTake( xArcoOkSync, 0 );
    xTaskNotifyGive( xPlasmaTaskHandle);
	arcoOkSet(false);
#endif
}

void pl_arcook_stop(void)
{
#ifndef MODULO
    IEN(ICU, IRQ0) = 0;            // Disable interrupt
    IR(ICU, IRQ0)  = 0;            //Clear any previously pending interrupts
#else
//    IEN(ICU, IRQ9) = 0;            // Disable interrupt
//    IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
//    xTaskNotifyGive( xPlasmaTaskHandle);
    ArcoOktaskIdle = false;
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
//	    BaseType_t xHigherPriorityTaskWoken;
//
//	    xHigherPriorityTaskWoken = pdFALSE;
//
//	    vTaskNotifyGiveFromISR( xPlasmaTaskHandle, &xHigherPriorityTaskWoken );
//
//	    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
//
//        IR(ICU, IRQ9)  = 0;            //Clear any previously pending interrupts
//        IEN(ICU, IRQ9) = 0;            // Enable interrupt

}

#pragma interrupt IRQ8_isr(vect=VECT(ICU, IRQ8))
static void IRQ8_isr (void) {
    BaseType_t xHigherPriorityTaskWoken;

    xHigherPriorityTaskWoken = pdFALSE;

    vTaskNotifyGiveFromISR( xEmergenciaTaskHandle, &xHigherPriorityTaskWoken );

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
        ArcoOktaskIdle = true;
        debounce = 0;
        while(ArcoOktaskIdle)
        {
			pinState = ARCO_OK;
			vTaskDelay(pdMS_TO_TICKS(10));
			if(pinState == ARCO_OK)
			{
				debounce++;
			}
			if(debounce == DEBOUNCE_COUNT)
			{
				if (!ARCO_OK)
				{
					if (!simTorch)
					{
						//xTaskNotifyGive(xCncTaskHandle);
						xSemaphoreGive( xArcoOkSync );
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
				}while(debounce != ARCOOK_DELAY_COUNT && isCutting && ArcoOktaskIdle);
				if (isCutting && ArcoOktaskIdle)
				{
					qSend = ARCO_OK_FAILED;
					xQueueSend( qKeyboard, &qSend, 0 );
				}
			}
		}
	}
}

void emergencia_task(void)
{
	uint32_t keyEntry = 0;
	uint8_t emergencyCount = 0;
	bool realease = false;
	while(1)
	{
		emergenciaFlag = false;
		realease = false;
	    IR(ICU, IRQ8)  = 0;            //Clear any previously pending interrupts
	    IEN(ICU, IRQ8) = 1;            // Enable interrupt
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
	    IR(ICU, IRQ8)  = 0;            //Clear any previously pending interrupts
	    IEN(ICU, IRQ8) = 0;            // Enable interrupt
		while(EMERGENCIA && !realease)
		{
			vTaskDelay(1 / portTICK_PERIOD_MS);
			emergencyCount++;
			if(emergencyCount == 200)
			{
				emergenciaFlag = true;
				xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
				cm_spindle_control(SPINDLE_OFF);
	    		lstop = true;
				warm_stop();
		    	if (currentLine == 0){
		    		strcpy(Str,"MODO DE EMERGÊNCIA\n");
		    	}
		    	else
		    	{
		    		sprintf(Str,"MODO DE EMERGÊNCIA\nPARADO LINHA\n%d\n",currentLine);
		    	}
		    	ut_lcd_output_warning(Str);
				while(keyEntry != KEY_ENTER){
					IWDT.IWDTRR = 0x00u;
					IWDT.IWDTRR = 0xFFu;
					xQueueReceive( qKeyboard, &keyEntry, portMAX_DELAY );
				}
				keyEntry = EMERGENCIA_SIGNAL;
				xQueueSend( qKeyboard, &keyEntry, 0 );
				realease = true;
				emergencyCount = 0;
			}
		}
	}
}

void timer_motorPower_callback(void *pdata)
{
	PWMCH ^= 1;
	if(delay_thcStart){
		if (delay_thc == 0xFFFF)
		{
			delay_thc = 0xFFFF;
		}
		else
		{
			delay_thc++;
		}
	}
}

void delay_thcStartStop(bool state)
{
	delay_thcStart = state;
	delay_thc = 0;
}

void isCuttingSet(bool state)
{
	isCutting = state;
	if(state == false)
	{
		delay_thcStartStop(false);
	}
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

void delay_thcSet(uint16_t state)
{
	delay_thc = state;
}

uint16_t delay_thcGet(void)
{
	return delay_thc;
}

