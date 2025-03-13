#include <stdint.h>
#include "gpio.h"
#include "nrf.h"
#include "spi.h"
#include "strings.h"
#include "printf.h"
#include "timer.h"
#include "spi_comm.h"

// define the chip enable to enable the NRF device
#define NRF24_CE_PIN GPIO_PD15


void enable_nrf_device(){
    gpio_write(NRF24_CE_PIN, 1);
}

// The NRF device must be in the disabled mode for user to configure the registers
void disable_nrf_device(){
    gpio_write(NRF24_CE_PIN, 0);
}

void nrf24_write_byte_register(uint8_t reg, uint8_t data){
    
    /* To write to a register of address A AAAA
    *  we send 001A AAAA to activate W_REGISTER command
    *  The byte that follows immediately is written in the register address
    */
   uint8_t tx_buf[2] = {reg | W_REGISTER, data};
   uint8_t rx_buf[2] = {0};
    //select_nrf_device();
    // NOTE: spi_transfer already selects the device by pulling down CSN low before communication

    spi_transfer_device(tx_buf, rx_buf, 2, NRF_MODE);    // sends 2 bytes
    //deselect_nrf_device();
    
    // TODO: for debugging only. Pls remove
    //printf("status byte: %x\n", rx_buf[0]);

   
}

void nrf24_write_register(uint8_t reg, uint8_t *data, int size) {

    uint8_t tx_buf[1 + size]; // Command byte + data bytes
    uint8_t rx_buf[1 + size]; // buffer for RX (for debugging purposes)

    // W_REGISTER command must be sent first
    tx_buf[0] = reg | W_REGISTER; 
    // Followed by the data
    memcpy(&tx_buf[1], data, size); 
    
    spi_transfer_device(tx_buf, rx_buf, 1 + size, NRF_MODE); 
    
}




void nrf24_read_register(uint8_t reg, uint8_t *data, int size){
    uint8_t tx_buf[1 + size];  // First byte: command, rest: dummy bytes for reading
    uint8_t rx_buf[1 + size];  


    // R_REGISTER command (000A AAAA)  
    tx_buf[0] = reg | R_REGISTER;
    memset(&tx_buf[1], 0xFF, size); // fill with dummy bytes to clock out data

    spi_transfer_device(tx_buf, rx_buf, 1 + size, NRF_MODE); 
    // array of received data (ignores the status byte)
    memcpy(data, &rx_buf[1], size);  
}


void nrf24_send_cmd(uint8_t cmd){
    spi_transfer_device(&cmd, NULL, 1, NRF_MODE);
}


// This configuration will remain common for both TX/RX modes
void nrf24_init(){
    spi_init(SPI_MODE_0);  // Configure SPI for nRF24L01
    disable_nrf_device();  // Disable NRF before configuration

    gpio_set_output(CSN_NRF24); // set the Chip select as output

    nrf24_reset(0);         // reset all the registers before configuration
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
    uint8_t config = 0;  
    nrf24_read_register(CONFIG, &config, 1);
    config |= (1 << 1);  // set PWR_UP bit
    config &= ~(1 << 0); // ensure PRIM_RX is 0 for TX mode
    nrf24_write_byte_register(CONFIG, config);
    enable_nrf_device();
}


uint8_t nrf24_transmit(const uint8_t *data){
    uint8_t tx_buf[1 + 32]; // command + 32-byte payload
    tx_buf[0] = W_TX_PAYLOAD;
    memcpy(&tx_buf[1], data, 32); 

    spi_transfer_device(tx_buf, NULL, 1 + 32, NRF_MODE); 
    
    // Testing the transmit status
    timer_delay_ms(1);

    uint8_t fifo_status = 0;
    nrf24_read_register(FIFO_STATUS, &fifo_status, 1);
    // If transmission is successful, TX FIFO must be empty
    if ((fifo_status & (1 << 4)) && !(fifo_status & (1 << 3))){
        nrf24_send_cmd(FLUSH_TX);

        nrf24_reset(FIFO_STATUS);
        disable_nrf_device();       // disable device to go Standby-1
        return 1;
    }
    // Failed to transmit
    return 0;   
}


 // Configure as a receiver
 // we enable RX data pipe 1 to receive data
