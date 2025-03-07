/* File: backtrace.c
 * -----------------
 * ***** TODO: add your file header comment here *****
 */
#include "backtrace.h"
#include "mango.h"
#include "printf.h"
#include "symtab.h"

// helper function implemented in file backtrace_asm.s
extern unsigned long backtrace_get_fp(void);

int backtrace_gather_frames(frame_t f[], int max_frames) {
    volatile uintptr_t* currFp = (uintptr_t *)backtrace_get_fp();
    unsigned int result = 0; 
    
    while(currFp){
	if(result == max_frames){
	    break;
	}
    	
	volatile uintptr_t* currRa = currFp - 1; // we move down by 8 bytes  	
    	f[result].resume_addr = *currRa;
   	currFp = (uintptr_t*)*(currFp - 2);

	result++;
    }

    return result; 
}

void backtrace_print_frames(frame_t f[], int n) {
    char labelbuf[128];

    for (int i = 0; i < n; i++) {
        symtab_label_for_addr(labelbuf, sizeof(labelbuf), f[i].resume_addr);
        printf("#%d 0x%08lx at %s\n", i, f[i].resume_addr, labelbuf);
    }
}

void backtrace_print(void) {
    int max = 50;
    frame_t arr[max];

    int n = backtrace_gather_frames(arr, max);
    backtrace_print_frames(arr+1, n-1);   // print frames starting at this function's caller
}


long __stack_chk_guard = 0xFABE1234; 

void __stack_chk_fail(void)  {
    int max = 2;
    frame_t arr[max];

    backtrace_gather_frames(arr, max);

    char function[100];

    symtab_label_for_addr(function, 100, arr[1].resume_addr - 4);
    printf("*** FAILED: Stack Smashing detected at end of %s\n", function);
    
    mango_abort();
}
