#ifndef MOTOR_H
#define MOTOR_H

#define ENA_PIN GPIO_PB10  // PWM7 for Motor A speed
#define ENB_PIN GPIO_PB11  // PWM2 for Motor B speed

#define IN1_PIN GPIO_PC0  // Motor A direction
#define IN2_PIN GPIO_PB2  
#define IN3_PIN GPIO_PB3  // Motor B direction
#define IN4_PIN GPIO_PB4  

void motor_init(void);
static void motorA_set_direction(int dir);
static void motorB_set_direction(int dir);

// Below are controls the user can use to turn the wheels and
// make the car move forward for a amount of time
void drive_forward_time(unsigned long millisecond);
void drive_reverse_time(unsigned long millisecond);


// motorA = left wheel
// motorB = right wheel
void drive_spin_right_time(unsigned long millisecond);
void drive_spin_left_time(unsigned long millisecond);

void drive_pivot_spin_right_time(unsigned long millisecond);
void drive_pivot_spin_left_time(unsigned long millisecond);


void motor_control_from_joystick();




#endif
