
/* File: test_nrf.c
 * ---------------------------
 * ***** This file contains code for testing functions implemented nrf module  *****
 */
#include "nrf.h"
#include "uart.h"
#include "printf.h"
/*
*  Address of the receive data pipe. 5 bytes wide.
*  Must be passed as the MSB first.  Address saved in NRF 0xAABBCCDDEE
*/ 
uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

void test_read_register(){
    uint8_t config_value = 0;
    nrf24_read_register(SETUP_AW, &config_value, 1);
    printf("SETUP_AW register: 0x%x\n", config_value);
}


void main(void){
    uart_init();
    nrf24_init();
    test_read_register();
}
