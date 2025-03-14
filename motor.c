#include "pwm.h"
#include "gpio.h"
#include "timer.h"
#include "mcp3008.h"  // For joystick readings
#include "motor.h"
#include "printf.h"
#include "math_float.h"
#include "nrf.h"
#include "strings.h"

void motor_init(void) {
    pwm_init();  
    pwm_config_channel(PWM7, ENA_PIN, 10000, false); // 10kHz frequency for Motor A
    pwm_config_channel(PWM2, ENB_PIN, 10000, false); // 10kHz frequency for Motor B

    gpio_set_output(IN1_PIN);
    gpio_set_output(IN2_PIN);
    gpio_set_output(IN3_PIN);
    gpio_set_output(IN4_PIN);

}

// set Motor A direction (1 = Forward, 0 = Reverse)
static void motorA_set_direction(int dir) {
    if (dir) {
        gpio_write(IN1_PIN, 1);
        gpio_write(IN2_PIN, 0);
    } else {
        gpio_write(IN1_PIN, 0);
        gpio_write(IN2_PIN, 1);
    }
}

// set Motor B direction (1 = Forward, 0 = Reverse)
static void motorB_set_direction(int dir) {
    if (dir) {
        gpio_write(IN3_PIN, 1);
        gpio_write(IN4_PIN, 0);
    } else {
        gpio_write(IN3_PIN, 0);
        gpio_write(IN4_PIN, 1);
    }
}


// For the below code I am assuming
// motorA = left wheel
// motorB = right wheel
void drive_forward_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
       motorB_set_direction(1);
    }
}

void drive_reverse_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(0);
       motorB_set_direction(0);
    }
}

//This will turn in place and both wheels turn in opposite directions. 
void drive_spin_right_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
       motorB_set_direction(0);
    }
}

void drive_spin_left_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(0);
       motorB_set_direction(1);
    }
}

//These below turns are done using only one motor while the other is stationary
void drive_pivot_spin_right_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorA_set_direction(1);
    }
}

void drive_pivot_spin_left_time(unsigned long millisecond){
    unsigned long startTime = timer_get_ticks();
    unsigned long endTime = millisecond * 24000;  // Convert milliseconds to ticks

    while (timer_get_ticks() - startTime < endTime) {
       motorB_set_direction(1);
    }
}

//Below code is from Mario Portillo Printf.c internal 
//This helper function convers a long num to a hexadecimal str 
static void sixteenBaseNum(unsigned long num, char *outstr){

    if(num == 0){
        outstr[1] = '\0';
        outstr[0] = '0';
        return; 
    }
    
    unsigned long itterator = num;
    size_t size = 0; 

    while(itterator){
        itterator = itterator / 16;
        size++; 
    } 

   outstr[size] = '\0';
   size_t digitIndex = size - 1; // decrement to our true starting index

    while(num){
            unsigned long currHexVal = num % 16;
            num = num / 16;

            if (currHexVal < 10){
                outstr[digitIndex] = currHexVal + '0';
            }else{
                outstr[digitIndex] = currHexVal - 10 + 'a';
            }
            
            digitIndex--;
    } 
}   

//This helper function given a ten based digit will properly conver the number to a string in the 10 baes system. 
static void tenBaseNum(unsigned long num, char *outstr){
    size_t digitCount = 0;
    unsigned long itterator = num;

    while(itterator){
        itterator = itterator / 10;
        digitCount++; // collects the numeber of digits in our number
    } 
    
    //if we have 0 digits we simply add a zero and a '/0' == "0"
    if(num == 0){
        outstr[1] = '\0';
        outstr[0] = '0';
        return; 
    }
 
   //adds a '\0' to the end
    outstr[digitCount] = '\0';
    size_t i = digitCount - 1;
    
    // going from right to left we add the ones, tens, hundres, etc. 
    while(num){
            unsigned long onesPlace = num % 10;
            num = num / 10;
            outstr[i] = onesPlace + '0';
            i--;  
    }
}

//This function takes in a value provided by user then outputs a string format to the the outstr. 
static void num_to_string(unsigned long num, int base, char *outstr) {
    // a, b, c, d, f
    // [97,102]
    // num = [48,57]
    if (base != 10 && base != 16){
        return;
    }

    if(base == 10){
        tenBaseNum(num, outstr); 
    }else if(base == 16){
        sixteenBaseNum(num, outstr);
    }
}

static const char *decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[25];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        num_to_string((-1*val), 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        num_to_string(val, 10, buf);
    }
    return buf;
}

//End of Mario's Code.


