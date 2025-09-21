#ifndef PRINTERROR_H
#define PRINTERROR_H

#include "constants.h"
#include "dynarrays.h"

#define NB_ERRORS 117

void printError(int code);
void affLine(char* file, int line);
int get_exception_from_code_error(int code_error);



#endif