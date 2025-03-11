#ifndef SPI_COMM_H
#define SPI_COMM_H

#define CSN_NRF24    GPIO_PD10   // CS pin for nRF24L01
#define CSN_MCP3008  GPIO_PD9    // CS pin for MCP3008
#define MCP3008_MODE 0
#define NRF_MODE 1


void select_nrf24();

void select_mcp3008();

void deselect_all();

void spi_transfer_device(uint8_t *tx, uint8_t *rx, int len, unsigned int mode);

#endif