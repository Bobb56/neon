#ifndef PRINTERROR_H
#define PRINTERROR_H

#include "constants.h"
#include "dynarrays.h"

#define NB_ERRORS 118

void printError(int code);
void affLine(char* file, int line);
int get_exception_from_code_error(int code_error);
void* neon_malloc(size_t size);
void neon_free(void* ptr);
int allocatedMem(void);

#endif