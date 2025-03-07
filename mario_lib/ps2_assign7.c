/* File: ps2_assign7.c
 * -------------------
 * Mario Portillo
 * CS 107e
 */
#include "ps2.h"
#include "ringbuffer.h"
#include "malloc.h"
#include "timer.h"
#include "gpio.h"
#include "gpio_extra.h"
#include "gpio_interrupt.h"
#include "interrupts.h"
#include "printf.h"
#include "ringbuffer.h"
#include "uart.h"

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
    uint8_t bitsRead;
    int numBits;
    int numOnBits;
    unsigned long previousTick;
    rb_t *rb;
};

uint8_t ps2_read(ps2_device_t *dev){
    while(rb_empty(dev->rb)){}
    return rb_dequeue(dev->rb);
}

void readBitInt(void *aux_data){
    if(!aux_data){
	    return; 
    }
    
    struct ps2_device* dev = (struct ps2_device*)aux_data;
    gpio_interrupt_clear(dev->clock);
    gpio_id_t data = dev->data;
    int curBit = gpio_read(data);
    
    unsigned long curTick = timer_get_ticks();
    unsigned long passingOftime = curTick - dev->previousTick;
    unsigned long microSeconds = passingOftime / 24;

    if(dev->previousTick != 0 && microSeconds >= 500){
        dev->bitsRead = 0;
        dev->numBits = 0;
        dev->numOnBits = 0;
    }

    if(!curBit && dev->numBits == 0){
        dev->numBits += 1;

    }else if(curBit && dev->numBits == 0){	
        dev->bitsRead = 0;
        dev->numBits = 0;
        dev->numOnBits = 0;

    }else if(dev->numBits > 0 && dev->numBits < 9){
        dev->bitsRead |= curBit << (dev->numBits - 1);
        dev->numBits += 1;

        if(curBit){
            dev->numOnBits++; 
        }

    }else if(dev->numBits == 9){ // we need to check for parity now
        
        if(dev->numOnBits % 2 == 1 && curBit == 1){ //wrong parity bit
            dev->bitsRead = 0;
            dev->numBits = 0;
            dev->numOnBits = 0;
            dev->previousTick = curTick;
            //we have an invalid parity bit so we reset everything.
            return;

        }else if(dev->numOnBits % 2 == 0 && curBit == 0){ //wrong parity bit
            dev->bitsRead = 0;
            dev->numBits = 0;
            dev->numOnBits = 0;
            dev->previousTick = curTick;
            //we have an invalid parity bit so we reset everything.
            return;
        }

        dev->numBits++;

    }else if(dev->numBits == 10){
        if(curBit == 1 && !rb_full(dev->rb)){
            //here we need to pass to the ring buffer
	        rb_enqueue(dev->rb, dev->bitsRead);
        }

	    //we reset everything
        dev->bitsRead = 0;
        dev->numBits = 0;
        dev->numOnBits = 0;
    }
    dev->previousTick = curTick;
}

// Creates a new PS2 device connected to given clock and data pins,
// The gpios are configured as input and set to use internal pull-up
// (PS/2 protocol requires clock/data to be high default)
ps2_device_t *ps2_new(gpio_id_t clock_gpio, gpio_id_t data_gpio) {
    // consider why must malloc be used to allocate device
    ps2_device_t *dev = malloc(sizeof(*dev));
    dev->previousTick = 0;
    dev->bitsRead = 0;
    dev->numBits = 0;
    dev->numOnBits = 0;
    dev->rb = rb_new();

    dev->clock = clock_gpio;
    gpio_interrupt_init();

    gpio_set_input(dev->clock);
    gpio_set_pullup(dev->clock);

    dev->data = data_gpio;
    gpio_set_input(dev->data);
    gpio_set_pullup(dev->data);


    gpio_interrupt_config(dev->clock, GPIO_INTERRUPT_NEGATIVE_EDGE, false);
    gpio_interrupt_register_handler(dev->clock, readBitInt, dev);
    
    
    gpio_interrupt_enable(dev->clock);
    return dev;
}
