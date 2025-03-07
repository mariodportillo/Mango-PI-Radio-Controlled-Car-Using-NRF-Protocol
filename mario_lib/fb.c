/* File: fb.c
 * ----------
 * Mario Portillo
 * CS 107E
 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"

// module-level variables, you may add/change this struct as you see fit
static struct {
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
    void *framebuffers[2];     // address of framebuffer memory
    int curIndex;
    int bufferMode;
} module = { //this should init our module to have nothing at first
    .width = 0,
    .height = 0,
    .depth = 4,
    .framebuffers = {NULL, NULL},
    .curIndex = 0,
    .bufferMode = 0 //0 means single buffer
};

void fb_init(int width, int height, fb_mode_t mode) { 
    //if the either framebuffer is in use it needs to be gone before proceeding
    if(module.framebuffers[0]){
    	free(module.framebuffers[0]);
    	module.framebuffers[0] = NULL;
    }

    if(module.framebuffers[1]){
	free(module.framebuffers[1]);
    	module.framebuffers[1] = NULL;
    }


    module.width = width;
    module.height = height;
    int nbytes = module.width * module.height * module.depth;

    //We alloc two so that we have two buffers. 
    module.framebuffers[0] = malloc(nbytes);
    memset(module.framebuffers[0], 0x0, nbytes);
    
    if(mode == FB_DOUBLEBUFFER){
        module.framebuffers[1] = malloc(nbytes);
        memset(module.framebuffers[1], 0x0, nbytes);
	module.bufferMode = 1;
	module.curIndex = 1;
    }else {
	module.curIndex = 0;
	module.bufferMode = 0;
    }
    

    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
     
    de_set_active_framebuffer(module.framebuffers[module.curIndex]);
}

int fb_get_width(void) {
    return module.width;
}

int fb_get_height(void) {
    return module.height;
}

int fb_get_depth(void) {
    return module.depth;
}

void* fb_get_draw_buffer(void){
    if(!module.bufferMode){
    	return module.framebuffers[0];
    }

    if(module.curIndex == 1){
    	return module.framebuffers[0];
    }

    return module.framebuffers[1];
}

void fb_swap_buffer(void) {
    // We should only be doing this function if
    // we are in FB_DOUBLEBUFFER;
    if(module.bufferMode == 0){
         return;
    }

    if(module.curIndex == 1){
	    module.curIndex = 0;
    }else{
	    module.curIndex = 1;
    }

    de_set_active_framebuffer(module.framebuffers[module.curIndex]);
}
