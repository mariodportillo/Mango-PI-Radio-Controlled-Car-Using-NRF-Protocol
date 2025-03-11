#include "gpio.h"
#include "spi.h"
#include "spi_comm.h"
#include "printf.h"

void select_nrf24() {
    gpio_write(CSN_NRF24, 0);  // Enable nRF
    gpio_write(CSN_MCP3008, 1); // Disable MCP3008
}

void select_mcp3008() {
    gpio_write(CSN_NRF24, 1);  // Disable nRF
    gpio_write(CSN_MCP3008, 1); // Disable MCP3008
    gpio_write(CSN_MCP3008, 0); // Enable MCP3008
}

void deselect_all() {
    gpio_write(CSN_NRF24, 1);
    gpio_write(CSN_MCP3008, 1);
}

// NRF is mode 1
// MCP is mode 0

void spi_transfer_device(uint8_t *tx, uint8_t *rx, int len, unsigned int mode){
    if(mode > 1 || mode < 0){
         printf("INCORRECT mode. Can only choose a mode between 0 and 1");
	 return;
    }

    if(mode){
	    select_nrf24();
    	spi_transfer(tx, rx, len);
        deselect_all();
    }else if(mode == 0){
       select_mcp3008();
       spi_transfer(tx, rx, len); 
       deselect_all();
    }

}