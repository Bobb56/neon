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

static int24_t currentsize;
static int24_t maxsize;
static void* mem = NULL;

void* malloc_noheap(size_t size){
    if (mem == NULL){
        maxsize = os_MemChk(&mem);
        currentsize = 0;
    }

    if (currentsize >= maxsize) {
        return NULL;
    }

    void* ret = (char*)mem + currentsize;
    currentsize += size;
    return ret;
}

void free_all_noheap(void) {
    mem = NULL;
}