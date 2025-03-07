/* File: ps2_assign5.c
 * -------------------
 *  Mario Portillo
 *  CS 107e
 */
#include "gpio.h"
#include "gpio_extra.h"
#include "malloc.h"
#include "ps2.h"
#include "timer.h"
#include "printf.h"

// A ps2_device is a structure that stores all of the state and information
// needed for a PS2 device. The clock field stores the gpio id for the
// clock pin, and the data field stores the gpio id for the data pin.
// Read ps2_new for example code that sets and uses these fields.
//
// You may extend the ps2_device structure with additional fields as needed.
// A pointer to the current ps2_device is passed into all ps2_ calls.
// Storing state in this structure is preferable to using global variables:
// it allows your driver to support multiple PS2 devices accessed concurrently
// (e.g., a keyboard and a mouse).
//
// This definition fills out the structure declared in ps2.h.
struct ps2_device {
    gpio_id_t clock;
    gpio_id_t data;
};

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)
ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));

    dev->clock = clock_gpio;
    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);
    return dev;
}


static void wait_for_falling_clock_edge(gpio_id_t clock) {
    while (gpio_read(clock) != 1) {} // wait until clock is high
    while (gpio_read(clock) == 1) {} // wait until clock is low
}

static int readBit(gpio_id_t clock, gpio_id_t data){
    static unsigned long startTime = 0;
    unsigned long curTime = timer_get_ticks();

    unsigned long passingOftime = curTime - startTime; 
    unsigned long microSeconds = passingOftime / 24;

    if (startTime != 0 && microSeconds >= 500) {
        startTime = curTime;
        return -1; 
    }

    wait_for_falling_clock_edge(clock);
    startTime = timer_get_ticks();

    return gpio_read(data);
}

static int countOneBits(int bits){
    unsigned int count = 0;
    while (bits) {
        count += bits & 1;
        bits >>= 1;
    }
    return count;
}

//checks for the proper parity to determine what the 
static int parityCheck(int bits){
    if (countOneBits(bits) % 2 == 0){
	return 1; //this represents what the parity bit should be
    }else{
	return 0; //this represents what the parity bit should be 
    }
}

uint8_t ps2_read(ps2_device_t *dev) {
    uint8_t result = 0; 
    
    for(int i = 0; i < 11; i++){
	    //This will return our bit;
	    //Least significant to most significant
        int curReadBit = readBit(dev->clock, dev->data); 

	    //if the curReadBit is ever -1 we reset immediatley
	    if (curReadBit == -1){
            i = -1;
            result = 0;
            continue;
        }
	
        //if we get an invalid start bit we restart.
        if(i == 0 && curReadBit == 1){
            i = -1;
            result = 0;
            continue;
        }

        // we only want to return the 8 data bits. 
        if (i > 0  && i < 9){
            result |= curReadBit << (i-1);
        }

        //This below code is very faulty needs to be checked and tested. 
        if(i == 9 && parityCheck(result) != curReadBit){	
            i = -1;
            result = 0;
            continue;
        }

        if(i == 10 && curReadBit != 1){
            i = -1;
            result = 0;
            continue;
        }
    }

    return result;
}

