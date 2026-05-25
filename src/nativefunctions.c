#define NEON_SOURCE_ID 25

#include "headers/nativefunctions.h"
#include "headers/standardmodule.h"
#include "headers/errors.h"

#ifdef HAS_GRAPHIC_MODULE
#include "headers/graphicmodule.h"
#endif

void init_module(Module module, NeonEnv* env) {
    switch (module) {
        case StandardModule:
            init_standardmodule(env);
            return;
#ifdef HAS_GRAPHIC_MODULE
        case GraphicModule:
            init_graphicmodule(env);
            return;
#endif
        default:
            neon_fail(98);
            return;
    }
}


NeObj call_function(Module module, int id, NeList* args) {
    switch (module) {
        case StandardModule:
            return call_standardfunction(id, args);
#ifdef HAS_GRAPHIC_MODULE
        case GraphicModule:
            return call_graphicfunction(id, args);
#endif
        default:
            neon_fail(98);
            return NEO_VOID;
    }
}