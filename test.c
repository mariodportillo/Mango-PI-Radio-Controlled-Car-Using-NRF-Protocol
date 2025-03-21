#include "motor.h"
#include "timer.h"

void main(void){
    while(1){
        drive_forward_time(5000);
        timer_delay_ms(1000);
    }
}