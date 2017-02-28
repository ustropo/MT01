/*
 * spiffs_hw.h
 *
 *  Created on: Jul 25, 2016
 *      Author: LAfonso01
 */

#ifndef SPIFFS_SPIFFS_HW_H_
#define SPIFFS_SPIFFS_HW_H_
#include "spiffs.h"

s32_t spiffs_init(void);
s32_t spiffs_format(void);
s32_t my_spi_read(u32_t addr, u32_t size, u8_t *buf);
s32_t my_spi_write(u32_t addr, u32_t size, u8_t *buf);
s32_t my_spi_erase(u32_t addr, u32_t size);

#endif /* SPIFFS_SPIFFS_HW_H_ */
