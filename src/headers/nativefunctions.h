#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H


#include "neobj.h"

typedef struct NeonEnv NeonEnv;

typedef enum {
    StandardModule,
    GraphicModule
} Module;


void init_module(Module module, NeonEnv* env);
NeObj call_function(Module module, int id, NeList* args);

#endif