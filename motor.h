#ifndef MOTOR_H
#define MOTOR_H

#define ENA_PIN GPIO_PB10  // PWM7 for Motor A speed
#define ENB_PIN GPIO_PB11  // PWM2 for Motor B speed

#define IN1_PIN GPIO_PC0  // Motor A direction
#define IN2_PIN GPIO_PC1  
#define IN3_PIN GPIO_PD14     // Motor B direction
#define IN4_PIN GPIO_PD21 

#define X_CHANNEL 0
#define Y_CHANNEL 1

#define BUTTON GPIO_PB3
#define KILL_SWITCH GPIO_PB2

typedef enum {
    MOVE_FORWARD = 0,
    MOVE_BACKWARD = 1,
    TURN_RIGHT = 2,
    TURN_LEFT = 3,
} motor_direction;

void motor_init(void);

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
void motorDriveRecieve (void);



#endif
