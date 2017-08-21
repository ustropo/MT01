/*
 * spiffs_hw.h
 *
 *  Created on: Jul 25, 2016
 *      Author: LAfonso01
 */

#ifndef SPIFFS_SPIFFS_HW_H_
#define SPIFFS_SPIFFS_HW_H_
#include "spiffs.h"
#include "spiflash.h"

s32_t spiffs_init(void);
s32_t spiffs_format(void);
s32_t spi_mem_read(u32_t addr, u32_t size, u8_t *dst);
s32_t spi_mem_write(u32_t addr, u32_t size, u8_t *src);
s32_t spi_mem_erase(u32_t addr, u32_t size);

extern spiflash_t spif;


#endif /* SPIFFS_SPIFFS_HW_H_ */
