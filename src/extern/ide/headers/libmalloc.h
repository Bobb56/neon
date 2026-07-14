#ifndef LIBMALLOC_H_
#define LIBMALLOC_H_

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


#define RAW_RAM_PTR      ((void*)0xD09466)
#define RAW_RAM_SIZE     ((size_t)21945*2)

void* malloc_noheap(size_t size);
void free_noheap(void* ptr);
void free_all_noheap(void);
void clean_memory(void);

#endif /*LIBMALLOC_H*/
