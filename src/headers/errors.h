#ifndef PRINTERROR_H
#define PRINTERROR_H

#include <stdlib.h>

#include "constants.h"
#include "dynarrays.h"

#define NB_ERRORS 123

#define return_on_error(value)                  if (global_env->CODE_ERROR != 0) return value
#define if_error                                if (global_env->CODE_ERROR != 0)
#define neon_assert(condition, return_value)    if (!(condition)) {neon_fail(120) ; return return_value ;}

#define neon_fail(code_error)                   neon_set_error(code_error, __LINE__, NEON_SOURCE_ID)
#define neon_internal_error()                   neon_set_error(120, __LINE__, NEON_SOURCE_ID)


#define neon_malloc     malloc
#define neon_realloc    realloc
#define neon_free       free

/*
void* neon_malloc(size_t size);
void* neon_realloc(void* ptr, size_t size);
void neon_free(void* ptr);
*/

void neon_reset_error(void);
void neon_set_error(int code_error, int line, int source_id);
void neon_raise_user_exception(int exception_code, char* message);

void printError(int code);
void printErrSource(char* file, int line);
int get_exception_from_code_error(int code_error);
int allocatedMem(void);
void bp(char* s);
void segfault(void);

#endif