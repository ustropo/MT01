/*
 * fsystem_spi.c
 *
 *  Created on: Jul 13, 2016
 *      Author: LAfonso01
 */
#include "platform.h"
#include "r_spi_flash_if.h"
#include "fsystem_spi.h"

#include "FreeRTOS.h"

#include <string.h>

#define FL_RSPI_CHANNEL				(1)
/* Valid Mask for Load Image Header */
#define FS_VALID_MASK                (0xAA)
/* SPI Flash FS block size */
#define FS_BLOCK_SIZE              (1024*1024)
/* SPI Flash FS init address */
#define FS_BLOCK_END              (1024*1024 - 1)
/* Number of blocks */
#define FS_BLOCK_MAX              (7)
/* Number of blocks */
#define FS_BLOCK_SIZE_64K         (16)

volatile uint8_t tx_data;
volatile uint8_t rx_data;

static uint8_t g_block_sanity;
static uint32_t g_write_addr;
static uint32_t g_read_addr;
static uint8_t g_file_block = 0;
static uint32_t g_file_index = 0;
static uint8_t file_block_index = 0;
static uint8_t g_wl_offset = 0;

fs_image_header_t g_fs_header[FS_BLOCK_MAX];
fs_image_header_t g_fs_header_format;
const uint32_t g_block_addr[FS_BLOCK_MAX] = {
	0x100000,
	0x200000,
	0x300000,
	0x400000,
	0x500000,
	0x600000,
	0x700000,
};

static uint8_t fs_find_next_block_erased(void);
static uint8_t fs_find_file_block(uint8_t fblock);

void FS_spi_init(void)
{
	uint8_t i;

	R_SF_Erase(FL_RSPI_CHANNEL, 0, SF_ERASE_BULK);

	for (i = 0; i < FS_BLOCK_MAX; i++)
	{

		/* Read the block header */
		FS_spi_read_blocking(g_block_addr[i], (uint8_t *)&g_fs_header[i], sizeof(fs_image_header_t));
		/* Read the block end */
		FS_spi_read_blocking(g_block_addr[i] + FS_BLOCK_END, &g_block_sanity, 1);
		/* Checking if the block is totally erased */
		if (g_fs_header[i].valid_mask == 0xFF && g_block_sanity == 0xFF)
		{
			/* Write block header format*/
			memset((uint8_t *)&g_fs_header_format,0xFF,sizeof(fs_image_header_t));
			g_fs_header_format.valid_mask = FS_VALID_MASK;
			g_fs_header_format.block_state = BLOCK_ERASED;
			FS_spi_write_blocking(g_block_addr[i], &g_fs_header_format.valid_mask, 1);
			FS_spi_write_blocking(g_block_addr[i] + offsetof(fs_image_header_t, block_state),\
					(uint8_t *)&g_fs_header_format.block_state, sizeof(uint16_t));
		}
	}
}

