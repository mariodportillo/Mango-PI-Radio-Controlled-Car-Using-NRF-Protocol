#include "pwm.h"
#include "gpio.h"
#include "timer.h"
#include "mcp3008.h"  // For joystick readings

#define ENA_PIN GPIO_PB10  // PWM7 for Motor A speed
#define ENB_PIN GPIO_PB11  // PWM2 for Motor B speed

#define IN1_PIN GPIO_PC0  // Motor A direction
#define IN2_PIN GPIO_PB2  
#define IN3_PIN GPIO_PB3  // Motor B direction
#define IN4_PIN GPIO_PB4  

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
void motorA_set_direction(int dir) {
    if (dir) {
        gpio_write(IN1_PIN, 1);
        gpio_write(IN2_PIN, 0);
    } else {
        gpio_write(IN1_PIN, 0);
        gpio_write(IN2_PIN, 1);
    }
}

// set Motor B direction (1 = Forward, 0 = Reverse)
void motorB_set_direction(int dir) {
    if (dir) {
        gpio_write(IN3_PIN, 1);
        gpio_write(IN4_PIN, 0);
    } else {
        gpio_write(IN3_PIN, 0);
        gpio_write(IN4_PIN, 1);
    }
}

// read joystick input and adjust motor speed
void motor_control_from_joystick(void) {
    // we continuously adjust the motor speed based on joystick
    while (1) {
        int joystick_value = mcp3008_read_channel(0);  // read ADC (joystick is on ADC channel 0)
        // Convert ADC (0-1023) to duty cycle (0-100%)
        int speed = (joystick_value * 100) / 1023; 

        pwm_set_duty(PWM7, speed);  // Set Motor A speed
        pwm_set_duty(PWM2, speed);  // Set Motor B speed
        
        if (speed > 50) {
            motorA_set_direction(1); // Forward
            motorB_set_direction(1); 
        } else {
            motorA_set_direction(0); // Reverse
            motorB_set_direction(0);
        }

        timer_delay_ms(100); 
    }
}

void main(void) {
    motor_init();
    motor_control_from_joystick();  
}
