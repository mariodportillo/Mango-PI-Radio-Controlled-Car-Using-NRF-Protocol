#include "us.h"
#include "servo.h"
#include "gl.h"
#include "uart.h"
#include "display.h"
#include "math_float.h"
#include "timer.h"

// for testing only. Pls remove
#include "strings.h"
#include "printf.h"



const int STEP_SIZE = 20;
const static int LINE_SPACING = 5;
#define NUM_ANGLES 10 // ((MAX_ANGLE / STEP_SIZE) + 1) : (180 / 20) + 1 = 10

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



void main(void) {
    uart_init();
    us_init();
    const int SCREEN_WIDTH = 40;  // Number of columns in console
    const int SCREEN_HEIGHT = 20; // Number of rows in console
    // 1 col is 2 inch i row is 1 inch
    console_init(SCREEN_HEIGHT, SCREEN_WIDTH, gl_color(255, 255, 255), gl_color(0, 0, 0));
    
    servo_init(PWM4, GPIO_PB1);  
    // for (int i = 0; i < NUM_ANGLES; i++){
    //     distance_data[i] = 10;
    //     //distance_data[i] = i * 2;  // test with increasing distances
    // }
    while (1) {
        radar_scan();

        radar_display(distance_data);
        timer_delay_ms(500);
    }
}