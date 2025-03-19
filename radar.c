#include "us.h"
#include "servo.h"
#include "gl.h"
#include "uart.h"
#include "display.h"
#include "math_float.h"
#include "timer.h"
#include "radar.h"
#include "malloc.h"
#include "gpio.h"


void radarInit(void){
    servo_init(PWM4, GPIO_PB1);
    us_init();
}

int* radar_scan() {
    int* distance_data = malloc(NUM_ANGLES * sizeof(int));

    for (int i = 0; i < NUM_ANGLES; i++) {  // 0, 5, 10, ... 180
        int angle = i * STEP_SIZE;
        servo_set_angle(PWM4, angle);
        timer_delay_ms(250);  
        
        int distance = sense_distance();
        distance_data[i] = distance;
    }
    return distance_data; 
}



