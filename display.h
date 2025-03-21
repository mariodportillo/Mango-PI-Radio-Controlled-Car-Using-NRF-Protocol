#ifndef DISPLAY_H__
#define DISPLAY_H__


#include "gl.h"
#include "console.h"
#include "radar.h"

/**
 * 
 * We initialize our screen buffer just as we did in assignment 6. 
 * 
*/
void console_init(int nrows, int ncols, color_t foreground, color_t background);

/**
 * 
 * We clear the funciton and the buffer to be ready for the next command. 
 * 
*/
void console_clear(void);

/**
 * 
 * Console Display will display anything in our character buffer out onto the screen. 
 * 
*/
void console_display(void);

/**
 * 
 * The radar display function will process the first 10 distances passed in by the pointer to the array.
 * After processing and calculating the distances using sin and cos we display this data on our buffer. 
 * 
*/
void radar_display(uint8_t *distance_data);



#endif
