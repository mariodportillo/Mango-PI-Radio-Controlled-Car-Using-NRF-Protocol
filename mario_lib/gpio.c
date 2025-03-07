/* File: gpio.c
 * ------------
 * Mario Portillo
 * CS 107E
 * 
 * bool gpio_id_is_valid(gpio_id_t pin)
 * static volatile unsigned int *get_cfg0_reg(unsigned int group) 
 * static volatile unsigned int *get_data_reg(unsigned int group)
 * 
 */
#include "gpio.h"
#include <stddef.h>

enum { GROUP_B = 0, GROUP_C, GROUP_D, GROUP_E, GROUP_F, GROUP_G };

typedef struct  {
    unsigned int group;
    unsigned int pin_index;
} gpio_pin_t;

// The gpio_id_t enumeration assigns a symbolic constant for each
// in such a way to use a single hex constant. The more significant
// hex digit identifies the group and lower 2 hex digits are pin index:
//       constant 0xNnn  N = which group,  nn = pin index within group
//
// This helper function extracts the group and pin index from a gpio_id_t
// e.g. GPIO_PB4 belongs to GROUP_B and has pin_index 4
static gpio_pin_t get_group_and_index(gpio_id_t gpio) {
    gpio_pin_t gp;
    gp.group = gpio >> 8;
    gp.pin_index = gpio & 0xff; // lower 2 hex digits
    return gp;
}

// The gpio groups are differently sized, e.g. B has 13 pins, C only 8.
// This helper function confirms that a gpio_id_t is valid (group
// and pin index are valid)
bool gpio_id_is_valid(gpio_id_t pin) {
    gpio_pin_t gp = get_group_and_index(pin);
    switch (gp.group) {
        case GROUP_B: return (gp.pin_index <= GPIO_PB_LAST_INDEX);
        case GROUP_C: return (gp.pin_index <= GPIO_PC_LAST_INDEX);
        case GROUP_D: return (gp.pin_index <= GPIO_PD_LAST_INDEX);
        case GROUP_E: return (gp.pin_index <= GPIO_PE_LAST_INDEX);
        case GROUP_F: return (gp.pin_index <= GPIO_PF_LAST_INDEX);
        case GROUP_G: return (gp.pin_index <= GPIO_PG_LAST_INDEX);
        default:      return false;
    }
}

// This helper function is suggested to return the address of
// the config0 register for a gpio group, i.e. get_cfg0_reg(GROUP_B)
// Refer to the D1 user manual to learn the address the config0 register
// for each group. Be sure to note how the address of the config1 and
// config2 register can be computed as relative offset from config0.
// (okay to discard this function if it doesn't fit with your design)
static volatile unsigned int *get_cfg0_reg(unsigned int group) {
    
    if(group == GROUP_B){
	    volatile unsigned int *PB_CFG0 = (unsigned int *)0x2000030;
    	return PB_CFG0; 
   
    }else if(group == GROUP_C){
	    volatile unsigned int *PC_CFG0 = (unsigned int *)0x2000060;
        return PC_CFG0;
   
    }else if(group == GROUP_D){
        volatile unsigned int *PD_CFG0 = (unsigned int *)0x2000090;
        return PD_CFG0;
   
    }else if(group == GROUP_E){
        volatile unsigned int *PE_CFG0 = (unsigned int *)0x20000C0;
        return PE_CFG0;
    
    }else if(group == GROUP_F){
        volatile unsigned int *PF_CFG0 = (unsigned int *)0x20000F0;
        return PF_CFG0;
    }
    
    else if(group == GROUP_G){
        volatile unsigned int *PG_CFG0 = (unsigned int *)0x2000120;
        return PG_CFG0;
    }

    return NULL;
}

