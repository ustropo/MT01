/*
 * fsystem_spi.h
 *
 *  Created on: Jul 13, 2016
 *      Author: LAfonso01
 */

#ifndef FSYSTEM_SPI_H_
#define FSYSTEM_SPI_H_

#include "ff.h"

/* SPI Flash FS page size */
#define FS_PAGE_SIZE              	 (256)
/* SPI Flash FS block size */
#define FS_BLOCK_SIZE              	 (1024*1024)
/* fs_image_header_t size in byte */
#define FS_IMAGE_BYTE_SIZE 			  (42)
/* fs_image_header_t size in byte */
#define FS_NEED_ERASE 			  	  (0xA5)


typedef enum{
	BLOCK_IN_USE = 0x0000,
	BLOCK_ERASED = 0xAAAA,
	BLOCK_NEED_ERASE = 0xFFFF,
}fs_block_state_t;

/* Structure of FlashLoader Load Image Header */
typedef struct{
		/* To confirm valid header */
		uint8_t     valid_mask;
		/* File name */
		uint8_t		filename[32];
		/* Next Block available */
		uint16_t    block_state;
		/* Enumeration of the file*/
		uint8_t    file_block_number;
		/* File size on the block */
		uint32_t    file_size_block;
		/* CRC-16 CCITT of image as in MCU flash */
		uint16_t    raw_crc;
}fs_image_header_t;

void FS_spi_init(void);
void FS_spi_start_block(uint32_t fsize);
void FS_spi_writeFile(FIL* fp, char * fname);
void FS_spi_readFile(uint8_t * buffer, uint32_t size);
uint8_t* FS_spi_gets (uint8_t* buff, int len);
bool FS_spi_eof(void);
void FS_spi_close(void);

void FS_spi_read_blocking(uint32_t rx_address, uint8_t * rx_buffer, uint32_t rx_bytes);
void FS_spi_write_blocking(uint32_t rx_address, uint8_t * rx_buffer, uint32_t rx_bytes);
void FS_spi_erase_blocking(const uint32_t address);
void FS_spi_hw_init(void);

#endif /* FSYSTEM_SPI_H_ */
