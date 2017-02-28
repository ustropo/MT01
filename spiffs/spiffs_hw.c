/*
 * spiffs_hw.c
 *
 *  Created on: Jul 25, 2016
 *      Author: LAfonso01
 */
#include "platform.h"
#include "fsystem_spi.h"
#include "spiffs.h"
#include "r_spi_flash_if.h"
#include "spiffs_hw.h"
#include "xio.h"

#define LOG_PAGE_SIZE       256

static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

static spiffs *fs = (spiffs *)&uspiffs[0].gSPIFFS;
static spiffs_config cfg;

s32_t spiffs_init(void)
{
	int res;

	FS_spi_hw_init();

//	R_SF_Erase(1, 0, SF_ERASE_BULK);

	cfg.hal_read_f = my_spi_read;
	cfg.hal_write_f = my_spi_write;
	cfg.hal_erase_f = my_spi_erase;

	res = SPIFFS_mount(fs,
			&cfg,
			spiffs_work_buf,
			spiffs_fds,
			sizeof(spiffs_fds),
			spiffs_cache_buf,
			sizeof(spiffs_cache_buf),
			0);

	return res;
}

s32_t spiffs_format(void)
{
	s32_t res = 0;

	R_SF_Erase(1, 0, SF_ERASE_BULK);

	res = SPIFFS_mount(fs,
			&cfg,
			spiffs_work_buf,
			spiffs_fds,
			sizeof(spiffs_fds),
			spiffs_cache_buf,
			sizeof(spiffs_cache_buf),
			0);

	SPIFFS_unmount(fs);

	res = SPIFFS_format(fs);
	if (res != SPIFFS_OK)
	{
		return res;
	}

	res = SPIFFS_mount(fs,
			&cfg,
			spiffs_work_buf,
			spiffs_fds,
			sizeof(spiffs_fds),
			spiffs_cache_buf,
			sizeof(spiffs_cache_buf),
			0);
	return res;
}

s32_t my_spi_read(u32_t addr, u32_t size, u8_t *buf)
{
	FS_spi_read_blocking(addr,(uint8_t *)buf,size);
	return SPIFFS_OK;
}
s32_t my_spi_write(u32_t addr, u32_t size, u8_t *buf)
{
	FS_spi_write_blocking(addr,(uint8_t *)buf,size);
	return SPIFFS_OK;
}
s32_t my_spi_erase(u32_t addr, u32_t size)
{
	FS_spi_erase_blocking(addr);
	return SPIFFS_OK;
}
