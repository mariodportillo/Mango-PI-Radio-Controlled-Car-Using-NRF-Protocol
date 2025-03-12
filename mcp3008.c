#include "gpio.h"
#include "spi_comm.h"
#include "printf.h"

// Read ADC from MCP3008 (channel 0)
int mcp3008_read_channel(int channel) {
    spi_init(SPI_MODE_0);
    uint8_t tx_buf[3] = {0};
    uint8_t rx_buf[3];

    tx_buf[0] = (8 | channel);

    spi_transfer_device(tx_buf, rx_buf, 3, MCP3008_MODE);
    
    int value = 0; // 10-bit ADC value
    return value;
}
