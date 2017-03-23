/*
 * nextion.h
 *
 *  Created on: Feb 28, 2017
 *      Author: LAfonso01
 */

#ifndef NEXTION_NEXTION_H_
#define NEXTION_NEXTION_H_

#include "platform.h"


typedef enum{
	COMPACTA_IMG = 0,
	BACKGROUND_IMG,
	SETTINGS_ICON,
	CONFIG_CORTE_ICON,
	FILE_ICON,
	AUTO_ICON,
	MANUAL_ICON,
}nt_img_t;

bool nexInit(void);
bool NexPage_Show(const char *name);
bool NexPage_Picture(uint16_t x,uint16_t y,nt_img_t img_number);
bool NexPage_Str(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t fontID, uint16_t fcolor,uint16_t bcolor,
		uint8_t xcenter,uint8_t ycenter, uint8_t sta, const char *str);

#endif /* NEXTION_NEXTION_H_ */
