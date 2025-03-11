/* File: test_receiver.c
 * ---------------------------
 * ***** Compile and run this code on the receiver  *****
 */

 #include "nrf.h"
 #include "uart.h"
 #include "printf.h"
 #include "gpio.h"
 #include "strings.h"
 
 uint8_t rx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
 uint8_t rx_data[32];
 
 void test_receiver(){
     nrf24_set_rx_mode(rx_address, 10);
     printf("Waiting for data...\n");
 
     while (1) {
         if (is_data_available(1)) {
             nrf24_receive(rx_data);
             printf("Received: %s\n", rx_data);
 
             // check received command and toggle LED
             if (strcmp((char *)rx_data, "ON") == 0) {
                 gpio_write(GPIO_PB2, 1);  // turn ON
                 printf("LED ON\n");
             } else if (strcmp((char *)rx_data, "OFF") == 0) {
                 gpio_write(GPIO_PB2, 0);  // turn OFF
                 printf("LED OFF\n");
             }
         }
     }
 }
 
 void main(void){
     uart_init();
     nrf24_init();
     gpio_set_output(GPIO_PB2);  
     nrf24_set_rx_mode(rx_address, 10);
     test_receiver();
 }
 