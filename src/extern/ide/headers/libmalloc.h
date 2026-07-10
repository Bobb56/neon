#ifndef LIBMALLOC_H
#define LIBMALLOC_H

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

void* malloc_noheap(size_t size);
void free_all_noheap(void);

#endif /*LIBMALLOC_H*/
