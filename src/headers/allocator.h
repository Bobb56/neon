#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "constants.h"

#ifdef TI_EZ80
    #include "../extern/ide/headers/libmalloc.h"
    #define neon_malloc                             custom_malloc
    #define neon_realloc                            custom_realloc
    #define neon_free                               custom_free
    #define neon_strdup                             custom_strdup
    #define neon_strndup                            custom_strndup
#else
    #define neon_malloc                             malloc
    #define neon_realloc                            realloc
    #define neon_free                               free
    #define neon_strdup                             strdup
    #define neon_strndup                            strndup
#endif

#ifdef MINIMAL_LIBC_RISCV64
#define align8(size)                            align(8, size)
#else
#define align8(size)                            size
#endif

#endif