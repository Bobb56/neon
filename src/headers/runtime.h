#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "objects.h"
#include "dynarrays.h"
#include "trees.h"

#define ADD_STACK_SIZE(n)   volatile char dummy[n] = {0}



bool neoIsTrue(NeObj neo);
bool isTrue(NeTree tree);
NeObj eval(NeTree tree);
NeObj* get_address(NeTree tree);
void local(Var var, ptrlist* var_loc);
void newContext(ptrlist* var_loc);
void deleteContext(ptrlist* var_loc);
int exec_aux(NeTree tree);
void exec(NeTree tree);
bool isTrue(NeTree tree);
void update__name__(char* name);
NeObj eval_aux(NeTree tree);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);


#endif