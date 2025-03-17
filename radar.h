#ifndef RADAR_H__
#define RADAR_H__


#define STEP_SIZE 20
#define NUM_ANGLES 10 // ((MAX_ANGLE / STEP_SIZE) + 1) : (180 / 20) + 1 = 10


void radar_scan();


#endif