#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <inttypes.h>
#include <stdlib.h>

#include "headers/libmalloc.h"

extern uint8_t __heap_low[];
extern uint8_t __heap_high[];

typedef struct __attribute__((packed)) block
{
    struct block *ptr;
    size_t size;
} __attribute__((packed)) block_t;


static uintptr_t heap_ptr = (uintptr_t)__heap_low;
static block_t _alloc_base;

static int24_t current_size = 0;


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
    custom_alloc_reset();
}

// Cleans up the memory used for the editor's buffers
void clean_memory(void) {
	//memset(__heap_low, 0x00, __heap_high - __heap_low);
}


/******* Standard allocator **********/

void* custom_malloc(size_t alloc_size)
{
    block_t *q;
    block_t *r;

    /* add size of block header to real size */
    const size_t size = alloc_size + sizeof(block_t);
    /* abort if alloc_size is 0 or size overflowed */
    if (size <= alloc_size)
    {
        return NULL;
    }

    for (block_t *p = &_alloc_base; (q = p->ptr); p = q)
    {
        if (q->size >= size)
        {
            if (q->size <= size + sizeof(block_t))
            {
                p->ptr = q->ptr;
            }
            else
            {
                q->size -= size;
                q = (block_t*)(((uint8_t*)q) + q->size);
                q->size = size;
            }

            return q + 1;
        }
    }

    /* compute next heap pointer */
    if (heap_ptr + size < heap_ptr || heap_ptr + size >= (uintptr_t)__heap_high)
    {
        return NULL;
    }

    r = (block_t*)heap_ptr;
    r->size = size;

    heap_ptr = heap_ptr + size;

    return r + 1;
}

void custom_free(void *ptr)
{
    if (ptr != NULL)
    {
        block_t *p;
        block_t *q;

        q = (block_t*)ptr - 1;

        for (p = &_alloc_base; p->ptr && p->ptr < q; p = p->ptr);

        if (p->ptr && (uint8_t*)p->ptr == ((uint8_t*)q) + q->size)
        {
            q->size += p->ptr->size;
            q->ptr = p->ptr->ptr;
        }
        else
        {
            q->ptr = p->ptr;
        }

        if (p->size && ((uint8_t*)p) + p->size == (uint8_t*)q)
        {
            p->size += q->size;
            p->ptr = q->ptr;
        }
        else
        {
            p->ptr = q;
        }
    }
}

void *custom_realloc(void *ptr, size_t size)
{
    block_t *h;
    void *p;

    if (ptr == NULL)
    {
        return custom_malloc(size);
    }

    h = (block_t*)ptr - 1;

    if (h->size >= size + sizeof(block_t))
    {
        return ptr;
    }

    p = custom_malloc(size);
    if (p == NULL)
    {
        return NULL;
    }

    memcpy(p, ptr, h->size - sizeof(block_t));
    custom_free(ptr);

    return p;
}


void custom_alloc_reset(void) {
    memset(&_alloc_base, 0, sizeof(block_t));
    heap_ptr = (uintptr_t)__heap_low;
}

char* custom_strdup(const char* string) {
    char* ptr = custom_malloc(strlen(string)+1);
    if (ptr != NULL)
        strcpy(ptr, string);
    return ptr;
}


char* custom_strndup(const char *s, size_t n) {
    size_t len = strnlen(s, n);
    char* ptr = custom_malloc(len + 1);
    if (ptr != NULL) {
        memcpy(ptr, s, len);
        ptr[len] = '\0';
    }
    return ptr;
  }