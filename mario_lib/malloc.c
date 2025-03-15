/* File: malloc.c
 * --------------
 * ***** This files contains the implementations for the functions in the malloc module *****
 * Sahan Samarakoon (CS 107E)
 */

 #include "malloc.h"
 #include "memmap.h"
 #include "printf.h"
 #include <stddef.h> // for NULL
 #include "strings.h"
 #include "backtrace.h"
 #include <stdint.h>  // uint32_t
 
 /*
  * Data variables private to this module used to track
  * statistics for debugging/validate heap:
  *    count_allocs, count_frees, total_bytes_requested
  */
 static int count_allocs, count_frees, total_bytes_requested;
 
 
 const int R_ZONE_CANARY =  0x99999999;
 
 typedef struct {
     int payload_size;	// 4 bytes
     int status;			// 4 bytes
     frame_t frames[3];	// 8 x 3 bytes = 24 bytes
     int real_size;		// Actual size requested by user
     int pre_red_zone;	// pre red zone is built into the header
 } header_t;	
 
 enum { IN_USE = 0, FREE = 1};
 
 const size_t HEADER_SIZE = sizeof(header_t);
 const size_t R_ZONE_SIZE = 4;		// 4 byte redzone
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
 
 /*
  * This helper takes an allocated memory segment, and adds red zones on front and back
  * Returns the pointer to header
  */
 void *fill_red_zone(header_t *hdr){
     hdr->pre_red_zone = R_ZONE_CANARY;
         
     uint32_t *post = (uint32_t *)((char *)hdr + hdr->real_size + HEADER_SIZE);
     memset(post, 0x99, 4);
 
     return hdr;
 }
 
 void fill_backtrace(header_t *hdr){
     frame_t all_frames[5];
     backtrace_gather_frames(all_frames, 5);
     frame_t relevant_frames[3];
     for(int i = 0; i < 3; i++){
         relevant_frames[i] = all_frames[i+2];
     }
     memcpy(hdr->frames,relevant_frames, sizeof(relevant_frames));
 }
 // Simple macro to round up x to multiple of n.
 // The efficient but tricky bitwise approach it uses
 // works only if n is a power of two -- why?
 #define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))
 
 void *malloc (size_t nbytes) {
     total_bytes_requested += nbytes;
     if (nbytes <= 0){
         return NULL;
     }
     
     count_allocs++;
     const int EXTRAS = HEADER_SIZE + 2 * R_ZONE_SIZE;
     int unrounded = nbytes;
     nbytes = roundup(nbytes, 8);
     size_t total_bytes = nbytes + EXTRAS;
 
     header_t *curr = (header_t *)&__heap_start;
     while ((void *)curr < sbrk(0)){
         if ((curr->payload_size >= nbytes) && (curr->status == FREE)){
             int split_size = curr->payload_size - total_bytes;
             curr->payload_size = nbytes;
             curr->status = IN_USE;
             curr->real_size = unrounded;
 
             fill_red_zone(curr);
             fill_backtrace(curr);
 
             header_t *next = (header_t *)((char *)curr + total_bytes);
             if (split_size > 0){	
                 next->payload_size = split_size;
                 next->status = FREE;
             } 
             else if (split_size <= 0){				// negative split size means nothing to split
                 int extra_bytes = EXTRAS + split_size;
                 if (extra_bytes > 0){
                 curr->payload_size += extra_bytes; // no point in creating a split if there's no
                 }								   // space to put stuff in
             }									
 
             return (void *)((char *)curr + HEADER_SIZE);
         }
         curr = (header_t *)((char *)curr + curr->payload_size + EXTRAS);
     }
     // extend the end of heap
     header_t *hdr = sbrk(total_bytes);
     if (hdr == NULL){
         count_allocs--;		// this does not count as a valid allocation
         return hdr;		
     }	
     hdr->payload_size = nbytes;
     hdr->status = IN_USE;
     hdr->real_size = unrounded;
     fill_red_zone(hdr);
     fill_backtrace(hdr);
     return (void *)((char *)hdr + HEADER_SIZE);
 
 }
 
 
 void report_damaged_redzone (void *ptr);
 void free (void *ptr) {
  
     if (ptr == NULL){
         return;			// if pointer is NULL, do nothing
     }
     count_frees++;
     header_t *hdr = ((header_t *)ptr - 1);
     int pre = hdr->pre_red_zone;
     int *post = (int *)((char *)ptr + hdr->real_size);
 
     if ((void *)post > sbrk(0)) { // avoid seg fault
         printf("Attempt to free an invalid pointer: %p\n", ptr);
         return;
     }
 
     if ((pre != R_ZONE_CANARY) || (*post != R_ZONE_CANARY)){ // memory has been abused
         report_damaged_redzone(ptr);
         return;
     }
 
 
     /* MEMORY NOT ABUSED */
 
     hdr->status = FREE;
     size_t total_bytes = hdr->payload_size + HEADER_SIZE + 2 * R_ZONE_SIZE;
     header_t *next = (header_t *)((char *)hdr + total_bytes);
     while (((void *)next < sbrk(0)) && (next->status == FREE)){
         total_bytes = next->payload_size + HEADER_SIZE + 2 * R_ZONE_SIZE;
         hdr->payload_size += total_bytes;
         next = (header_t *)((char *)next + total_bytes);
     }
 
 }
 
 
 void heap_dump (const char *label) {
     printf("\n---------- HEAP DUMP (%s) ----------\n", label);
     printf("Heap segment at %p - %p\n", &__heap_start, sbrk(0));
 
     header_t *hdr = (header_t *) &__heap_start;
     int i = 1;
     while ((void *)hdr < sbrk(0)){
         printf("BLOCK %d\nSize: %d\nStatus: %d\n\n", i, hdr->payload_size, hdr->status);
         hdr = (header_t *)((char *)hdr + hdr->payload_size + HEADER_SIZE + 2 * R_ZONE_SIZE);
         i++;
     }
     printf("----------  END DUMP (%s) ----------\n", label);
     printf("Stats: %d in-use (%d allocs, %d frees), %d total payload bytes requested\n\n",
         count_allocs - count_frees, count_allocs, count_frees, total_bytes_requested);
 }
 
 
 void malloc_report (void) {
     printf("\n=============================================\n");
     printf(  "         Mini-Valgrind Malloc Report         \n");
     printf(  "=============================================\n");
     printf("final stats: %d allocs, %d frees, %d total payload bytes requested\n\n",
         count_allocs, count_frees, total_bytes_requested);
 
     int total_lost = 0; 
     int lost_blocks = 0; 
     header_t *hdr = (header_t *) &__heap_start; 
     
     while ((void *)hdr < sbrk(0)) { 
         if (hdr->status == IN_USE) {  
             lost_blocks++; 
             total_lost += hdr->real_size; 
             printf("\n%d bytes are lost, allocated by\n", hdr->real_size); 
             backtrace_print_frames(hdr->frames, 3); 
         } 
 
         // next block
         hdr = (header_t *)((char *)hdr + hdr->payload_size + HEADER_SIZE + 2 * R_ZONE_SIZE); 
     } 
     
     if (lost_blocks > 0) { 
         printf("\nLost %d total bytes in %d blocks\n", total_lost, lost_blocks); 
     } 
     else { 
         printf("\nNo memory leaks detected! \n");
      }	
 }
 
 
 void report_damaged_redzone (void *ptr) {
     printf("\n=============================================\n");
     printf(  " **********  Mini-Valgrind Alert  ********** \n");
     printf(  "=============================================\n");
     
     
     header_t *hdr = ((header_t *)ptr - 1);
     int pre = hdr->pre_red_zone;
     int *post = (int *)((char *)ptr + hdr->real_size);
 
     printf("Attempt to free address %p that has damaged red zone(s):[%8x], [%8x]\n",
                        ptr, pre, *post);
     printf("Block of size %d bytes, allocated by\n", hdr->real_size);
 
     backtrace_print_frames(hdr->frames, 3);
 
 
 }
 