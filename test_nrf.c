
/* File: test_nrf.c
 * ---------------------------
 * ***** This file contains code for testing functions implemented nrf module  *****
 */
#include "nrf.h"
#include "uart.h"
#include "printf.h"
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
    test_transmission();
    test_spi();
    test_read_register();
}
