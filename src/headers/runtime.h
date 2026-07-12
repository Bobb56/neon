#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#include "constants.h"
#include "objects.h"
#include "dynarrays.h"
#include "trees.h"

#define ADD_STACK_SIZE(n)   volatile char dummy[n] = {0} ; (void)dummy

#define MAY_INTERRUPT()     if (global_env->atomic_counter == 0) {                      \
                                global_env->atomic_counter = global_env->ATOMIC_TIME;   \
                                interrupt();                                            \
                            }                                                           \
                            global_env->atomic_counter--

#define tb_eval_aux(tb)     eval_aux(tb, (tb)->entry_point)
#define tb_exec(tb)         exec(tb, (tb)->entry_point)
#define tb_exec_aux(tb)     exec_aux(tb, (tb)->entry_point)

void neon_interp_yield(void);
void treeToList(NeList* l, TreeBuffer* tb, TreeBufferIndex tree_list);
void stop_execution(void);
bool neoIsTrue(NeObj neo);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
NeObj eval(TreeBuffer* tb, TreeBufferIndex tree);
NeObj* get_address(TreeBuffer* tb, TreeBufferIndex tree);
int exec_aux(TreeBuffer* tb, TreeBufferIndex tree);
void exec(TreeBuffer* tb, TreeBufferIndex tree);
bool isTrue(TreeBuffer* tb, TreeBufferIndex tree);
void update__name__(char* name);
NeObj eval_aux(TreeBuffer* tb, TreeBufferIndex tree);
NeObj callUserFunc(UserFunc* fun, Var* variables, NeObj* values, int variable_index, NeObj local_args, NeObj* self_address);
void initRuntime(void);
void exitRuntime(void);


#endif