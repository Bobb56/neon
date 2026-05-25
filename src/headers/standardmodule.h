#ifndef BUILTINFUNCTIONS_H
#define BUILTINFUNCTIONS_H

#include "constants.h"
#include "objects.h"

#define NBBUILTINFUNC 61


#define ARG(n)      args->tab[n]

const char* get_standardfunction_name(int id);
NeObj get_standardfunction(int id);
NeObj call_standardfunction(int id, NeList* list);
void init_standardmodule(NeonEnv* env);

#endif