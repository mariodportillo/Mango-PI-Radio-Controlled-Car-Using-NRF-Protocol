/* File: display.c
 * ---------------
 * ***** Contains functions borrowed from Sahan's console module *****
 */

#include "gl.h"
#include "strings.h"
#include "malloc.h"
#include "math_float.h"
#include "radar.h"


// module-level variables
static struct {
    color_t bg_color, fg_color;
    int line_height;
    int char_width;
    int nrows, ncols;
    char *text_grid;
} module = {0};

const size_t MAX_OUTPUT_LEN = 1024;

void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    // Please add this amount of vertical space between console rows
    const static int LINE_SPACING = 5;

    module.ncols = ncols;
    module.nrows = nrows;
    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.char_width = gl_get_char_width();
    module.fg_color = foreground;
    module.bg_color = background;
    
    if (module.text_grid != NULL){
        free(module.text_grid);
    }

    module.text_grid =  malloc(ncols * nrows);
    memset(module.text_grid, ' ', ncols * nrows);

    const int WIDTH = module.char_width * ncols;
    const int HEIGHT = module.line_height * nrows;

    gl_init(WIDTH, HEIGHT, GL_DOUBLEBUFFER);
    // set color
    gl_clear(module.bg_color);
    gl_swap_buffer();
    gl_clear(module.bg_color);      // setup both buffers  

}

void console_clear(void) {
    gl_clear(module.bg_color);
    gl_swap_buffer();
    gl_clear(module.bg_color);      // clear the second buffer
    memset(module.text_grid, ' ', module.ncols * module.nrows);
    
}


void console_display(void){
    gl_clear(module.bg_color);

    char (*text)[module.ncols] = (char (*)[module.ncols]) module.text_grid;      // use text grid as a 2D array
    for (int j = 0; j < module.nrows; j++) {
        for (int i = 0; i < module.ncols; i++) {
            char ch = text[j][i];
            if (ch != ' '){
                gl_draw_char(i * module.char_width, j * module.line_height , ch, module.fg_color);
            } 
        }
    }
 
    gl_swap_buffer();
}

void radar_display(uint8_t *distance_data) {
    // Car is at bottom center
    int car_x = module.ncols / 2;
    int car_y = module.nrows - 1;
    console_clear();

    char (*text)[module.ncols] = (char (*)[module.ncols]) module.text_grid; 
    text[car_y][car_x] = '^';


    // Draw obstacles based on scanned distances
    for (int i = 0; i < NUM_ANGLES; i++) {
        int angle = i * STEP_SIZE;  // Convert index back to angle
        uint8_t distance = distance_data[i];  // Read corresponding data
    
        // Convert angle + distance into screen coordinates
        int dx = (distance) * cos(angle * 3.14 /180);  // Scale X
        int dy = (distance) * sin(angle * 3.14 /180); // Scale Y

        int x = car_x + dx;
        int y = car_y - dy; // Moving upwards

        // Ensure within screen bounds
        if (x >= 0 && x < module.ncols && y >= 0 && y < module.nrows && x != car_x && car_y != y) {
            text[y][x] = '*';
        }
    }
    
    console_display();
    
}
