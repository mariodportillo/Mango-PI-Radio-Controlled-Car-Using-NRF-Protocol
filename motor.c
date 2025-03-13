#include "pwm.h"
#include "gpio.h"
#include "timer.h"
#include "mcp3008.h"  // For joystick readings
#include "motor.h"
#include "printf.h"
#include "math_float.h"

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
    int y_value = mcp3008_read_channel(Y_CHANNEL);

    int speed = 0;     //(joystick_value * 100) / 1023; // -> needs to be between 0 and 100%
    motor_direction cur_dir = MOVE_FORWARD; //
    
    if(y_value >= 550){ // this value should be tested.
	//we should be moving forward
	speed = ((512 - y_value)*100) / 512; // 512 = starting position
	cur_dir = MOVE_FORWARD; 

    }else if(y_value <= 550){ //these ranges need to be tested.
	//we should be reversing
	speed = ((y_value - 512)*100) / 512; // 512 = starting position
	cur_dir = MOVE_BACKWARD;

    }else if(x_value <= 550){
	//we should be turning right
	speed = ((x_value - 512)*100) / 512; // 512 = starting position	
	cur_dir = TURN_RIGHT;

    }else if(x_value >= 550){
	//we should be turning left
	speed = ((512 - x_value)*100) / 512;
	cur_dir = TURN_LEFT;
    }

    if(speed == 0){
	return; 
    }

    pwm_set_duty(PWM7, speed);  // Set Motor A speed
    pwm_set_duty(PWM2, speed);  // Set Motor B speed

    switch(cur_dir){

    case MOVE_FORWARD:
        printf("Forward \n");
        break;

    case MOVE_BACKWARD:
        printf("Backwards\n");
        break;

    case TURN_RIGHT:
        printf("Right\n");
        break;

    case TURN_LEFT:
        printf("Left\n");
        break;
    default:
	printf("Invalid cur_dir: %d \n", cur_dir);
	//do nothing -> invalid cur_dir
	return;
    }
}