// This below function must be continously called in order to work properly
// i.e a while loop.
void motor_control_from_joystick(void) {
    //int joystick_value = mcp3008_read_channel(0);  // read ADC (joystick is on ADC channel 0)
    // Convert ADC (0-1023) to duty cycle (0-100%)
    int x_value = mcp3008_read_channel(X_CHANNEL); // const X_CHANNEL defined in the header file
    int y_value = mcp3008_read_channel(Y_CHANNEL); // const Y_CHANNEL defined in the header file

    int speed = 0;     //(joystick_value * 100) / 1023; // -> needs to be between 0 and 100%
    motor_direction cur_dir = MOVE_FORWARD; // default direction is forward
    int startingRange = 515; //these values can be adjusted
    int noMotion = 30; // these values can be adjusted

    if (y_value > startingRange + noMotion) {  // Moving forward
    	speed = ((y_value - startingRange) * 100) / 511; //gives us a value between 0 and 100
    	cur_dir = MOVE_BACKWARD;

    }else if (y_value < startingRange - noMotion) {  // Moving backward
    	speed = ((startingRange - y_value) * 100) / 512;
    	cur_dir = MOVE_FORWARD;

    }else if (x_value > startingRange + noMotion) {  // Turning right
    	speed = ((x_value - startingRange) * 100) / 511;
    	cur_dir = TURN_LEFT;

    }else if (x_value < startingRange - noMotion) {  // Turning left
   	speed = ((startingRange - x_value) * 100) / 512;
    	cur_dir = TURN_RIGHT;
    } else {
	speed = 0;  // Neutral position
    }

    if(speed == 0){
	return; 
    }
    
    nrf24_init();
    uint8_t tx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    nrf24_set_tx_mode(tx_address, 10);
    

    switch(cur_dir){

    case MOVE_FORWARD:{
	//send a message	
    	uint8_t tx_data[32];
	tx_data[0] = '\0'; 
	strlcat((char *)tx_data, "Forward", 32);
	strlcat((char *)tx_data, " Speed: ", 32);
	//New string = "Forward Speed: "
	const char *dec = decimal_string(speed);
	strlcat((char *)tx_data, dec, 32);

        if (nrf24_transmit(tx_data)) {
            printf("%s \n", tx_data);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case MOVE_BACKWARD: {
	//send a message	
    	uint8_t tx_data[32];
	tx_data[0] = '\0'; 
	strlcat((char *)tx_data, "Backward", 32);
	strlcat((char *)tx_data, " Speed: ", 32);
	//New string = "Forward Speed: "
	const char *dec = decimal_string(speed);
	strlcat((char *)tx_data, dec, 32);

        if (nrf24_transmit(tx_data)) {
            printf("%s \n", tx_data);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case TURN_RIGHT: {
	//send a message	
    	uint8_t tx_data[32];
	tx_data[0] = '\0'; 
	strlcat((char *)tx_data, "Right", 32);
	strlcat((char *)tx_data, " Speed: ", 32);
	//New string = "Forward Speed: "
	const char *dec = decimal_string(speed);
	strlcat((char *)tx_data, dec, 32);

        if (nrf24_transmit(tx_data)) {
            printf("%s \n", tx_data);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    case TURN_LEFT: {
	//send a message	
    	uint8_t tx_data[32];
	tx_data[0] = '\0'; 
	strlcat((char *)tx_data, "Left", 32);
	strlcat((char *)tx_data, " Speed: ", 32);
	//New string = "Forward Speed: "
	const char *dec = decimal_string(speed);
	strlcat((char *)tx_data, dec, 32);

        if (nrf24_transmit(tx_data)) {
            printf("%s \n", tx_data);
        } else {
            printf("Transmission failed\n");
        }

        break;
    }
    default:
	printf("Invalid cur_dir: %d \n", cur_dir);
	//do nothing -> invalid cur_dir
	return;
    }
}
static int checkFirstChars(const uint8_t* rx_data, const uint8_t* direction, size_t size){
     for(int i = 0; i < size; i++){
	if(rx_data[i] != direction[i]){
		return 0;
	}
     }

     return 1; 
}

static unsigned int grabSpeed(uint8_t* rx_data){
     int max = strlen((const char*)rx_data);
     uint8_t *numPtr = NULL;
     for(int i = 0; i < max; i++){
	   if(rx_data[i] >= '0' && rx_data[i] <= '9'){
		numPtr = &rx_data[i];
		break; 
	   }
     }
     return strtonum((char *)numPtr, NULL);
}

void motorDriveRecieve (void){
    uint8_t rx_address[] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    uint8_t rx_data[32];
    memset(rx_data, 0x7E, 32);
    rx_data[31] = '\0';
    
    nrf24_init();
    nrf24_set_rx_mode(rx_address, 10);
    
    if (is_data_available(1)){
    	nrf24_receive(rx_data);
    }else{
	printf("Waiting for transmission...\n");
	return;
    }

    uint8_t Forward[] = "Forward Speed: ";
    uint8_t Backward[] = "Backward Speed: ";
    uint8_t Right[] = "Right Speed: ";
    uint8_t Left[] = "Left Speed: ";

    unsigned int speed = 0;
    if(checkFirstChars(rx_data, Forward, strlen((const char*)Forward)) == 0){
    	speed = grabSpeed(rx_data);
	pwm_set_duty(PWM7, speed);  // Set Motor A speed
    	pwm_set_duty(PWM2, speed);  // Set Motor B speed
	printf("I am going Forward, Speed: %d", speed);
    }else if(checkFirstChars(rx_data, Backward, strlen((const char*)Backward)) == 0){
    	speed = grabSpeed(rx_data);	
	pwm_set_duty(PWM7, speed);  // Set Motor A speed
    	pwm_set_duty(PWM2, speed);  // Set Motor B speed
	printf("I am going Backward, Speed: %d", speed);
    }else if(checkFirstChars(rx_data, Right, strlen((const char*)Right)) == 0){
    	speed = grabSpeed(rx_data);
	pwm_set_duty(PWM7, speed);  // Set Motor A speed
    	pwm_set_duty(PWM2, speed);  // Set Motor B speed
	printf("I am going Right, Speed: %d", speed);
    }else if(checkFirstChars(rx_data, Left, strlen((const char*)Left)) == 0){
    	speed = grabSpeed(rx_data);	
	pwm_set_duty(PWM7, speed);  // Set Motor A speed
    	pwm_set_duty(PWM2, speed);  // Set Motor B speed

	printf("I am going Left, Speed: %d", speed);
    }else{
	return;
    }
}
