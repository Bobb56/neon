#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H


#include "neobj.h"

typedef struct NeonEnv NeonEnv;

typedef enum {
    StandardModule,
    GraphicModule
} Module;


void init_module(Module module, NeonEnv* env);
NeObj call_function(int id, Module module, NeList* args);
const char* get_function_name(int id, Module module);
NeObj get_function(int id, Module module);

#endif