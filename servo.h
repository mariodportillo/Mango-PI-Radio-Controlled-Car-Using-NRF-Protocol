#ifndef SERVO_H__
#define SERVO_H__
/* File: servo.h
* ---------------
* CS 107e 
* Authors: Sahan Samarakoon and Mario Portillo 
* Date: 20th March 2025
*/

#include "pwm.h"
#include "gpio.h"

/**
 * This funciton initilizes PWM from code/extra file and then begins setting up the servo. 
 * We also configure the PWM channel to be ready for use at 50 Hz. The servo is positionined 
 * at zero to be retested over and over again. 
*/
void servo_init(pwm_channel_id_t ch, gpio_id_t pin);

/**
 * This handy function will allow us to position the servo at any angle we would like given a specific
 * pwm channel.
*/
void servo_set_angle(pwm_channel_id_t ch, int angle);


#endif
