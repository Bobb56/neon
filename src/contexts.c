#define NEON_SOURCE_ID 2

#include "headers/contexts.h"
#include "headers/errors.h"
#include "headers/objects.h"
#include "headers/dynarrays.h"
#include "headers/processcycle.h"
#include "headers/neonio.h"
#include <string.h>

bool isContextMark(NeSave nesave) {
    return neo_is_void(nesave.object) && nesave.var == -1;
}



void ContextStack_init(ContextStack* stack)
{
    stack->capacity = 1;
  
    stack->tab=neon_malloc((1<<stack->capacity)*sizeof(NeSave));

    if (stack->tab == NULL) {
        neon_fail(12, NO_ARGS);
        return;
    }
  
    stack->len = 0;
    return;
}




void ContextStack_append(ContextStack* stack, NeSave nesave)
{
    NeSave* tmp;
    if (1<<stack->capacity == stack->len) {
        stack->capacity++;
        tmp = neon_realloc(stack->tab, (1<<stack->capacity)*sizeof(NeSave));//réallocation de list.tab
        if (tmp == NULL) {
            neon_fail(12, NO_ARGS);
            return;
        }
        stack->tab = tmp;//affectation du pointeur de tmp vers list.tab
    }
  
    stack->tab[stack->len]=nesave;
    stack->len++;
}




void ContextStack_destroy(ContextStack* stack)
{
    neon_free(stack->tab);
}



NeSave ContextStack_pop(ContextStack* stack)
{
    neon_assert(stack->len > 0, (NeSave){0});
    return stack->tab[--stack->len];
}



void newContext(ContextStack* var_loc) {
    ContextStack_append(var_loc, CONTEXT_MARK);
}



void deleteContext(ContextStack* var_loc) {
    NeSave nesave;
    while (!isContextMark(ContextStack_top(var_loc))) {
        nesave = ContextStack_pop(var_loc);
        replace_var(nesave.var, nesave.object);
    }

    ContextStack_pop(var_loc);
    return ;
}


void CapturedVars_init(CapturedVars* varsToSave) {
    ContextStack_init(&varsToSave->vars);
    bitmap_init(&varsToSave->is_captured);
}

void CapturedVars_destroy(CapturedVars* varsToSave) {
    ContextStack_destroy(&varsToSave->vars);
    bitmap_destroy(&varsToSave->is_captured);
}


CapturedVarsCheckPoint CapturedVars_get_checkpoint(CapturedVars* varsToSave) {
    return varsToSave->vars.len;
}



void local(Var var, Process* process)
{
    // Marquage de l'objet comme local au processus actuel, pour le sauvegarder lors des changements de contextes
    // Il est impératif d'appeler save_later avant de sauvegarder la variable comme variable locale
    // En effet la valeur captée par save_later est la variable qui sera exposée aux autres processus, ce doit donc être la valeur présente dans la variable avant toute modification de cette variable en tant que variable locale
    // Si on ne le fait pas, save_later va capter la valeur vide sauvegardée par local, et le remplacement final des variables du processus par les objets de varsToSave va écraser la valeur extérieure de la variable par l'objet vide capté par save_later
    // Cela va donc résulter en une fuite de l'objet original
    save_later(&process->varsToSave, var);

    // sauvegarde de l'object actuel
    NeSave ns = (NeSave) {.object = get_var_value(var), .var = var};

    // réinitialisation de la variable
    var_reset(get_absolute_address(var));

    //ajout de la sauvegarde au contexte actuel
    ContextStack_append(&process->var_loc, ns);
}


bool isLocal(Var var, ContextStack* context) {
    int i = context->len-1;
    while (!isContextMark(context->tab[i]) && context->tab[i].var != var) i--;
    return !isContextMark(context->tab[i]);
}