#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <fileioc.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphx.h>
#include <keypadc.h>
#include <fontlibc.h>

#include "headers/libmalloc.h"

static int24_t current_size = 0;

extern uint8_t __heap_low[];
extern uint8_t __heap_high[];

void* malloc_noheap(size_t size) {
    void* ptr = __heap_low + current_size;
    current_size += size;
    return ptr;
}

void free_noheap(void *ptr) {
    current_size = (intptr_t)ptr - (intptr_t)__heap_low;
}

void free_all_noheap(void) {
    current_size = 0;
}

// Cleans up the memory used for the editor's buffers
void clean_memory(void) {
	//memset(__heap_low, 0x00, __heap_high - __heap_low);
}