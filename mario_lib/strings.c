/* File: strings.c
 * ---------------
 *  Mario Portillo
 *  CS 107 E
 * This file deals with a lot of the under the hood
 * string functions necessary to complete a proper printf.
 * 
 * Functions:
 * void *memcpy(void *dst, const void *src, size_t n)
 * void *memset(void *dst, int val, size_t n)
 * size_t strlen(const char *str) 
 * int strcmp(const char *s1, const char *s2)
 * size_t strlcat(char *dst, const char *src, size_t dstsize)
 * static unsigned long PWR(int exp, int base){ 
 * static unsigned long PWR(int exp, int base){
 * static size_t findHexNums(const char *str, const char **endptr){
 * static unsigned long hexConversion(const char *str, const char **endptr) 
 * static size_t findNums(const char *str, const char **endptr){
 * unsigned long strtonum(const char *str, const char **endptr) 
 */ 

#include "strings.h"
#include "printf.h"


//This function here can copy N bytes from a srce and place it into the desitination
void *memcpy(void *dst, const void *src, size_t n) {
    /* Copy contents from src to dst one byte at a time */
    char *d = dst;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

//This funciton will replace N bytes of data in a destination with an int val.
void *memset(void *dst, int val, size_t n) {
    char *result = dst;
    int oneByte = 0xFF; 
    
    // checks for nullptr
    if (!result){
        return NULL;
    }

    while(n--){
      // use bit mask to only input each byte
      *result = val & oneByte;
       result++;
    }

    return dst;
}

//This function will count the number of visable chars in a string (does not include '\0')
size_t strlen(const char *str) {
    /* Implementation a gift to you from lab3 */
    size_t n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}

//This function will compare all the chars in one source string to another
int strcmp(const char *s1, const char *s2) {
    if (!s1 || !s2) {
        return -1; // Return -1 if either string is NULL
    }
    
    size_t n = 0;

    // Compare character by character
    while (s1[n] != '\0' && s2[n] != '\0') {
        int compareVal1 = s1[n];
        int compareVal2 = s2[n];

        // Lexicographical comparison
        if (compareVal1 > compareVal2) {
            return 1; // s1 is greater than s2
        } else if (compareVal1 < compareVal2) {
            return -1; // s1 is less than s2
        }

        n++;
    }

    // If we exit the loop, one string might have finished before the other
    if (s1[n] != '\0') {
        return 1;  // s1 is longer than s2
    }
    
    if (s2[n] != '\0') {
        return -1; // s2 is longer than s1
    }

    return 0;
}

static size_t checkString(const char *dst, size_t dstsize) {
    size_t i;
    for (i = 0; i < dstsize; i++) {
        if (dst[i] == '\0') {
            return i;  // Found null terminator at index `i`
        }
    }

    return dstsize;  // dst is not null-terminated within `dstsize`
}

//This function will concatenate one src string to another dst string so long as the dstszie is big enough.
size_t strlcat(char *dst, const char *src, size_t dstsize) {
    if(!src){
        return 0;
    }

    if(!dst){
        return strlen(src); 
    }

    size_t dstCurLen = checkString(dst, dstsize);
    size_t srcLen = strlen(src);

    // If `dst` was not null-terminated, treat it as full
    if (dstCurLen == dstsize) {
        return dstsize + srcLen;  // `dst` is full, return required size
    }
      
    int MAX = dstsize - dstCurLen - 1; 
   
    if (MAX > srcLen) {
        MAX = srcLen;
    }

    if (MAX > 0) {
        memcpy(dst + dstCurLen, src, MAX);
        dst[dstCurLen + MAX] = '\0';
    }
    
    // return what I wanted not what it 
    return dstCurLen + srcLen;
}

// This function takes and exponent and a base to calculate the value of (base)^(exp).
static unsigned long PWR(int exp, int base){
    unsigned long result = 1;
    while(exp){
        result = result*base;
        exp--; 
    }
    return result; 
}

//This function takes in a str and searches for the total number of hexNums in a str after "0x"
static size_t findHexNums(const char *str, const char **endptr){
    // offset by 2 because 0 index = 0 and 1 index = x
    size_t n = 2; 
    size_t lastValidIndex = 1;

    while(str[n] != '\0'){
        // these bottom checks make sure only numbers or letters are used for calculation
        if (str[n] < '0'){
            if(endptr){
                *endptr = &str[n];
            } 
            return lastValidIndex;  
        }

        if (str[n] > '9' && str[n] < 'A'){
            if(endptr){
                *endptr = &str[n];
            }
            return lastValidIndex;  
        }

        if (str[n] > 'F' && str[n] < 'a') {
            if(endptr){
                *endptr = &str[n];
            }
            return lastValidIndex;  
        }

        if (str[n] > 'f') {
            if(endptr){
                *endptr = &str[n];
            }
            return lastValidIndex;  
        }
        n++;
        lastValidIndex++;
    }

    if(endptr){ 
        *endptr = &str[n];
    }

    return lastValidIndex;
}

//This function does the conversion of taking a hex string and making it into a true hex value
static unsigned long hexConversion(const char *str, const char **endptr) {
    //count from least significant to most significant
    size_t n = findHexNums(str, endptr); // -> should start at the least significant number
    unsigned long result = 0; 
    size_t exp = 0;

    while(str[n] != 'x'){
        char currentChar = str[n];
        
        // if the chars are lower case make them uppercase
        if(str[n] >= 'a' && str[n] <= 'f'){
            currentChar = str[n] - 87;
        }
        // checking to see if we are a digit or a letter
        if(str[n] >=  'A' && str[n] < 'G'){
            currentChar = str[n] - 55; // this value is the offset to get the proper value

        }else if(str[n] >= '0' && str[n] < ':'){
            currentChar = str[n] - 48; // this is another offset to make currentChar correct
        }

        result += (currentChar * PWR(exp, 16)); 
        exp++;
        n--; 
    }
    
    return result;
}

//This function takes in str and returns the total number of base 10 digits in the string. 
static size_t findNums(const char *str, const char **endptr){
    size_t n = 0; 
    size_t indexStart = 0;

    while(str[n] != '\0'){
        if (str[n] < '0' || str[n] > '9') {
            if(endptr){
                *endptr = &str[n];
            }
            return indexStart;
        }
        n++;
        indexStart++;  
    }
    
    if(endptr){ 
        *endptr = &str[n];
    }

    return indexStart;

}

//This function will take in a str and then output either the hex long or the decimal lonf of the string. 
unsigned long strtonum(const char *str, const char **endptr) {
    if(str[0] == '0' && str[1] == 'x'){   
        return hexConversion(str, endptr);
    }
    
    size_t n = findNums(str, endptr); // -> should start at the least significant number
    unsigned long result = 0; 
    size_t exp = 0;

    while(n--){
        char currentChar = str[n] - 48; // this is another offset to make currentChar correct 
        result += currentChar * PWR(exp, 10); 
        exp++; 
    }

    if(endptr){
        const char *ptr = &str[strlen(str) + 1];
        endptr = &ptr;
    }else{
        endptr = NULL; 
    }
        

    return result;
}
