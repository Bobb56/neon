#define NEON_SOURCE_ID 25

#include "headers/nativefunctions.h"
#include "headers/standardmodule.h"
#include "headers/errors.h"
#include "headers/constants.h"
#include "headers/objects.h"

#ifdef HAS_GRAPHIC_MODULE
#include "headers/graphicmodule.h"
#endif



char* get_module_name(Module module) {
    switch (module) {
        case StandardModule:
            return "standard";
        case GraphicModule:
            return "graphics";
        default:
            return "unknown";
    }
}


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
            neon_fail(98, neo_new_str_create(get_module_name(module)), neo_new_str_create(PLATFORM));
            return;
    }
}


NeObj call_function(int id, Module module, NeList* args) {
    switch (module) {
        case StandardModule:
            return call_standardfunction(id, args);
#ifdef HAS_GRAPHIC_MODULE
        case GraphicModule:
            return call_graphicfunction(id, args);
#endif
        default:
            neon_fail(98, neo_new_str_create(get_module_name(module)), neo_new_str_create(PLATFORM));
            return NEO_VOID;
    }
}


const char* get_function_name(int id, Module module) {
    switch (module) {
        case StandardModule:
            return get_standardfunction_name(id);
#ifdef HAS_GRAPHIC_MODULE
        case GraphicModule:
            return get_graphicfunction_name(id);
#endif
        default:
            neon_fail(98, neo_new_str_create(get_module_name(module)), neo_new_str_create(PLATFORM));
            return NULL;
    }
}


NeObj get_function(int id, Module module) {
    switch (module) {
        case StandardModule:
            return get_standardfunction(id);
#ifdef HAS_GRAPHIC_MODULE
        case GraphicModule:
            return get_graphicfunction(id);
#endif
        default:
            neon_fail(98, neo_new_str_create(get_module_name(module)), neo_new_str_create(PLATFORM));
            return NEO_VOID;
    }
}