#include "pwm.h"
#include "gpio.h"
#include "timer.h"
#include "mcp3008.h"  // For joystick readings
#include "motor.h"
#include "printf.h"
#include "math_float.h"
#include "nrf.h"

void motor_init(void) {
    pwm_init();  
    pwm_config_channel(PWM7, ENA_PIN, 10000, false); // 10kHz frequency for Motor A
    pwm_config_channel(PWM2, ENB_PIN, 10000, false); // 10kHz frequency for Motor B

    gpio_set_output(IN1_PIN);
    gpio_set_output(IN2_PIN);
    gpio_set_output(IN3_PIN);
    gpio_set_output(IN4_PIN);

}

// set Motor A direction (1 = Forward, 0 = Reverse)
static void motorA_set_direction(int dir) {
    if (dir) {
        gpio_write(IN1_PIN, 1);
        gpio_write(IN2_PIN, 0);
    } else {
        gpio_write(IN1_PIN, 0);
        gpio_write(IN2_PIN, 1);
    }
}

// set Motor B direction (1 = Forward, 0 = Reverse)
static void motorB_set_direction(int dir) {
    if (dir) {
        gpio_write(IN3_PIN, 1);
        gpio_write(IN4_PIN, 0);
    } else {
        gpio_write(IN3_PIN, 0);
        gpio_write(IN4_PIN, 1);
    }
}


// For the below code I am assuming
// motorA = left wheel
// motorB = right wheel
void drive_forward_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
       motorB_set_direction(1);
    }
}

void drive_reverse_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(0);
       motorB_set_direction(0);
    }
}

//This will turn in place and both wheels turn in opposite directions. 
void drive_spin_right_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
       motorB_set_direction(0);
    }
}

void drive_spin_left_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(0);
       motorB_set_direction(1);
    }
}

//These below turns are done using only one motor while the other is stationary
void drive_pivot_spin_right_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
    }
}

void drive_pivot_spin_left_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorB_set_direction(1);
    }
}

// This below function must be continously called in order to work properly
// i.e a while loop.
void motor_control_from_joystick(void) {
    //int joystick_value = mcp3008_read_channel(0);  // read ADC (joystick is on ADC channel 0)
    // Convert ADC (0-1023) to duty cycle (0-100%)
    int x_value = mcp3008_read_channel(X_CHANNEL); // const X_CHANNEL defined in the header file
    int y_value = mcp3008_read_channel(Y_CHANNEL); // const Y_CHANNEL defined in the header file

    int speed = 0;     //(joystick_value * 100) / 1023; // -> needs to be between 0 and 100%
    motor_direction cur_dir = MOVE_FORWARD; // default direction is forward
    int startingRange = 515; //these values can be adjusted
    int noMotion = 30; // these values can be adjusted

    if (y_value > startingRange + noMotion) {  // Moving forward
    	speed = ((y_value - startingRange) * 100) / 511; //gives us a value between 0 and 100
    	cur_dir = MOVE_BACKWARD;

    }else if (y_value < startingRange - noMotion) {  // Moving backward
    	speed = ((startingRange - y_value) * 100) / 512;
    	cur_dir = MOVE_FORWARD;

    }else if (x_value > startingRange + noMotion) {  // Turning right
    	speed = ((x_value - startingRange) * 100) / 511;
    	cur_dir = TURN_LEFT;

    }else if (x_value < startingRange - noMotion) {  // Turning left
   	speed = ((startingRange - x_value) * 100) / 512;
    	cur_dir = TURN_RIGHT;
    } else {
	speed = 0;  // Neutral position
    }

    if(speed == 0){
	return; 
    }
    //TODO: Gert rid of this below code and make a reciever funciton of this code. 
    // in the future this below pwm code needs to go to the reciever not the transmitter.
    pwm_set_duty(PWM7, speed);  // Set Motor A speed
    pwm_set_duty(PWM2, speed);  // Set Motor B speed
    
    nrf24_init();
    uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    nrf24_set_tx_mode(tx_address, 10);
    

    switch(cur_dir){

    case MOVE_FORWARD:{
	//send a message	
    	uint8_t tx_data[] = "Forward";

        if (nrf24_transmit(tx_data)) {
            printf("%s, Speed: %d\n", tx_data, speed);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case MOVE_BACKWARD: {
    	uint8_t tx_data[] = "Backward";
        if (nrf24_transmit(tx_data)) {
            printf("%s, Speed: %d\n", tx_data, speed);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case TURN_RIGHT: {
	//send a message
    	uint8_t tx_data[] = "Right";
        if (nrf24_transmit(tx_data)) {
            printf("%s, Speed: %d\n", tx_data, speed);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case TURN_LEFT: {
    	uint8_t tx_data[] = "Left";
	//send a message
        if (nrf24_transmit(tx_data)) {
            printf("%s, Speed: %d\n", tx_data, speed);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    default:
	printf("Invalid cur_dir: %d \n", cur_dir);
	//do nothing -> invalid cur_dir
	return;
    }
}
