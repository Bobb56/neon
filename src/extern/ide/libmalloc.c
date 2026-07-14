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

void* malloc_noheap(size_t size) {
    void* ptr = RAW_RAM_PTR + current_size;
    current_size += size;
    return ptr;
}

void free_noheap(void *ptr) {
    current_size = (intptr_t)ptr - (intptr_t)RAW_RAM_PTR;
}

void free_all_noheap(void) {
    current_size = 0;
}


// Cleans up the memory used for the editor's buffers
void clean_memory(void) {
	memset(RAW_RAM_PTR, 0xff, RAW_RAM_SIZE);
}