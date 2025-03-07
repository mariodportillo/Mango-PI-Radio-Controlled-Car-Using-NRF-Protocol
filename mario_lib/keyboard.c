/* File: keyboard.c
 * -----------------
 *  Mario Portillo
 *  CS 107e
 */
#include "keyboard.h"
#include "ps2.h"
#include "printf.h"

static ps2_device_t *dev;

void keyboard_init(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    dev = ps2_new(clock_gpio, data_gpio);
}

uint8_t keyboard_read_scancode(void) {
    return ps2_read(dev);
}

key_action_t keyboard_read_sequence(void) {
    int firstKey = (int)keyboard_read_scancode();
    key_action_t action = {0, 0};
    
    //if the following read scandcode in the sequence is the same we know
    // it is a simple press.
    if(firstKey == 0xe0){
    	int secondKey = (int)keyboard_read_scancode();
	
	if(secondKey == 0xf0){
    	    int thirdKey = (int)keyboard_read_scancode();
	    action.what = KEY_RELEASE;
	    action.keycode = (uint8_t)thirdKey;
	}else{
	    action.what = KEY_PRESS;
	    action.keycode = (uint8_t)secondKey;
	}

    // If the scancode is a release code we assume the following reading will be
    // what is released.
    }else if(firstKey == 0xf0){
    	int secondKey = (int)keyboard_read_scancode();
	action.what = KEY_RELEASE;
	action.keycode = (uint8_t)secondKey;
    }else {
   	action.what = KEY_PRESS;
	action.keycode = (uint8_t)firstKey;
    }

    return action;
}

key_event_t keyboard_read_event(void) {
    key_event_t event = {0};
    key_action_t curAction = keyboard_read_sequence();
    uint8_t curKey = curAction.keycode;
    static keyboard_modifiers_t modifier = 0; // we init to be zero this static will hold our current modifier val

    while(1) {
        // Check if the current key is a modifier key
        if (curKey == 0x11) { // Alt modifier
            if (curAction.what == KEY_PRESS) {
                modifier |= KEYBOARD_MOD_ALT;  // Set the Alt modifier bit

            } else if (curAction.what == KEY_RELEASE) {
                modifier &= ~KEYBOARD_MOD_ALT; // Clear the Alt modifier bit
            }

        } else if (curKey == 0x12 || curKey == 0x59) { // Shift modifier
            if (curAction.what == KEY_PRESS) {
                modifier |= KEYBOARD_MOD_SHIFT;  // Set the Shift modifier bit

            } else if (curAction.what == KEY_RELEASE) {
                modifier &= ~KEYBOARD_MOD_SHIFT; // Clear the Shift modifier bit
            }

        } else if (curKey == 0x14) { // Control modifier
            if (curAction.what == KEY_PRESS) {
                modifier |= KEYBOARD_MOD_CTRL;  // Set the Control modifier bit

            } else if (curAction.what == KEY_RELEASE) {
                modifier &= ~KEYBOARD_MOD_CTRL; // Clear the Control modifier bit
            }

        } else if (curKey == 0x58) { // Caps Lock
            //this static varible helps keep track of the caps lock when being held down.
            // We only want to toggle the caps lock when we release and press the caps lock again.
            static int caps_lock_state = 0; 

            if (curAction.what == KEY_PRESS && caps_lock_state == 0) {
                modifier ^= KEYBOARD_MOD_CAPS_LOCK;  // Toggle the Caps Lock modifier bit
                caps_lock_state = 1;

            }else if(curAction.what == KEY_RELEASE){
                caps_lock_state = 0;
            }

            curAction = keyboard_read_sequence();
	        curKey = curAction.keycode;
            continue;

        } else { // we need to keep looping until we find a key press that is not a modifier.
            break; 
        } 
	    
        curAction = keyboard_read_sequence();
	    curKey = curAction.keycode;
    }

    event.action = curAction;
    event.key = ps2_keys[curKey];
    event.modifiers = modifier; 

    return event;
}

char keyboard_read_next(void) { 
    key_event_t curEvent = keyboard_read_event();
    key_action_t curAction = curEvent.action;
    ps2_key_t curKey = curEvent.key;
    keyboard_modifiers_t curModifier = curEvent.modifiers;
    
    while(curAction.what == KEY_RELEASE){
    	curEvent = keyboard_read_event();
    	curAction = curEvent.action;
    	curKey = curEvent.key;
    	curModifier = curEvent.modifiers;
    }
    char result = '\0';
    
    if(curModifier == KEYBOARD_MOD_SHIFT){
	    result = curKey.other_ch;
    
    }else if(curModifier == KEYBOARD_MOD_CAPS_LOCK){
        if(curKey.other_ch >= 'A' && curKey.other_ch <= 'Z'){
	        result = curKey.other_ch;
	    }else {
	        result = curKey.ch;
        } 
    
    }else if(curModifier == 0x9){ //when both shift and caps lock are pressed
        result = curKey.other_ch;
    
    }else{
	    result = curKey.ch; 
    } 

    return result;
}
