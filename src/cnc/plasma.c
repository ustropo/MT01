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
#include "planner.h"
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
#include "ut_state_config_var.h"

#include "platform.h"
#include "r_s12ad_rx_if.h"
#include "r_mtu_rx_if.h"

#define DEBOUNCE_COUNT 15
#define ARCOOK_DELAY_COUNT 33

#define THC_HISTERESE 1.5
#define THC_KERF_PORCENTAGE 0.18
#define THC_MERGULHO_PORCENTAGE 0.80

#define THC_MULT  (0.000001428*1.15)
#define THC_MULT_RAP (THC_MULT*1.25)
#define THC_MAX 0.01


/* Lento */
#define KI 0.000000125
#define KP 0.00025
/* Rapido */
//#define KI 0.0000005
//#define KP 0.001

void timer_motorPower_callback(void *pdata);
void emergencia_task(void);
extern void warm_stop(uint8_t flag);

extern TaskHandle_t xCncTaskHandle;
extern bool simTorch;
extern bool lstop;

static bool stopDuringCut = false;
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
	xTaskCreate((pdTASK_CODE)plasma_task, "Plasma task", 512, NULL, 3, &xPlasmaTaskHandle );
    /* Attempt to create a semaphore. */
	xArcoOkSync = xSemaphoreCreateBinary();
}

void pl_thc_init(void)
{
    adc_cfg_t config;
    adc_time_t ad_conv_time;
    adc_ch_cfg_t ch_cfg;

    config.trigger = ADC_TRIG_SOFTWARE;
    config.priority = 0;
    config.add_cnt = ADC_ADD_OFF;
    config.alignment = ADC_ALIGN_RIGHT;
    config.clearing = ADC_CLEAR_AFTER_READ_ON;
    config.conv_speed = ADC_CONVERT_SPEED_PCLK;
    R_ADC_Open(ADC_MODE_SS_ONE_CH, &config, FIT_NO_FUNC);
    ad_conv_time.reg_id = ADC_SST_CH0_TO_20;
    ad_conv_time.num_states = 0xFF;
    R_ADC_Control(ADC_CMD_SET_SAMPLE_STATE_CNT, &ad_conv_time);

    ch_cfg.chan_mask = ADC_MASK_CH3;
    R_ADC_Control(ADC_CMD_ENABLE_CHANS, &ch_cfg);
}

void pl_thc_read(void)
{
	uint16_t u16thc_read;
	uint32_t u16thc_sum = 0;
	uint16_t u16thc_value;
	for(uint8_t i = 0; i < 50;i++){
		/* CAUSE SOFTWARE TRIGGER */
		R_ADC_Control(ADC_CMD_SCAN_NOW, NULL);
		/* WAIT FOR SCAN TO COMPLETE */
		while (R_ADC_Control(ADC_CMD_CHECK_SCAN_DONE, NULL) == ADC_ERR_SCAN_NOT_DONE)
		nop();
		/* READ RESULT */
		R_ADC_Read(ADC_REG_CH3, &u16thc_read);
		u16thc_sum += (uint32_t)u16thc_read;
	}
	u16thc_value = (uint16_t)(u16thc_sum/50);
	THC_real = (float)(((float)300/4095)*u16thc_value);
}

float pl_thc_pid(void)
{
	float result = 0;
	float THCVel = 0;
	int16_t ldelay_thc;
	/* velocidade do THC proporcinal a velocidade de feddrate */
	if (configVarPl[PL_CONFIG_VELOC_CORTE] < 3500)
		THCVel = configVarPl[PL_CONFIG_VELOC_CORTE]*THC_MULT;
	else
		THCVel = configVarPl[PL_CONFIG_VELOC_CORTE]*THC_MULT_RAP;

	/* reta do delay inversamente proporcinal a velocidade de feddrate */
	ldelay_thc = (uint16_t)(17500 - configVarPl[PL_CONFIG_VELOC_CORTE]*5);
	/* limite maximo da velocidade do THC */
	if(THCVel > THC_MAX)
	{
		THCVel = THC_MAX;
	}
	/* limite minimo do delay do THC */
	if(ldelay_thc < 4000)
	{
		ldelay_thc = 4000;
	}
	pl_thc_read();
	if(delay_thcGet() > ldelay_thc){
		if(THC_real > THC_VMIN)
		{
			THC_err = configVarPl[PL_CONFIG_TENSAO_THC] - THC_real;
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
			if (configFlags[KERF] == HABILITADO)
			{
				if(fabs(THC_err) > configVarPl[PL_CONFIG_TENSAO_THC]*THC_KERF_PORCENTAGE)
				{
					result = 0;
				}
			}
			if (configFlags[MERGULHO] == HABILITADO)
			{
				if(mp_get_runtime_velocity() < configVarPl[PL_CONFIG_VELOC_CORTE]*THC_MERGULHO_PORCENTAGE)
				{
					result = 0;
				}
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
    xTaskNotifyGive( xPlasmaTaskHandle);
	arcoOkSet(false);
}

void pl_arcook_stop(void)
{
    ArcoOktaskIdle = false;
	arcoOkSet(false);
}

#pragma interrupt IRQ9_isr(vect=VECT(ICU, IRQ9))
static void IRQ9_isr (void) {
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
	uint32_t qSend = 0;
    bool pinState;

	while(1)
	{
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
					xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
					stopDuringCut_Set(true);
				    vTaskPrioritySet( xCncTaskHandle, 3 );
					warm_stop(2);
				    vTaskPrioritySet( xCncTaskHandle, 1 );
					TORCH = FALSE;
					if( uxTaskPriorityGet( xCncTaskHandle ) != 1 )
					{

					}
					lstop = true;
					ut_lcd_output_warning("PLASMA NÃO\nTRANSFERIDO\n");
					while(qSend != KEY_ESC){
						WDT_FEED
						xQueueReceive( qKeyboard, &qSend, portMAX_DELAY );
					}
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
			if(emergencyCount == 100)
			{
				emergenciaFlag = true;
				xTimerStop( swTimers[AUTO_MENU_TIMER], 0 );
	    		if (isCuttingGet() == true)
	    		{
	    			stopDuringCut_Set(true);
	    		}

			    vTaskPrioritySet( xCncTaskHandle, 3 );
				warm_stop(2);
			    vTaskPrioritySet( xCncTaskHandle, 1 );
				TORCH = FALSE;
			    if( uxTaskPriorityGet( xCncTaskHandle ) != 1 )
				{

				}
	    		lstop = true;

		    	if (currentLine == 0){
		    		strcpy(Str,"MODO DE EMERGÊNCIA\n");
		    	}
		    	else
		    	{
		    		sprintf(Str,"MODO DE EMERGÊNCIA\nPARADO LINHA\n%d\n",currentLine);
		    	}
		    	ut_lcd_output_warning(Str);
				while(keyEntry != KEY_ESC){
					WDT_FEED
					xQueueReceive( qKeyboard, &keyEntry, portMAX_DELAY );
				}
				keyEntry = EMERGENCIA_SIGNAL;
				xQueueSend( qKeyboard, &keyEntry, 0 );
			//	macro_func_ptr = command_idle;
				realease = true;
				emergencyCount = 0;
			}
		}
	}
}

void timer_motorPower_callback(void *pdata)
{
	PWMCH ^= 1;
	if (TORCH == TRUE)
	{
		if (arcoOkGet() == true)
		{
			isCuttingSet(true);
		}
	}
	else
	{
		isCuttingSet(false);
	}
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

void stopDuringCut_Set(bool state)
{
	stopDuringCut = state;
}

bool stopDuringCut_Get(void)
{
	return stopDuringCut;
}


