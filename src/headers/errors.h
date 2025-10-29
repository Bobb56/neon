#ifndef PRINTERROR_H
#define PRINTERROR_H

#include <stdlib.h>

#include "constants.h"
#include "dynarrays.h"

#define NB_ERRORS 120

#define return_on_error(value)      if (global_env->CODE_ERROR != 0) return value
#define if_error                    if (global_env->CODE_ERROR != 0)

#define neon_malloc     malloc
#define neon_realloc    realloc
#define neon_free       free

/*
void* neon_malloc(size_t size);
void* neon_realloc(void* ptr, size_t size);
void neon_free(void* ptr);
*/

void printError(int code);
void printErrSource(char* file, int line);
int get_exception_from_code_error(int code_error);
int allocatedMem(void);
void segfault(void);

#endif