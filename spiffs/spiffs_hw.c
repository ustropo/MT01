/*
 * spiffs_hw.c
 *
 *  Created on: Jul 25, 2016
 *      Author: LAfonso01
 */
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fsystem_spi.h"
#include "spiffs.h"
#include "r_spi_flash_if.h"
#include "spiffs_hw.h"
#include "xio.h"
#include "spiflash.h"

#define LOG_PAGE_SIZE       256

static u8_t spiffs_work_buf[LOG_PAGE_SIZE*2];
static u8_t spiffs_fds[32*4];
static u8_t spiffs_cache_buf[(LOG_PAGE_SIZE+32)*4];

static spiffs *fs = (spiffs *)&uspiffs[0].gSPIFFS;
static spiffs_config cfg;



static void SPI_init(void);
static bool RSPI1_Write( const uint8_t *pSrc, uint16_t usBytes);
static bool RSPI1_Read( uint8_t *pDest,uint16_t usBytes);
static bool RSPI1_rx_buffer_full (void);

static int impl_spiflash_spi_txrx(spiflash_t *spi, const uint8_t *tx_data,
      uint32_t tx_len, uint8_t *rx_data, uint32_t rx_len);

static void impl_spiflash_spi_cs(spiflash_t *spi, uint8_t cs);

static void impl_spiflash_wait(spiflash_t *spi, uint32_t ms);

static const spiflash_hal_t my_spiflash_hal = {
  ._spiflash_spi_txrx = impl_spiflash_spi_txrx,
  ._spiflash_spi_cs = impl_spiflash_spi_cs,
  ._spiflash_wait = impl_spiflash_wait
};

//static s32_t my_spiffs_read(u32_t addr, u32_t size, u8_t *dst) ;
//static s32_t my_spiffs_write(u32_t addr, u32_t size, u8_t *src);
//static s32_t my_spiffs_erase(u32_t addr, u32_t size);

const spiflash_cmd_tbl_t my_spiflash_cmds = { \
	    .write_disable = 0x04, \
	    .write_enable = 0x06, \
	    .page_program = 0x02, \
	    .read_data = 0x03, \
	    .read_data_fast = 0x0b, \
	    .write_sr = 0x01, \
	    .read_sr = 0x05, \
		.clear_sr = 0x30, \
	    .block_erase_4 = 0x20, \
	    .block_erase_8 = 0x00, \
	    .block_erase_16 = 0x00, \
	    .block_erase_32 = 0x52, \
	    .block_erase_64 = 0xd8, \
	    .chip_erase = 0xc7, \
	    .device_id = 0x90, \
	    .jedec_id = 0x9f, \
	    .sr_busy_bit = 0x01, \
	  };

const spiflash_config_t my_spiflash_config = {
  .sz = 1024*1024*8, // e.g. for a 2 MB flash
  .page_sz = 256, // normally 256 byte pages
  .addr_sz = 3, // normally 3 byte addressing
  .addr_dummy_sz = 0, // using single line data, not quad or something
  .addr_endian = SPIFLASH_ENDIANNESS_BIG, // normally big endianess on addressing
  .sr_write_ms = 10,
  .page_program_ms = 2,
  .block_erase_4_ms = 0,
  .block_erase_8_ms = 0, // not supported
  .block_erase_16_ms = 0, // not supported
  .block_erase_32_ms = 0,
  .block_erase_64_ms = 130,
  .chip_erase_ms = 35000
};

spiflash_t spif;

