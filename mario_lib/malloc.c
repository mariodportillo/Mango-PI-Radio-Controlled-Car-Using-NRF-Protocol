/* File: malloc.c
 * --------------
 * Mario Portillo 
 * CS 107e
 */


 /*
 * The code given below is simple "bump" allocator from lecture.
 * An allocation request is serviced by using sbrk to extend
 * the heap segment.
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You will replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "memmap.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

/*
 * Data variables private to this module used to track
 * statistics for debugging/validate heap:
 *    count_allocs, count_frees, total_bytes_requested
 */
static int count_allocs, count_frees, total_bytes_requested;

/*
 * The segment of memory available for the heap runs from &__heap_start
 * to &__heap_max (symbols from memmap.ld establish these boundaries)
 *
 * The variable cur_head_end is initialized to &__heap_start and this
 * address is adjusted upward as in-use portion of heap segment
 * enlarges. Because cur_head_end is qualified as static, this variable
 * is not stored in stack frame, instead variable is located in data segment.
 * The one variable is shared by all and retains its value between calls.
 */
//requires 8 bytes of memory -> 4 bytes per int
struct header {
    int payload_size;
    int status;       // 0 if free, 1 if in use
};

// Call sbrk to enlarge in-use heap area
void *sbrk(size_t nbytes) {
    // __heap_start, __heap_max symbols are defined in linker script memmap.ld
    static void *cur_heap_end =  &__heap_start;     // IMPORTANT: static

    void *new_heap_end = (char *)cur_heap_end + nbytes;
    if (new_heap_end > (void *)&__heap_max)    // if request would extend beyond heap max
        return NULL;                // reject
    
    void *prev_heap_end = cur_heap_end;
    cur_heap_end = new_heap_end;
    
    return prev_heap_end;
}

//This should take a pyloadptr and move backwards to the header.
struct header* payload_to_header(void *payloadptr){
    return (struct header *)((char *)payloadptr - 8); 
}

//This function should take a header and return the ptr to start of the payload. 
void* header_to_payload(struct header *currHeader ){
    return (void *)((char *)currHeader + 8); 
}

//This function should take in a header and give you the next header. 
struct header* header_to_header(struct header *currHeader ){
    return (struct header *)((char *)currHeader + currHeader->payload_size + 8);
}

//Adds a header within an in use heap location if there is enough excess to make a new header. 
void addHeader(struct header* currHeader,size_t newNbytes, size_t nbytesDifference){
    struct header *init_header = (struct header*)((char *)header_to_payload(currHeader) + newNbytes); 
    init_header->payload_size = nbytesDifference - 8;
    init_header->status = 0; 
}

//This is the beefy helper function which will do most of our computation for adding or changing headers on the heap
struct header* recycle(size_t nbytes) {
    struct header *curr_header = (struct header *)&__heap_start; //start at heap start
    void *heap_end = sbrk(0);

    //Given that we aren't at the end keep looping
    while((void *)curr_header < heap_end){

        // If our header is invalid we simpy break and assume we must add to the end
        if ((void *)(curr_header + 1) > heap_end) {
            break;
        }

        // We grab our status and payload_size
        int status = curr_header->status; 
        int payload_size = curr_header->payload_size;
        
        // This is the case where we can change a header in our heap
        if(status == 0 && payload_size >= nbytes){

            //In the case where the requested bytes is less than the payload_size 
            if(payload_size > nbytes && (payload_size - nbytes) >= 16){
               addHeader(curr_header, nbytes, payload_size - nbytes); 
            }
            //We assume the current header is going to be used so we init it as so. 
            curr_header->status = 1;
            
            //This following condition is only when we added a header. 
            //In the case where we do not add a header the payload_size of this
            //curr_header should be unchanged. -> Client does not have to know we gave them more space
            if((payload_size - nbytes) >= 16){
                curr_header->payload_size = nbytes;
            }
            
            return curr_header; // we found a good header so we return the pointer to it!
        }
        //we go to the next header if this is not our size. 
        curr_header =  header_to_header(curr_header);
    }

