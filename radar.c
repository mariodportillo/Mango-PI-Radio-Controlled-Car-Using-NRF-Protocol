#include "us.h"
#include "servo.h"





void radar_scan() {
    for (int angle = 0; angle <= 180; angle += 5) {
        servo_set_angle(PWM4, angle);
        timer_delay_ms(100); 
        
        int distance = sense_distance(GPIO_PD2, GPIO_PD3); // Trigger & Echo
        distance_data[angle / 10] = distance; // Store data
    }
}

void radar_display() {
    console_clear();
    
    for (int i = 0; i <= 18; i++) {
        int distance = distance_data[i];
        int pos = distance / 2; // scale down for screen
        
        if (pos < module.ncols) {
            module.text_grid[i][pos] = '*'; // Mark obstacle
        }
    }
    
    console_display();
}