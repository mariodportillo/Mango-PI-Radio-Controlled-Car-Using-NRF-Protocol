#ifndef RADAR_H__
#define RADAR_H__
/* File: radar.h
 * ---------------
* CS 107e 
* Authors: Sahan Samarakoon and Mario Portillo
* Date: 14th March 2025
*/

#define STEP_SIZE 20
#define NUM_ANGLES 10 // ((MAX_ANGLE / STEP_SIZE) + 1) : (180 / 20) + 1 = 10

/**
 * Radar init will take care of the servo and radar init. After calling this function you do not have 
 * to worry about initializing those peripherals.
 * 
*/

void radarInit(void);

/*
* This function is what we use to collect Ultrasonic sensor data. By using this function you will
* collect and return a pointer to an array of integer values that represent distances from the sensor
* as it pans 180 degrees. 
*/
uint8_t* radar_scan();


#endif