void FS_spi_start_block(uint32_t fsize)
{
	uint8_t i;
	uint8_t blocks_needed = 0;
	uint8_t blocks_avalible = 0;
	uint8_t block_erase_count= 0;
	uint8_t block_offset= 0;

	/* Calculate how many block are needed for the file  */
	blocks_needed = (fsize/FS_BLOCK_SIZE) + 1;

	for (i = 0; i < FS_BLOCK_MAX; i++)
	{
		/* Read the block header */
		FS_spi_read_blocking(g_block_addr[i], (uint8_t *)&g_fs_header[i], sizeof(fs_image_header_t));
		/* Check if the block is available */
		if (g_fs_header[i].block_state == BLOCK_ERASED)
		{
			/* Get how many blocks is available*/
			blocks_avalible++;
		}
		/* Check if the block was in use */
		if (g_fs_header[i].block_state == BLOCK_IN_USE)
		{
			/* Erase the used block header -  initial 64K block */
			FS_spi_erase_blocking(g_block_addr[i]);

			g_wl_offset = i;
		}
	}
	/* Check if there is enough space available to write the file*/
	if(blocks_avalible < blocks_needed)
	{
		/* Calculate how many block is needed*/
		blocks_needed = blocks_needed - blocks_avalible;

		block_offset = g_wl_offset;
		for (i = 0; i < FS_BLOCK_MAX; i++)
		{
			block_offset += i;

			if (block_offset == FS_BLOCK_MAX)
				block_offset = 0;
			/* Read the block header */
			FS_spi_read_blocking(g_block_addr[block_offset], (uint8_t *)&g_fs_header[block_offset], sizeof(fs_image_header_t));
			/* Check if the block should be erased */
			if (g_fs_header[block_offset].block_state == BLOCK_NEED_ERASE)
			{
				/* Erases all the 1MB Block except the first 64K block*/
				for (block_erase_count = 1; block_erase_count < FS_BLOCK_SIZE_64K ; block_erase_count++)
				{
					FS_spi_erase_blocking(g_block_addr[block_offset] + 0x10000*block_erase_count);
				}
				/* Read the block header */
				FS_spi_read_blocking(g_block_addr[block_offset], (uint8_t *)&g_fs_header[block_offset], sizeof(fs_image_header_t));
				/* Read the block end */
				FS_spi_read_blocking(g_block_addr[block_offset] + FS_BLOCK_END, &g_block_sanity, 1);
				/* Checking if the block is totally erased */
				if (g_fs_header[block_offset].valid_mask == 0xFF && g_block_sanity == 0xFF)
				{
					/* Write block header format*/
					memset((uint8_t *)&g_fs_header_format,0xFF,sizeof(fs_image_header_t));
					g_fs_header_format.valid_mask = FS_VALID_MASK;
					g_fs_header_format.block_state = BLOCK_ERASED;
					FS_spi_write_blocking(g_block_addr[block_offset], &g_fs_header_format.valid_mask, 1);
					FS_spi_write_blocking(g_block_addr[block_offset] + offsetof(fs_image_header_t, block_state),\
							(uint8_t *)&g_fs_header_format.block_state, sizeof(uint16_t));
					/* Break the for loop if there is enough space to the incoming file*/
					if(--blocks_needed == 0)
						break;
				}
			}
		}
	}
}

void FS_spi_writeFile(FIL* fp, char * fname)
{
	uint16_t page_index = 0;
	uint8_t cur_block_index = 0;
	uint8_t file_index = 0;
	uint32_t file_size_onblock = 0;
	uint8_t block_sanity_word = 0;
	void *temp = NULL;

	uint32_t remained;
	g_write_addr = 0;
	temp = pvPortMalloc( FS_PAGE_SIZE );

	f_open(fp,fname,FA_READ);

	cur_block_index = fs_find_next_block_erased();

	FS_spi_write_blocking(g_block_addr[cur_block_index] + offsetof(fs_image_header_t, file_block_number),\
				(uint8_t *)&file_index, 1);

	FS_spi_write_blocking(g_block_addr[cur_block_index] + offsetof(fs_image_header_t, filename),\
				(uint8_t *)fname, 32);

	file_index++;

	/* get the block erased address */
	g_write_addr = g_block_addr[cur_block_index];

	page_index = 0;

	while(!f_eof(fp))
	{
		if(page_index == 4095)
		{

			cur_block_index = fs_find_next_block_erased();

			FS_spi_write_blocking(g_block_addr[cur_block_index] + offsetof(fs_image_header_t, file_block_number),\
					(uint8_t *)&file_index, 1);

			file_index++;

			file_size_onblock = page_index*FS_PAGE_SIZE;

			FS_spi_write_blocking(g_block_addr[cur_block_index] + offsetof(fs_image_header_t, file_size_block),\
					(uint8_t *)&file_size_onblock, sizeof (uint32_t));

			/* get the block erased address */
			g_write_addr = g_block_addr[cur_block_index];

			page_index = 0;

		}

		f_read(fp,temp,FS_PAGE_SIZE,(UINT *)&remained);

		FS_spi_write_blocking(g_write_addr + (page_index + 1)*FS_PAGE_SIZE, (uint8_t *)temp, FS_PAGE_SIZE);

		page_index++;
	}

	file_size_onblock = (page_index - 1)*FS_PAGE_SIZE + remained;

	FS_spi_write_blocking(g_block_addr[cur_block_index] + offsetof(fs_image_header_t, file_size_block),\
			(uint8_t *)&file_size_onblock, sizeof (uint32_t));

	block_sanity_word = FS_VALID_MASK;

	g_write_addr = g_block_addr[cur_block_index] + FS_BLOCK_END;

	FS_spi_write_blocking(g_write_addr, &block_sanity_word, 1);

	vPortFree(temp);
}

