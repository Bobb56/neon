#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "objects.h"
#include "dynarrays.h"






bool neoIsTrue(NeObj neo);
bool isTrue(Tree* tree);
NeObj eval(Tree* tree);
NeObj* get_address(Tree* tree);
void local(Var var, ptrlist* var_loc);
void newContext(ptrlist* var_loc);
void deleteContext(ptrlist* var_loc);
int exec_aux(Tree* tree);
void exec(Tree* tree);
bool isTrue(Tree* tree);
void updateFileName(char* name);
NeObj eval_aux(Tree* tree);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);


#endif