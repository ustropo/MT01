/*
 * plasma.h
 *
 *  Created on: 14/03/2016
 *      Author: leocafonso
 */

#ifndef CNC_PLASMA_H_
#define CNC_PLASMA_H_
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//#define ARCO_OK_FAILED  0xFFFFFFFF
//#define ARCO_OK_OFF     0xFFFFFFFE
//#define MATERIAL_FAILED 0xFFFFFFFD
#define ARCO_OK_INIT_FAILED  0xFFFC0000
#define ARCO_OK_FAILED  0xFFFF0000
#define ARCO_OK_OFF     0xFFFE0000
#define MATERIAL_FAILED 0xFFFD0000
extern SemaphoreHandle_t xArcoOkSync;
extern uint32_t currentLine;

void pl_arcook_init(void);
void pl_thc_init(void);
void pl_thc_read(void);
float pl_thc_pid(void);
void pl_arcook_start(void);
void pl_arcook_stop(void);
void pl_emergencia_init(void);
void plasma_task(void);
void delay_thcStartStop(bool state);

/*Setter and getters*/
void isCuttingSet(bool state);
bool isCuttingGet(void);
void arcoOkSet(bool state);
bool arcoOkGet(void);
float THC_realGet(void);
void delay_thcSet(uint16_t state);
uint16_t delay_thcGet(void);
void stopDuringCut_Set(bool state);
bool stopDuringCut_Get(void);

#endif /* CNC_PLASMA_H_ */
