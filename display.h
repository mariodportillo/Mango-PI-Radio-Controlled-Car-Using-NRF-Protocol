#ifndef DISPLAY_H__
#define DISPLAY_H__


#include "gl.h"
#include "console.h"
#include "radar.h"

void console_init(int nrows, int ncols, color_t foreground, color_t background);
void console_clear(void);
void console_display(void);
void radar_display(int *distance_data);



#endif