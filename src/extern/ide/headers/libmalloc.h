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


void* malloc_noheap(size_t size);
void free_noheap(void* ptr);
void free_all_noheap(void);
void clean_memory(void);

void* custom_malloc(size_t size);
void custom_free(void* ptr);
void* custom_realloc(void* ptr, size_t size);
char* custom_strdup(const char* string);
char* custom_strndup(const char *s, size_t n);
void custom_alloc_reset(void);

#endif /*LIBMALLOC_H*/
