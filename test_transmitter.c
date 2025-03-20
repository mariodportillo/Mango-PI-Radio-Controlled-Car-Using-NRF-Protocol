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
#include "display.h"
#include "pwm.h"
#include "servo.h"
#include "us.h"
#include "gpio_interrupt.h"

uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t tx_on[] = "ON";
uint8_t tx_off[] = "OFF";
int led_state = 0; // track LED state

void test_transmission_simple(){
    printf("Press 'q' to stop transmission.\n");
    uint8_t tx_data[] = "Hello World\n";

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

  while(1){
	unsigned int yvalue = mcp3008_read_channel(0);
	unsigned int xvalue = mcp3008_read_channel(1);
	printf("Current x value: %d  ", xvalue);
	printf("Current y value: %d \n", yvalue); 

	timer_delay_ms(100);
  }

}

void test_joystick_to_motor(){
  while(1){
     motor_control_from_joystick();
     timer_delay_ms(300); 
  }
}



void main(void){
    uart_init();
    nrf24_init();
    spi_init(SPI_MODE_0);
    mcp3008_init();
    //nrf24_set_tx_mode(tx_address, 10);
    
    //test_read_channel_mcp();
    //test_transmission_simple();
    //test_transmission();
    const int SCREEN_WIDTH = 40;  // Number of columns in console
    const int SCREEN_HEIGHT = 20; // Number of rows in console

    console_init(SCREEN_HEIGHT, SCREEN_WIDTH, gl_color(255, 255, 255), gl_color(0, 0, 0));
    motor_init();
    test_joystick_to_motor();
}


// void radar_scan_and_transmit() {
//     for (int i = 0; i < NUM_ANGLES; i++) {
//         int angle = i * 5;
//         servo_set_angle(PWM4, angle);
//         timer_delay_ms(100);

//         int distance = sense_distance();  
//         radar_data[i].angle = angle;
//         radar_data[i].distance = (distance > 255) ? 255 : distance;  // Limit distance

//         // Send data in chunks of 16
//         if (i % 16 == 15 || i == NUM_ANGLES - 1) {
//             nrf24_transmit((uint8_t*)radar_data);  
//             timer_delay_ms(10);
//         }
//     }
// }


// a lil tricky
// void main(void) {
//     uart_init();
//     us_init();
//     const int SCREEN_WIDTH = 40;  // Number of columns in console
//     const int SCREEN_HEIGHT = 20; // Number of rows in console
//     // 1 col is 2 inch i row is 1 inch
//     console_init(SCREEN_HEIGHT, SCREEN_WIDTH, gl_color(255, 255, 255), gl_color(0, 0, 0));
    
//     servo_init(PWM4, GPIO_PB1);  
//     // for (int i = 0; i < NUM_ANGLES; i++){
//     //     distance_data[i] = 10;
//     //     //distance_data[i] = i * 2;  // test with increasing distances
//     // }
//     while (1) {
//         radar_scan();

//         radar_display(distance_data);
//         timer_delay_ms(500);
//     }
// }
