#ifndef PRINTERROR_H
#define PRINTERROR_H

#include <stdlib.h>

#include "constants.h"
#include "dynarrays.h"
#include "objects.h"
#include "neobj.h"

#define NB_ERRORS 123

#define NO_ARGS                                 NEO_VOID
#define neon_fail(code_error, ...)              neon_set_error(code_error, __LINE__, NEON_SOURCE_ID, nelist_literal_create((NeObj[]){__VA_ARGS__, NEO_VOID}))
#define neon_internal_error()                   neon_fail(120, NO_ARGS)
#define SHORT_REPR_ERR_SIZE                     30

#define return_on_error(value)                  if (global_env->CODE_ERROR != 0) return value
#define if_error                                if (global_env->CODE_ERROR != 0)
#define neon_assert(condition, return_value)    if (!(condition)) {neon_fail(120, NO_ARGS) ; return return_value ;}

#define neon_malloc                             malloc
#define neon_realloc                            realloc
#define neon_free                               free

#ifdef MINIMAL_LIBC_RISCV64
#define align8(size)                            align(8, size)
#else
#define align8(size)                            size
#endif


/*
void* neon_malloc(size_t size);
void* neon_realloc(void* ptr, size_t size);
void neon_free(void* ptr);
*/

void neon_reset_error(void);
void get_error_info_and_reset(int* code, int* source_id, int* line_number, NeList** error_message_arguments);
void neon_set_error(int code_error, int line, int source_id, NeList* error_message_arguments);
void neon_raise_user_exception(int exception_code, char* format, NeList* args);

size_t align(uint8_t bytes, size_t size);

void printErrorString(char* format, NeList* error_message_arguments);
void printError(void);
void printErrSource(char* file, int line);
int get_exception_from_code_error(int code_error);
int allocatedMem(void);
void bp(int i);
void b(int i);
void segfault(void);

#endif