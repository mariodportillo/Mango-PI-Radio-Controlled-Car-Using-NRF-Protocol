#include <stdint.h>
#include "gpio.h"
#include "nrf.h"
#include "spi.h"
#include "strings.h"
#include "printf.h"
#include "timer.h"

// define the chip enable to enable the NRF device
#define NRF24_CE_PIN GPIO_PD15
// chip select pin for SPI 
#define NRF_24_CSN_PIN GPIO_PD10


void enable_nrf_device(){
    gpio_write(NRF24_CE_PIN, 1);
}

// The NRF device must be in the disabled mode for user to configure the registers
void disable_nrf_device(){
    gpio_write(NRF24_CE_PIN, 0);
}

void select_nrf_device(){
    gpio_write(NRF_24_CSN_PIN, 0);
}

void deselect_nrf_device(){
    gpio_write(NRF_24_CSN_PIN, 1);
}


void nrf24_write_byte_register(uint8_t reg, uint8_t data){
    
    /* To write to a register of address A AAAA
    *  we send 001A AAAA to activate W_REGISTER command
    *  The byte that follows immediately is written in the register address
    */
   uint8_t tx_buf[2] = {reg | (1 << 5), data};
   uint8_t rx_buf[2] = {0};
    //select_nrf_device();
    // NOTE: spi_transfer already selects the device by pulling down CSN low before communication
    spi_transfer(tx_buf, rx_buf, 2);    // sends 2 bytes
    //deselect_nrf_device();
    
    // TODO: for debugging only. Pls remove
    //printf("status byte: %x\n", rx_buf[0]);

   
}

void nrf24_write_register(uint8_t reg, uint8_t *data, int size) {

    uint8_t tx_buf[1 + size]; // Command byte + data bytes
    uint8_t rx_buf[1 + size]; // buffer for RX (for debugging purposes)

    // W_REGISTER command must be sent first
    tx_buf[0] = reg | (1 << 5); 
    // Followed by the data
    memcpy(&tx_buf[1], data, size); 
    
    spi_transfer(tx_buf, rx_buf, 1 + size); 
    
}




void nrf24_read_register(uint8_t reg, uint8_t *data, int size){
    uint8_t tx_buf[1 + size];  // First byte: command, rest: dummy bytes for reading
    uint8_t rx_buf[1 + size];  


    // R_REGISTER command (001A AAAA)  
    tx_buf[0] = reg & 0x1F;
    memset(&tx_buf[1], 0xFF, size); // fill with dummy bytes to clock out data

    spi_transfer(tx_buf, rx_buf, 1 + size); 
    // array of received data (ignores the status byte)
    memcpy(data, &rx_buf[1], size);  
}


void nrf24_send_cmd(uint8_t cmd){
    spi_transfer(&cmd, NULL, 1);
}


// This configuration will remain common for both TX/RX modes
void nrf24_init(){
    spi_init(SPI_MODE_0);  // Configure SPI for nRF24L01
    disable_nrf_device();  // Disable NRF before configuration


    nrf24_write_byte_register(CONFIG, 0);   // will be configured later

    nrf24_write_byte_register(EN_AA, 0);   // no auto-acknowledgement

    nrf24_write_byte_register(EN_RXADDR, 0);   // not enabling any data pipe right now

    nrf24_write_byte_register(SETUP_AW, 0x03);   // 5 bytes for TX/RX address (only lowest 2 bits modified)
    
    // when the transmitter does not receieve Auto-ACK, it can retransmit
    nrf24_write_byte_register(SETUP_RETR, 0);   // no retransmission
    // Channel selection
    nrf24_write_byte_register(RF_CH, 0);   // will be setup during TX/RX

    // Setting transmission rate and power
    /*
    *   RF DATA RATE = 2 Mbps,  RF POWER = 0 dB
    *   NOTE: DO NOT MODIFY THE FIRST BIT, ONLY THE 2-4
    */
    nrf24_write_byte_register(RF_SETUP, 0x0E); 

    enable_nrf_device();    // Enable the NRF device after configuration

}



/* Configure as a transmitter
*  parameters: Address of the receiver pipe (this is configured to be 5 bytes), channel number
*/
void nrf24_set_tx_mode(uint8_t *address, uint8_t channel){
    disable_nrf_device();  // Disable NRF before configuration

    // select the channel
    nrf24_write_byte_register(RF_CH, channel);

    // write the TX address
    nrf24_write_register(TX_ADDR, address, 5);

    // power up the device
    uint8_t config;
    nrf24_read_register(CONFIG, &config, 1);
    config = config | (1 << 1);
    nrf24_write_byte_register(CONFIG, config);

    enable_nrf_device();

}

uint8_t nrf24_transmit(uint8_t *data){
    uint8_t tx_buf[1 + 32]; // command + 32-byte payload
    tx_buf[0] = W_TX_PAYLOAD;
    memcpy(&tx_buf[1], data, 32); 

    spi_transfer(tx_buf, NULL, 1 + 32); 
    
    // Testing the transmit status
    timer_delay_ms(1);

    uint8_t fifo_status = 0;
    nrf24_read_register(FIFO_STATUS, &fifo_status, 1);
    // If transmission is successful, TX FIFO must be empty
    if ((fifo_status & (1 << 4)) && !(fifo_status & (1 << 3))){
        nrf24_send_cmd(FLUSH_TX);
        return 1;
    }
    // Failed to transmit
    return 0;   
}


 // Configure as a receiver
void nrf24_set_rx_mode(){

}