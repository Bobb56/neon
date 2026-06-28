#ifndef CONTEXTS_H
#define CONTEXTS_H

#include "dynarrays.h"
#include "neobj.h"

typedef struct Process Process;

typedef struct NeSave
{
    NeObj object;
    Var var;
} NeSave;



typedef struct {
    int len;
    uint8_t capacity;
    NeSave* tab;
} ContextStack;


// Représente les variables capturées par un processus et leur valeur dans le monde extérieur
typedef struct {
    bitmap is_captured; // Dit si une variable est capturée par un processus
    ContextStack vars; // Stocke les valeurs des variables dans le monde extérieur
} CapturedVars;

typedef int CapturedVarsCheckPoint;


#define CONTEXT_MARK                ((NeSave){.object = NEO_VOID, .var = -1})
#define ContextStack_top(stack)     (stack->tab[stack->len-1])

void CapturedVars_init(CapturedVars* varsToSave);
CapturedVarsCheckPoint CapturedVars_get_checkpoint(CapturedVars* varsToSave);
void CapturedVars_destroy(CapturedVars* varsToSave);
bool isContextMark(NeSave nesave);
void ContextStack_init(ContextStack* stack);
void ContextStack_append(ContextStack* stack, NeSave nesave);
NeSave ContextStack_pop(ContextStack* stack);
void ContextStack_destroy(ContextStack* stack);
void newContext(ContextStack* var_loc);
void deleteContext(ContextStack* var_loc);
void local(Var var, Process* process);
bool isLocal(Var var, ContextStack* context);


#endif