s32_t spiffs_init(void)
{
	int res;

	SPI_init();
	SPIFLASH_init(&spif,
			&my_spiflash_config,
			&my_spiflash_cmds,
			&my_spiflash_hal,
			0,
			SPIFLASH_SYNCHRONOUS,
			NULL);


	cfg.hal_read_f = spi_mem_read;
	cfg.hal_write_f = spi_mem_write;
	cfg.hal_erase_f = spi_mem_erase;

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

s32_t spi_mem_read(u32_t addr, u32_t size, u8_t *dst) {
	SPIFLASH_read(&spif,addr, size, dst);
  return SPIFFS_OK;
}

s32_t spi_mem_write(u32_t addr, u32_t size, u8_t *src) {
	SPIFLASH_write(&spif,addr, size, src);
  return SPIFFS_OK;
}

s32_t spi_mem_erase(u32_t addr, u32_t size) {
	SPIFLASH_erase(&spif,addr, size);
  return SPIFFS_OK;
}

static int impl_spiflash_spi_txrx(spiflash_t *spi, const uint8_t *tx_data,
      uint32_t tx_len, uint8_t *rx_data, uint32_t rx_len) {
  int res = SPIFLASH_OK;
  if (tx_len > 0) {
    // first transmit tx_len bytes from tx_data if needed
    res = RSPI1_Write(tx_data, tx_len);
  }

  if (res == SPIFLASH_OK && rx_len > 0) {
    // then receive rx_len bytes into rx_data if needed
    res = RSPI1_Read(rx_data, rx_len);
  }

  return res;
}

static void impl_spiflash_spi_cs(spiflash_t *spi, uint8_t cs) {
  if (cs) {
    // assert cs pin
	SPIFLASH_CS = 0;
  } else {
    // de assert cs pin
	SPIFLASH_CS = 1;
  }
}

static void impl_spiflash_wait(spiflash_t *spi, uint32_t ms) {
  vTaskDelay(ms/portTICK_PERIOD_MS);
}

static void SPI_init(void)
{
bool lock;
    /* Initialize peripherals used for talking to SPI flash */
	//R_RSPI_Open(channel, &spiflash_config, spiflash_callback, &spiflash_handle);
	/* Get the looking */
    if(R_BSP_HardwareLock((mcu_lock_t)(BSP_LOCK_RSPI1)))
    {
    	SPIFLASH_CS = 1;
		R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
		MSTP(RSPI1) = 0; /* Init peripheral */
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);
        IPR(RSPI1, SPRI1) = 3;
        IEN(RSPI1, SPRI1) = 0;
        IEN(RSPI1, SPTI1) = 0;
        IR(RSPI1, SPRI1) = 0 ;

        RSPI1.SPCR.BYTE = 0x00; /*Clock synchronous operation; Master mode*/
        /* Set RSPI bit rate (SPBR) */
        /* -Set baud rate to 24Mbps (48MHz / (2 * (0 + 1) * 2^0) ) = 24Mbps */
        RSPI1.SPBR = 0;

        /* Set RSPI data control register (SPDCR) */
        /* -SPDR is accessed in longwords (32 bits)
           -Transfer 1 frame at a time */
        RSPI1.SPDCR.BYTE = 0x20;

        /* Set RSPI control register 2 (SPCR2) */
        /* -Disable Idle interrupt */
        RSPI1.SPCR2.BYTE = 0x00;

        /* Set RSPI command register 0 (SPCMD0) */
        /* -MSB first
           -8 bits data length
           -SSL0 (handled manually)
           -Use bit rate % 1
            */
        RSPI1.SPCMD0.WORD = 0x0400;

        /* Set RSPI control register (SPCR) */
        /* -Clock synchronous operation (3-wire)
           -Full duplex operation
           -Master mode
           -SPTI and SPRI enabled in RSPI (have to check ICU also)
           -Enable RSPI function */
        RSPI1.SPCR.BYTE = 0xE9;
    }
}

/***********************************************************************************************************************
* Function Name: R_RSPI_Read
* Description  : Reads data using RSPI
* Arguments    : channel -
*                    Which channel to use
*                pDest -
*                    Pointer to location to put the received data.
*                    Returned value will be incremented by number of bytes received.
*                usBytes -
*                    number of bytes to be received
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
static bool RSPI1_Read( uint8_t *pDest,
                 uint16_t usBytes)
{
    uint16_t byte_count;
    volatile uint32_t temp;

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while (RSPI1.SPSR.BIT.IDLNF) ;

        /* If just reading then transmit 0xFF */
        RSPI1.SPDR.LONG = 0xFFFFFFFF ;

        while (false == RSPI1_rx_buffer_full())
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }

        /* Read received data.  If transmit only, then ignore it */
        pDest[byte_count] = (uint8_t) (RSPI1.SPDR.LONG & 0xFF);
    }

    return SPIFLASH_OK;
}


/***********************************************************************************************************************
* Function Name: R_RSPI_Write
* Description  : Write to a SPI device
* Arguments    : channel -
*                    Which channel to use
*                pSrc -
*                    Pointer to data buffer with data to be transmitted.
*                    Returned value will be incremented by number of attempted writes.
*                usBytes -
*                    Number of bytes to be sent
*                pid -
*                    Unique task ID. Used to make sure tasks don't step on each other.
* Return Value : true -
*                    Operation completed.
*                false -
*                    This task did lock the RSPI fist.
***********************************************************************************************************************/
static bool RSPI1_Write( const uint8_t *pSrc, uint16_t usBytes)
{
    uint16_t byte_count;
    volatile uint32_t temp;

    for (byte_count = 0; byte_count < usBytes; byte_count++)
    {
        /* Ensure transmit register is empty */
        while (RSPI1.SPSR.BIT.IDLNF) ;

        /* If just reading then transmit 0xFF */
        RSPI1.SPDR.LONG = pSrc[byte_count];

        while (false == RSPI1_rx_buffer_full())
        {
            /* Transfer is complete when a byte has been shifted in (full duplex) */
        }

        /* Read received data.  If transmit only, then ignore it */
        temp = RSPI1.SPDR.LONG;
    }
    return SPIFLASH_OK;
}

/***********************************************************************************************************************
* Function Name: rspi_rx_buffer_full
* Description  : Returns whether the receive buffer full flag is set for a RSPI channel. Clear flag after read.
* Arguments    : channel -
*                    Which channel to use.
* Return Value : true -
*                    Flag is set.
*                false -
*                    Flag is not set.
***********************************************************************************************************************/
static bool RSPI1_rx_buffer_full (void)
{
    bool flag_set = false;

        if (1 == IR(RSPI1, SPRI1))
        {
            /* Clear bit. */
            IR(RSPI1, SPRI1) = 0;

            flag_set = true;
        }
    return flag_set;
}
