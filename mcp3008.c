#include "gpio.h"
#include "spi.h"

#define CSN_NRF24    GPIO_PD10   // CS pin for nRF24L01
#define CSN_MCP3008  GPIO_PD9    // CS pin for MCP3008


// Read ADC from MCP3008 (channel 0)
int mcp3008_read_channel(int channel) {
    spi_init(SPI_MODE_0);
    uint8_t tx_buf[3] = {0};
    uint8_t rx_buf[3];

    tx_buf[0] = (8 | channel);


    select_mcp3008();
    spi_transfer(tx_buf, rx_buf, 3);
    deselect_all();

    int value = 0; // 10-bit ADC value
    return value;
}
