#include "servo.h"
#include "timer.h"

// for testing. Pls remove
#include "printf.h"
#include "uart.h"

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

// TODO: uncomment for testing. If uncommented, be sure to remove this file from the Makefile before building
// int main(void) {
//     servo_init(PWM4, GPIO_PB1);  
//     uart_init();
//     printf("I ran\n");
//     while (1) {
//         servo_set_angle(PWM4, 0);    // Move to 0°
//         timer_delay_ms(1000);
//         servo_set_angle(PWM4, 90);   // Move to 90°
//         timer_delay_ms(1000);
//         servo_set_angle(PWM4, 180);  // Move to 180°
//         timer_delay_ms(1000);
//     }
// }