void FS_spi_readFile(uint8_t * buffer, uint32_t size)
{
	file_block_index = fs_find_file_block(g_file_block);

	g_read_addr = g_block_addr[file_block_index] + FS_PAGE_SIZE + g_file_index;

	FS_spi_read_blocking(g_read_addr, buffer, size);

	g_file_index += size;

	if(g_file_index > (FS_BLOCK_SIZE - FS_PAGE_SIZE))
	{
		g_file_index = 0;
		g_file_block++;
	}
}

bool FS_spi_eof(void)
{
	if(g_file_index > g_fs_header[file_block_index].file_size_block)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void FS_spi_close(void)
{
	g_file_index = 0;
	g_file_block = 0;
}

/*-----------------------------------------------------------------------*/
/* Get a string from the file                                            */
/*-----------------------------------------------------------------------*/

uint8_t* FS_spi_gets (
	uint8_t* buff,	/* Pointer to the string buffer to read */
	int len		/* Size of string buffer (characters) */
)
{
	int n = 0;
	uint8_t c, *p = buff;
	uint8_t s[2];

	while (n < len - 1) {	/* Read characters until buffer gets filled */
		FS_spi_readFile(s, 1);
		if (FS_spi_eof()) break;
		c = s[0];
		if (_USE_STRFUNC == 2 && c == '\r') continue;	/* Strip '\r' */
		*p++ = c;
		n++;
		if (c == '\n') break;		/* Break on EOL */
	}
	*p = 0;
	return n ? buff : 0;			/* When no data read (eof or error), return with error. */
}

void FS_spi_idle(uint32_t size)
{

}

void FS_spi_read_blocking(uint32_t rx_address, uint8_t * rx_buffer, uint32_t rx_bytes)
{
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1)
    {
        /* Make sure SPI flash is not busy */
    }

    /* Read data from external SPI flash */
    R_SF_ReadData( FL_RSPI_CHANNEL,
                   rx_address,
                   rx_buffer,
                   rx_bytes);
}

void FS_spi_write_blocking(uint32_t rx_address, uint8_t * rx_buffer, uint32_t rx_bytes)
{
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1)
    {
        /* Make sure SPI flash is not busy */
    }

    /* Read data from external SPI flash */
    R_SF_WriteData( FL_RSPI_CHANNEL,
                   rx_address,
                   rx_buffer,
                   rx_bytes);
}

void FS_spi_erase_blocking(const uint32_t address)
{
    while((R_SF_ReadStatus(FL_RSPI_CHANNEL) & SF_WIP_BIT_MASK) == 1)
    {
        /* Make sure SPI flash is not busy */
    }

	/* Block erase */
	R_SF_Erase(FL_RSPI_CHANNEL, address, SF_ERASE_BLOCK);

}

void FS_spi_hw_init(void)
{
	R_SF_Init(FL_RSPI_CHANNEL);
}

static uint8_t fs_find_next_block_erased(void)
{
	uint8_t i = 0;
	uint16_t cur_block_state;
	uint8_t block_offset;

	block_offset = g_wl_offset;
	for (i = 0; i < FS_BLOCK_MAX; i++)
	{
			block_offset += i;

			if (block_offset == FS_BLOCK_MAX)
				block_offset = 0;
			/* Read the block header */
			FS_spi_read_blocking(g_block_addr[block_offset], (uint8_t *)&g_fs_header[block_offset], sizeof(fs_image_header_t));
			/* Check if the block is erased */
			if (g_fs_header[block_offset].block_state == BLOCK_ERASED)
			{
				/* Write block state to used*/
				cur_block_state = BLOCK_IN_USE;
				FS_spi_write_blocking(g_block_addr[block_offset] + offsetof(fs_image_header_t, block_state),\
						(uint8_t *)&cur_block_state, 2);

				return block_offset;
			}
	}

	return 0xff;
}

static uint8_t fs_find_file_block(uint8_t fblock)
{
	uint8_t i = 0;
	uint8_t block_offset;

	block_offset = g_wl_offset;
	for (i = 0; i < FS_BLOCK_MAX; i++)
	{
			block_offset += i;

			if (block_offset == FS_BLOCK_MAX)
				block_offset = 0;
			/* Read the block header */
			FS_spi_read_blocking(g_block_addr[block_offset], (uint8_t *)&g_fs_header[block_offset], sizeof(fs_image_header_t));
			/* Check if the block is erased */
			if (g_fs_header[block_offset].block_state == BLOCK_IN_USE)
			{
				if(g_fs_header[block_offset].file_block_number == fblock)
				{
					return block_offset;
				}
			}
	}

	return 0xff;
}
