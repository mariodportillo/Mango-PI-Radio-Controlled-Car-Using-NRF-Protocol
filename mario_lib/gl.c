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

void gl_draw_rect(int x, int y, int w, int h, color_t c) {
   
   int x_end = x + w;
   int y_end = y + h;
 
   for(int curY = y; curY < y_end; curY++ ){
	for(int curX = x; curX < x_end; curX++){
	   gl_draw_pixel(curX,curY,c); 
	}
   }
}

void gl_draw_char(int x, int y, char ch, color_t c) {

   int h = font_get_glyph_height();
   int w = font_get_glyph_width(); 

   uint8_t buf[w*h];

   if(!font_get_glyph(ch, buf, sizeof(buf))){
	return;
   }
   
   uint8_t secondBuf[h][w];
   memcpy(secondBuf, buf, sizeof(buf));

   for(int curY = 0; curY < h; curY++){
        for(int curX = 0; curX < w; curX++){
	    if(secondBuf[curY][curX] == 0xff){
	         gl_draw_pixel(curX + x, curY + y, c);
	    }
	}
   }
}

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