    //In the case where we looped through the whole heap list and found nothing
    //We need to add to our heap by incrmenting by nbytes and 8 bytes for our header struct
    struct header *new_header = (struct header *)sbrk(nbytes + 8);
    new_header->payload_size = nbytes;
    new_header->status = 1; 

    return new_header; //We return the start of the new_header of allocated memory.
}

// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

//Malloc is what the client uses to allocate memory on the heap. 
void *malloc (size_t nbytes) {
    count_allocs++;
    total_bytes_requested += nbytes;
    nbytes = roundup(nbytes, 8);
    
    void* startPtr = NULL;

    if (sbrk(0) == (void *)&__heap_start) {
        struct header *init_header = (struct header*)sbrk(nbytes + 8); 
        init_header->payload_size = nbytes;
        init_header->status = 1;
        return header_to_payload(init_header);
    	//If this is not the first header in our heap we can call recycle
    }else {
        startPtr = recycle(nbytes); 
    }


    //we simply do not complete the request if this is NULL.
    if(startPtr == NULL){
        return NULL; 
    }

    //Given that recycle returns the headerptr we need to move it to the clientptr. 
    void* clientPtr = header_to_payload(startPtr); 
    return clientPtr;
}

void forwardCoalesce(struct header* curHeader){
    void *heap_end = sbrk(0);

    // Validate payload_size
    struct header* nextHeader = header_to_header(curHeader);
    
    // We need to make sure we have a valid header by checking to make sure we aren't going past the heap_end
    if ((void *)(header_to_payload(nextHeader)) > heap_end) {
        return;
    }

    // Keep merging forward while the next header is valid and free
    while ((void *)nextHeader < heap_end && nextHeader->status == 0) {
        curHeader->payload_size += nextHeader->payload_size + 8;
        nextHeader = header_to_header(curHeader); // Move to the next block
    }
}

//This free function takes in a ptr and if given a correct pointer it will deallocate from our heap
void free (void *ptr) {
    count_frees++;
    
    if(ptr == NULL){
        return;
    }

    //Assuming that we are given a valid ptr we will grab the header
    struct header* curHeader = payload_to_header(ptr);
    
    //We check our start and current end of heap
    void *heap_start = &__heap_start;
    void *heap_end = sbrk(0);

    //We don't want to do anything with an invalid ptr so we break and return
    // if cur header is somehow greater than our end we know we have an invalid ptr. 
    if ((void *)curHeader < heap_start || (void *)curHeader >= heap_end) {
        return;  // Invalid pointer
    }

    //We deallocate our current block by setting our status to to 0. 
    curHeader->status = 0;
    forwardCoalesce(curHeader);
}

//For debugging purposes this below function will output reports on the heap
void heap_dump (const char *label) {
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    printf("Heap segment at %p - %p\n", &__heap_start, sbrk(0));
    
    void *itterator = &__heap_start;
    void *heap_end = sbrk(0);

    // Iterate through the entire heap memory
    while (itterator != heap_end) {
        // Print the current block's header information
        struct header *curHeader = (struct header *)itterator;
        int status = curHeader->status; 
        int payload_size = curHeader->payload_size;
        
        printf("Header Starts: %p - Payload size: %d, Status: %d\n", itterator, payload_size, status);

        // Move to the next block (after the header and payload)

        itterator = (void *)header_to_header(curHeader);
    }

    printf("----------  END DUMP (%s) ----------\n", label);
    printf("Stats: %d in-use (%d allocs, %d frees), %d total payload bytes requested\n\n",
        count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
}

void malloc_report (void) {
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Malloc Report         \n");
    printf(  "=============================================\n");
    /***** TODO EXTENSION: Your code goes here if implementing extension *****/
}

void report_damaged_redzone (void *ptr) {
    printf("\n=============================================\n");
    printf(  " **********  Mini-Valgrind Alert  ********** \n");
    printf(  "=============================================\n");
    printf("Attempt to free address %p that has damaged red zone(s):", ptr);
    /***** TODO EXTENSION: Your code goes here if implementing extension *****/
}
