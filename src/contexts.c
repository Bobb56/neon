#define NEON_SOURCE_ID 2

#include "headers/contexts.h"
#include "headers/errors.h"
#include "headers/objects.h"
#include <string.h>


bool isContextMark(NeSave nesave) {
    return neo_is_void(nesave.object) && nesave.var == -1;
}



void ContextStack_init(ContextStack* stack)
{
    stack->capacity = 1;
  
    stack->tab=neon_malloc((1<<stack->capacity)*sizeof(NeSave));

    if (stack->tab == NULL) {
        neon_fail(12);
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
            neon_fail(12);
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
    if (stack->len == 0) {
        neon_fail(38);
        return (NeSave){0};
    }
  
    return stack->tab[--stack->len];
}



void newContext(ContextStack* var_loc) {
    ContextStack_append(var_loc, CONTEXT_MARK);
}



void deleteContext(ContextStack* var_loc)
{
    NeSave nesave;
    while (!isContextMark(ContextStack_top(var_loc))) {
        nesave = ContextStack_pop(var_loc);
        replace_var(nesave.var, nesave.object);
    }

    ContextStack_pop(var_loc);
    return ;
}



void local(Var var, ContextStack* var_loc)
{
    // sauvegarde de l'object actuel
    NeSave ns = (NeSave) {.object = get_var_value(var), .var = var};

    // réinitialisation de la variable
    var_reset(get_absolute_address(var));

    //ajout de la sauvegarde au contexte actuel
    ContextStack_append(var_loc, ns);
}

