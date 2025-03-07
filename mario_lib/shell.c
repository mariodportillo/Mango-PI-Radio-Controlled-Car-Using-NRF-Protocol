/* File: shell.c
 * -------------
 * Mario Portillo 
 * CS 107e
 */
#include "shell.h"
#include "shell_commands.h"
#include "uart.h"
#include "strings.h"
#include "malloc.h" 
#include "mango.h"
#include "ps2_keys.h"

#define LINE_LEN 80

// Module-level global variables for shell
static struct {
    input_fn_t shell_read;
    formatted_fn_t shell_printf;
} module;


// NOTE TO STUDENTS:
// Your shell commands output various information and respond to user
// error with helpful messages. The specific wording and format of
// these messages would not generally be of great importance, but
// in order to streamline grading, we ask that you aim to match the
// output of the reference version.
//
// The behavior of the shell commands is documented in "shell_commands.h"
// https://cs107e.github.io/header#shell_commands
// The header file gives example output and error messages for all
// commands of the reference shell. Please match this wording and format.
//
// Your graders thank you in advance for taking this care!


static const command_t commands[] = {
    {"help",  "help [cmd]",  "print command usage and description", cmd_help},
    {"echo",  "echo [args]", "print arguments", cmd_echo},
    {"clear", "clear",       "clear screen (if your terminal supports it)", cmd_clear},
    {"reboot", "reboot",     "reboot the Mango Pi", cmd_reboot},
    {"peek", "peek [addr]",  "print contents of memory at address", cmd_peek},
    {"poke", "poke [addr] [val]",  "store value into memory at address", cmd_poke},

};


//validates our  inputs to ensure we have a proper input
static int validateNumToStr(const char* str){
    int max = strlen(str);
    
    int i = 0;


    // Check for optional hex prefix
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2; // Skip the "0x" or "0X"
	if (i == max) return 0;
    }

    while(i < max){
	if ((str[i] < '0' || str[i] > '9') && (str[i] < 'A' || str[i] > 'F') && (str[i] < 'a' || str[i] > 'f')){
		return 0;
	}
	i++; 
    } 

    return 1;
}

int cmd_reboot (int argc, const char *argv[]){
    mango_reboot();
    return 0; 
}

int cmd_peek (int argc, const char *argv[]){
    if (argc < 2) {
        module.shell_printf("error: peek expects 1 argument [addr]\n");
        return -1;
    }
    
    if(validateNumToStr(argv[1]) == 0){
        module.shell_printf("error: peek cannot convert '%s'\n", argv[1]);
        return -1;
    }

    unsigned long hexValaddr = strtonum(argv[1], NULL);
    
    // here we do a check to make sure: A 4-byte value should only be read on address aligned to a 4-byte boundary (i.e. multiple of 4).
    if(hexValaddr % 4 != 0){
        module.shell_printf("error: peek address must be 4-byte aligned\n"); 
        return -1;
    }

    // if we have 0 return from strtonum then we know we have an invalid hexValaddr 
    if(hexValaddr == 0){
        module.shell_printf("error: peek cannot convert '%s'\n", argv[1]);
        return -1;
    } 

    uint32_t val = *(uint32_t *)hexValaddr;
    module.shell_printf("0x%8lx:    %8x\n", hexValaddr, val);
    
    return 0; 
}

int cmd_poke (int argc, const char *argv[]){
    if(argc < 3){
        module.shell_printf("error: poke expects 2 arguments [addr] and [val]\n");
        return -1;
    }

    unsigned long hexaddr = strtonum(argv[1], NULL);
    unsigned long hexVal = strtonum(argv[2], NULL);

    if(validateNumToStr(argv[1]) == 0){
        module.shell_printf("error: poke cannot convert '%s'\n", argv[1]);
        return -1;
    }else if(validateNumToStr(argv[2]) == 0){
        module.shell_printf("error: poke cannot convert '%s'\n", argv[2]);
        return -1;
    }

    // here we do a check to make sure: A 4-byte value should only be read on address aligned to a 4-byte boundary (i.e. multiple of 4).
    if(hexaddr % 4 != 0){
        module.shell_printf("error: poke address must be 4-byte aligned\n"); 
        return -1;
    }

    // if we have 0 return from strtonum then we know we have an invalid hexaddr 
    if(hexaddr == 0){
        module.shell_printf("error: poke cannot convert '%s'\n", argv[1]);
        return -1;
    }

    *(volatile uint32_t *)hexaddr = hexVal;  
    
    return 0;
}

int cmd_echo(int argc, const char *argv[]) {
    for (int i = 1; i < argc; ++i)
        module.shell_printf("%s ", argv[i]);
    module.shell_printf("\n");
    return 0;
}

