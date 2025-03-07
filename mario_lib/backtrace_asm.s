/* File: backtrace_asm.s
 * ---------------------
 * Mario Portillo
 * CS 107e
 * This below code will grab the current
 * stack pointer an return it. 
 */

.globl backtrace_get_fp
backtrace_get_fp:
    li a0, 0
    add a0, a0, s0
    ret
