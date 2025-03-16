#include "us.h"
#include "servo.h"
#include "gl.h"
#include "display.h"
#include "uart.h"
#include "math_float.h"
#include "timer.h"

// for testing only. Pls remove
#include "strings.h"
#include "printf.h"


const int SCREEN_WIDTH = 40;  // Number of columns in console
const int SCREEN_HEIGHT = 20; // Number of rows in console
const int STEP_SIZE = 5;
const static int LINE_SPACING = 5;
#define NUM_ANGLES 37 // ((MAX_ANGLE / STEP_SIZE) + 1) : (180 / 5) + 1 = 37

int distance_data[NUM_ANGLES];


void radar_scan() {
    for (int i = 0; i < NUM_ANGLES; i++) {  // 0, 5, 10, ... 180
        int angle = i * STEP_SIZE;
        
        servo_set_angle(PWM4, angle);
        timer_delay_ms(100);  
        
        int distance = sense_distance();
        distance_data[i] = distance;  // Store it in our compact array
    }
}

void radar_display() {
    console_clear();
    

    // Car is at bottom center
    int car_x = SCREEN_WIDTH / 2;
    int car_y = SCREEN_HEIGHT - 1;

    int char_width = gl_get_char_width();
    int line_height = gl_get_char_height() + LINE_SPACING;
    color_t color = gl_color(0xff, 0xff, 0xff);
    gl_draw_char(car_x * char_width, car_y * line_height, '^', color); // Draw car

    // Draw obstacles based on scanned distances
    for (int i = 0; i < NUM_ANGLES; i++) {
        int angle = i * STEP_SIZE;  // Convert index back to angle
        int distance = distance_data[i];  // Read corresponding data

        // Convert angle + distance into screen coordinates
        int dx = (distance * SCREEN_WIDTH / 100) * cos(angle * 3.14 / 180);  // Scale X
        int dy = (distance * SCREEN_HEIGHT / 100) * sin(angle * 3.14 / 180); // Scale Y

        int x = car_x + dx;
        int y = car_y - dy; // Moving upwards

        // Ensure within screen bounds
        if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
            gl_draw_char(x * char_width, y * line_height, '*', color); // Draw obstacle
        }
    }
    
    console_display();
    timer_delay_ms(5000);
}

void main(void) {
    uart_init();
    console_init(SCREEN_HEIGHT, SCREEN_WIDTH, gl_color(255, 255, 255), gl_color(0, 0, 0));
    
    servo_init(PWM4, GPIO_PB1);  
    for (int i = 0; i < NUM_ANGLES; i++){
        distance_data[i] = 10;
        //distance_data[i] = i * 2;  // test with increasing distances
    }
    while (1) {
        //radar_scan();

        radar_display();
        timer_delay_ms(5000);
    }
}