/* File: printf.c
 * --------------
 *  Mario Portillo
 *  CS 107 E
 * 
 * Functions:
 * const char *hex_string(unsigned long val)
 * const char *decimal_string(long val) 
 * static void sixteenBaseNum(unsigned long num, char *outstr)
 * static void tenBaseNum(unsigned long num, char *outstr)
 * void num_to_string(unsigned long num, int base, char *outstr) 
 * size_t padNumLength(const char *format, int startingIndex) 
 * size_t grabNumberPadding(const char *format, int startingIndex)
 * size_t paddingFunction(char *buf, size_t bufsize, const char *format, int startingIndex, const char* input)
 * int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)  
 * int snprintf(char *buf, size_t bufsize, const char *format, ...) 
 * int printf(const char *format, ...)  
 * 
 */
#include "printf.h"
#include <stdarg.h>
#include <stdint.h>
#include "strings.h"
#include "uart.h"

/* Prototypes for internal helpers.
 * Typically we would qualify these functions as static (private to module)
 * but in order to call them from the test program, must declare externally
 */
void num_to_string(unsigned long num, int base, char *outstr);
const char *hex_string(unsigned long val);
const char *decimal_string(long val);

// max number of digits in long + space for negative sign and null-terminator
#define MAX_DIGITS 25


/* Convenience functions `hex_string` and `decimal_string` are provided
 * to you.  You should use the functions as-is, do not change the code!
 *
 * A key implementation detail to note is these functions declare
 * a buffer to hold the output string and return the address of buffer
 * to the caller. If that buffer memory were located on stack, it would be
 * incorrect to use pointer after function exit because local variables
 * are deallocated. To ensure the buffer memory is accessible after
 * the function exists, the declaration is qualified `static`. Memory
 * for a static variable is not stored on stack, but instead in the global data
 * section, which exists outside of any function call. Additionally static
 * makes it so there is a single copy of the variable, which is shared by all
 * calls to the function. Each time you call the function, it overwrites/reuses
 * the same variable/memory.
 *
 * Adding static qualifier to a variable declared inside a function is a
 * highly atypical practice and appropriate only in very specific situations.
 * You will likely never need to do this yourself.
 * Come talk to us if you want to know more!
 * 
 * 
 */

/* From here to end of file is some sample code and suggested approach
 * for those of you doing the disassemble extension. Otherwise, ignore!
 *
 * The struct insn bitfield is declared using exact same layout as bits are organized in
 * the encoded instruction. Accessing struct.field will extract just the bits
 * apportioned to that field. If you look at the assembly the compiler generates
 * to access a bitfield, you will see it simply masks/shifts for you. Neat!
 */

static const char *reg_names[32] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                                    "s0/fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                                    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                                    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6" };
// R-type
struct insn  {
    uint32_t opcode: 7;
    uint32_t reg_d:  5;
    uint32_t funct3: 3;
    uint32_t reg_s1: 5;
    uint32_t reg_s2: 5;
    uint32_t funct7: 7;
};
// I-type
struct insn1  {
    uint32_t opcode: 7;
    uint32_t reg_d:  5;
    uint32_t funct3: 3;
    uint32_t reg_s1: 5;
    uint32_t imm: 12;
};
// S-type
struct insn2  {
    uint32_t opcode: 7;
    uint32_t imm1:  5;
    uint32_t funct3: 3;
    uint32_t reg_s1: 5;
    uint32_t reg_s2: 5;
    uint32_t imm2: 7;
};
// U-type
struct insn3  {
    uint32_t opcode: 7;
    uint32_t reg_d: 5;
    uint32_t imm: 20;
};

//This function takes in a value and outputs a hex string.
const char *hex_string(unsigned long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    num_to_string(val, 16, buf); // num_to_string does the hard work
    return buf;
}