// This helper function is suggested to return the address of
// the data register for a gpio group. Refer to the D1 user manual
// to learn the address of the data register for each group.
// (okay to discard this function if it doesn't fit with your design)
static volatile unsigned int *get_data_reg(unsigned int group) {
    if(group == GROUP_B){
        volatile unsigned int *PB_CFG0 = (unsigned int *)0x2000040;
        return PB_CFG0;
   
    }else if(group == GROUP_C){
        volatile unsigned int *PC_CFG0 = (unsigned int *)0x2000070;
        return PC_CFG0;
   
    }else if(group == GROUP_D){
        volatile unsigned int *PD_CFG0 = (unsigned int *)0x20000A0;
        return PD_CFG0;
   
    }else if(group == GROUP_E){
        volatile unsigned int *PE_CFG0 = (unsigned int *)0x20000D0;
        return PE_CFG0;
    
    }else if(group == GROUP_F){
        volatile unsigned int *PF_CFG0 = (unsigned int *)0x2000100;
        return PF_CFG0;
    }
    
    else if(group == GROUP_G){
        volatile unsigned int *PG_CFG0 = (unsigned int *)0x2000130;
        return PG_CFG0;
    }

    return NULL;
}

// This was implemented with the help of Ben -> TA
static volatile unsigned int *get_pull_reg(unsigned int group) {
    long offset = 0x30 * group;
    long base = 0x02000000 + 0x0054;
    long total = base + offset;

    volatile unsigned int *PB_CFG0 = (unsigned int *)total;
    return PB_CFG0;
}
// Attempted gpio_set_pullup() function. This function should be working
// as it was tested and unfortunatley ran out of time to implement a fully
// functional output device using the numpad. 

void gpio_set_pullup(gpio_id_t pin){
    // checks for vaid pin and makes sure value is correct
    if (!gpio_id_is_valid(pin)){
    	return;
    }
    gpio_pin_t validPin = get_group_and_index(pin);
    volatile unsigned int *pul = get_pull_reg(validPin.group); //grabs this groups data_reg

    // if ever this is a nullptr we need to get out IMMEDIATLY!
    if (!pul){
	    return;	
    } 

    // we need to check to make sure we don't have to switch PULL locations
    if (validPin.pin_index > 16){ // if the pin index is greater 16 we know we are in pul1  
        pul += 1;  // pul register 1
    }

    // we first want to clear whatever was there in memory because we want to input our fucnction
    unsigned int adder = 0b11 << ((validPin.pin_index % 16) * 2); 
    *pul = *pul & (~adder); // this will clear the pullreg at the desired pin index. 


    adder = 0b01 << ((validPin.pin_index % 16) * 2); 
    *pul = *pul | adder;  

}


void gpio_init(void) {
    // no initialization required for this peripheral
}

void gpio_set_input(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_INPUT);
}

void gpio_set_output(gpio_id_t pin) {
    gpio_set_function(pin, GPIO_FN_OUTPUT);
}

/*
 * 
 * The set function is responsible for ensuring the
 * proper GPIO is set to a specific function given by the user.
 * The affect of running this function is the the GPIO pin
 * should be set to a specific function specified by the paramters
 * 
 */
void gpio_set_function(gpio_id_t pin, unsigned int function) {
    // Checks to make sure we have a valid function and gpio_id
    if (!gpio_id_is_valid(pin)){
    	return;	
    }else if((function != GPIO_FN_INPUT) && (function != GPIO_FN_OUTPUT) && 
       (function != GPIO_FN_ALT2) && (function != GPIO_FN_ALT3) &&
       (function != GPIO_FN_ALT4) && (function != GPIO_FN_ALT5) &&
       (function != GPIO_FN_ALT6) && (function != GPIO_FN_ALT7) &&
       (function != GPIO_FN_ALT8) && (function != GPIO_FN_INTERRUPT) &&
       (function != GPIO_FN_DISABLED)){
    	return;
    }

    // grabs our gpio_pin info
    gpio_pin_t validPin = get_group_and_index(pin);
    volatile unsigned int *cfg0 = get_cfg0_reg(validPin.group); //grabs this groups cfg0
    // first we want to clear the bits in our desired location
    if (!cfg0){
	    return; 
    }

    // we need to check to make sure we don't have to switch CFG locations
    if (validPin.pin_index >= 8 && validPin.pin_index < 16){ // if the pin index is greater than 8 and less than 16 we know we are in CFG1
        cfg0 += 1;  // cfg1 
    } else if (validPin.pin_index >= 16 && validPin.pin_index < 24){ // if the pin index is greater than 16 and less than 24 we know we are in CFG2
        cfg0 += 2;  // cfg2 
    }

    // we first want to clear whatever was there in memory because we want to input our fucnction
    unsigned int adder = 0b1111 << ((validPin.pin_index % 8) * 4); 
    *cfg0 = *cfg0 & (~adder); // this will clear the cfg at the desired pin index. 

    // output is just 0000 so we can simply quit early
    if (function == GPIO_FN_INPUT){
        return;
    }

    // for any valid function type we will change the config at that pin to the function code
    // i.e 0001 is our output function -> we put this to our config and make that change
    adder = function << ((validPin.pin_index % 8) * 4);
    *cfg0 = *cfg0 | adder;  
}

