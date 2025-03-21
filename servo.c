/* File: servo.c
* ---------------
* CS 107e 
* Authors: Sahan Samarakoon and Mario Portillo 
* Date: 20th March 2025
*/
#include "servo.h"
#include "timer.h"


void servo_init(pwm_channel_id_t ch, gpio_id_t pin){
    pwm_init();

    int freq = 50; // 50 Hz
    pwm_config_channel(ch, pin, freq, false);
    servo_set_angle(PWM4, 0);
}

void servo_set_angle(pwm_channel_id_t ch, int angle) {
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    
    int min_pulse = 750;   // 0 degree position
    int max_pulse = 2500;  // 180 degree position

    // map angle (0-180) to pulse width (min_pulse to max_pulse)
    int pulse_width_us = min_pulse + ((angle * (max_pulse - min_pulse)) / 180);
    //printf("pulse width: %d\n", pulse_width_us);

    int duty_percent = (pulse_width_us * 100) / 20000;  
    pwm_set_duty(ch, duty_percent);
}
