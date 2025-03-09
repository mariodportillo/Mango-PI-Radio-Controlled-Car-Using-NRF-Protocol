
/* File: test_transmitter.c
 * ---------------------------
 * ***** Compile and run this code on the transmitter  *****
 */
#include "nrf.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"



uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t tx_data[] = "Hello World\n";


void test_transmission(){
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

void main(void){
    uart_init();
    nrf24_init();
    nrf24_set_tx_mode(tx_address, 10);
    test_transmission();

}
