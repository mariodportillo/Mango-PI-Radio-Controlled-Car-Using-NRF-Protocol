#include "us.h"
#include "servo.h"
#include "gl.h"
#include "uart.h"
#include "display.h"
#include "math_float.h"
#include "timer.h"
#include "radar.h"

// for testing only. Pls remove
#include "strings.h"
#include "printf.h"


int distance_data[NUM_ANGLES];


void radar_scan() {
    for (int i = 0; i < NUM_ANGLES; i++) {  // 0, 5, 10, ... 180
        int angle = i * STEP_SIZE;
        servo_set_angle(PWM4, angle);
        timer_delay_ms(250);  
        
        int distance = sense_distance();
        distance_data[i] = distance;  // Store it in our compact array
        //printf("distance_data in radar_scan[%d]: %d  ", i, distance);
    }

}



