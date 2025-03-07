/* File: console.c
 * ---------------
 * Mario Portillo
 * CS 107e
 */
#include "console.h"
#include "gl.h"
#include "malloc.h"
#include "printf.h"
#include "strings.h"
#include <stdarg.h>

// module-level variables, you may add/change this struct as you see fit!
static struct {
    color_t bg_color, fg_color;
    int line_height;
    int char_width;
    char **stringBuffer;
    int x_cursor; //This will always start at 0
    int y_cursor; //This will always start at 0
    int ncols;
    int nrows; 
} module;


void console_init(int nrows, int ncols, color_t foreground, color_t background) {
    // Please add this amount of vertical space between console rows
    const static int LINE_SPACING = 5;
   
    if(nrows < 0 || ncols < 0 ){
         return;
    }//this input makes no sense

    module.line_height = gl_get_char_height() + LINE_SPACING;
    module.char_width = gl_get_char_width();

    module.fg_color = foreground;
    module.bg_color = background;

    module.ncols = ncols;
    module.nrows = nrows;
    
    int width = module.char_width * ncols;
    int height = module.line_height * nrows;

    gl_init(width, height, GL_DOUBLEBUFFER);


    module.stringBuffer = (char **)malloc(nrows * sizeof(char *));
    
    for (int i = 0; i < nrows; i++) {
        module.stringBuffer[i] = (char *)malloc(ncols * sizeof(char));
        memset(module.stringBuffer[i], '\0', ncols);
    }

    module.x_cursor = 0; //init to be zero
    module.y_cursor = 0; //init to be zero 
    gl_clear(module.bg_color);
    gl_swap_buffer();

}

void updateScreen(void) {
    gl_clear(module.bg_color); // we set our draw buffer to have the background

    for (int y = 0; y < module.nrows; y++) {
        for (int x = 0; x < module.ncols; x++) {
            char ch = module.stringBuffer[y][x];
            if (ch != '\0') { 
                gl_draw_char(x * module.char_width, y * module.line_height, ch, module.fg_color);
            }
        }
    }

    gl_swap_buffer(); // Swap the buffer to reflect changes

}

void console_clear(void) {
    char** newBuf = (char **)malloc(module.nrows * sizeof(char *)); 
    
    for (int i = 0; i < module.nrows; i++) {
        newBuf[i] = (char *)malloc(module.ncols * sizeof(char));
        memset(newBuf[i], '\0', module.ncols);
    }

    //We then need to free the old buffer
    for (int i = 0; i < module.nrows; i++) {
        free(module.stringBuffer[i]);
    } 
    
    free(module.stringBuffer);
    
    //our new buffer then becomes our actual stringBuffer
    module.stringBuffer = newBuf;

    module.x_cursor = 0; //reset the cursor position
    module.y_cursor = 0; //reset the cursor position

    updateScreen();
}


void scrollFunction(void){
    // we want to remove the first line in our old string buffer and move everything
    // else up one.
    char** newBuf = (char **)malloc(module.nrows * sizeof(char *)); 
    
    for (int i = 0; i < module.nrows; i++) {
        newBuf[i] = (char *)malloc(module.ncols * sizeof(char));
        memset(newBuf[i], '\0', module.ncols);
    }

    //We shift everything up by 1 by 
    for (int y = 0; y < module.nrows - 1; y++) {
        // we only need to copy everything from the second line down
        memcpy(newBuf[y], module.stringBuffer[y + 1], module.ncols);
    }

    // we need to clear the last row to ensure there is nothing there. 
    memset(newBuf[module.nrows - 1], '\0', module.ncols);

    //We then need to free the old buffer
    for (int i = 0; i < module.nrows; i++) {
        free(module.stringBuffer[i]);
    }

    free(module.stringBuffer);
    
    //our new buffer then becomes our actual stringBuffer
    module.stringBuffer = newBuf;
    
    //this should effectively move our cursor down while the buffer moved up.
    module.y_cursor = gl_get_height() - module.line_height;

}

void drawCharAtCursor(char ch){
    if (module.x_cursor + module.char_width > gl_get_width()) {
        module.x_cursor = 0;
        if (module.y_cursor + module.line_height >= gl_get_height()) {
            scrollFunction();
        }else{
            module.y_cursor += module.line_height; //moves our cursor to the next line. 
        }
    }

    int x_stringBuffer_index = module.x_cursor / module.char_width;
    int y_stringBuffer_index = module.y_cursor / module.line_height;

    
    module.stringBuffer[y_stringBuffer_index][x_stringBuffer_index] = ch; // Store character in the correct position
    module.x_cursor += module.char_width; 

    updateScreen(); // Redraw screen after each character

}

int console_printf(const char *format, ...) {    
    va_list args;
    va_start(args, format);  // initialize args to point to the variable arguments
    char tempBuf[1024]; // given by our max buf length

    int result = vsnprintf(tempBuf, sizeof(tempBuf), format, args);
    va_end(args);// clean up

    int x_stringBuffer_index = module.x_cursor / module.char_width;
    int y_stringBuffer_index = module.y_cursor / module.line_height;
    
    // Write formatted text to stringBuffer
    for (int i = 0; tempBuf[i] != '\0'; i++) {    
	if (tempBuf[i] == '\n') {
            module.x_cursor = 0;

            if (module.y_cursor + module.line_height  >= gl_get_height()) {
            	scrollFunction(); 
	    }else {
	    	module.y_cursor += module.line_height;
	    }

        } else if (tempBuf[i] == '\b') {
            if(module.x_cursor - module.char_width > 0){
	    	module.x_cursor -= module.char_width;
	    }else{
		module.x_cursor = 0; 
	    }

	    x_stringBuffer_index = module.x_cursor / module.char_width;
	    module.stringBuffer[y_stringBuffer_index][x_stringBuffer_index] = ' ';

        }else if(tempBuf[i] == '\f') {
           console_clear();
        } else {
            drawCharAtCursor(tempBuf[i]); //this is supposed to update our string buffer and update the monitor 
        }
    }
    return result; 
}


void console_startup_screen(void) {
   /***** TODO: Your code goes here IFF doing the extension *****/
}