//This function takes in a value and outputs a base 10 number in string format.  
const char *decimal_string(long val) {
    // static buffer to allow use after function returns (see note above)
    static char buf[MAX_DIGITS];
    if (val < 0) {
        buf[0] = '-';   // add negative sign in front first
        num_to_string((-1*val), 10, buf + 1); // pass positive val as arg, start writing at buf + 1
    } else {
        num_to_string(val, 10, buf);
    }
    return buf;
}

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
void num_to_string(unsigned long num, int base, char *outstr) {
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

// this helper function loops until it collectst the total cound of digits
size_t padNumLength(const char *format, int startingIndex){
    int numCount = 0;

    while (format[startingIndex] >= '0' && format[startingIndex] <= '9'){
        numCount++; 
        startingIndex++;
    }
    return numCount; 
}

// this helper function will convert the digits in our format to a str then to a number so we know how much to pad
size_t grabNumberPadding(const char *format, int startingIndex){
    size_t arrLength = padNumLength(format, startingIndex);
    char numStr[arrLength]; 
    int numStrIndex = 0; 
    //loops from start index onward until we have no more digits
    while (format[startingIndex] >= '0' && format[startingIndex] <= '9'){
        numStr[numStrIndex] = format[startingIndex];
        numStrIndex++; 
        startingIndex++; 
    }
    numStr[numStrIndex] = '\0'; 
    //converts the string version of our number to a number we can use to calculate in later functions. 
    return strtonum(numStr, NULL);
}

// This is the beafy function which will take the bulk of the load when dealing with padding cases
size_t paddingFunction(char *buf, size_t bufsize, const char *format, int startingIndex, const char* input){
    size_t PaddingSize = 0;
    
    if (startingIndex == 0){ //this is for when we simply have *p
        PaddingSize = 8; 
        startingIndex = 1; 
    }else { //every other pading case
        PaddingSize = grabNumberPadding(format, startingIndex); //otherwise we need to figure out our pad length
    }
   
    size_t inputLength = strlen(input);

    // build an "ideal" string which has padding + our str
    char unFilterArr[PaddingSize + inputLength + 1];
    memset(unFilterArr, 0, PaddingSize + inputLength + 1);

    //tells us how much padding to add
    size_t paddingAdded = 0;

    if ((int)PaddingSize - (int)inputLength > 0){
        paddingAdded = PaddingSize - inputLength;
    }

    //use some ptrs to keep track of where we are in the array
    char* startptr = unFilterArr; 
    char* currptr = unFilterArr;

    //gives us an offset so we can use that to determine if we need ' ' or '0'
    size_t offset = padNumLength(format, startingIndex); 

    //adds the ideal amount of padding
    while(paddingAdded--){
        if(format[startingIndex + offset] == 'p'||
            format[startingIndex + offset] == 'x' ||
            (format[startingIndex + offset] == 'l' && format[startingIndex + offset+ 1] == 'x')){
            char space[] = {'0'};
            memcpy(currptr, space , 1);
        }else{
            char space[] = {' '};
            memcpy(currptr, space , 1); 
        }
        
        currptr++;   
    }

    memcpy(currptr, input, inputLength); // grabs all the input string
    currptr += inputLength;
    *currptr = '\0';
    //at this point we have a very big string which contains all the padding and all our input string
    //this bottom line will take this and truncate it down to fit into the proper bufsize
    // 
    //          snprintf(buf, 5, "%7d", -9999) -> the buffer would be ' ' but for exp we will use 0s
    //          currptr above points an array = ['0', '0', '-', '9', '9', '9', '9']
    //          What the strlcat will do is take our current array and truncate it down to fit in the buffsize or
    //          if the buffsize allows for it then it will simply add our array.
    //
    return strlcat(buf, startptr, bufsize);
}

char* Utype(struct insn3 in){
    const char* rd1 = reg_names[in.reg_d];

    const char *immstr = decimal_string((long)in.imm);

    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    strlcat(result, "lui" , bufsize);
    
    strlcat(result, " " , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, immstr , bufsize);

    return result; 
}

char* Btype(struct insn2 in){
    const char* rd1 = reg_names[in.reg_s1];
    const char* rd2 = reg_names[in.reg_s2];

    char imstr[MAX_DIGITS];
    size_t sizeIM = sizeof(imstr);
    memset(imstr, 0, sizeIM); // init array contents with fixed repeat value
    memcpy(imstr, decimal_string((long)in.imm1), sizeIM);


    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    if(in.funct3 == 0x0){
        strlcat(result, "beq" , bufsize);
    }else if(in.funct3 == 0x1){
        strlcat(result, "bne" , bufsize);
    }else if(in.funct3 == 0x4){
        strlcat(result, "blt" , bufsize);
    }else if(in.funct3 == 0x5){
        strlcat(result, "bge" , bufsize);
    }else if(in.funct3 == 0x6){
        strlcat(result, "bltu" , bufsize);
    }else if(in.funct3 == 0x7){
        strlcat(result, "bgeu" , bufsize);
    }

    strlcat(result, " " , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, rd2 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, imstr , bufsize);

    return result; 
}

char* Stype(struct insn2 in){
    const char* rd1 = reg_names[in.reg_s1];
    const char* rd2 = reg_names[in.reg_s2];

    char imstr[MAX_DIGITS];
    size_t sizeIM = sizeof(imstr);
    memset(imstr, 0, sizeIM); // init array contents with fixed repeat value
    memcpy(imstr, decimal_string((long)in.imm1), sizeIM);

    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    if(in.funct3 == 0x0){
        strlcat(result, "sb" , bufsize);
    }else if(in.funct3 == 0x1){
        strlcat(result, "sh" , bufsize);
    }else if(in.funct3 == 0x2){
        strlcat(result, "sw" , bufsize);
    }

    strlcat(result, " " , bufsize);
    strlcat(result, rd2 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, imstr , bufsize);
    strlcat(result, "(" , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, ")" , bufsize);

    return result; 
}

char* Itype_1(struct insn1 in){
    const char* dest = reg_names[in.reg_d];
    const char* rd1 = reg_names[in.reg_s1];

    char imstr[MAX_DIGITS];
    size_t sizeIM = sizeof(imstr);
    memset(imstr, 0, sizeIM); // init array contents with fixed repeat value
    memcpy(imstr, decimal_string((long)in.imm), sizeIM);
    

    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    if(in.funct3 == 0x0){
        strlcat(result, "addi" , bufsize);
    }else if(in.funct3 == 0x4){
        strlcat(result, "xori" , bufsize);
    }else if(in.funct3 == 0x6){
        strlcat(result, "ori" , bufsize);
    }else if(in.funct3 == 0x7){
        strlcat(result, "andi" , bufsize);
    }else if(in.funct3 == 0x1){
        strlcat(result, "slli" , bufsize);
    }else if(in.funct3 == 0x5){
        strlcat(result, "srli" , bufsize);
    }else if(in.funct3 == 0x5){
        strlcat(result, "srai" , bufsize);
    }else if(in.funct3 == 0x2){
        strlcat(result, "slti" , bufsize);
    }else if(in.funct3 == 0x3){
        strlcat(result, "sltiu" , bufsize);
    }
    
    
    strlcat(result, " " , bufsize);
    strlcat(result, dest , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, imstr, bufsize);

    return result; 

}

char* Itype_2(struct insn1 in){
    const char* dest = reg_names[in.reg_d];
    const char* rd1 = reg_names[in.reg_s1];
    const char *immstr = decimal_string((long)in.imm);

    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    if(in.funct3 == 0x0){
        strlcat(result, "addi" , bufsize);
    }else if(in.funct3 == 0x1){
        strlcat(result, "xori" , bufsize);
    }else if(in.funct3 == 0x2){
        strlcat(result, "ori" , bufsize);
    }else if(in.funct3 == 0x4){
        strlcat(result, "andi" , bufsize);
    }else if(in.funct3 == 0x1){
        strlcat(result, "slli" , bufsize);
    }else if(in.funct3 == 0x5){
        strlcat(result, "srli" , bufsize);
    }
    
    strlcat(result, " " , bufsize);
    strlcat(result, dest , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, immstr , bufsize);

    return result; 
}

char* Rtype(struct insn in){
    const char* dest = reg_names[in.reg_d];
    const char* rd1 = reg_names[in.reg_s1];
    const char* rd2 = reg_names[in.reg_s2];
    
    static char result[20];
    size_t bufsize = sizeof(result);
    memset(result, 0, bufsize); //Init our inital string to have a bunch of '/0'

    if(in.funct3 == 0x0 && in.funct7 == 0x00){
        strlcat(result, "add" , bufsize);

    }else if(in.funct3 == 0x0 && in.funct7 == 0x20){
        strlcat(result, "sub" , bufsize);

    }else if(in.funct3 == 0x4 && in.funct7 == 0x00){
        strlcat(result, "xor" , bufsize);

    }else if(in.funct3 == 0x6 && in.funct7 == 0x00){
        strlcat(result, "or" , bufsize);

    }else if(in.funct3 == 0x7 && in.funct7 == 0x00){
        strlcat(result, "and" , bufsize);

    }else if(in.funct3 == 0x1 && in.funct7 == 0x00){
        strlcat(result, "sll" , bufsize);
    
    }else if(in.funct3 == 0x5 && in.funct7 == 0x00){
        strlcat(result, "srl" , bufsize);

    }else if(in.funct3 == 0x5 && in.funct7 == 0x20){
        strlcat(result, "sra" , bufsize);

    }else if(in.funct3 == 0x2 && in.funct7 == 0x00){
        strlcat(result, "sltu" , bufsize);
    }

    strlcat(result, " " , bufsize);
    strlcat(result, dest , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, rd1 , bufsize);
    strlcat(result, "," , bufsize);
    strlcat(result, rd2 , bufsize);
     
    return result; 
}

char* convertToAseembly(unsigned int* ptr){
    struct insn in =  *(struct insn *)ptr; 
    uint32_t opcode = in.opcode;
    
    if(opcode == 0x33){
        return Rtype(in);

    }else if(opcode == 0x13){
        struct insn1 in2 = *(struct insn1 *)ptr;  
        return Itype_1(in2);

    }else if(opcode == 0xB){
        struct insn1 in2 = *(struct insn1 *)ptr; 
        return Itype_2(in2);
    }else if(opcode == 0x186AB){
        struct insn2 in2 = *(struct insn2 *)ptr; 
        return Stype(in2);
    }else if (opcode == 0x10C8EB){
        struct insn2 in2 = *(struct insn2 *)ptr; 
        return Btype(in2);
    }else if (opcode == 0x1AE1F || opcode == 0x277F){
        struct insn3 in2 = *(struct insn3 *)ptr; 
        return Utype(in2);
    }

    return NULL;


}

//This function is the meat of this entire program. This function is responsible for taking in a buf, bufsize, format, and args.
//Based on the specs given to us in the assignment specific cases will lead to specific outputs being written to the buf. 
int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args) {
    size_t formatIndex = 0;
    if (bufsize > 0) {
        buf[0] = '\0';
    }
 
    size_t formatLen = strlen(format);
    size_t result = 0; //this keeps tracks of how many strings we would have added to the output 

    while(format[formatIndex] != '\0'){
        if(format[formatIndex] == '%' && formatIndex + 1 < formatLen && format[formatIndex + 1] == '%'){
            // Replace '%%' with a single '%'
            char newStr[] = "%";
            strlcat(buf, newStr, bufsize); 
            result++;
            formatIndex += 2;  // Skip past the '%%'

        }else if(format[formatIndex] == '%' &&  formatIndex + 1 < formatLen &&  format[formatIndex + 1] == 'c'){
            int charArg = va_arg(args, int); // access value of next argument, type is `char`
            char newStr[] = {charArg, '\0'};
            strlcat(buf, newStr, bufsize);
            result++;

            formatIndex += 2;  // Skip past the '%c' 

        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] == 's') {
            const char* stringArg = va_arg(args, char*);    
            // Use strlcat to safely concatenate the string
            strlcat(buf, stringArg, bufsize);
            result += strlen(stringArg);

            formatIndex += 2;  // Skip over '%s'

        }else if (format[formatIndex] == '%'  &&  formatIndex + 1 < formatLen &&  format[formatIndex + 1] == 'd') {
            int decArg = va_arg(args, int);
            const char *dec = decimal_string(decArg); 
            
            strlcat(buf, dec, bufsize);
            // Use strlcat to safely concatenate the string
            result += strlen(dec);
            formatIndex += 2;  // Skip over '%d'
        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] == 'p' 
                    && format[formatIndex + 2] == 'I'){

            unsigned int* ptr = va_arg(args, unsigned int*); //grabs the address of what we are looking for
            
            char* assStr = convertToAseembly(ptr);
            
            strlcat(buf, assStr, bufsize);
            result += strlen(assStr); 

            formatIndex += 3;  // Skip over '%pI'   
        
        }else if (format[formatIndex] == '%'  &&  formatIndex + 1 < formatLen &&  format[formatIndex + 1] == 'p') {
            void* ptr = va_arg(args, void*);
            
            const char *ptrString = hex_string((unsigned long)ptr);
             
            if((int)bufsize - (int)strlen(buf) <= 0){
                result += strlcat(buf, "0x", bufsize); //in this case we want to add
                                                              // this to our total because it won't be able to read it                                         
                result += paddingFunction(buf, 0, format, 0, ptrString);
            }else{
                strlcat(buf, "0x", bufsize);
                result += paddingFunction(buf, bufsize, format, 0, ptrString);
            }
            
            formatIndex += 2;  // Skip over '%p'

        
        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] == 'l' 
                && format[formatIndex + 2] == 'd') {
            long decArg = va_arg(args, long);
            const char *dec = decimal_string(decArg);
            strlcat(buf, dec, bufsize); 
            result += strlen(dec);
            
            formatIndex += 3;  // Skip over '%ld' 

        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] == 'x'){
            unsigned int hexArg = va_arg(args, unsigned int);
            const char *hex = hex_string(hexArg);
            
            strlcat(buf, hex, bufsize);
            result += strlen(hex);
            // Use strlcat to safely concatenate the string
            formatIndex += 2;  // Skip over '%x'

        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] == 'l' 
                    && format[formatIndex + 2] == 'x'){
            int hexArg = va_arg(args, int);
            const char *hex = hex_string(hexArg); 

            // Use strlcat to safely concatenate the string
            strlcat(buf, hex, bufsize); 
            result += strlen(hex);
            formatIndex += 3;  // Skip over '%xl'

        }else if (format[formatIndex] == '%' &&  formatIndex + 1 < formatLen && format[formatIndex + 1] >= '0' 
                && format[formatIndex + 1] <= '9'){
            size_t startingIndex = formatIndex + 1; //This is where the first digit starts
            size_t offset = padNumLength(format, formatIndex + 1);

            if(format[startingIndex + offset] == 'c'){
                
                int charArg = va_arg(args, int); // access value of next argument, type is `char`
                char newStr[] = {charArg, '\0'};
                // bufsize = bufsize - strlen(buf); 

                result = paddingFunction(buf, bufsize, format, startingIndex, newStr); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 's'){
                char* stringArg = va_arg(args, char*);    
                // Use strlcat to safely concatenate the string
                result = paddingFunction(buf, bufsize, format, startingIndex, stringArg); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 'l' && format[startingIndex + offset + 1] == 'd'){
                int decArg = va_arg(args, int);
                const char *dec = decimal_string(decArg);

                result = paddingFunction(buf, bufsize, format, startingIndex, dec); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 'l' && format[startingIndex + offset + 1] == 'x'){
                unsigned int hexArg = va_arg(args, unsigned int);
                const char *hex = hex_string(hexArg);

                result = paddingFunction(buf, bufsize, format, startingIndex, hex); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 'd'){
                int decArg = va_arg(args, int);
                const char *dec = decimal_string(decArg);

                result = paddingFunction(buf, bufsize, format, startingIndex, dec); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 'x'){
                volatile unsigned int hexArg = va_arg(args, unsigned int);
                const char *hex = hex_string(hexArg); 

                result = paddingFunction(buf, bufsize, format, startingIndex, hex); // this should do the padding and insert string

            }else if(format[startingIndex + offset] == 'p'){
                void* ptr = va_arg(args, void*);
            
                const char *ptrString = hex_string((unsigned long)ptr);
                
                if((int)bufsize - (int)strlen(buf) <= 0){
                    result += strlcat(buf, "0x", bufsize);
                    result += paddingFunction(buf, 0, format, startingIndex, ptrString);
                }else{
                    strlcat(buf, "0x", bufsize);
                    result = paddingFunction(buf, bufsize, format, startingIndex, ptrString);
                }
                
            }
            
            if(format[startingIndex + offset] == 'l' && format[startingIndex + offset + 1] == 'd'){
                formatIndex += 3 + padNumLength(format, formatIndex + 1); 
            }else if(format[startingIndex + offset] == 'l' && format[startingIndex + offset + 1] == 'x'){
                formatIndex += 3 + padNumLength(format, formatIndex + 1); 
            }else{
                formatIndex += 2 + padNumLength(format, formatIndex + 1);
            }
            
        }else { 
            char str[] = {format[formatIndex], '\0'};
            strlcat(buf, str, bufsize);
            formatIndex++;
            result++; 
        }
    }

    return result;
}

//This function acts a wrapper to then be passed off to vsnprintf
int snprintf(char *buf, size_t bufsize, const char *format, ...) {
    va_list args;
    va_start(args, format);  // initialize args to point to the variable arguments
    int result = vsnprintf(buf, bufsize, format, args);
    va_end(args);  // clean up
    return result;
}

// ok to assume printf output is never longer that MAX_OUTPUT_LEN
#define MAX_OUTPUT_LEN 1024

//This function similar to snprintf is a wrapper that is used by the client. The underlying functions are done by vsnprintf.
int printf(const char *format, ...) {
    char stackArr[MAX_OUTPUT_LEN]; 
    memset(stackArr, 0, MAX_OUTPUT_LEN); 
    
    va_list args;
    va_start(args, format);  // initialize args to point to the variable arguments
    int result = vsnprintf(stackArr, MAX_OUTPUT_LEN, format, args);
    va_end(args);  // clean up
    uart_putstring(stackArr);
    return result;
    
}
