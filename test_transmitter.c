/* File: test_transmitter.c
 * ---------------------------
 * ***** Compile and run this code on the transmitter  *****
 */
#include "nrf.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"

uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t tx_on[] = "ON";
uint8_t tx_off[] = "OFF";
int led_state = 0; // track LED state

void test_transmission(){
    printf("Press 'f' to toggle LED\n");
    
    while (1) {
        if (uart_haschar()) {  
            char c = uart_getchar();  
            if (c == 'q') {  
                printf("Transmission stopped by user.\n");
                break;  
            } else if (c == 'f') {  
                // toggle LED state
                led_state = !led_state;
<<<<<<< HEAD
                uint8_t *message = 0;
=======
                
                uint8_t *message = NULL;  
                
>>>>>>> 655914633900c759d23349944d3932161d8790ba
                if (led_state){
                    message = tx_on;
                }
                else {
                    message = tx_off;
                }

                if (nrf24_transmit(message)) {
                    printf("Sent: %s\n", message);
                } else {
                    printf("Transmission failed, retrying...\n");
                }
            }
        }
        timer_delay_ms(500); // delay to avoid multiple triggers
    }    
}

void main(void){
    uart_init();
    nrf24_init();
    nrf24_set_tx_mode(tx_address, 10);
    test_transmission();
}
