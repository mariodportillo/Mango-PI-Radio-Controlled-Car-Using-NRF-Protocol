#include "gpio.h"
#include "spi_comm.h"
#include "mcp3008.h"
#include "spi.h"
// // Read ADC from MCP3008 (channel 0)



void mcp3008_init(void)  {
    spi_init(SPI_MODE_0);
    gpio_set_output(CSN_MCP3008);
}

unsigned int mcp3008_read_channel(int channel) {
    uint8_t tx[3], rx[3];

    // "Start bit", wakes up the ADC chip. 
    tx[0] = 1;
    // "Configuration byte", single mode + channel
    // valid channels 0-7
    tx[1] = 0x80 | ((channel & 0x7) << 4);
    tx[2] = 0;

    spi_transfer_device(tx, rx, sizeof(tx), MCP3008_MODE);
    // reading is 10-bit (0-1023)
    return ((rx[1] & 0x3) << 8) + rx[2];
}