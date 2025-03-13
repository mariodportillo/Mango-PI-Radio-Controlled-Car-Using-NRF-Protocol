#include "gpio.h"
#include "spi_comm.h"
#include "printf.h"
#include "mcp3008.h"
// Read ADC from MCP3008 (channel 0)

void mcp3008_config(){
    gpio_set_output(CSN_MCP3008);
}

//Spi must be initted to use this function
int mcp3008_read_channel(int channel) {
    uint8_t tx_buf[3] = {0};
    uint8_t rx_buf[3] = {0};

    tx_buf[0] = (8 | channel) << 4;

    spi_transfer_device(tx_buf, rx_buf, 3, MCP3008_MODE);
    
    
    int value = ((rx_buf[1] & 3) << 8) | rx_buf[2]; // 10-bit ADC value
    
    // for(int i = 0; i < 3; i++){
	// value |= rx_buf[i] << (i * 8);
    // }
    
    return value;
}
