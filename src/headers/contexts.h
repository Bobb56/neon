#ifndef CONTEXTS_H
#define CONTEXTS_H

#include "dynarrays.h"
#include "neobj.h"


typedef struct NeSave
{
    NeObj object;
    Var var;
} NeSave;



typedef struct ContextStack {
    int len;
    uint8_t capacity;
    NeSave* tab;
} ContextStack;


#define CONTEXT_MARK                ((NeSave){.object = NEO_VOID, .var = -1})
#define ContextStack_top(stack)     (stack->tab[stack->len-1])

bool isContextMark(NeSave nesave);
void ContextStack_init(ContextStack* stack);
void ContextStack_destroy(ContextStack* stack);
void newContext(ContextStack* var_loc);
void deleteContext(ContextStack* var_loc);
void local(Var var, ContextStack* var_loc);


#endif