int cmd_help(int argc, const char *argv[]) {

    if(argc == 1){ //when help is the only string
       module.shell_printf("help [cmd]         print command usage and description \n");
       module.shell_printf("echo [args]        print arguments \n");
       module.shell_printf("reboot             reboot the Mango Pi\n");
       module.shell_printf("peek [addr]        print contents of memory at address\n");
       module.shell_printf("poke [addr] [val]  store value into memory at address\n");
       return 0;
    }

    if(strcmp(argv[1], "help") == 0){
       module.shell_printf("help [cmd]         print command usage and description \n");
       return 0; 
    }else if(strcmp(argv[1], "echo") == 0){
       module.shell_printf("echo [args]        print arguments \n");
       return 0; 
    }else if(strcmp(argv[1], "reboot") == 0){
       module.shell_printf("reboot             reboot the Mango Pi\n");
       return 0; 
    }else if(strcmp(argv[1], "peek") == 0){
       module.shell_printf("peek [addr]        print contents of memory at address\n");
       return 0; 
    }else if(strcmp(argv[1], "poke") == 0){
       module.shell_printf("poke [addr] [val]  store value into memory at address\n");
       return 0; 
    }

    module.shell_printf("error: no such command '%s'\n", argv[1]);
    return -1;
}

int cmd_clear(int argc, const char* argv[]) {
    module.shell_printf("\f");   // formfeed character
    return 0;
}

void shell_init(input_fn_t read_fn, formatted_fn_t print_fn) {
    module.shell_read = read_fn;
    module.shell_printf = print_fn;
}

void shell_bell(void) {
    uart_putchar('\a');
}

static void back_space(char buf[]){
    size_t index = strlen(buf);
    buf[index] = '\0';
    module.shell_printf("%c", '\b');
    module.shell_printf("%c", ' ');
    module.shell_printf("%c", '\b'); 
}

void shell_readline(char buf[], size_t bufsize) {
    input_fn_t rawInput = module.shell_read; 
    char input = rawInput(); 
    
    memset(buf, '\0', bufsize); // we init our buffer to be empty 
    size_t charsAdded = 0; 

    while(input != '\n'){
        if(charsAdded == (bufsize - 1)){
            shell_bell();
            break;
        }

        if (input <= 0x7f && input != '\b'){
            buf[charsAdded++] = input;
            module.shell_printf("%c", input);
        }else if(input == '\b' && charsAdded != 0){
           back_space(buf);
           charsAdded--;  
        }else if(input == '\b' && charsAdded == 0){
           shell_bell(); 
        }
        input = rawInput(); 
    }
    module.shell_printf("\n");
    buf[charsAdded] = '\0'; // Ensure buffer is null-terminated

}

static char *strndup(const char *src, size_t n) {
    char* result = malloc((n + 1) * sizeof(char));
    memcpy(result, src, n);
    result[n] = '\0';
    return result;
}

static bool isspace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int tokenize(const char *line, char *array[],  int max) {
    int ntokens = 0;
    const char *cur = line;

    while (ntokens < max) {
        while (isspace(*cur)) cur++;    // skip spaces (stop non-space/null)
        if (*cur == '\0') break;        // no more non-space chars
        const char *start = cur;
        
        while (*cur != '\0' && !isspace(*cur)) cur++; // advance to end (stop space/null)

        array[ntokens++] = strndup(start, cur - start);   // make heap-copy, add to array        
    }
    return ntokens;
}

int shell_evaluate(const char *line) {
    char *arrOfStr[LINE_LEN];
    int totalTokens = tokenize(line, arrOfStr, LINE_LEN);
    int result = -1; // assume failure


    //we need to make sure we have a valid number of tokens
    if(totalTokens <= 0){
        return -1; //we can immediatly return -1 and output an error message.  
    }

    size_t totalCommands = sizeof(commands) / sizeof(commands[0]); // use this for our total number of commands in our array
    
    int counter = 0; 
    for(int i = 0; i < totalCommands; i++){
        if (strcmp(arrOfStr[0], commands[i].name) == 0) { 
            result = commands[i].fn(totalTokens, (const char **)arrOfStr);
            break;
        }
	counter++; 
    }

    if(counter == totalCommands && result == -1){
        module.shell_printf("error: no such command '%s'\n", line);
    } 
    


    // This below funciton is necessary to ensure we do not have data leaks
    for (int i = 0; i < totalTokens; i++) {
        free(arrOfStr[i]);
    }

    return result;
}

void shell_run(void) {
    module.shell_printf("Welcome to the CS107E shell.\nRemember to type on your PS/2 keyboard!\n");
    while (1) {
        char line[LINE_LEN];

        module.shell_printf("Pi> ");
        shell_readline(line, sizeof(line));
        shell_evaluate(line);
    }
}