unsigned int gpio_get_function(gpio_id_t pin) {
    // we always want to mkae sure we have a valid pin before doing anything
    if (!gpio_id_is_valid(pin)){
	    return GPIO_INVALID_REQUEST;	
    }

    gpio_pin_t validPin = get_group_and_index(pin);
    volatile unsigned int *cfg0 = get_cfg0_reg(validPin.group); //grabs this groups cfg0
   
    // if we accidently grab a nullptr we need to stop what we are doing
    if (!cfg0){
	    return GPIO_INVALID_REQUEST;	
    }

    // Similar to the set function we need to make sure we are on the correct CFG by doing some math and checking bounds
    if (validPin.pin_index >= 8 && validPin.pin_index < 16){
        cfg0 += 1;  // cfg1 
    } else if (validPin.pin_index >= 16 && validPin.pin_index < 24){
        cfg0 += 2;  // cfg2 
    }

    // to check the pin function our bit mask needs to be the right length for our cfg
    // we are guranteed it will be 4 bits long so we can multiply the pin_index by 4 to get to the right place
    // we need modulo here in order to account for the fact certain groups have more than 8 pins therefore
    // our multiplier value must be adjusted for being a in a new cfg
    // exp. GPIO_PB12 -> pin_index = 13 > 8. If we apply modulo to  13 we get 5 which is our correct multiplier to get
    // to the correct spot in cfg1 (cfg1 because 8 =< pin_index < 16).

    unsigned int adder = 0b1111 << ((validPin.pin_index % 8) * 4);  

    // we return the value that is at the desired place in memory 
    // whatever binary value returned should equal one of our functions
    return (*cfg0 & adder) >> ((validPin.pin_index % 8) * 4);
}

void gpio_write(gpio_id_t pin, int value) {
    // checks for vaid pin and makes sure value is correct
    if (!gpio_id_is_valid(pin)){
    	return;	
    }else if((value != 1) && (value != 0)) {
	    return;
    }

    gpio_pin_t validPin = get_group_and_index(pin);
    volatile unsigned int *dat = get_data_reg(validPin.group); //grabs this groups data_reg
    
    //clears the bit at the position we care about 
    unsigned int adder = 1 << (validPin.pin_index);
    
    // if ever this is a nullptr we need to get out IMMEDIATLY!
    if (!dat){
	    return;	
    } 

    // we dereference and use our adder to properly clear the bit at the desired place in DAT
    *dat = *dat & (~adder);
    
    // only need to add a 1 if necessary -> value == 1
    if(value == 1){
    	*dat = *dat | adder;
    } 
}

int gpio_read(gpio_id_t pin) {

    // checks for vaid pin and makes sure value is correct
    if (!gpio_id_is_valid(pin)){
    	return GPIO_INVALID_REQUEST;
    }

    gpio_pin_t validPin = get_group_and_index(pin);
    volatile unsigned int *dat = get_data_reg(validPin.group); //grabs this groups data_reg
    
    // if ever this is a nullptr we need to get out IMMEDIATLY!
    if (!dat){
	    return GPIO_INVALID_REQUEST;	
    }

    // bit mask to desired location in memory
    unsigned int adder = 1 << (validPin.pin_index);

    // The below variable will extract the int value of *dat & adder
    // This is useful because if this value is on it will give us 1
    // if this value is off at the desired location it will be 0
    unsigned int rawVal = *dat & adder; 
    
    // Below is using truthy values
    if (rawVal){ // if the binary value is not 0 then this should return 1
	    return 1;
    }

    return 0;

}

/*
void gpio_set_pullup(gpio_id_t pin) {

}
*/
