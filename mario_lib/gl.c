/* File: gl.c
 * ----------
 * Mario Portillo
 * CS 107e
 */
#include "gl.h"
#include "font.h"
#include "strings.h"


void gl_init(int width, int height, gl_mode_t mode) {
    fb_init(width, height, mode);
}

int gl_get_width(void) {
    return fb_get_width();
}

int gl_get_height(void) {
    return fb_get_height();
}

color_t gl_color(uint8_t r, uint8_t g, uint8_t b) {
    color_t result = 0xFF000000;
    result |= r << 16; //shift over 16 bits to get it in place
    result |= g << 8;
    result |= b;
    
    return result;
}

void gl_swap_buffer(void) {
    fb_swap_buffer();
}

void gl_draw_pixel(int x, int y, color_t c) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    
    if(x < 0){
	return;
    }

    if(y < 0){
    	return;
    }

    if (x >= per_row){
	return;
    }

    if(y >= fb_get_height()){
	return;
    } 

    im[y][x] = c;
}
// Should be faster by clearing pixel by pixel but using 1D array instead of 2D
void gl_clear(color_t c) {
    unsigned int totalWidth = fb_get_width();
    unsigned int totalHeight = fb_get_height();
    unsigned int *im = (unsigned int *)fb_get_draw_buffer();
    unsigned int totalPix = totalWidth * totalHeight;
    
    for (int i = 0; i < totalPix; i++) {
        im[i] = c;
    }
}

//void gl_clear(color_t c) {
//    unsigned int totalWidth = fb_get_width();
//    unsigned int totalHeight = fb_get_height();
//    unsigned int (*im)[totalWidth] = fb_get_draw_buffer();

//    for(int y = 0; y < totalHeight; y++){
//        for(int x = 0; x < totalWidth; x++){
//            im[y][x] = c;
//        }
//    }

//}

color_t gl_read_pixel(int x, int y) {
    unsigned int per_row = fb_get_width();
    unsigned int (*im)[per_row] = fb_get_draw_buffer();
    if(x < 0){
	return 0;
    }

    if(y < 0){
	return 0;
    }

    if(x >= per_row){
	return 0;
    }

    if(y >= fb_get_height()){
	return 0; 
    }

    return (color_t)im[y][x];
}

// Sahan's Code
void gl_draw_rect(int x, int y, int w, int h, color_t c) {
    int WIDTH = gl_get_width();
    int HEIGHT = gl_get_height();
    
    color_t (*img)[WIDTH] = fb_get_draw_buffer();
    if (x < 0){
        w += x;
        x = 0;
        
    } 
    if (y < 0){
        h += y;
        y = 0;
    } 

    if (x + w > WIDTH) w = WIDTH - x;
    if (y + h > HEIGHT)  h = HEIGHT - y;
    if ((w < 0) || (h < 0)) return;

    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){
            img[y + j][x + i] = c;
        }
    }
}

void gl_draw_char(int x, int y, char ch, color_t c) {
    uint8_t buf[font_get_glyph_size()];
    if (!font_get_glyph(ch, buf, sizeof(buf))){
        return;             // no character found
    }
    // for our glyph
    int glyph_width = font_get_glyph_width();
    int glyph_height = font_get_glyph_height();
    uint8_t (*glyph)[glyph_width] = (uint8_t (*)[glyph_width])buf;      // buffer containing the glyph


    for (int j = 0; j < glyph_height; j++) {
        for (int i = 0; i < glyph_width; i++) {
            if (glyph[j][i] == 0xff){
                gl_draw_pixel(x + i, y + j, c);
            }
            
        }
    }

}

// End of Sahan's code

void gl_draw_string(int x, int y, const char* str, color_t c) { 
    for(int i = 0; i < strlen(str); i++){
	gl_draw_char(x + (i * gl_get_char_width()), y, str[i], c);
    }
}

int gl_get_char_height(void) {
    return font_get_glyph_height();
}

int gl_get_char_width(void) {
    return font_get_glyph_width();
}
