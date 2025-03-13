#ifndef SPI_COMM_H
#define SPI_COMM_H

#define CSN_NRF24    GPIO_PD10   // CS pin for nRF24L01
#define CSN_MCP3008  GPIO_PD17   // CS pin for MCP3008
#include <stdint.h>

typedef enum {
    MCP3008_MODE = 0,
    NRF_MODE, 
} spi_mode;

void select_nrf24();

void select_mcp3008();

void deselect_all();

void spi_transfer_device(uint8_t *tx, uint8_t *rx, int len, spi_mode mode);

#endif
