/* File: test_transmitter.c
 * ---------------------------
 * ***** Compile and run this code on the transmitter  *****
 */

#include "nrf.h"
#include "uart.h"
#include "printf.h"
#include "timer.h"
#include "mcp3008.h"
#include "spi.h"
#include "motor.h"

uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t tx_on[] = "ON";
uint8_t tx_off[] = "OFF";
int led_state = 0; // track LED state
uint8_t tx_data[] = "Hello World\n";

void test_transmission_simple(){
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
                uint8_t *message = 0;
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

void test_read_channel_mcp(){
  printf("Now reading chaneel of MCP3008: \n");
  int value = 0;

  while(1){
	value = mcp3008_read_channel(0);
	printf("Current value: %d \n", value); 
	timer_delay_ms(100);
  }

}

void test_joystick_to_motor(){
  while(1){
     motor_control_from_joystick();
     int x_value = mcp3008_read_channel(0);
     int y_value = mcp3008_read_channel(1);
     timer_delay_ms(100);
     printf("x: %d y: %d \n", x_value, y_value); 
     timer_delay_ms(100);
  }
}

void main(void){
    uart_init();
    //nrf24_init();
    spi_init(SPI_MODE_0);
    mcp3008_init();
    motor_init();
    //nrf24_set_tx_mode(tx_address, 10);
    
    //test_read_channel_mcp();
    //test_transmission_simple();
    //test_transmission();
    test_joystick_to_motor();
}
