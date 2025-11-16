#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "objects.h"
#include "dynarrays.h"
#include "trees.h"

#define ADD_STACK_SIZE(n)   volatile char dummy[n] = {0}

#define MAY_INTERRUPT()     if (global_env->atomic_counter == 0) {                      \
                                global_env->atomic_counter = global_env->ATOMIC_TIME;   \
                                interrupt();                                            \
                            }                                                           \
                            global_env->atomic_counter--


bool neoIsTrue(NeObj neo);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval(TreeBuffer* tb, TreeBufferIndex tree);
NeObj* get_address(TreeBuffer* tb, TreeBufferIndex tree);
void local(Var var, ptrlist* var_loc);
void newContext(ptrlist* var_loc);
void deleteContext(ptrlist* var_loc);
int exec_aux(TreeBuffer* tb, TreeBufferIndex tree);
void exec(TreeBuffer* tb, TreeBufferIndex tree);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
void update__name__(char* name);
NeObj eval_aux(TreeBuffer* tb, TreeBufferIndex tree);
NeObj callUserFunc(UserFunc* fun, NeList* args, NeObj neo_local_args);
void initRuntime(void);
void exitRuntime(void);


#endif