#include "pwm.h"
#include "gpio.h"
#include "timer.h"
//#include "mcp3008.h"  // For joystick readings
#include "motor.h"
#include "printf.h"

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
    int joystick_value = 0;
    int speed = (joystick_value * 100) / 1023; 

    pwm_set_duty(PWM7, speed);  // Set Motor A speed
    pwm_set_duty(PWM2, speed);  // Set Motor B speed
        
    if (speed > 50) {
        drive_forward_time(20);
    } else {
       drive_reverse_time(20); 
    }
        timer_delay_ms(100); 
}