void nrf24_set_rx_mode(uint8_t *address, uint8_t channel){
    disable_nrf_device();  // Disable NRF before configuration

    // select the channel
    nrf24_write_byte_register(RF_CH, channel);
    
    // select data pipe 1
    uint8_t en_rxaddr = 0;
    nrf24_read_register(EN_RXADDR, &en_rxaddr, 1);
    en_rxaddr |= (1 << 1);
    nrf24_write_byte_register(EN_RXADDR, en_rxaddr);   

    // set the address for the data pipe 1
    nrf24_write_register(RX_ADDR_P1, address, 5);
   
    // set the data size for the data pipe: set to max 32 bytes
    // Note: we don't use dynamic payload, the receiver will always receieve 32 bytes
    nrf24_write_byte_register(RX_PW_P1, 32); 

    // power up the device in RX mode
    uint8_t config = 0;  
    nrf24_read_register(CONFIG, &config, 1);
    config |= (1 << 1);  // set PWR_UP bit
    config |= (1 << 0); // ensure PRIM_RX is 1 for RX mode
    nrf24_write_byte_register(CONFIG, config);
    enable_nrf_device();
}


// before we receive data, we need to check if data is available in the FIFO
uint8_t is_data_available(int pipenum){
    // check the status register
    uint8_t status = 0;
    nrf24_read_register(STATUS, &status, 1);
    // Bit number 6 is the RX DR: sets when a new Data arrives in the RX FIFO
    // We must also check if the given pipe number. The RX pipe number bit starts from bit 1
    // Therefore, we must shift the given pipe number by one bit
    if ((status & (1 << 6)) && (status & (pipenum << 1))){
        // if data available, we clear RX DR bit by writing a 1
        nrf24_write_byte_register(STATUS, (1 << 6));
        return 1;   // success
    }
    return 0;   // data not received
}


// this function is used to copy data from the pipe to our buffer
void nrf24_receive(uint8_t *data){
    uint8_t tx_buf[1 + 32]; // command + 32-byte payload
    uint8_t rx_buf[1 + 32];
    // inform the NRF that we want to receive data from the data pipe
    tx_buf[0] = R_RX_PAYLOAD;
    memset(&tx_buf[1], 0xFF, 32); 

    spi_transfer_device(tx_buf, rx_buf, 1 + 32, NRF_MODE); 
    memcpy(data, &rx_buf[1], 32);
    // Testing the transmit status
    timer_delay_ms(1);

    // flush the RX FIFO after data has been received
    nrf24_send_cmd(FLUSH_RX);
}


void nrf24_reset(uint8_t reg)
{
	if (reg == STATUS)
	{
		nrf24_write_byte_register(STATUS, 0x00);
	}

	else if (reg == FIFO_STATUS)
	{
		nrf24_write_byte_register(FIFO_STATUS, 0x11);
	}

	else {
    nrf24_write_byte_register(CONFIG, 0x08);
	nrf24_write_byte_register(EN_AA, 0x3F);
	nrf24_write_byte_register(EN_RXADDR, 0x03);
	nrf24_write_byte_register(SETUP_AW, 0x03);
	nrf24_write_byte_register(SETUP_RETR, 0x03);
	nrf24_write_byte_register(RF_CH, 0x02);
	nrf24_write_byte_register(RF_SETUP, 0x0E);
	nrf24_write_byte_register(STATUS, 0x00);
	nrf24_write_byte_register(OBSERVE_TX, 0x00);
	nrf24_write_byte_register(CD, 0x00);
	uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_write_register(RX_ADDR_P0, rx_addr_p0_def, 5);
	uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	nrf24_write_register(RX_ADDR_P1, rx_addr_p1_def, 5);
	nrf24_write_byte_register(RX_ADDR_P2, 0xC3);
	nrf24_write_byte_register(RX_ADDR_P3, 0xC4);
	nrf24_write_byte_register(RX_ADDR_P4, 0xC5);
	nrf24_write_byte_register(RX_ADDR_P5, 0xC6);
	uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_write_register(TX_ADDR, tx_addr_def, 5);
	nrf24_write_byte_register(RX_PW_P0, 0);
	nrf24_write_byte_register(RX_PW_P1, 0);
    nrf24_write_byte_register(RX_PW_P2, 0);
	nrf24_write_byte_register(RX_PW_P3, 0);
	nrf24_write_byte_register(RX_PW_P4, 0);
	nrf24_write_byte_register(RX_PW_P5, 0);
	nrf24_write_byte_register(FIFO_STATUS, 0x11);
	nrf24_write_byte_register(DYNDPD, 0);
	nrf24_write_byte_register(FEATURE, 0);
	}
}
