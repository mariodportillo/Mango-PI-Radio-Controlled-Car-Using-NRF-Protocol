#ifndef MOTOR_H
#define MOTOR_H
/* File: motor.h
* ---------------
* CS 107e 
* Authors: Mario Portillo and Sahan Samarakoon
* Date: 15th March 2025
*/

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

/*
*
* This motor_init funciton handles a lot of the setup required for the interrupts,
*  pwm, and gpio_pins. 
* 
*/
void motor_init(void);


/**
 * These following functions are useful for someone who is using the l298n and would like to power
 * the driver using a mango pi. These are some very basic commands that will allow turning for time.
 * In further iterations of this library it could be expanded to include turn for angle, turn 90 degrees,
 * and drive a distance. For our purposes we only needed commands for time so we stuck with these 
 * motor drive functions. 
*/
void drive_forward_time(unsigned long millisecond);
void drive_reverse_time(unsigned long millisecond);


// motorA = left wheel
// motorB = right wheel
void drive_spin_right_time(unsigned long millisecond);
void drive_spin_left_time(unsigned long millisecond);

void drive_pivot_spin_right_time(unsigned long millisecond);
void drive_pivot_spin_left_time(unsigned long millisecond);


/**
 * motor_control_from_joystick: Registers user input through the switch and the
 * joystick on the controller. This function takes care of transmitting the correct
 * command to then be recieved and processed by the receiver i.e the car. 
 * 
 * Potential Actions:
 * Yellow Button Pressed -> "All Stop" will be transmitted to the reciever and the controller mango_reboots().
 * 
 * Switch button is pressed on joystick -> "Activate Radar Scan" will be transmitted to the reciever and data will
 * be sent back to the controller and outputted on the screen.
 * 
 * Normal Joystick movement -> specific directions will be transmitted to the reciever in the form of "Direction Speed: "
 * the speed is determined by the positioning of the joystick and so is the direction.
 * Up = forward
 * Left = left
 * Right = right
 * Back = backwards
 * 
 */
void motor_control_from_joystick(void);

/**
 * motorDriveRecieve: The reciever on the car which will mostly recieve function commands from the other PI
 * using NRF communication to then move the motors on the recievers end will call this function forever in some
 * kinds of while loop such that it is constantly responding to user inputs froom the controller. 
 * 
 **/
void motorDriveRecieve (void);



#endif
