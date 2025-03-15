#ifndef SERVO_H__
#define SERVO_H__

#include "pwm.h"
#include "gpio.h"

void servo_init(pwm_channel_id_t ch, gpio_id_t pin);
void servo_set_angle(pwm_channel_id_t ch, int angle);


#endif
