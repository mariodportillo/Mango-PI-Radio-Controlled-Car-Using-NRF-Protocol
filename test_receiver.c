/* File: test_receiver.c
 * ---------------------------
 * ***** Compile and run this code on the receiver  *****
 */

 #include "nrf.h"
 #include "uart.h"
 #include "printf.h"


uint8_t rx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t rx_data[32];

void test_receiver(){
    nrf24_set_rx_mode(rx_address, 10);
    printf("Waiting for data...\n");

    while(1){
        if (is_data_available(1)){
            nrf24_receive(rx_data);
            printf("Received: %s\n", rx_data);  // Print received data
        }
    }
}


void main(void){
    uart_init();
    nrf24_init();
    nrf24_set_rx_mode(rx_address, 10);
    test_receiver();

}
