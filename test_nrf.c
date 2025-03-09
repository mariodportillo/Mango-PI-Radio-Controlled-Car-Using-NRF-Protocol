
/* File: test_nrf.c
 * ---------------------------
 * ***** This file contains code for testing functions implemented nrf module  *****
 */
#include "nrf.h"
#include "uart.h"
#include "printf.h"
#include "assert.h"
#include "timer.h"
/*
*  Address of the receive data pipe. 5 bytes wide.
*  Must be passed as the MSB first.  Address saved in NRF 0xAABBCCDDEE
*/ 
uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t tx_data[] = "Hello World\n";


void test_transmission(){
    nrf24_set_tx_mode(tx_address, 10);
    printf("Press 'q' to stop transmission.\n");

    while (1) {  // Infinite loop for continuous transmission
        if (nrf24_transmit(tx_data)) {
            printf("Transmission successful!\n");
        } else {
            printf("Transmission failed, retrying...\n");
        }
        timer_delay_ms(1000); // Wait before retrying
    
        // Check for a key press
        if (uart_haschar()) {  
            char c = uart_getchar();  
            if (c == 'q') {  
                printf("Transmission stopped by user.\n");
                break;  
            }
        }
    }    
}


void test_read_write(){
// nrf24_write_byte_register(uint8_t reg, uint8_t data)
// nrf24_write_register(uint8_t reg, uint8_t *data, int size)
    uint8_t writeData = 0x12;
    nrf24_write_byte_register(RX_ADDR_P0, writeData);
    uint8_t result[1];

    nrf24_read_register(RX_ADDR_P0, result, 1);
    assert(writeData = result[0]);
    printf("RX_ADDR_P0 currently stores: 0x%x \n", result[0]);

    writeData = 0x0;
    nrf24_write_byte_register(RX_ADDR_P1, writeData);
    
    nrf24_read_register(RX_ADDR_P1, result, 1);
    printf("This is what result1[0] is: 0x%x \n", result[0]);
    assert(writeData == result[0]);

    printf("RX_ADDR_P1 currently stores: 0x%x \n", result[0]);

}


void test_read_init(){
    uint8_t config_value_CONFIG = 0;

    nrf24_read_register(CONFIG, &config_value_CONFIG, 1);
    assert(config_value_CONFIG == 0);
    printf("CONFIG register: 0x%x\n", config_value_CONFIG);
    
    uint8_t config_value_EN_AA = 0;

    nrf24_read_register(EN_AA, &config_value_EN_AA, 1);
    assert(config_value_EN_AA == 0);
    printf("EN_AA register: 0x%x\n", config_value_EN_AA);
    
    uint8_t config_value_EN_RXADDR = 0;

    nrf24_read_register(EN_RXADDR, &config_value_EN_RXADDR, 1);
    assert(config_value_EN_RXADDR == 0);
    printf("EN_RXADDR register: 0x%x\n", config_value_EN_RXADDR);
    
    uint8_t config_value_SETUP_AW = 0;

    nrf24_read_register(SETUP_AW, &config_value_SETUP_AW, 1);
    assert(config_value_SETUP_AW == 0x03);
    printf("SETUP_AW register: 0x%x\n", config_value_SETUP_AW);

    uint8_t config_value_SETUP_RETR = 0;

    nrf24_read_register(SETUP_RETR, &config_value_SETUP_RETR, 1);
    assert(config_value_SETUP_RETR== 0);
    printf("SETUP_RETR register: 0x%x\n", config_value_SETUP_RETR);
    
    uint8_t config_value_RF_CH = 0;

    nrf24_read_register(RF_CH, &config_value_RF_CH, 1);
    assert(config_value_RF_CH == 0);
    printf("RF_CH register: 0x%x\n", config_value_RF_CH);

    uint8_t config_value_RF_SETUP = 0;

    nrf24_read_register(RF_SETUP, &config_value_RF_SETUP, 1);
    assert(config_value_RF_SETUP == 0x0E);
    printf("RF_SETUP register: 0x%x\n", config_value_RF_SETUP);

}

void test_read_register(){
    uint8_t config_value = 0;
    nrf24_read_register(SETUP_AW, &config_value, 1);
    printf("SETUP_AW register: 0x%x\n", config_value);
}

void test_spi(){
    uint8_t config = 0;
    nrf24_read_register(CONFIG, &config, 1);
    printf("CONFIG register: 0x%x\n", config);
}

void main(void){
    uart_init();
    nrf24_init();
    test_read_write();
    test_read_init();
    test_transmission();
    test_spi();
    test_read_register();
}
