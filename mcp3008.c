#include "gpio.h"
#include "spi_comm.h"
#include "printf.h"
#include "mcp3008.h"
// Read ADC from MCP3008 (channel 0)


//Spi must be initted to use this function
int mcp3008_read_channel(int channel) {
    uint8_t tx_buf[3] = {0};
    uint8_t rx_buf[3] = {0};

    tx_buf[0] = (8 | channel) << 4;

    spi_transfer_device(tx_buf, rx_buf, 3, MCP3008_MODE);
    
    
    int value = 0; // 10-bit ADC value
    
    for(int i = 0; i < 3; i++){
	value |= rx_buf[i] << (i * 8);
    }
    
    return value;
}
